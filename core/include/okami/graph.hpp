#pragma once

#include <okami/collection.hpp>

#include <vector>
#include <unordered_map>

namespace okami {
/*
    Diagraph utilities for graph computations outside the registry.
*/

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    class Digraph;

    using graph_idx_t = int64_t;

    constexpr graph_idx_t invalid_graph_vertex = -1;
    constexpr graph_idx_t invalid_graph_edge = -1;

    struct NoGraphData {};

    template <typename VertexId, typename VertexData>
    struct DigraphVertexImpl {
        VertexData data;
        VertexId id;

        graph_idx_t mFirstOut = invalid_graph_edge;
        graph_idx_t mLastOut = invalid_graph_edge;
        graph_idx_t mFirstIn = invalid_graph_edge;
        graph_idx_t mLastIn = invalid_graph_edge;

        DigraphVertexImpl(VertexId id, VertexData&& data) : id(id), data(std::move(data)) {}
    };

    template <typename EdgeData>
    struct DigraphEdgeImpl {
        EdgeData data;

        graph_idx_t source = invalid_graph_vertex;
        graph_idx_t dest = invalid_graph_vertex;

        graph_idx_t mNextIn = invalid_graph_edge;
        graph_idx_t mPrevIn = invalid_graph_edge;
        graph_idx_t mNextOut = invalid_graph_edge;
        graph_idx_t mPrevOut = invalid_graph_edge;

        DigraphEdgeImpl(EdgeData&& data, graph_idx_t source, graph_idx_t dest) : 
            data(std::move(data)),
            source(source),
            dest(dest) {}
    };

    template <typename VertexId, typename VertexData, bool isConst>
    class DigraphVertex {
    private:
        using vertex_impl_ref_t = std::conditional_t<isConst,
            const DigraphVertexImpl<VertexId, VertexData>&,
            DigraphVertexImpl<VertexId, VertexData>&>;

        using vertex_impl_ptr_t = 
            std::add_pointer_t<std::remove_reference_t<vertex_impl_ref_t>>;

        using vertex_data_ref_t = std::conditional_t<isConst,
            const VertexData&,
            VertexData&>;

        vertex_impl_ptr_t vertex;

    public:
        DigraphVertex(vertex_impl_ref_t vertex) : vertex(&vertex) {}
        template <bool isOtherConst>
        DigraphVertex(const DigraphVertex<VertexId, VertexData, isOtherConst>& vertex) : vertex(vertex.vertex) {}
        template <bool isOtherConst>
        DigraphVertex(DigraphVertex<VertexId, VertexData, isOtherConst>&& vertex) : vertex(vertex.vertex) {}
        template <bool isOtherConst>
        DigraphVertex& operator=(const DigraphVertex<VertexId, VertexData, isOtherConst>& vertex) {
            vertex = vertex.vertex;
            return *this;
        }
        template <bool isOtherConst>
        DigraphVertex& operator=(DigraphVertex<VertexId, VertexData, isOtherConst>&& vertex) {
            vertex = vertex.vertex;
            return *this;
        }

        VertexId Id() const {
            return vertex->id;
        }

        bool operator==(const DigraphVertex& other) {
            return Id() == other.Id();
        }

        bool operator!=(const DigraphVertex& other) {
            return !operator==(other);
        }

        vertex_data_ref_t Data() const {
            return vertex->data;
        }

        bool operator==(const DigraphVertex<VertexId, VertexData, isConst>& other) const {
            return Id() == other.Id();
        }

        template <typename Vid, typename Vdata, typename Edata, typename Vhash>
        friend class Digraph;
        template <typename Vid, typename Vdata, bool isc>
        friend class DigraphVertex; 
    };

    template <typename VertexId, typename VertexData, typename EdgeData, bool isConst>
    class DigraphEdge {
    private:
        using edge_impl_ref_t = std::conditional_t<isConst,
            const DigraphEdgeImpl<EdgeData>&,
            DigraphEdgeImpl<EdgeData>&>;

        using vertex_impl_ref_t = std::conditional_t<isConst,
            const DigraphVertexImpl<VertexId, VertexData>&,
            DigraphVertexImpl<VertexId, VertexData>&>;

