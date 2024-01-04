#pragma once

#include <plog/Log.h>

#include <okami/error.hpp>

namespace okami::log {
    void Init();
}

namespace okami {
    void Log(Error const& err);

    template <typename T>
    void Log(Expected<T> const& exp) {
        if (!exp.has_value()) {
            return Log(exp.error());
        }
    }
}