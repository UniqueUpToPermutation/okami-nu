#include <okami/okami.hpp>

#include <entt/entt.hpp>

#include <GLFW/glfw3.h>

#include <vector>
#include <variant>
#include <sstream>
#include <nonstd/expected.hpp>

using entity = entt::entity;

struct ErrorGlfw {
    int code;
    const char* description;
};

std::ostream& operator<<(std::ostream& os, const ErrorGlfw& err)
{
    os << "GLFW Error (Code: " << err.code << "): " << err.description;\
    return os;
}

using ErrorDetails = std::variant<std::monostate, ErrorGlfw>;

struct Error {
    int line = 0;
    std::string_view file = "N/A";
    ErrorDetails details;

    inline bool IsOk() const {
        return std::holds_alternative<std::monostate>(details);
    }

    inline bool IsErr() const {
        return !IsOk();
    }

    std::string ToString() const {
        return std::visit([this](auto const& obj) -> std::string {
            if constexpr (std::is_same_v<std::decay_t<decltype(obj)>, std::monostate>) {
                return std::string("No error");
            } else {
                std::stringstream ss;
                ss << file << ": " << line << ": ";
                ss << obj;
                return ss.str();
            }
        }, details);
    }

    operator bool() const {
        return IsOk();
    }
};

Error NoError() {
    return {};
}

template <typename T>
struct SignalSource {
    std::optional<T> value;

    T* operator->() {
        return &*value;
    }

    void Fire(T value) {
        value = std::move(value);
    }
};

template <typename T>
concept EmptyType = std::is_empty<T>::value;

template <EmptyType T>
struct SignalSource<T> {
    bool value;

    inline void Fire() {
        value = true;
    }
};

template <typename T>
inline void FireSignal(entt::registry& reg, entt::entity e, T&& value) {
    reg.get<SignalSource<T>>(e).Fire(std::forward(value));
}
template <typename T, typename... Ts, typename... ExTs>
inline void FireSignal(
    entt::basic_view<entt::get_t<Ts...>, entt::exclude_t<ExTs...>>& view, 
    entt::entity e, T&& value) {
    view.template get<SignalSource<T>>(e).Fire(std::forward(value));
}
template <EmptyType T>
inline void FireSignal(entt::registry& reg, entt::entity e) {
    reg.get<SignalSource<T>>(e).Fire();
}
template <EmptyType T, typename... Ts, typename... ExTs>
inline void FireSignal(
    entt::basic_view<entt::get_t<Ts...>, entt::exclude_t<ExTs...>>& view, 
    entt::entity e) {
    view.template get<SignalSource<T>>(e).Fire();
}

template <typename T>
inline bool CheckSignal(entt::registry& reg, entt::entity e) {
    return reg.template get<SignalSource<T>>(e).value;
}
template <typename T, typename... Ts, typename... ExTs>
inline bool CheckSignal(
    entt::basic_view<entt::get_t<Ts...>, entt::exclude_t<ExTs...>>& view, 
    entt::entity e) {
    return view.template get<SignalSource<T>>(e).value;
}

template <typename T>
struct MessageBuffer {
    std::vector<T> messages;

    auto begin() { return messages.begin(); }
    auto end() { return messages.end(); }
};

template <typename T>
struct MessageSource : public MessageBuffer<T> {};
template <typename T>
struct MessageSink : public MessageBuffer<T> {};

#define OKAMI_ERR_SET(err, errDetails) \
    err = Error{__LINE__, __FILE__, errDetails}
#define OKAMI_ERR_IF(cond, err, errDetails) \
    if (cond) { \
        OKAMI_ERR_SET(err, errDetails); \
    } 
#define OKAMI_ERR_RETURN(err) \
    if (err.IsErr()) { \
        return err; \
    }
#define OKAMI_UNEXP_RETURN(err) \
if (err.IsErr()) { \
    return nonstd::make_unexpected(std::move(err)); \
}

class Module {
public:
    virtual Error Initialize(entt::registry& registry) const = 0;
    virtual void PreExecute(entt::registry& registry) const = 0;
    virtual void PostExecute(entt::registry& registry) const = 0;
    virtual Error Destroy(entt::registry& registry) const = 0;

