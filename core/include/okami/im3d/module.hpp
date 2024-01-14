#pragma once

#include <okami/okami.hpp>

namespace okami {
    class Im3dModule : public Module {
    public:
        Im3dModule();

        Error Initialize(Registry& registry) const override;
        Error PreExecute(Registry& registry) const override;
        Error PostExecute(Registry& registry) const override;
        Error Destroy(Registry& registry) const override;
    };
}