        using edge_impl_ptr_t = std::add_pointer_t<std::remove_reference_t<edge_impl_ref_t>>;
        using vertex_impl_ptr_t = std::add_pointer_t<std::remove_reference_t<vertex_impl_ref_t>>;

        using edge_data_ref_t = std::conditional_t<isConst,
            const EdgeData&,
            EdgeData&>;

        edge_impl_ptr_t edge;
        vertex_impl_ptr_t source;
        vertex_impl_ptr_t dest;

    public:
        DigraphEdge(
            edge_impl_ref_t edge, 
            vertex_impl_ref_t source,
            vertex_impl_ref_t dest) : 
                edge(&edge),
                source(&source),
                dest(&dest) {}
        template <bool isOtherConst>
        DigraphEdge(const DigraphEdge<VertexId, VertexData, EdgeData, isOtherConst>& edge) :
            edge(edge.edge),
            source(edge.source),
            dest(edge.dest) {}

        DigraphVertex<VertexId, VertexData, isConst> Source() const {
            return *source;
        }

        DigraphVertex<VertexId, VertexData, isConst> Dest() const {
            return *dest;
        }

        bool operator==(const DigraphEdge& other) const {
            return source->id == other.source->id && dest->id == other.dest->id;
        }

        bool operator!=(const DigraphEdge& other) const {
            return !operator!=(other);
        }

        edge_data_ref_t Data() const {
            return edge->data;
        }

        template <typename Vid, typename Vdata, typename Edata, typename Vhash>
        friend class Digraph;
        template <typename Vid, typename Vdata, typename Edata, bool isc>
        friend class DigraphEdge;
    };

    template <typename VertexId, 
        typename VertexData = NoGraphData, 
        typename EdgeData = NoGraphData, 
        typename VertexIdHash = std::hash<VertexId>>
    class Digraph {
    public:
        using edge_impl_t = DigraphEdgeImpl<EdgeData>;
        using vertex_impl_t = DigraphVertexImpl<VertexId, VertexData>;

        using vertex_t = DigraphVertex<VertexId, VertexData, false>;
        using edge_t = DigraphEdge<VertexId, VertexData, EdgeData, false>;

        using vertex_const_t = DigraphVertex<VertexId, VertexData, true>;
        using edge_const_t = DigraphEdge<VertexId, VertexData, EdgeData, true>;

        template <bool isConst>
        using edge_cond_t = std::conditional_t<isConst, edge_const_t, edge_t>;
        template <bool isConst>
        using vertex_cond_t = std::conditional_t<isConst, vertex_const_t, vertex_t>;
        template <bool isConst>
        using graph_ref_t = std::conditional_t<isConst, const Digraph&, Digraph&>;

        template <bool isConst>
        using vertex_ref_t = std::conditional_t<isConst, const vertex_impl_t&, vertex_impl_t&>;
        template <bool isConst>
        using edge_ref_t = std::conditional_t<isConst, const edge_impl_t&, edge_impl_t&>;

    private:
        template <typename T1, typename T2, typename Hasher1 = std::hash<T1>, typename Hasher2 = std::hash<T2>>
            struct PairHash {
                std::size_t operator()(const std::pair<T1, T2>& t) const noexcept {
                    return std::hash<T1>()(t.first) ^ std::hash<T2>()(t.second);
                }
            }; 

        std::vector<vertex_impl_t> vertices;
        std::vector<edge_impl_t> edges;

        std::unordered_map<VertexId, graph_idx_t, VertexIdHash> idToVertex;
        std::unordered_map<std::pair<VertexId, VertexId>, 
            graph_idx_t, PairHash<VertexId, VertexId, VertexIdHash, VertexIdHash>> idToEdge;

        void SwapEdges(graph_idx_t idx1, graph_idx_t idx2);
        void SwapVertices(graph_idx_t idx1, graph_idx_t idx2);
        void AddToInLinkedList(vertex_impl_t& v, edge_impl_t& e, graph_idx_t eidx);
        void AddToOutLinkedList(vertex_impl_t& v, edge_impl_t& e, graph_idx_t eidx);
        void RemoveFromInLinkedList(edge_impl_t& e);
        void RemoveFromOutLinkedList(edge_impl_t& e);
        std::tuple<graph_idx_t, graph_idx_t> RemoveEdgeAtIdx(graph_idx_t idx);
        void RemoveVertexAtIdx(graph_idx_t idx);

