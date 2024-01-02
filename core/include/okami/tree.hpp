#pragma once

#include <okami/collection.hpp>

#include <stdint.h>
#include <queue>
#include <unordered_map>
#include <optional>
#include <stdexcept>

namespace okami {

    template <typename VertexId, typename VertexData, typename VertexIdHasher>
    class Forest;

    using forest_idx_t = int64_t;

    constexpr forest_idx_t invalid_forest_vertex = -1;
    
    struct NoTreeData {};

    template <typename VertexId, typename VertexData>
    struct TreeVertexImpl {
        VertexId id;
        VertexData data;

        forest_idx_t parent = invalid_forest_vertex;

        forest_idx_t next = invalid_forest_vertex;
        forest_idx_t prev = invalid_forest_vertex;

        forest_idx_t firstChild = invalid_forest_vertex;
        forest_idx_t lastChild = invalid_forest_vertex;

        TreeVertexImpl(VertexId id, VertexData&& data) :
            id(id), data(std::move(data)) {}
    };

    template <typename VertexId, typename VertexData, bool isConst>
    class TreeVertex {
    private:
        using tree_vertex_ref_t = std::conditional_t<isConst,
            const TreeVertexImpl<VertexId, VertexData>&,
            TreeVertexImpl<VertexId, VertexData>&>;
    
        using tree_data_ref_t = std::conditional_t<isConst,
            const VertexData&,
            VertexData&>;

        tree_vertex_ref_t vertex;

    public:
        TreeVertex(tree_vertex_ref_t vertex) : vertex(vertex) {}
        template <bool isOtherConst>
        TreeVertex(TreeVertex<VertexId, VertexData, isOtherConst> vertex) : vertex(vertex.vertex) {}

        VertexId Id() const {
            return vertex.id;
        }

        bool operator==(const TreeVertex& other) {
            return Id() == other.Id();
        }

        bool operator!=(const TreeVertex& other) {
            return !operator==(other);
        }

        tree_data_ref_t Data() const {
            return vertex.data;
        }

        template <typename Vid, typename Vdata, typename Vhash>
        friend class Forest;
        template <typename Vid, typename Vdata, bool isc>
        friend class TreeVertex;
    };

    template <typename VertexId, typename VertexData=NoTreeData, typename VertexIdHasher = std::hash<VertexId>>
    class Forest {
    public:
        using vertex_t = TreeVertex<
            VertexId, VertexData, false>;
        using vertex_const_t = TreeVertex<
            VertexId, VertexData, true>;

    private:
        std::vector<TreeVertexImpl<VertexId, VertexData>> vertices;
        std::unordered_map<VertexId, forest_idx_t, VertexIdHasher> idToIndex;

        template <bool isConst>
        using user_vertex_cond_t = std::conditional_t<isConst, vertex_const_t, vertex_t>;
        template <bool isConst>
        using forest_ref_t = std::conditional_t<isConst, const Forest&, Forest&>;

        void FreeVertex(vertex_t vert);

        void CollectDescendantsPrefix(forest_idx_t start, std::queue<forest_idx_t>& queue) const;
        void CollectDescendantsPostfix(forest_idx_t start, std::queue<forest_idx_t>& queue) const;
   
    public:
        std::optional<vertex_t> TryGetVertex(VertexId id);
        std::optional<vertex_const_t> TryGetVertex(VertexId id) const;
        vertex_t GetVertex(VertexId id);
        vertex_const_t GetVertex(VertexId id) const;

