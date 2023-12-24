#pragma once

namespace okami {
    template <typename VertexId, typename VertexData, typename VertexIdHash>
    std::optional<typename Forest<VertexId, VertexData, VertexIdHash>::vertex_t> 
        Forest<VertexId, VertexData, VertexIdHash>::TryGetVertex(VertexId id) {
        auto it = mIdToIndex.find(id);
        if (it != mIdToIndex.end() && !(it->second >= mVertices.size())) {
            return vertex_t{
                mVertices[it->second]
            };
        } else {
            return std::optional<typename Forest<VertexId, VertexData, VertexIdHash>::vertex_t>();
        }
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    std::optional<typename Forest<VertexId, VertexData, VertexIdHash>::vertex_const_t>
        Forest<VertexId, VertexData, VertexIdHash>::TryGetVertex(VertexId id) const {
        auto it = mIdToIndex.find(id);
        if (it != mIdToIndex.end() && !(it->second >= mVertices.size())) {
            return vertex_const_t{
                mVertices[it->second]
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
        forest_idx_t idx = &vert.mVertex - &mVertices[0];
        forest_idx_t last_idx = mVertices.size() - 1;

        auto& vertexToRemove = mVertices[idx];
        auto& vertexToSwap = mVertices[last_idx];

        auto toSwapId = vertexToSwap.mId;
        auto toRemoveId = vertexToRemove.mId;

        std::swap(vertexToRemove, vertexToSwap);

        mIdToIndex[toSwapId] = idx;
        mIdToIndex.erase(toRemoveId);

        mVertices.pop_back();
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    void Forest<VertexId, VertexData, VertexIdHash>::CollectDescendantsPrefix(
        forest_idx_t start, std::queue<forest_idx_t>& queue) const {
        queue.emplace(start);
        auto& vertex = mVertices[start];

        for (auto node = vertex.mFirstChild; 
            node != invalid_forest_vertex; 
            node = mVertices[node].mNext) {
            CollectDescendantsPrefix(node, queue);
        }
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    void Forest<VertexId, VertexData, VertexIdHash>::CollectDescendantsPostfix(
        forest_idx_t start, std::queue<forest_idx_t>& queue) const {
        auto& vertex = mVertices[start];

        for (auto node = vertex.mFirstChild; 
            node != invalid_forest_vertex; 
            node = mVertices[node].mNext) {
            CollectDescendantsPrefix(node, queue);
        }
        queue.emplace(start);
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    typename Forest<VertexId, VertexData, VertexIdHash>::vertex_t 
        Forest<VertexId, VertexData, VertexIdHash>::CreateVertex(VertexId id, VertexData&& data) {
        forest_idx_t idx = mVertices.size();

        auto it = mIdToIndex.find(id);
        if (it != mIdToIndex.end()) {
            throw std::runtime_error("Vertex already exists!");
        } else {
            mIdToIndex.emplace_hint(it, id, idx);
        }

        mVertices.emplace_back(TreeVertexImpl<VertexId, VertexData>(id, std::move(data)));

        return mVertices.back();
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    std::enable_if_t<std::is_default_constructible_v<VertexData>> 
        Forest<VertexId, VertexData, VertexIdHash>::CreateVertex(VertexId id) {
        CreateVertex(id, VertexData());
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    void Forest<VertexId, VertexData, VertexIdHash>::Orphan(vertex_t vert) {
        auto& vertex = vert.mVertex;
        auto vertex_idx = &vertex - &mVertices[0];

        if (vertex.mParent != invalid_forest_vertex) {
            auto& parent = mVertices[vertex.mParent];

            if (parent.mFirstChild == vertex_idx) {
                parent.mFirstChild = vertex.mNext;
                if (vertex.mNext != invalid_forest_vertex) {
                    auto& next = mVertices[vertex.mNext];
                    next.mPrev = invalid_forest_vertex;
                }
            }

            if (parent.mLastChild == vertex_idx) {
                parent.mLastChild = vertex.mPrev;
                if (vertex.mPrev != invalid_forest_vertex) {
                    auto& prev = mVertices[vertex.mPrev];
                    prev.mNext = invalid_forest_vertex;
                }
            }
        }

        vertex.mParent = invalid_forest_vertex;
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    void Forest<VertexId, VertexData, VertexIdHash>::Orphan(VertexId id) {
        Orphan(GetVertex(id));
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    void Forest<VertexId, VertexData, VertexIdHash>::Clear() {
        mIdToIndex.clear();
        mVertices.clear();
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
        auto parent_idx = &parent.mVertex - &mVertices[0];
        auto child_idx = &child.mVertex - &mVertices[0];

        auto& parent_vertex = parent.mVertex;
        auto& child_vertex = child.mVertex;

        if (child_vertex.mParent != invalid_forest_vertex) {
            // Make this node into an orphan
            Orphan(child_vertex);
        }

        // Add to the end of linked child list
        if (parent_vertex.mLastChild != invalid_forest_vertex) {
            auto& prev_last_vertex = mVertices[parent_vertex.mLastChild];

            prev_last_vertex.mNext = child_idx;
            child_vertex.mPrev = parent_vertex.mLastChild;
            parent_vertex.mLastChild = child_idx;

        } else {
            parent_vertex.mFirstChild = child_idx;
            parent_vertex.mLastChild = child_idx;
        }

        child_vertex.mParent = parent_idx;
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
            ChildIterator<true>{vert.mVertex.mFirstChild, *this},
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
            ChildIterator<false>{vert.mVertex.mFirstChild, *this},
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
        queue.emplace(&vertex.mVertex - &mVertices[0]);
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
        queue.emplace(&vertex.mVertex - &mVertices[0]);
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
            ChildIterator<true>{&vert.mVertex - &mVertices[0], *this},
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
            AncestorIterator<false>{&vert.mVertex - &mVertices[0], *this},
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
        return VertexIterator<false>(mVertices.size(), *this);
    }
    template <typename VertexId, typename VertexData, typename VertexIdHash>
    typename Forest<VertexId, VertexData, VertexIdHash>::template VertexIterator<true> 
        Forest<VertexId, VertexData, VertexIdHash>::begin() const {
        return VertexIterator<true>(0, *this);
    }
    template <typename VertexId, typename VertexData, typename VertexIdHash>
    typename Forest<VertexId, VertexData, VertexIdHash>::template VertexIterator<true> 
        Forest<VertexId, VertexData, VertexIdHash>::end() const {
        return VertexIterator<true>(mVertices.size(), *this);
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
        auto idx = vert.mVertex.mParent;
        if (idx == invalid_forest_vertex)
            return std::optional<vertex_t>();
        else
            return std::optional<vertex_t>(mVertices[idx]);
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    std::optional<typename Forest<VertexId, VertexData, VertexIdHash>::vertex_const_t> 
        Forest<VertexId, VertexData, VertexIdHash>::GetParent(vertex_const_t vert) const {
        auto idx = vert.mVertex.mParent;
        if (idx == invalid_forest_vertex)
            return std::optional<vertex_const_t>();
        else
            return std::optional<vertex_const_t>(mVertices[idx]);
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
        auto idx = vert.mVertex.mNext;
        if (idx == invalid_forest_vertex)
            return std::optional<vertex_t>();
        else
            return std::optional<vertex_t>(mVertices[idx]);
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    std::optional<typename Forest<VertexId, VertexData, VertexIdHash>::vertex_const_t> 
        Forest<VertexId, VertexData, VertexIdHash>::GetNextSibling(vertex_const_t vert) const {
        auto idx = vert.mVertex.mNext;
        if (idx == invalid_forest_vertex)
            return std::optional<vertex_const_t>();
        else
            return std::optional<vertex_const_t>(mVertices[idx]);
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
        auto idx = vert.mVertex.mNext;
        if (idx == invalid_forest_vertex)
            return std::optional<vertex_t>();
        else
            return std::optional<vertex_t>(mVertices[idx]);
    }

    template <typename VertexId, typename VertexData, typename VertexIdHash>
    std::optional<typename Forest<VertexId, VertexData, VertexIdHash>::vertex_const_t> 
        Forest<VertexId, VertexData, VertexIdHash>::GetPreviousSibling(vertex_const_t vert) const {
        auto idx = vert.mVertex.mNext;
        if (idx == invalid_forest_vertex)
            return std::optional<vertex_const_t>();
        else
            return std::optional<vertex_const_t>(mVertices[idx]);
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