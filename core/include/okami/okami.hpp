#pragma once

#include <entt/entt.hpp>
#include <vector>
#include <variant>
#include <sstream>
#include <nonstd/expected.hpp>

namespace okami {
    using entity = entt::entity;

    struct RuntimeError {
        const char* description;
    };

    struct ErrorGlfw {
        int code;
        const char* description;
    };

    std::ostream& operator<<(std::ostream& os, const ErrorGlfw& err);
    std::ostream& operator<<(std::ostream& os, const RuntimeError& err);

    using ErrorDetails = std::variant<std::monostate, ErrorGlfw, RuntimeError>;
    struct Error;

    template <typename T>
    using Expected = nonstd::expected<T, Error>;

    struct Error {
        int line = 0;
        std::string_view file = "N/A";
        ErrorDetails details;

        bool IsOk() const;
        bool IsError() const;
        std::string ToString() const;
    };

    Error operator|(Error const& err1, Error const& err2);

    inline bool IsError(Error err) {
        return err.IsError();
    }
    inline Error MakeError(Error err) {
        return err;
    }
    template <typename T>
    inline bool IsError(Expected<T> exp) {
        return !exp.has_value();
    }
    template <typename T>
    inline Error MakeError(Expected<T> exp) {
        return exp.error();
    }
    inline nonstd::unexpected_type<Error> MakeUnexpected(Error err) {
        return nonstd::make_unexpected(err);
    }
    template <typename T>
    inline nonstd::unexpected_type<Error> MakeUnexpected(nonstd::expected<T, Error> exp) {
        return nonstd::make_unexpected(std::move(exp.error()));
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
        if (IsError(err)) { \
            return MakeError(err); \
        }
    #define OKAMI_EXP_RETURN(err) \
        if (IsError(err)) { \
            return MakeUnexpected(err); \
        }

    template <typename T>
    Expected<std::reference_wrapper<T>> TryGet(entt::registry& reg, entity e) {
        Error err;
        auto result = reg.template try_get<T>(e);
        OKAMI_ERR_IF(!result, err, RuntimeError{"Could not find component!"});
        OKAMI_EXP_RETURN(err);
        return *result;
    }

    template <typename T, typename... Ts, typename... ExTs>
    Expected<std::reference_wrapper<T>> TryGet(
        entt::basic_view<entt::get_t<Ts...>, 
        entt::exclude_t<ExTs...>>& view, entity e) {
        Error err;
        auto result = view.template try_get<T>(e);
        OKAMI_ERR_IF(!result, err, RuntimeError{"Could not find component!"});
        OKAMI_EXP_RETURN(err);
        return *result;
    }

    class Module {
    public:
        virtual Error Initialize(entt::registry& registry) const = 0;
        virtual void PreExecute(entt::registry& registry) const = 0;
        virtual void PostExecute(entt::registry& registry) const = 0;
        virtual Error Destroy(entt::registry& registry) const = 0;

        virtual ~Module() = default;

        Module() = default;
        Module(Module const&) = delete;
    };

    template <typename T>
    concept ModuleType = std::is_base_of<Module, T>::value;

    class Engine {
    private:
        std::vector<std::unique_ptr<Module>> _modules;

    public:
        template <ModuleType T, typename... ParamTs>
        T* Add(ParamTs&&... params) {
            auto modul = std::make_unique<T>(std::forward<ParamTs>(params)...);
            auto ptr = modul.get();
            _modules.emplace_back(std::move(modul));
            return ptr;
        }

        Error Initialize(entt::registry& registry) const;
        Error Destroy(entt::registry& registry) const;
        void Execute(entt::registry& registry) const;
    };
}