        vertex_t CreateVertex(VertexId id, VertexData&& data);
        std::enable_if_t<std::is_default_constructible_v<VertexData>> 
            CreateVertex(VertexId id);
        void RemoveVertex(VertexId id);
        void RemoveVertex(vertex_t vertex);
        void RemoveVertexAndDescendants(VertexId id);
        void RemoveVertexAndDescendants(vertex_t vertex);
        vertex_t CreateChild(VertexId parent, VertexId child, VertexData&& childData);
        std::enable_if_t<std::is_default_constructible_v<VertexData>>
            CreateChild(VertexId parent, VertexId child);
        vertex_t CreateChild(vertex_t parent, VertexId childId, VertexData&& childData);
        std::enable_if_t<std::is_default_constructible_v<VertexData>>
            CreateChild(vertex_t parent, VertexId childId);
        void Orphan(VertexId id);
        void Orphan(vertex_t vertex);
        std::optional<VertexId> GetParent(VertexId id) const;
        std::optional<VertexId> GetNextSibling(VertexId id) const;
        std::optional<VertexId> GetPreviousSibling(VertexId id) const;
        std::optional<vertex_t> GetParent(vertex_t vertex);
        std::optional<vertex_t> GetNextSibling(vertex_t vertex);
        std::optional<vertex_t> GetPreviousSibling(vertex_t vertex);
        std::optional<vertex_const_t> GetParent(vertex_const_t vertex) const;
        std::optional<vertex_const_t> GetNextSibling(vertex_const_t vertex) const;
        std::optional<vertex_const_t> GetPreviousSibling(vertex_const_t vertex) const;
        void Clear();
        void SetParent(VertexId child, VertexId parent);
        void SetParent(vertex_t child, vertex_t parent);

        template <bool isConst>
        struct VertexIteratorBase {
            forest_idx_t idx;
            forest_ref_t<isConst> forest;

            VertexIteratorBase(forest_idx_t idx, forest_ref_t<isConst> forest) :
                idx(idx), forest(forest) {}

            bool operator==(const VertexIteratorBase<isConst>& other) const {
                return idx == other.idx;
            }
            bool operator!=(const VertexIteratorBase<isConst>& other) const {
                return !operator==(other);
            }

            user_vertex_cond_t<isConst> get() const {
                return user_vertex_cond_t<isConst>{
                    forest.vertices[idx]
                };
            }

            user_vertex_cond_t<isConst> operator*() const {
                return get();
            }
        };

        template <bool isConst>
        struct ChildIterator : public VertexIteratorBase<isConst> {
            void operator++() {
                this->idx = this->forest.vertices[this->idx].next;
            }

            ChildIterator(forest_idx_t idx, forest_ref_t<isConst> forest) :
                VertexIteratorBase<isConst>(idx, forest) {}
            ChildIterator(const ChildIterator& other) :
                VertexIteratorBase<isConst>(other.idx, other.forest) {}
            ChildIterator& operator=(const ChildIterator& other) {
                this->idx = other.idx;
                this->forest = other.forest;
                return *this;
            }
        };

        template <bool isConst> 
        struct DescendantIterator {
            forest_ref_t<isConst> forest;
            std::queue<forest_idx_t> mIndicesQueued;

            bool operator==(const DescendantIterator<isConst>& other) const {
                return mIndicesQueued.size() == 0 && other.mIndicesQueued.size() == 0;
            }
            bool operator!=(const DescendantIterator<isConst>& other) const {
                return !operator==(other);
            }

            user_vertex_cond_t<isConst> get() const {
                return user_vertex_cond_t<isConst>{
                    forest.vertices[mIndicesQueued.front()]
                };
            }

            user_vertex_cond_t<isConst> operator*() const {
                return get();
            }

            void operator++() {
                auto idx = mIndicesQueued.front();
                mIndicesQueued.pop();

                const auto& vert = forest.vertices[idx];

                for (auto idx = vert.firstChild; 
                    idx != invalid_forest_vertex; 
                    idx = forest.vertices[idx].next) {
                    mIndicesQueued.emplace(idx);
                }
            }
        };

        template <bool isConst> 
        struct AncestorIterator : public VertexIteratorBase<isConst> {
            void operator++() {
                this->idx = this->forest.vertices[this->idx].parent;
            }

            AncestorIterator(forest_idx_t idx, forest_ref_t<isConst> forest) :
                VertexIteratorBase<isConst>(idx, forest) {}
            AncestorIterator(const AncestorIterator& other) :
                VertexIteratorBase<isConst>(other.idx, other.forest) {}
            AncestorIterator& operator=(const AncestorIterator& other) {
                this->idx = other.idx;
                this->forest = other.forest;
                return *this;
            }
        };

