#pragma once

namespace okami {
    template <typename VertexId, typename VertexData, typename VertexIdHash>
    std::optional<typename Forest<VertexId, VertexData, VertexIdHash>::vertex_t> 
        Forest<VertexId, VertexData, VertexIdHash>::TryGetVertex(VertexId id) {
        auto it = idToIndex.find(id);
        if (it != idToIndex.end() && !(it->second >= vertices.size())) {
            return vertex_t{
                vertices[it->second]
            };
        } else {
            return std::optional<typename Forest<VertexId, VertexData, VertexIdHash>::vertex_t>();
        }
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    std::optional<typename Forest<VertexId, VertexData, VertexIdHash>::vertex_const_t>
        Forest<VertexId, VertexData, VertexIdHash>::TryGetVertex(VertexId id) const {
        auto it = idToIndex.find(id);
        if (it != idToIndex.end() && !(it->second >= vertices.size())) {
            return vertex_const_t{
                vertices[it->second]
            };
        } else {
            return std::optional<typename Forest<VertexId, VertexData, VertexIdHash>::vertex_const_t>();
        }
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    typename Forest<VertexId, VertexData, VertexIdHash>::vertex_const_t
        Forest<VertexId, VertexData, VertexIdHash>::GetVertex(VertexId id) const {
        auto result = TryGetVertex(id);
        if (result.has_value()) {
            return result.value();
        } else {
            throw std::runtime_error("Vertex does not exist!");
        }
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    typename Forest<VertexId, VertexData, VertexIdHash>::vertex_t
        Forest<VertexId, VertexData, VertexIdHash>::GetVertex(VertexId id) {
        auto result = TryGetVertex(id);
        if (result.has_value()) {
            return result.value();
        } else {
            throw std::runtime_error("Vertex does not exist!");
        }
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    void Forest<VertexId, VertexData, VertexIdHash>::FreeVertex(vertex_t vert) {
        forest_idx_t idx = &vert.vertex - &vertices[0];
        forest_idx_t last_idx = vertices.size() - 1;

        auto& vertexToRemove = vertices[idx];
        auto& vertexToSwap = vertices[last_idx];

        auto toSwapId = vertexToSwap.id;
        auto toRemoveId = vertexToRemove.id;

        std::swap(vertexToRemove, vertexToSwap);

        idToIndex[toSwapId] = idx;
        idToIndex.erase(toRemoveId);

        vertices.pop_back();
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    void Forest<VertexId, VertexData, VertexIdHash>::CollectDescendantsPrefix(
        forest_idx_t start, std::queue<forest_idx_t>& queue) const {
        queue.emplace(start);
        auto& vertex = vertices[start];

        for (auto node = vertex.firstChild; 
            node != invalid_forest_vertex; 
            node = vertices[node].next) {
            CollectDescendantsPrefix(node, queue);
        }
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    void Forest<VertexId, VertexData, VertexIdHash>::CollectDescendantsPostfix(
        forest_idx_t start, std::queue<forest_idx_t>& queue) const {
        auto& vertex = vertices[start];

        for (auto node = vertex.firstChild; 
            node != invalid_forest_vertex; 
            node = vertices[node].next) {
            CollectDescendantsPrefix(node, queue);
        }
        queue.emplace(start);
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    typename Forest<VertexId, VertexData, VertexIdHash>::vertex_t 
        Forest<VertexId, VertexData, VertexIdHash>::CreateVertex(VertexId id, VertexData&& data) {
        forest_idx_t idx = vertices.size();

        auto it = idToIndex.find(id);
        if (it != idToIndex.end()) {
            throw std::runtime_error("Vertex already exists!");
        } else {
            idToIndex.emplace_hint(it, id, idx);
        }

        vertices.emplace_back(TreeVertexImpl<VertexId, VertexData>(id, std::move(data)));

        return vertices.back();
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    std::enable_if_t<std::is_default_constructible_v<VertexData>> 
        Forest<VertexId, VertexData, VertexIdHash>::CreateVertex(VertexId id) {
        CreateVertex(id, VertexData());
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    void Forest<VertexId, VertexData, VertexIdHash>::Orphan(vertex_t vert) {
        auto& vertex = vert.vertex;
        auto vertex_idx = &vertex - &vertices[0];

        if (vertex.parent != invalid_forest_vertex) {
            auto& parent = vertices[vertex.parent];

            if (parent.firstChild == vertex_idx) {
                parent.firstChild = vertex.next;
                if (vertex.next != invalid_forest_vertex) {
                    auto& next = vertices[vertex.next];
                    next.prev = invalid_forest_vertex;
                }
            }

            if (parent.lastChild == vertex_idx) {
                parent.lastChild = vertex.prev;
                if (vertex.prev != invalid_forest_vertex) {
                    auto& prev = vertices[vertex.prev];
                    prev.next = invalid_forest_vertex;
                }
            }
        }

        vertex.parent = invalid_forest_vertex;
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    void Forest<VertexId, VertexData, VertexIdHash>::Orphan(VertexId id) {
        Orphan(GetVertex(id));
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    void Forest<VertexId, VertexData, VertexIdHash>::Clear() {
        idToIndex.clear();
        vertices.clear();
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    void Forest<VertexId, VertexData, VertexIdHash>::RemoveVertex(vertex_t vertex) {
        for (auto child : GetChildren(vertex)) {
            Orphan(child);
        }
        auto vec = GetChildren(vertex).ToVector();
        Orphan(vertex);
        FreeVertex(vertex);
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    void Forest<VertexId, VertexData, VertexIdHash>::RemoveVertex(VertexId id) {
        RemoveVertex(GetVertex(id));
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    void Forest<VertexId, VertexData, VertexIdHash>::RemoveVertexAndDescendants(vertex_t vert) {
        Orphan(vert);
        for (auto descendant : GetDescendants(vert)) {
            FreeVertex(descendant);
        }
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    void Forest<VertexId, VertexData, VertexIdHash>::RemoveVertexAndDescendants(VertexId id) {
        RemoveVertexAndDescendants(GetVertex(id));
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    void Forest<VertexId, VertexData, VertexIdHash>::SetParent(vertex_t child, vertex_t parent) {
        auto parent_idx = &parent.vertex - &vertices[0];
        auto child_idx = &child.vertex - &vertices[0];

        auto& parent_vertex = parent.vertex;
        auto& child_vertex = child.vertex;

        if (child_vertex.parent != invalid_forest_vertex) {
            // Make this node into an orphan
            Orphan(child_vertex);
        }

        // Add to the end of linked child list
        if (parent_vertex.lastChild != invalid_forest_vertex) {
            auto& prev_last_vertex = vertices[parent_vertex.lastChild];

            prev_last_vertex.next = child_idx;
            child_vertex.prev = parent_vertex.lastChild;
            parent_vertex.lastChild = child_idx;

        } else {
            parent_vertex.firstChild = child_idx;
            parent_vertex.lastChild = child_idx;
        }

        child_vertex.parent = parent_idx;
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    void Forest<VertexId, VertexData, VertexIdHash>::SetParent(VertexId child, VertexId parent) {
        SetParent(GetVertex(child), GetVertex(parent));
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    typename Forest<VertexId, VertexData, VertexIdHash>::vertex_t 
        Forest<VertexId, VertexData, VertexIdHash>::CreateChild(
        VertexId parent, VertexId child, VertexData&& data) {
        return CreateChild(GetVertex(parent), child, std::move(data)); 
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    std::enable_if_t<std::is_default_constructible_v<VertexData>>
        Forest<VertexId, VertexData, VertexIdHash>::CreateChild(VertexId parent, VertexId child) {
        CreateChild(GetVertex(parent), child);
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    typename Forest<VertexId, VertexData, VertexIdHash>::vertex_t 
        Forest<VertexId, VertexData, VertexIdHash>::CreateChild(
        vertex_t parent, VertexId childId, VertexData&& data) {
        auto child = CreateVertex(childId, std::move(data));
        SetParent(child, parent);
        return child;
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    std::enable_if_t<std::is_default_constructible_v<VertexData>>
        Forest<VertexId, VertexData, VertexIdHash>::CreateChild(vertex_t parent, VertexId childId) {
        CreateChild(parent, childId, VertexData());
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    Collection<typename Forest<VertexId, VertexData, VertexIdHash>::template ChildIterator<true>> 
        Forest<VertexId, VertexData, VertexIdHash>::GetChildren(vertex_const_t vert) const {
        return Collection<ChildIterator<true>>{
            ChildIterator<true>{vert.vertex.firstChild, *this},
            ChildIterator<true>{invalid_forest_vertex, *this}
        };
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    Collection<typename Forest<VertexId, VertexData, VertexIdHash>::template ChildIterator<true>> 
        Forest<VertexId, VertexData, VertexIdHash>::GetChildren(VertexId id) const {
        return GetChildren(GetVertex(id));
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    Collection<typename Forest<VertexId, VertexData, VertexIdHash>::template ChildIterator<false>> 
        Forest<VertexId, VertexData, VertexIdHash>::GetChildren(vertex_t vert) {
        return Collection<ChildIterator<false>>{
            ChildIterator<false>{vert.vertex.firstChild, *this},
            ChildIterator<false>{invalid_forest_vertex, *this}
        };
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    Collection<typename Forest<VertexId, VertexData, VertexIdHash>::template ChildIterator<false>> 
        Forest<VertexId, VertexData, VertexIdHash>::GetChildren(VertexId id) {
        return GetChildren(GetVertex(id));
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    Collection<typename Forest<VertexId, VertexData, VertexIdHash>::template DescendantIterator<true>> 
        Forest<VertexId, VertexData, VertexIdHash>::GetDescendants(vertex_const_t vertex) const {
        std::queue<forest_idx_t> queue;
        queue.emplace(&vertex.vertex - &vertices[0]);
        return Collection<DescendantIterator<true>> {
            DescendantIterator<true>{*this, std::move(queue)},
            DescendantIterator<true>{*this},
        };
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    Collection<typename Forest<VertexId, VertexData, VertexIdHash>::template DescendantIterator<true>> 
        Forest<VertexId, VertexData, VertexIdHash>::GetDescendants(VertexId id) const {
        return GetDescendants(GetVertex(id));
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    Collection<typename Forest<VertexId, VertexData, VertexIdHash>::template DescendantIterator<false>> 
        Forest<VertexId, VertexData, VertexIdHash>::GetDescendants(vertex_t vertex) {
        std::queue<forest_idx_t> queue;
        queue.emplace(&vertex.vertex - &vertices[0]);
        return Collection<DescendantIterator<false>> {
            DescendantIterator<false>{*this, std::move(queue)},
            DescendantIterator<false>{*this},
        };
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    Collection<typename Forest<VertexId, VertexData, VertexIdHash>::template DescendantIterator<false>> 
        Forest<VertexId, VertexData, VertexIdHash>::GetDescendants(VertexId id) {
        return GetDescendants(GetVertex(id));
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    Collection<typename Forest<VertexId, VertexData, VertexIdHash>::template AncestorIterator<true>> 
        Forest<VertexId, VertexData, VertexIdHash>::GetAncestors(vertex_const_t vert) const {
        return Collection<AncestorIterator<true>>{
            ChildIterator<true>{&vert.vertex - &vertices[0], *this},
            ChildIterator<true>{invalid_forest_vertex, *this}
        };
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    Collection<typename Forest<VertexId, VertexData, VertexIdHash>::template AncestorIterator<true>> 
        Forest<VertexId, VertexData, VertexIdHash>::GetAncestors(VertexId id) const {
        return GetAncestors(GetVertex(id));
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    Collection<typename Forest<VertexId, VertexData, VertexIdHash>::template AncestorIterator<false>> 
        Forest<VertexId, VertexData, VertexIdHash>::GetAncestors(vertex_t vert) {
        return Collection<AncestorIterator<false>>{
            AncestorIterator<false>{&vert.vertex - &vertices[0], *this},
            AncestorIterator<false>{invalid_forest_vertex, *this}
        };
    }

     template <typename VertexId, typename VertexData, typename VertexIdHash>
    Collection<typename Forest<VertexId, VertexData, VertexIdHash>::template AncestorIterator<false>> 
        Forest<VertexId, VertexData, VertexIdHash>::GetAncestors(VertexId id) {
        return GetAncestors(GetVertex(id));
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    typename Forest<VertexId, VertexData, VertexIdHash>::template VertexIterator<false> 
        Forest<VertexId, VertexData, VertexIdHash>::begin() {
        return VertexIterator<false>(0, *this);
    }
    template <typename VertexId, typename VertexData, typename VertexIdHash>
    typename Forest<VertexId, VertexData, VertexIdHash>::template VertexIterator<false> 
        Forest<VertexId, VertexData, VertexIdHash>::end() {
        return VertexIterator<false>(vertices.size(), *this);
    }
    template <typename VertexId, typename VertexData, typename VertexIdHash>
    typename Forest<VertexId, VertexData, VertexIdHash>::template VertexIterator<true> 
        Forest<VertexId, VertexData, VertexIdHash>::begin() const {
        return VertexIterator<true>(0, *this);
    }
    template <typename VertexId, typename VertexData, typename VertexIdHash>
    typename Forest<VertexId, VertexData, VertexIdHash>::template VertexIterator<true> 
        Forest<VertexId, VertexData, VertexIdHash>::end() const {
        return VertexIterator<true>(vertices.size(), *this);
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    Collection<typename Forest<VertexId, VertexData, VertexIdHash>::template VertexIterator<false>> 
        Forest<VertexId, VertexData, VertexIdHash>::GetVertices() {
        return Collection<typename Forest<VertexId, VertexData, VertexIdHash>::template VertexIterator<false>>{
            begin(),
            end()
        };
    }
    template <typename VertexId, typename VertexData, typename VertexIdHash>
    Collection<typename Forest<VertexId, VertexData, VertexIdHash>::template VertexIterator<true>> 
        Forest<VertexId, VertexData, VertexIdHash>::GetVertices() const {
        return Collection<typename Forest<VertexId, VertexData, VertexIdHash>::template VertexIterator<true>>{
            begin(),
            end()
        };
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    std::optional<typename Forest<VertexId, VertexData, VertexIdHash>::vertex_t> 
        Forest<VertexId, VertexData, VertexIdHash>::GetParent(vertex_t vert) {
        auto idx = vert.vertex.parent;
        if (idx == invalid_forest_vertex)
            return std::optional<vertex_t>();
        else
            return std::optional<vertex_t>(vertices[idx]);
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    std::optional<typename Forest<VertexId, VertexData, VertexIdHash>::vertex_const_t> 
        Forest<VertexId, VertexData, VertexIdHash>::GetParent(vertex_const_t vert) const {
        auto idx = vert.vertex.parent;
        if (idx == invalid_forest_vertex)
            return std::optional<vertex_const_t>();
        else
            return std::optional<vertex_const_t>(vertices[idx]);
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    std::optional<VertexId> Forest<VertexId, VertexData, VertexIdHash>::GetParent(VertexId id) const {
        auto result = GetParent(GetVertex(id));
        if (result.has_value()) {
            return std::optional<VertexId>(result.value().Id());
        } else {
            return std::optional<VertexId>();
        }
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    typename Forest<VertexId, VertexData, VertexIdHash>::vertex_t 
        Forest<VertexId, VertexData, VertexIdHash>::GetRoot(VertexId id) {
        return GetAncestors(id).Last().value();
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    typename Forest<VertexId, VertexData, VertexIdHash>::vertex_const_t
        Forest<VertexId, VertexData, VertexIdHash>::GetRoot(VertexId id) const {
        return GetAncestors(id).Last().value();
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    typename Forest<VertexId, VertexData, VertexIdHash>::vertex_t 
        Forest<VertexId, VertexData, VertexIdHash>::GetRoot(vertex_t v) {
        return GetAncestors(v).Last().value();
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    typename Forest<VertexId, VertexData, VertexIdHash>::vertex_const_t
        Forest<VertexId, VertexData, VertexIdHash>::GetRoot(vertex_const_t v) const {
        return GetAncestors(v).Last().value();
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    std::optional<typename Forest<VertexId, VertexData, VertexIdHash>::vertex_t> 
        Forest<VertexId, VertexData, VertexIdHash>::GetNextSibling(vertex_t vert) {
        auto idx = vert.vertex.next;
        if (idx == invalid_forest_vertex)
            return std::optional<vertex_t>();
        else
            return std::optional<vertex_t>(vertices[idx]);
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    std::optional<typename Forest<VertexId, VertexData, VertexIdHash>::vertex_const_t> 
        Forest<VertexId, VertexData, VertexIdHash>::GetNextSibling(vertex_const_t vert) const {
        auto idx = vert.vertex.next;
        if (idx == invalid_forest_vertex)
            return std::optional<vertex_const_t>();
        else
            return std::optional<vertex_const_t>(vertices[idx]);
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    std::optional<VertexId> 
        Forest<VertexId, VertexData, VertexIdHash>::GetNextSibling(VertexId id) const {
        auto result = GetNextSibling(GetVertex(id));
        if (result.has_value()) {
            return std::optional<VertexId>(result.value().Id());
        } else {
            return std::optional<VertexId>();
        }
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    std::optional<typename Forest<VertexId, VertexData, VertexIdHash>::vertex_t> 
        Forest<VertexId, VertexData, VertexIdHash>::GetPreviousSibling(vertex_t vert) {
        auto idx = vert.vertex.next;
        if (idx == invalid_forest_vertex)
            return std::optional<vertex_t>();
        else
            return std::optional<vertex_t>(vertices[idx]);
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    std::optional<typename Forest<VertexId, VertexData, VertexIdHash>::vertex_const_t> 
        Forest<VertexId, VertexData, VertexIdHash>::GetPreviousSibling(vertex_const_t vert) const {
        auto idx = vert.vertex.next;
        if (idx == invalid_forest_vertex)
            return std::optional<vertex_const_t>();
        else
            return std::optional<vertex_const_t>(vertices[idx]);
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    std::optional<VertexId> Forest<VertexId, VertexData, VertexIdHash>::GetPreviousSibling(VertexId id) const {
        auto result = GetPreviousSibling(GetVertex(id));
        if (result.has_value()) {
            return std::optional<VertexId>(result.value().Id());
        } else {
            return std::optional<VertexId>();
        }
    }
}