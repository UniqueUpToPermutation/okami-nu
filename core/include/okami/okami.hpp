#pragma once

#include <entt/entt.hpp>
#include <vector>
#include <variant>
#include <sstream>
#include <nonstd/expected.hpp>

namespace okami {
    using entity = entt::entity;
    constexpr auto null = entt::null;

    template <class T>
    concept DefaultInitializable = requires { T{}; };

    struct Error;

    struct RuntimeError {
        const char* description;
    };

    struct InvalidPathError {
        std::string path;
    };

    struct GlfwError {
        int code;
        const char* description;
    };

    struct MultipleErrors {
        std::vector<Error> errors;
    };

    std::ostream& operator<<(std::ostream& os, const GlfwError& err);
    std::ostream& operator<<(std::ostream& os, const RuntimeError& err);
    std::ostream& operator<<(std::ostream& os, const MultipleErrors& err);
    std::ostream& operator<<(std::ostream& os, const InvalidPathError& err);

    using ErrorDetails = std::variant<
        std::monostate, 
        GlfwError, 
        RuntimeError,
        InvalidPathError,
        MultipleErrors>;
    struct Error;

    template <typename T>
    using Expected = nonstd::expected<T, Error>;
    template <typename T>
    struct ExpectedRef;

    struct Error {
        int line = 0;
        std::string_view file = "N/A";
        ErrorDetails details;

        bool IsOk() const;
        bool IsError() const;
        std::string ToString() const;

        Error& operator|=(Error const& other);
        Error& operator+=(Error const& other);

        // Unwraps the expected object and stores the error in this
        template <typename T>
        std::optional<T> Unwrap(Expected<T>&& ex);
        // Unwraps the expected object and stores the result in this
        template <typename T>
        T* Unwrap(ExpectedRef<T>&& ref);
    };

    std::ostream& operator<<(std::ostream& os, const Error& err);

    Error operator|(Error const& err1, Error const& err2);
    Error operator+(Error const& err1, Error const& err2);

    template <typename T>
    struct ExpectedRef {
        Expected<std::reference_wrapper<T>> value;

        T* operator->() {
            return &value.value().get();
        }

        inline ExpectedRef(T& ref) : value(ref) {}
        inline ExpectedRef(nonstd::unexpected<Error> err) : value(err) {}
    };

    inline bool IsError(ErrorDetails err) {
        return !std::holds_alternative<std::monostate>(err);
    }
    inline bool IsError(Error err) {
        return err.IsError();
    }
    inline Error MakeError(Error err) {
        return err;
    }
    template <typename T>
    inline bool IsError(Expected<T> const& exp) {
        return !exp.has_value();
    }
    template <typename T>
    inline bool IsError(ExpectedRef<T> const& exp) {
        return !exp.value.has_value();
    }
    template <typename T>
    inline Error MakeError(Expected<T> const& exp) {
        return exp.error();
    }
    template <typename T>
    inline Error MakeError(ExpectedRef<T> const& exp) {
        return exp.value.error();
    }
    inline nonstd::unexpected_type<Error> MakeUnexpected(Error err) {
        return nonstd::make_unexpected(err);
    }
    template <typename T>
    inline nonstd::unexpected_type<Error> MakeUnexpected(nonstd::expected<T, Error> exp) {
        return nonstd::make_unexpected(std::move(exp.error()));
    }

    template <typename T>
    std::optional<T> Error::Unwrap(Expected<T>&& ex) {
        if (!ex.has_value()) {
            *this = ex.error();
            return {};
        } else {
            return std::move(ex.value());
        }
    }
    template <typename T>
    T* Error::Unwrap(ExpectedRef<T>&& ref) {
        if (!ref.value.has_value()) {
            *this = ref.value.error();
            return {};
        } else {
            return &ref.value.value().get();
        }
    }

    template <typename T>
    struct EntityInput;
    template <typename T>
    struct EntityOutput;

    template <typename T>
    struct SignalSource {
    private:
        std::optional<T> value;

    public:
        friend struct EntityInput<T>;
        friend struct EntityOutput<T>;
    };

    template <typename T>
    struct SignalSink {
    private:
        entity source = null;

    public:
        friend struct EntityInput<T>;
        friend struct EntityOutput<T>;
    };

    template <typename T>
    concept EmptyType = std::is_empty<T>::value;

    template <EmptyType T>
    struct SignalSource<T> {
        bool value;

    public:
        friend struct EntityInput<T>;
        friend struct EntityOutput<T>;
    };

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