        template <bool isConst>
        struct VertexIterator : public VertexIteratorBase<isConst> {
            void operator++() {
                ++this->idx;
            }

            VertexIterator(forest_idx_t idx, forest_ref_t<isConst> forest) :
                VertexIteratorBase<isConst>(idx, forest) {}
            VertexIterator(const VertexIterator& other) : 
                VertexIteratorBase<isConst>(other.idx, other.forest) {
            }
            VertexIterator& operator=(const VertexIterator& other) {
                this->idx = other.idx;
                this->forest = other.forest;
                return *this;
            }
        };

        Collection<ChildIterator<true>> GetChildren(VertexId id) const;
        Collection<ChildIterator<false>> GetChildren(VertexId id);
        Collection<DescendantIterator<true>> GetDescendants(VertexId id) const;
        Collection<DescendantIterator<false>> GetDescendants(VertexId id);
        Collection<AncestorIterator<true>> GetAncestors(VertexId id) const;
        Collection<AncestorIterator<false>> GetAncestors(VertexId id);

        Collection<ChildIterator<true>> GetChildren(vertex_const_t vert) const;
        Collection<ChildIterator<false>> GetChildren(vertex_t vert);
        Collection<DescendantIterator<true>> GetDescendants(vertex_const_t vert) const;
        Collection<DescendantIterator<false>> GetDescendants(vertex_t vert);
        Collection<AncestorIterator<true>> GetAncestors(vertex_const_t vert) const;
        Collection<AncestorIterator<false>> GetAncestors(vertex_t vert);
        
        VertexIterator<false> begin();
        VertexIterator<false> end();
        VertexIterator<true> begin() const;
        VertexIterator<true> end() const;

        Collection<VertexIterator<false>> GetVertices();
        Collection<VertexIterator<true>> GetVertices() const;

        vertex_t GetRoot(VertexId id);
        vertex_const_t GetRoot(VertexId id) const;

        vertex_t GetRoot(vertex_t vert);
        vertex_const_t GetRoot(vertex_const_t vert) const;
    };

    template <typename VertexId, typename VertexData=NoTreeData>
    class Tree {
    private:
        Forest<VertexId, VertexData> forest;
        VertexId root;

    public:
        Tree(VertexId root) : root(root) {
            forest.CreateVertex(root);
        }
        
        void RemoveVertexAndDescendants(VertexId id) {
            if (id == root) {
                throw std::runtime_error("Cannot remove root!");
            }
            forest.RemoveVertexAndDescendants(id);
        }

        void SetParent(VertexId child, VertexId parent) {
            forest.SetParent(child, parent);
        }

        void CreateChild(VertexId parent, VertexId child, VertexData&& childData) {
            forest.CreateChild(parent, child, std::move(childData));
        }

        std::enable_if_t<std::is_default_constructible_v<VertexData>>
            CreateChild(VertexId parent, VertexId child) {
            forest.CreateChild(parent, child);
        }

        auto GetChildren(VertexId id) const {
            return forest.GetChildren(id);
        }
        auto GetChildren(VertexId id) {
            return forest.GetChildren(id);
        }
        auto GetDescendants(VertexId id) const {
            return forest.GetDescendants(id);
        }
        auto GetDescendants(VertexId id) {
            return forest.GetDescendants(id);
        }
        auto GetAncestors(VertexId id) const {
            return forest.GetAncestors(id);
        }
        auto GetAncestors(VertexId id) {
            return forest.GetAncestors(id);
        }
        std::optional<VertexId> GetParent(VertexId id) {
            return forest.GetParent(id);
        }
        std::optional<VertexId> GetNextSibling(VertexId id) {
            return forest.GetNextSibling(id);
        }
        std::optional<VertexId> GetPreviousSibling(VertexId id) {
            return forest.GetPreviousSibling(id);
        }

        void Clear() {
            forest.Clear();
            forest.CreateVertex(root);
        }
    };
}

#include <okami/tree_inl.hpp>