        Digraph(const Digraph&) = default;
        Digraph& operator=(const Digraph&) = default;

    public:
        template <bool isConst>
        struct VertexIterator {
            graph_idx_t idx;
            graph_ref_t<isConst> graph;

            bool operator==(const VertexIterator<isConst>& other) const {
                return idx == other.idx;
            }
            bool operator!=(const VertexIterator<isConst>& other) const {
                return !operator==(other);
            }

            void operator++() {
                idx++;
            }

            vertex_cond_t<isConst> get() const {
                return vertex_cond_t<isConst>{
                    graph.vertices[idx]
                };
            }
            vertex_cond_t<isConst> operator*() const {
                return get();
            }
        };

        template <bool isConst>
        struct EdgeIterator {
            graph_idx_t idx;
            graph_ref_t<isConst> graph;

            bool operator==(const EdgeIterator& other) const {
                return idx == other.idx;
            }
            bool operator!=(const EdgeIterator& other) const {
                return !operator==(other);
            }

            void operator++() {
                ++idx;
            }

            edge_cond_t<isConst> get() const {
                return edge_cond_t<isConst>{
                    graph.edges[idx],
                    graph.vertices[graph.edges[idx].source],
                    graph.vertices[graph.edges[idx].dest],
                };
            }
            edge_cond_t<isConst> operator*() const {
                return get();
            }
        };

        template <bool isConst>
        struct InEdgeIterator {
            graph_idx_t idx;
            vertex_ref_t<isConst> dest;
            graph_ref_t<isConst> graph;

            bool operator==(const InEdgeIterator& other) const {
                return idx == other.idx;
            }
            bool operator!=(const InEdgeIterator& other) const {
                return !operator==(other);
            }

            void operator++() {
                idx = graph.edges[idx].mNextIn;
            }

            edge_cond_t<isConst> get() const {
                return edge_cond_t<isConst>{
                    graph.edges[idx],
                    graph.vertices[graph.edges[idx].source],
                    dest,
                };
            }
            edge_cond_t<isConst> operator*() const {
                return get();
            }
        };

        template <bool isConst>
        struct OutEdgeIterator {
            graph_idx_t idx;
            vertex_ref_t<isConst> source;
            graph_ref_t<isConst> graph;

            bool operator==(const OutEdgeIterator& other) const {
                return idx == other.idx;
            }
            bool operator!=(const OutEdgeIterator& other) const {
                return !operator==(other);
            }

            void operator++() {
                idx = graph.edges[idx].mNextOut;
            }

            edge_cond_t<isConst> get() const {
                return edge_cond_t<isConst>{
                    graph.edges[idx],
                    source,
                    graph.vertices[graph.edges[idx].dest],
                };
            }

            edge_cond_t<isConst> operator*() const {
                return get();
            }
        };

        std::optional<vertex_t> TryGetVertex(VertexId id);
        std::optional<vertex_const_t> TryGetVertex(VertexId id) const;
        std::optional<edge_t> TryGetEdge(VertexId source, VertexId dest);
        std::optional<edge_const_t> TryGetEdge(VertexId source, VertexId dest) const;

        vertex_t GetVertex(VertexId id);
        vertex_const_t GetVertex(VertexId id) const;
        edge_t GetEdge(VertexId source, VertexId dest);
        edge_const_t GetEdge(VertexId source, VertexId dest) const;

        Collection<OutEdgeIterator<true>> GetOutgoing(vertex_const_t vertex) const;
        Collection<InEdgeIterator<true>> GetIngoing(vertex_const_t vertex) const;
        Collection<OutEdgeIterator<false>> GetOutgoing(vertex_t vertex);
        Collection<InEdgeIterator<false>> GetIngoing(vertex_t vertex);

        Collection<OutEdgeIterator<true>> GetOutgoing(VertexId id) const;
        Collection<InEdgeIterator<true>> GetIngoing(VertexId id) const;
        Collection<OutEdgeIterator<false>> GetOutgoing(VertexId id);
        Collection<InEdgeIterator<false>> GetIngoing(VertexId id);

        vertex_t CreateVertex(VertexId id, VertexData&& data);
        vertex_t CreateVertex(VertexId id);

