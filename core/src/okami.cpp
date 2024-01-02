#include <okami/okami.hpp>

#include <okami/glfw/module.hpp>
#include <okami/ogl/module.hpp>

using namespace okami;

std::ostream& okami::operator<<(std::ostream& os, const GlfwError& err) {
    os << "GLFW Error (Code: " << err.code << "): " << err.description;
    return os;
}

std::ostream& okami::operator<<(std::ostream& os, const RuntimeError& err) {
    os << "Runtime Error: " << err.description;
    return os;
}

std::ostream& okami::operator<<(std::ostream& os, const MultipleErrors& errs) {
    os << "Multiple Errors: \n";
    for (auto& err : errs.errors) {
        os << err.ToString() << "\n";
    }
    return os;
}

std::ostream& okami::operator<<(std::ostream& os, const InvalidPathError& err) {
    os << "Invalid Path: " << err.path;
    return os;
}

std::ostream& okami::operator<<(std::ostream& os, const Error& err) {
    std::visit([&err, &os](auto const& obj) {
        if constexpr (std::is_same_v<std::decay_t<decltype(obj)>, std::monostate>) {
            os << "No error";
        } else {
            if constexpr (!std::is_same_v<std::decay_t<decltype(obj)>, MultipleErrors>) {
                os << err.file << ":" << err.line << ": ";
            } else {
                os << obj;
            }
        }
    }, err.details);
    return os;
}

Error okami::operator|(Error const& err1, Error const& err2) {
    Error result = err1;
    result |= err2;
    return result;
}

Error okami::operator+(Error const& err1, Error const& err2) {
    Error result = err1;
    result += err2;
    return result;
}

Error& okami::Error::operator|=(Error const& other) {
    if (!IsError() && other.IsError()) {
        *this = other;
    }
    return *this;
}

Error& okami::Error::operator+=(Error const& other) {
    if (IsError() && other.IsError()) {
        auto thisMulti = std::get_if<MultipleErrors>(&details);
        auto otherMulti = std::get_if<MultipleErrors>(&other.details);
        if (thisMulti && otherMulti) {
            std::copy(otherMulti->errors.begin(), otherMulti->errors.end(), 
                std::back_inserter(thisMulti->errors));
        } else if (thisMulti) {
            thisMulti->errors.emplace_back(other);
        } else if (otherMulti) {
            MultipleErrors merge{.errors = {*this}};
            std::copy(otherMulti->errors.begin(), otherMulti->errors.end(), 
                std::back_inserter(merge.errors));
            *this = Error{.details = std::move(merge)};
        } else {
            MultipleErrors merge{.errors = {*this, other}};
            *this = Error{.details = std::move(merge)};
        }
    } else if (!IsError() && other.IsError()) {
        *this = other;
    }

    return *this;
}

bool okami::Error::IsOk() const {
    return !IsError();
}

bool okami::Error::IsError() const {
    return ::IsError(details);
}

std::string okami::Error::ToString() const {
    return std::visit([this](auto const& obj) -> std::string {
        if constexpr (std::is_same_v<std::decay_t<decltype(obj)>, std::monostate>) {
            return std::string("No error");
        } else {
            std::stringstream ss;
            ss << file << ":" << line << ": ";
            ss << obj;
            return ss.str();
        }
    }, details);
}

std::string_view okami::Module::GetName() const {
    return GetDesc().name;
}

ModuleDesc const& okami::Module::GetDesc() const {
    return _desc;
}

EngineDesc const& okami::Engine::GetDesc() const {
    return _desc;
}

struct okami::Engine::Impl {
    GlfwModule* glfw = nullptr;
    GLRendererModule* renderer = nullptr;
};

okami::Engine::Engine() {
    auto glfw = Add<GlfwModule>();
    Add<GLRendererModule>(*glfw);
}

Error okami::Engine::Initialize(entt::registry& registry) const {
    Error err;
    for (auto const& module : _desc.modules) {
        err += module->Initialize(registry);
    }
    return err;
}

Error okami::Engine::Destroy(entt::registry& registry) const {
    Error err;
    for (auto it = _desc.modules.rbegin(); it != _desc.modules.rend(); ++it) {
        err += (*it)->Destroy(registry);
    }
    return err;
}

Error okami::Engine::Execute(entt::registry& registry) const {   
    for (auto const& module : _desc.modules) {
        OKAMI_ERR_RETURN_IF_FAIL(module->PreExecute(registry));
    }

    for (auto const& module : _desc.modules) {
        OKAMI_ERR_RETURN_IF_FAIL(module->PostExecute(registry));
    }

    return {};
}