    virtual ~Module() = default;
};

template <typename T>
concept ModuleType = std::is_base_of<Module, T>::value;

class Engine {
private:
    std::vector<std::unique_ptr<Module>> _modules;

public:
    template <ModuleType T, typename... ParamTs>
    T* Add(ParamTs&&... params) {
        auto modul = std::make_unique<T>(std::forward(params)...);
        auto ptr = modul.get();
        _modules.emplace_back(std::move(modul));
        return ptr;
    }

    Error Initialize(entt::registry& registry) const {
        Error err;
        for (auto const& module : _modules) {
            err = module->Initialize(registry);
            OKAMI_ERR_RETURN(err);
        }
        return {};
    }

    Error Destroy(entt::registry& registry) const {
        Error err;
        for (auto it = _modules.rbegin(); it != _modules.rend(); ++it) {
            auto possibleErr = (*it)->Destroy(registry);
            if (!err) {
                err = possibleErr;
            }
        }
        return err;
    }

    void Execute(entt::registry& registry) const { 
        for (auto const& module : _modules) {
            module->PreExecute(registry);
        }

        for (auto const& module : _modules) {
            module->PostExecute(registry);
        }
    }
};

struct CGlfwWindowInstance {
    GLFWwindow* window = nullptr;
};

struct SWindowClosed {};

class GlfwModule : public Module {
private:
    static GlfwModule* gSingleton;

    Error _error;

    static void ErrorHandler(int error, const char* desc) {
        OKAMI_ERR_SET(gSingleton->_error, (ErrorGlfw{error, desc}));
    }

public:
    inline GlfwModule() {
        if (gSingleton) {
            throw std::runtime_error("Only one instance of GlfwModule allowed!");
        }
        gSingleton = this;
    }

    inline ~GlfwModule() {
        gSingleton = nullptr;
    }

    entity CreateWindow(entt::registry& reg) {
        auto e = reg.create();
        reg.emplace<CGlfwWindowInstance>(e, CGlfwWindowInstance{nullptr});
        reg.emplace<SignalSource<SWindowClosed>>(e);
        return e;
    }

    Error Initialize(entt::registry& registry) const override {
        glfwSetErrorCallback(ErrorHandler);
        
        glfwInit();
        OKAMI_ERR_RETURN(_error);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
        auto view = registry.view<CGlfwWindowInstance>();
        for (auto e : view) {
            auto& instance = view.get<CGlfwWindowInstance>(e);
            auto window = glfwCreateWindow(1920, 1080, "Test", nullptr, nullptr);      
            OKAMI_ERR_RETURN(_error);
            instance.window = window;
        }

        return {};
    }

    Error Destroy(entt::registry& registry) const override {
        auto windowView = registry.view<CGlfwWindowInstance>();

        for (auto e : windowView) {
            auto& instance = windowView.get<CGlfwWindowInstance>(e);
            if (instance.window) {
                glfwDestroyWindow(instance.window);
            }
            instance.window = nullptr;
        }

        glfwTerminate();

        return {};
    }

    void PreExecute(entt::registry& registry) const override {
        glfwPollEvents();

        auto view = registry.view<CGlfwWindowInstance, SignalSource<SWindowClosed>>();
        for (auto e : view) {
            auto& instance = view.get<CGlfwWindowInstance>(e);

            if (instance.window) {
                auto shouldClose = glfwWindowShouldClose(instance.window);
                if (shouldClose) {
                    FireSignal<SWindowClosed>(view, e);
                }
            }
        }
    }

    void PostExecute(entt::registry& registry) const override {

    }
};
GlfwModule* GlfwModule::gSingleton = nullptr;

int main() {
    Engine en;

    entt::registry reg;

    auto glfw = en.Add<GlfwModule>();
    auto window = glfw->CreateWindow(reg);

    en.Initialize(reg);
    while (!CheckSignal<SWindowClosed>(reg, window)) {
        en.Execute(reg);
    }
    en.Destroy(reg);

    return 0;
}