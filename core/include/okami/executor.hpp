#pragma once

#include <okami/okami.hpp>
#include <okami/graph.hpp>

namespace okami {
    enum class BarrierPhase {
        LoadToWrite,
        WriteToPipe,
        PipeToRead
    };
    
    struct BarrierNode {
        entt::meta_type type;
        BarrierPhase phase;
    };

    struct ExecutorNodeData {
        std::string name;
    };
}

template<>
struct std::hash<okami::BarrierNode> {
    std::size_t operator()(const okami::BarrierNode& s) const noexcept {
        return static_cast<size_t>(s.type.info().hash()) ^ 
            std::hash<int>{}(static_cast<int>(s.phase));
    }
};

namespace okami {
    using ExecutorKey = std::variant<System*, BarrierNode>;

    // TODO: Implement this eventually
    Digraph<ExecutorKey, ExecutorNodeData> CreateExecutionGraph(Engine& en);
}