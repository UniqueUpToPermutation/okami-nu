#pragma once

#include <okami/okami.hpp>
#include <okami/graph.hpp>

namespace okami {
    struct IBindable {
        virtual Error Bind(entt::registry&) = 0;
    };

    struct SystemInterfaceDesc {
        std::vector<entt::meta_type> owning;
        std::vector<entt::meta_type> outputs;
        std::vector<entt::meta_type> inputs;
        std::vector<entt::meta_type> pipes;
        std::vector<entt::meta_type> loads;

        IBindable* bindable = nullptr;

        Error Bind(entt::registry& registry) {
            OKAMI_ERR_RETURN_IF(!bindable, 
                RuntimeError{"System interface has no binding interface!"});
            bindable->Bind(registry);
            return {};
        }
    };

    struct SystemDesc {
        std::vector<SystemInterfaceDesc> interfaces;
        std::string_view name;

        Error Bind(entt::registry& reg) {
            Error err;
            for (auto& interface : interfaces) {
                err |= interface.Bind(reg);
            }
            return err;
        }
    };

    class System {
    private:
        SystemDesc _desc;

    public:
        virtual Error Execute() = 0;
        
        Error BindAndExecute(entt::registry& reg) {
            _desc.Bind(reg);
            return Execute();
        }
        SystemDesc const& GetDesc() const {
            return _desc;
        }

        System(SystemDesc&& desc) : _desc(std::move(desc)) {}
        System(System const&) = delete;
        System(System&&) = delete;
        virtual ~System() = default;
    };

    template <DefaultInitializable T>
    struct Input final : public IBindable {
        T const* value = nullptr;

        inline T const& operator*() { 
            return *value;
        }
        inline T const& operator->() {
            return *value;
        }

        Error Bind(entt::registry& reg) override {
            auto memory = reg.ctx().find(entt::type_id<T>());
            if (!memory) {
                memory = &reg.template emplace<T>();
            }
            value = memory;
            return {};
        }

        SystemInterfaceDesc GetInterface() {
            return SystemInterfaceDesc{
                .owning = {},
                .outputs = {},
                .inputs = { entt::resolve<T>() },
                .pipes = {},
                .loads = {},
                .bindable = this
            };
        }
    };

    template <DefaultInitializable T>
    struct OutputBase : public IBindable {
        T* value = nullptr;

        inline T& operator*() {
            return *value;
        }
        inline T& operator->() {
            return *value;
        }

        Error Bind(entt::registry& reg) override {
            auto memory = reg.ctx().find(entt::type_id<T>());
            if (!memory) {
                memory = &reg.template emplace<T>();
            }
            value = memory;
            return {};
        }
    };

    template <DefaultInitializable T>
    struct Output final : public OutputBase<T> {
        SystemInterfaceDesc GetInterface() {
            return SystemInterfaceDesc{
                .owning = {},
                .outputs = { entt::resolve<T>() },
                .inputs = {},
                .pipes = {},
                .loads = {},
                .bindable = this
            };
        }
    };
    template <DefaultInitializable T>
    struct Pipe final : public OutputBase<T> {
        SystemInterfaceDesc GetInterface() {
            return SystemInterfaceDesc{
                .owning = {},
                .outputs = {},
                .inputs = {},
                .pipes = { entt::resolve<T>() },
                .loads = {},
                .bindable = this
            };
        }
    };

    template <typename T>
    struct EntityWrite final : public IBindable {
    private:
        entt::registry* _reg = nullptr;

    public:
        using view_t = decltype(_reg->template view<T>());

        inline Expected<std::reference_wrapper<T>> TryGet(entity e) {
            return TryGet<T>(*_reg, e);
        }
        inline T& Get(entity e) {
            return _reg->template get<T>(e);
        }
        inline view_t AsView() {
            return _reg->template view<T>();
        }

        inline Collection<typename view_t::iterator> AsCollection() {
            auto view = AsView();
            return Collection{
                view.begin(),
                view.end()
            };
        }

        Error Bind(entt::registry& reg) override {
            _reg = &reg;
            return {};
        }

        SystemInterfaceDesc GetInterface() {
            return SystemInterfaceDesc{
                .owning = {},
                .outputs = {},
                .inputs = {},
                .pipes = {entt::resolve<T>()},
                .loads = {},
                .bindable = this
            };
        }
    };