    #define OKAMI_ERR_MAKE(errDetails) \
        Error{__LINE__, __FILE__, errDetails}
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
    #define OKAMI_ERR_RETURN_IF(cond, errDetails) \
        { \
            Error err; \
            OKAMI_ERR_IF(cond, err, errDetails); \
            OKAMI_ERR_RETURN(err); \
        }
    #define OKAMI_EXP_RETURN(err) \
        if (IsError(err)) { \
            return MakeUnexpected(err); \
        }
    #define OKAMI_EXP_RETURN_IF(cond, errDetails) \
        { \
            Error err; \
            OKAMI_ERR_IF(cond, err, errDetails); \
            OKAMI_EXP_RETURN(err); \
        }
    #define OKAMI_ERR_UNWRAP(statement, error) \
        error.Unwrap([&]() { return statement; }()); \
        OKAMI_ERR_RETURN(error);
    #define OKAMI_EXP_UNWRAP(statement, error) \
        error.Unwrap([&]() { return statement; }()); \
        OKAMI_EXP_RETURN(error);
    #define OKAMI_ERR_UNWRAP_INTO(dest, statement) \
        { \
            auto result = statement; \
            OKAMI_ERR_RETURN(result); \
            dest = std::move(result); \
        }
    #define OKAMI_EXP_UNWRAP_INTO(dest, statement) \
        { \
            auto result = statement; \
            OKAMI_EXP_RETURN(result); \
            dest = std::move(result); \
        }
    #define OKAMI_ERR_RETURN_IF_FAIL(statement) \
        { \
            auto err = statement; \
            OKAMI_ERR_RETURN(err); \
        }
    #define OKAMI_EXP_RETURN_IF_FAIL(statement) \
        { \
            auto err = statement; \
            OKAMI_EXP_RETURN(err); \
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

    template <typename T>
    inline Error FireSignal(entt::registry& reg, entt::entity e, T signal) {
        auto result = TryGet<SignalSource<T>>(reg, e);
        OKAMI_ERR_RETURN(result);
        result->get().value.emplace(std::move(signal));
        return {};
    }
    
    template <EmptyType T>
    inline Error FireSignal(entt::registry& reg, entt::entity e) {
        auto result = TryGet<SignalSource<T>>(reg, e);
        OKAMI_ERR_RETURN(result);
        result->get().value = true;
        return {};
    }
    
    template <typename T>
    inline bool CheckSignal(entt::registry& reg, entt::entity e) {
        return reg.template get<SignalSource<T>>(e).value;
    }

    class System;

    struct ModuleDesc {
        // TODO: Finish Later
        // std::vector<std::unique_ptr<System>> systems;
        std::string_view name;
    };

    class Module {
    private:
        ModuleDesc _desc;

    public:
        virtual Error Initialize(entt::registry& registry) const = 0;
        virtual Error PreExecute(entt::registry& registry) const = 0;
        virtual Error PostExecute(entt::registry& registry) const = 0;
        virtual Error Destroy(entt::registry& registry) const = 0;

        std::string_view GetName() const;
        ModuleDesc const& GetDesc() const;

        Module(ModuleDesc&& desc) : _desc(std::move(desc)) {}
        Module(Module const&) = delete;
        Module(Module&&) = delete;
        virtual ~Module() = default;
    };

    template <typename T>
    concept ModuleType = std::is_base_of<Module, T>::value;

    struct TypeHash {
        size_t operator()(entt::meta_type const& a) const {
            return a.info().hash();
        }
    };
    template <typename Value>
    using UnorderedTypeMap = std::unordered_map<entt::meta_type, Value, TypeHash>;

    struct EngineDesc {
        std::vector<std::shared_ptr<Module>> modules;
        UnorderedTypeMap<std::shared_ptr<Module>> modulesByType;
    };

    class Engine {
    private:
        struct Impl;

        EngineDesc _desc;

    public:
        Engine();

        template <typename T>
        ExpectedRef<T> Get() {
            auto it = _desc.modulesByType.find(entt::resolve<T>());
            OKAMI_EXP_RETURN_IF(it == _desc.modulesByType.end(), 
                RuntimeError{"Could not find module! Use Add() to create a module!"});
            auto convert = dynamic_cast<T*>(it->second.get());
            OKAMI_EXP_RETURN_IF(convert == nullptr,
                RuntimeError{"Could not dynamic cast module to correct type!"});
            return *convert;
        }

        template <ModuleType T, typename... ParamTs>
        std::shared_ptr<T> Add(ParamTs&&... params) {
            auto modul = std::make_shared<T>(std::forward<ParamTs>(params)...);
            _desc.modules.emplace_back(modul);
            _desc.modulesByType.emplace(entt::resolve<T>(), modul);
            return modul;
        }

        EngineDesc const& GetDesc() const;
        Error Initialize(entt::registry& registry) const;
        Error Destroy(entt::registry& registry) const;
        Error Execute(entt::registry& registry) const;
    };
}