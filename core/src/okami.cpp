#include <okami/okami.hpp>

#include <okami/graph.hpp>
#include <okami/tree.hpp>

using namespace okami;

std::ostream& okami::operator<<(std::ostream& os, const ErrorGlfw& err)
{
    os << "GLFW Error (Code: " << err.code << "): " << err.description;
    return os;
}

std::ostream& okami::operator<<(std::ostream& os, const RuntimeError& err)
{
    os << "Runtime Error: " << err.description;
    return os;
}

Error okami::operator|(Error const& err1, Error const& err2) {
    if (err1.IsError()) {
        return err1;
    } else if (err2.IsError()) {
        return err2;
    } else {
        return {};
    }
} 

bool okami::Error::IsOk() const {
    return std::holds_alternative<std::monostate>(details);
}

bool okami::Error::IsError() const {
    return !IsOk();
}

std::string okami::Error::ToString() const {
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

Error okami::Engine::Initialize(entt::registry& registry) const {
    Error err;
    for (auto const& module : _modules) {
        err = module->Initialize(registry);
        OKAMI_ERR_RETURN(err);
    }
    return {};
}

Error okami::Engine::Destroy(entt::registry& registry) const {
    Error err;
    for (auto it = _modules.rbegin(); it != _modules.rend(); ++it) {
        err = err | (*it)->Destroy(registry);
    }
    return err;
}

void okami::Engine::Execute(entt::registry& registry) const { 
    for (auto const& module : _modules) {
        module->PreExecute(registry);
    }

    for (auto const& module : _modules) {
        module->PostExecute(registry);
    }
}