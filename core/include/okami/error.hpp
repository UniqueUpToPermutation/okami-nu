#pragma once

#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <nonstd/expected.hpp>

namespace okami {
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

        inline T* operator->() {
            return &value.value().get();
        }

        inline operator bool() const {
            return value.has_value();
        }

        inline ExpectedRef(std::reference_wrapper<T> ref) : value(ref.get()) {}
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
    inline nonstd::unexpected_type<Error> MakeUnexpected(ExpectedRef<T> ref) {
        return nonstd::make_unexpected(std::move(ref.value.error()));
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
        error.Unwrap([&]() { return statement; }()) \
            .value_or(typename decltype(statement)::value_type{}); \
        OKAMI_ERR_RETURN(error);
    #define OKAMI_EXP_UNWRAP(statement, error) \
        error.Unwrap([&]() { return statement; }()) \
            .value_or(typename decltype(statement)::value_type{}); \
        OKAMI_EXP_RETURN(error);
    #define OKAMI_ERR_UNWRAP_INTO(dest, statement) \
        { \
            auto result = statement; \
            OKAMI_ERR_RETURN(result); \
            dest = std::move(result.value()); \
        }
    #define OKAMI_EXP_UNWRAP_INTO(dest, statement) \
        { \
            auto result = statement; \
            OKAMI_EXP_RETURN(result); \
            dest = std::move(result.value()); \
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

    #define OKAMI_NO_COPY(name) \
        name(name const&) = delete; \
        name& operator=(name const&) = delete
    #define OKAMI_MOVE_ONLY(name) \
        OKAMI_NO_COPY(name); \
        name(name&&) = default; \
        name& operator=(name&&) = default;
}