    template <typename T>
    struct EntityReadBase : public IBindable {
    private:
        entt::registry* _reg = nullptr;

    public:
        using view_t = entt::basic_view<entt::get_t<T const>, entt::exclude_t<>>;

        inline Expected<std::reference_wrapper<T const>> TryGet(entity e) {
            return TryGet<T const>(*_reg, e);
        }
        inline T const& Get(entity e) {
            return _reg->template get<T const>(e);
        }
        inline view_t AsView() {
            return _reg->template view<T>();
        }

        inline Collection<typename view_t::iterator> AsCollection() {
            auto view = AsView();
            return Collection{
                view.begin(),
                view.end()
            };
        }

        Error Bind(entt::registry& reg) override {
            _reg = &reg;
            return {};
        }
    };

    template <typename T>
    struct EntityRead final : public EntityReadBase<T> {
    public:
        SystemInterfaceDesc GetInterface() {
            return SystemInterfaceDesc{
                .owning = {},
                .outputs = {},
                .inputs = {entt::resolve<T>()},
                .pipes = {},
                .loads = {},
                .bindable = this
            };
        }
    };
    template <typename T>
    struct EntityLoad final : public EntityReadBase<T> {
    public:
        SystemInterfaceDesc GetInterface() {
            return SystemInterfaceDesc{
                .owning = {},
                .outputs = {},
                .inputs = {},
                .pipes = {},
                .loads = {entt::resolve<T>()},
                .bindable = this
            };
        }
    };

    template <typename T>
    struct EntityInput : public IBindable {
    private:
        entt::registry* _reg = nullptr;

    public:

        Expected<std::reference_wrapper<T const>> Read(entity e) {
            auto targetEntity = TryGet<SignalSink<T> const>(*_reg, e);
            OKAMI_ERR_RETURN(targetEntity);
            OKAMI_ERR_RETURN_IF(targetEntity->source == null, 
                RuntimeError{"Read failed because signal sink is not linked to a source!"});

            auto result = TryGet<SignalSource<T> const>(*_reg, targetEntity->source);
            OKAMI_ERR_RETURN(result);

            return *result->value;
        }

        Error Bind(entt::registry& reg) override {
            _reg = &reg;
            return {};
        }

        SystemInterfaceDesc GetInterface() {
            return SystemInterfaceDesc{
                .owning = {},
                .outputs = { },
                .inputs = { entt::resolve<T>() },
                .pipes = {},
                .loads = {},
                .bindable = this
            };
        }
    };

    template <typename T>
    struct EntityOutputBase : public IBindable {
    protected:
        entt::registry* _reg = nullptr;

    public:
        Error Bind(entt::registry& reg) final override {
            _reg = &reg;
            return {};
        }

        SystemInterfaceDesc GetInterface() {
            return SystemInterfaceDesc{
                .owning = {},
                .outputs = { entt::resolve<T>() },
                .inputs = {},
                .pipes = {},
                .loads = {},
                .bindable = this
            };
        }
    };

    template <typename T>
    struct EntityOutput final : public EntityOutputBase<T> {
        Error Send(entity e, T signal) {
            return FireSignal(*this->_reg, e, std::move(signal));
        }
    };

    template <EmptyType T>
    struct EntityOutput<T> final : public EntityOutputBase<T> {
        Error Send(entity e) {
            return FireSignal<T>(*this->_reg, e);
        }
    };


    template <typename T>
    struct EntityPipe : public IBindable {
    private:
        entt::registry* _reg = nullptr;

    public:
        Expected<std::reference_wrapper<T>> Get(entity e) {
            auto result = TryGet<SignalSource<T> const>(*_reg, e);
            OKAMI_EXP_RETURN(result);
            OKAMI_ERR_RETURN_IF(!result->value,
                RuntimeError{"No signal has been sent into pipe!"});
            return *result->value;
        }

        Error Bind(entt::registry& reg) override {
            _reg = &reg;
            return {};
        }

        SystemInterfaceDesc GetInterface() {
            return SystemInterfaceDesc{
                .owning = {},
                .outputs = {},
                .inputs = {},
                .pipes = { entt::resolve<T>() },
                .loads = {},
                .bindable = this
            };
        }
    };
}