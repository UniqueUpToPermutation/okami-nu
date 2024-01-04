#pragma once

#include <entt/entt.hpp>

#include <vector>
#include <unordered_map>

#include <okami/error.hpp>

namespace okami {
    constexpr int kMajorVersion = 0;
    constexpr int kMinorVersion = 0;

    using entity = entt::entity;
    constexpr auto null = entt::null;

    template <class T>
    concept DefaultInitializable = requires { T{}; };

    template <typename T>
    ExpectedRef<T> TryGet(entt::registry& reg, entity e) {
        Error err;
        auto result = reg.template try_get<T>(e);
        OKAMI_ERR_IF(!result, err, RuntimeError{"Could not find component!"});
        OKAMI_EXP_RETURN(err);
        return *result;
    }

    template <typename T>
    ExpectedRef<T> TryGet(entt::registry const& reg, entity e) {
        static_assert(std::is_const_v<T>, "Type is not const, but registry is!");
        Error err;
        auto result = reg.template try_get<T>(e);
        OKAMI_ERR_IF(!result, err, RuntimeError{"Could not find component!"});
        OKAMI_EXP_RETURN(err);
        return std::ref(*result);
    }

    template <typename T>
    struct EntityInput;
    template <typename T>
    struct EntityOutput;

    template <typename T>
    struct RemoteProperty {
        entity source = null;

        ExpectedRef<T const> Get(entt::registry const& reg) const {
            OKAMI_EXP_RETURN_IF(source == null, 
                RuntimeError{"Source entity is not set!"});
            return TryGet<T const>(reg, source);
        }

        Error Link(entt::registry& reg, entity source) {
            this->source = source;
            OKAMI_ERR_RETURN_IF(!reg.all_of<T>(source),
                RuntimeError{"Source entity does not have component!"});
            return {};
        }
    };

    template <typename T>
    struct SignalSource {
        std::optional<T> value;
    };

    template <typename T>
    struct SignalSink {
        entity source = null;

        ExpectedRef<T const> Get(entt::registry const& reg) const {
            OKAMI_EXP_RETURN_IF(source == null, 
                RuntimeError{"Source entity is not set!"});
            return TryGet<T const>(reg, source);
        }

        Error Link(entt::registry& reg, entity source) {
            OKAMI_ERR_RETURN_IF(!reg.all_of<T>(source),
                RuntimeError{"Source entity does not have component!"});
            this->source = source;
            return {};
        }
    };

    template <typename T>
    concept EmptyType = std::is_empty<T>::value;

    template <EmptyType T>
    struct SignalSource<T> {
        bool value;
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
        result->value = true;
        return {};
    }
    
    template <typename T>
    inline bool CheckSignal(entt::registry const& reg, entt::entity e) {
        return reg.template get<SignalSource<T> const>(e).value;
    }

    template <typename T>
    inline Expected<T> GetProperty(entt::registry const& reg, entt::entity e) {
        auto result = TryGet<T const>(reg, e);
        OKAMI_EXP_RETURN(result);
        return result.value->get();
    }

    template <typename T>
    inline Expected<T> GetRemoteProperty(entt::registry const& reg, entt::entity e) {
        auto remoteProp = TryGet<RemoteProperty<T> const>(reg, e);
        OKAMI_EXP_RETURN(remoteProp);
        auto value = remoteProp->Get(reg);
        OKAMI_EXP_RETURN(value);
        return value.value->get();
    }

    // For visitor pattern matching
    template<class... Ts>
    struct Matcher : Ts... { using Ts::operator()...; };
    template<class... Ts>
    Matcher(Ts...) -> Matcher<Ts...>;
    template <typename T, class... Ts>
    auto MatchReturn(T&& variant, Ts&&... cases) {
        return std::visit(Matcher{
            std::forward<Ts>(cases)...
        }, std::forward<T>(variant));
    }
    template <typename T, class... Ts>
    auto Match(T&& variant, Ts&&... cases) {
        std::visit(Matcher{
            std::forward<Ts>(cases)...
        }, std::forward<T>(variant));
    }

    class System;

    struct ModuleDesc {
        // TODO: Finish Later
        // std::vector<std::unique_ptr<System>> systems;
        std::string_view name;
    };

    struct BufferData {
        std::vector<uint8_t> bytes;

        inline size_t size() const { return bytes.size(); }
        inline uint8_t* data() { return bytes.data(); }
        inline uint8_t const* data() const { return bytes.data(); }
        inline void const* vdata() const { return bytes.data(); }
        inline void* vdata() { return bytes.data(); }
        inline auto begin() { return bytes.begin(); }
        inline auto end() { return bytes.end(); }
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