        template <typename It>
        vertex_t Contract(It begin, It end, VertexId newId);
        template <typename It>
        vertex_t Contract(It begin, It end, VertexId newId, VertexData&& data);

        edge_t CreateEdge(VertexId source, VertexId dest, EdgeData&& data);
        edge_t CreateEdge(VertexId source, VertexId dest);

        edge_t CreateEdge(vertex_t source, vertex_t dest);
        edge_t CreateEdge(vertex_t source, vertex_t dest, EdgeData&& data);

        void RemoveEdge(edge_t edge);
        void RemoveEdge(edge_const_t edge);
        void RemoveEdge(VertexId source, VertexId dest);
        void RemoveVertex(vertex_t vertex);
        void RemoveVertex(vertex_const_t vertex);
        void RemoveVertex(VertexId id);

        Collection<VertexIterator<true>> GetVertices() const;
        Collection<VertexIterator<false>> GetVertices();

        Collection<EdgeIterator<true>> GetEdges() const;
        Collection<EdgeIterator<false>> GetEdges();

        VertexIterator<true> begin() const;
        VertexIterator<true> end() const;

        VertexIterator<false> begin();
        VertexIterator<false> end();

        size_t GetVertexCount() const;
        size_t GetEdgeCount() const;

        void Clear();

        Digraph() = default;
        Digraph(Digraph&&) = default;
        Digraph& operator=(Digraph&&) = default;

        Digraph Clone() const {
            return Digraph(*this);
        }
        Digraph<VertexId, NoGraphData, NoGraphData, VertexIdHash> 
            CloneWithoutData() const;

        bool Verify() const;

        size_t GetStorageIndexOf(vertex_t vertex) const {
            return vertex.vertex - &vertices[0];
        }
        size_t GetStorageIndexOf(vertex_const_t vertex) const {
            return vertex.vertex - &vertices[0];
        }
        size_t GetStorageIndexOf(edge_t edge) const {
            return edge.edge - &edges[0];
        }
        size_t GetStorageIndexOf(edge_const_t edge) const {
            return edge.edge - &edges[0];
        }

        vertex_t GetVertexAtStorageIndex(size_t idx) {
            return vertex_t(vertices[idx]);
        }
        vertex_const_t GetVertexAtStorageIndex(size_t idx) const {
            return vertex_const_t(vertices[idx]);
        }
        edge_t GetEdgeAtStorageIndex(size_t idx) {
            return edge_t(edges[idx], vertices[edges[idx].source], vertices[edges[idx].dest]); 
        }
        edge_const_t GetEdgeAtStorageIndex(size_t idx) const {
            return edge_const_t(edges[idx], vertices[edges[idx].source], vertices[edges[idx].dest]); 
        }

        std::tuple<vertex_t, bool> GetVertexOrCreate(VertexId id, VertexData&& data) {
            auto vert = TryGetVertex(id);
            if (vert.has_value()) {
                return std::make_tuple(vert.value(), false);
            } else {
                return std::make_tuple(CreateVertex(id, std::move(data)), true);
            }
        }
        std::tuple<vertex_t, bool> GetVertexOrCreate(VertexId id) {
            auto vert = TryGetVertex(id);
            if (vert.has_value()) {
                return std::make_tuple(vert.value(), false);
            } else {
                return std::make_tuple(CreateVertex(id), true);
            }
        }
        std::tuple<edge_t, bool> GetEdgeOrCreate(VertexId source, VertexId dest, EdgeData&& data) {
            auto edge = TryGetEdge(source, dest);
            if (edge.has_value()) {
                return std::make_tuple(edge.value(), false);
            } else {
                return std::make_tuple(CreateEdge(source, dest, std::move(data)), true);
            }
        }
        std::tuple<edge_t, bool> GetEdgeOrCreate(VertexId source, VertexId dest) {
            auto edge = TryGetEdge(source, dest);
            if (edge.has_value()) {
                return std::make_tuple(edge.value(), false);
            } else {
                return std::make_tuple(CreateEdge(source, dest), true);
            }
        }

        template <typename T1, typename T2, typename T3, typename T4>
        friend class Digraph;
    };
}

#include <okami/graph_inl.hpp>