#pragma once

#include <okami/collection.hpp>

#include <algorithm>
#include <queue>

namespace okami {
    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    void Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::SwapEdges(graph_idx_t idx1, graph_idx_t idx2) {
        edge_impl_t& e1 = edges[idx1];
        edge_impl_t& e2 = edges[idx2];

        vertex_impl_t& e1s = vertices[e1.source];
        vertex_impl_t& e1d = vertices[e1.dest];

        vertex_impl_t& e2s = vertices[e2.source];
        vertex_impl_t& e2d = vertices[e2.dest];

        graph_idx_t e1pi = e1.mPrevIn;
        graph_idx_t e1po = e1.mPrevOut;
        graph_idx_t e1ni = e1.mNextIn;
        graph_idx_t e1no = e1.mNextOut;

        graph_idx_t e2pi = e2.mPrevIn;
        graph_idx_t e2po = e2.mPrevOut;
        graph_idx_t e2ni = e2.mNextIn;
        graph_idx_t e2no = e2.mNextOut;

        graph_idx_t e1sfo = e1s.mFirstOut;
        graph_idx_t e1slo = e1s.mLastOut;
        graph_idx_t e1dfi = e1d.mFirstIn;
        graph_idx_t e1dli = e1d.mLastIn;

        graph_idx_t e2sfo = e2s.mFirstOut;
        graph_idx_t e2slo = e2s.mLastOut;
        graph_idx_t e2dfi = e2d.mFirstIn;
        graph_idx_t e2dli = e2d.mLastIn;

        if (e1sfo == idx1)
            e1s.mFirstOut = idx2;
        if (e1slo == idx1)
            e1s.mLastOut = idx2;
        if (e1dfi == idx1)
            e1d.mFirstIn = idx2;
        if (e1dli == idx1)
            e1d.mLastIn = idx2;
        
        if (e2sfo == idx2)
            e2s.mFirstOut = idx1;
        if (e2slo == idx2)
            e2s.mLastOut = idx1;
        if (e2dfi == idx2)
            e2d.mFirstIn = idx1;
        if (e2dli == idx2)
            e2d.mLastIn = idx1;

        if (e1pi != invalid_graph_edge)
            edges[e1pi].mNextIn = idx2;
        if (e1ni != invalid_graph_edge)
            edges[e1ni].mPrevIn = idx2;
        if (e1po != invalid_graph_edge)
            edges[e1po].mNextOut = idx2;
        if (e1no != invalid_graph_edge)
            edges[e1no].mPrevOut = idx2;

        if (e2pi != invalid_graph_edge)
            edges[e2pi].mNextIn = idx1;
        if (e2ni != invalid_graph_edge)
            edges[e2ni].mPrevIn = idx1;
        if (e2po != invalid_graph_edge)
            edges[e2po].mNextOut = idx1;
        if (e2no != invalid_graph_edge)
            edges[e2no].mPrevOut = idx1;

        idToEdge[std::make_pair(e1s.id, e1d.id)] = idx2;
        idToEdge[std::make_pair(e2s.id, e2d.id)] = idx1;

        std::swap(e1, e2);
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    void Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::SwapVertices(graph_idx_t idx1, graph_idx_t idx2) {
        vertex_impl_t& v1 = vertices[idx1];
        vertex_impl_t& v2 = vertices[idx2];

        for (graph_idx_t it = v1.mFirstIn; it != invalid_graph_edge; it = edges[it].mNextIn) {
            edges[it].dest = idx2;
        }

        for (graph_idx_t it = v1.mFirstOut; it != invalid_graph_edge; it = edges[it].mNextOut) {
            edges[it].source = idx2;
        }

        for (graph_idx_t it = v2.mFirstIn; it != invalid_graph_edge; it = edges[it].mNextIn) {
            edges[it].dest = idx1;
        }

        for (graph_idx_t it = v2.mFirstOut; it != invalid_graph_edge; it = edges[it].mNextOut) {
            edges[it].source = idx1;
        }

        idToVertex[v1.id] = idx2;
        idToVertex[v2.id] = idx1;

        std::swap(v1, v2);
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    void Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::AddToInLinkedList(vertex_impl_t& v, edge_impl_t& e, graph_idx_t eidx) {
        if (v.mFirstIn == invalid_graph_edge) {
            v.mFirstIn = eidx;
            v.mLastIn = eidx;
        } else {
            edge_impl_t& le = edges[v.mLastIn];
            le.mNextIn = eidx;
            e.mPrevIn = v.mLastIn;
            v.mLastIn = eidx;
        }
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    void Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::AddToOutLinkedList(vertex_impl_t& v, edge_impl_t& e, graph_idx_t eidx) {
        if (v.mFirstOut == invalid_graph_edge) {
            v.mFirstOut = eidx;
            v.mLastOut = eidx;
        } else {
            edge_impl_t& le = edges[v.mLastOut];
            le.mNextOut = eidx;
            e.mPrevOut = v.mLastOut;
            v.mLastOut = eidx;
        }
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    void Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::RemoveFromInLinkedList(edge_impl_t& e) {
        if (e.mPrevIn != invalid_graph_edge) {
            edges[e.mPrevIn].mNextIn = e.mNextIn;
        } else {
            vertices[e.dest].mFirstIn = e.mNextIn;
        }
        if (e.mNextIn != invalid_graph_edge) {
            edges[e.mNextIn].mPrevIn = e.mPrevIn;
        } else {
            vertices[e.dest].mLastIn = e.mPrevIn;
        }

        e.mNextIn = invalid_graph_edge;
        e.mPrevIn = invalid_graph_edge;
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    void Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::RemoveFromOutLinkedList(edge_impl_t& e) {
        if (e.mPrevOut != invalid_graph_edge) {
            edges[e.mPrevOut].mNextOut = e.mNextOut;
        } else {
            vertices[e.source].mFirstOut = e.mNextOut;
        }
        if (e.mNextOut != invalid_graph_edge) {
            edges[e.mNextOut].mPrevOut = e.mPrevOut;
        } else {
            vertices[e.source].mLastOut = e.mPrevOut;
        }

        e.mNextOut = invalid_graph_edge;
        e.mPrevOut = invalid_graph_edge;
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    std::tuple<graph_idx_t, graph_idx_t> Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::RemoveEdgeAtIdx(graph_idx_t idx) {
        if (idx != edges.size() - 1) {
            SwapEdges(idx, edges.size() - 1);
        }

        auto& e = edges.back();
        auto& s = vertices[e.source];
        auto& d = vertices[e.dest];
        idToEdge.erase(std::make_pair(s.id, d.id));
        
        auto nextIn = e.mNextIn;
        auto nextOut = e.mNextOut;
        RemoveFromOutLinkedList(e);
        RemoveFromInLinkedList(e);
        edges.pop_back();

        return std::make_tuple(nextIn, nextOut);
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    void Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::RemoveVertexAtIdx(graph_idx_t idx) {
        const vertex_impl_t& v = vertices[idx];

        for (graph_idx_t it = v.mFirstIn; it != invalid_graph_edge;) {
            it = std::get<0>(RemoveEdgeAtIdx(it));
        }

        for (graph_idx_t it = v.mFirstOut; it != invalid_graph_edge;) {
            it = std::get<1>(RemoveEdgeAtIdx(it));
        }

        if (idx != vertices.size() - 1) {
            SwapVertices(idx, vertices.size() - 1);
        }

        idToVertex.erase(vertices.back().id);
        vertices.pop_back();
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    void Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::Clear() {
        vertices.clear();
        edges.clear();
        idToVertex.clear();
        idToEdge.clear();
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    Collection<typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::template OutEdgeIterator<true>> 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::GetOutgoing(vertex_const_t vertex) const {

        return Collection<OutEdgeIterator<true>>{
            OutEdgeIterator<true>{vertex.vertex->mFirstOut, *vertex.vertex, *this},
            OutEdgeIterator<true>{invalid_graph_edge, *vertex.vertex, *this}};
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    Collection<typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::template InEdgeIterator<true>> 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::GetIngoing(vertex_const_t vertex) const {
        
        return Collection<InEdgeIterator<true>>{
            InEdgeIterator<true>{vertex.vertex->mFirstIn, *vertex.vertex, *this},
            InEdgeIterator<true>{invalid_graph_edge, *vertex.vertex, *this}};
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    Collection<typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::template OutEdgeIterator<false>> 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::GetOutgoing(vertex_t vertex) {

        return Collection<OutEdgeIterator<false>>{
            OutEdgeIterator<false>{vertex.vertex->mFirstOut, *vertex.vertex, *this},
            OutEdgeIterator<false>{invalid_graph_edge, *vertex.vertex, *this}};
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    Collection<typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::template InEdgeIterator<false>> 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::GetIngoing(vertex_t vertex) {

        return Collection<InEdgeIterator<false>>{
            InEdgeIterator<false>{vertex.vertex->mFirstIn, *vertex.vertex, *this},
            InEdgeIterator<false>{invalid_graph_edge, *vertex.vertex, *this}};
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    Collection<typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::template OutEdgeIterator<true>> 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::GetOutgoing(VertexId id) const {
        return GetOutgoing(GetVertex(id));
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    Collection<typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::template InEdgeIterator<true>> 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::GetIngoing(VertexId id) const {
        return GetIngoing(GetVertex(id));
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    Collection<typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::template OutEdgeIterator<false>> 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::GetOutgoing(VertexId id) {
        return GetOutgoing(GetVertex(id));
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    Collection<typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::template InEdgeIterator<false>> 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::GetIngoing(VertexId id) {
        return GetIngoing(GetVertex(id));
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    std::optional<typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::vertex_t> 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::TryGetVertex(VertexId id) {
        auto it = idToVertex.find(id);
        if (it != idToVertex.end()) {
            return vertex_t{
                vertices[it->second]
            };
        }
        else {
            return std::optional<vertex_t>{};
        }
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    std::optional<typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::vertex_const_t>
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::TryGetVertex(VertexId id) const {
        auto it = idToVertex.find(id);
        if (it != idToVertex.end()) {
            return vertex_const_t{
                vertices[it->second]
            };
        }
        else {
            return std::optional<vertex_const_t>{};
        }
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    std::optional<typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::edge_t> 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::TryGetEdge(VertexId source, VertexId dest) {
        auto it = idToEdge.find(std::make_pair(source, dest));
        if (it != idToEdge.end()) {
            return edge_t{
                edges[it->second],
                vertices[edges[it->second].source],
                vertices[edges[it->second].dest]
            };
        } else {
            return std::optional<edge_t>{};
        }
    }
    
    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    std::optional<typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::edge_const_t> 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::TryGetEdge(VertexId source, VertexId dest) const {
        auto it = idToEdge.find(std::make_pair(source, dest));
        if (it != idToEdge.end()) {
            return edge_const_t{
                edges[it->second],
                vertices[edges[it->second].source],
                vertices[edges[it->second].dest]
            };
        } else {
            return std::optional<edge_const_t>{};
        }
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::vertex_t 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::GetVertex(VertexId id) {
        return TryGetVertex(id).value();
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::vertex_const_t 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::GetVertex(VertexId id) const {
        return TryGetVertex(id).value();
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::edge_t 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::GetEdge(VertexId source, VertexId dest) {
        return TryGetEdge(source, dest).value();
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::edge_const_t 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::GetEdge(VertexId source, VertexId dest) const {
        return TryGetEdge(source, dest).value();
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::vertex_t 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::CreateVertex(VertexId id, VertexData&& data) {
        idToVertex.emplace(id, vertices.size());
        vertices.emplace_back(vertex_impl_t(id, std::move(data)));
        return vertex_t{
            vertices.back()
        };
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::vertex_t
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::CreateVertex(VertexId id) {
        std::conditional_t<std::is_default_constructible_v<VertexData>,
            VertexData, NoGraphData> defaultData;
        static_assert(std::is_default_constructible_v<VertexData>,
            "VertexData must be default constructible for this method!");
        return CreateVertex(id, std::move(defaultData));
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::edge_t 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::CreateEdge(
        VertexId source, VertexId dest, EdgeData&& data) {
        CreateEdge(GetVertex(source), GetVertex(dest), std::move(data));
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::edge_t 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::CreateEdge(VertexId source, VertexId dest) {
        std::conditional_t<std::is_default_constructible_v<EdgeData>,
            EdgeData, NoGraphData> defaultData;
        static_assert(std::is_default_constructible_v<EdgeData>,
            "EdgeData must be default constructible for this method!");
        return CreateEdge(source, dest, std::move(defaultData));
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::edge_t 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::CreateEdge(vertex_t source, vertex_t dest) {
        return CreateEdge(source.Id(), dest.Id());
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::edge_t 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::CreateEdge(vertex_t source, vertex_t dest, EdgeData&& data) {
        auto idx = edges.size();
        idToEdge.emplace(std::make_pair(source.Id(), dest.Id()), idx);

        graph_idx_t sidx = source.vertex - &vertices[0];
        graph_idx_t didx = dest.vertex - &vertices[0];

        edges.emplace_back(edge_impl_t(std::move(data), sidx, didx));

        vertex_impl_t& s = *source.vertex;
        vertex_impl_t& d = *dest.vertex;

        edge_impl_t& e = edges[idx];

        AddToInLinkedList(d, e, idx);
        AddToOutLinkedList(s, e, idx);

        return edge_t{
            e,
            s,
            d,
        };
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    template <typename It>
    typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::vertex_t 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::Contract(It begin, It end, VertexId newId) {
        std::conditional_t<std::is_default_constructible_v<VertexData>,
            VertexData, NoGraphData> defaultData;
        static_assert(std::is_default_constructible_v<VertexData>,
            "VertexData must be default constructible for this method!");
        return Contract(begin, end, newId, std::move(defaultData));
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    template <typename It>
    typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::vertex_t 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::Contract(It begin, It end, VertexId newId, VertexData&& data) {
        std::unordered_set<VertexId> ingoing;
        std::unordered_set<VertexId> outgoing;

        for (auto it = begin; it != end; ++it) {
            for (auto edge : GetIngoing(*it)) {
                ingoing.emplace(edge.source);
            }
            for (auto edge : GetOutgoing(*it)) {
                outgoing.emplace(edge.dest);
            }
        }

        for (auto it = begin; it != end; ++it) {
            RemoveVertex(*it);
        }

        CreateVertex(newId, std::move(data));

        for (auto in : ingoing) {
            CreateEdge(in, newId);
        }
        for (auto out : outgoing) {
            CreateEdge(newId, out);
        }
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    void Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::RemoveEdge(edge_t edge) {
        RemoveEdgeAtIdx(edge.edge - &edges[0]);
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    void Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::RemoveEdge(edge_const_t edge) {
        RemoveEdgeAtIdx(edge.edge - &edges[0]);
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    void Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::RemoveEdge(VertexId source, VertexId dest) {
        RemoveEdge(GetEdge(source, dest));
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    void Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::RemoveVertex(VertexId id) {
        RemoveVertex(GetVertex(id));
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    void Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::RemoveVertex(vertex_t vertex) {
        RemoveVertexAtIdx(vertex.vertex - &vertices[0]);
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    void Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::RemoveVertex(vertex_const_t vertex) {
        RemoveVertexAtIdx(vertex.vertex - &vertices[0]);
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    Collection<typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::template VertexIterator<true>> 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::GetVertices() const {
        return Collection<VertexIterator<true>>{
            begin(),
            end()
        };
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    Collection<typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::template VertexIterator<false>> 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::GetVertices() {
        return Collection<VertexIterator<false>>{
            begin(),
            end()
        };
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    Collection<typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::template EdgeIterator<true>> 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::GetEdges() const {
        return Collection<EdgeIterator<true>>{
            EdgeIterator<true>{0, *this},
            EdgeIterator<true>{edges.size(), *this}
        };
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    Collection<typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::template EdgeIterator<false>> 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::GetEdges() {
        return Collection<EdgeIterator<false>>{
            EdgeIterator<false>{0, *this},
            EdgeIterator<false>{edges.size(), *this}
        };
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::template VertexIterator<false> 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::begin() {
        return VertexIterator<false>{0, *this};
    }
    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::template VertexIterator<false> 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::end() {
        return VertexIterator<false>{vertices.size(), *this};
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::template VertexIterator<true> 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::begin() const {
        return VertexIterator<true>{0, *this};
    }
    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::template VertexIterator<true> 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::end() const {
        return VertexIterator<true>{vertices.size(), *this};
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    size_t Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::GetVertexCount() const {
        return vertices.size();
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    size_t Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::GetEdgeCount() const {
        return edges.size();
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    Digraph<VertexId, NoGraphData, NoGraphData, VertexIdHash> 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::CloneWithoutData() const {
        
        Digraph<VertexId, NoGraphData, NoGraphData, VertexIdHash> result;

        result.vertices.reserve(vertices.size());
        result.edges.reserve(edges.size());

        for (auto& v : vertices) {
            DigraphVertexImpl<VertexId, NoGraphData> newVert{
                v.id,
                NoGraphData{}
            };

            newVert.mFirstOut = v.mFirstOut;
            newVert.mFirstIn = v.mFirstIn;
            newVert.mLastOut = v.mLastOut;
            newVert.mLastIn = v.mLastIn;
            
            result.vertices.emplace_back(std::move(newVert));
        }

        for (auto& e : edges) {
            DigraphEdgeImpl<NoGraphData> newEdge{
                NoGraphData{},
                e.source,
                e.dest,
            };

            newEdge.mNextIn = e.mNextIn;
            newEdge.mPrevIn = e.mPrevIn;
            newEdge.mNextOut = e.mNextOut;
            newEdge.mPrevOut = e.mPrevOut;

            result.edges.emplace_back(std::move(newEdge));
        }

        result.idToVertex = idToVertex;
        result.idToEdge = idToEdge;

        return result;
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    bool Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::Verify() const {
        for (graph_idx_t i = 0; i < vertices.size(); ++i) {
            const auto& v = vertices[i];

            auto first = v.mFirstIn;
            auto last = invalid_graph_vertex;

            for (graph_idx_t it = v.mFirstIn; it != invalid_graph_edge; it = edges[it].mNextIn) {
                const auto& e = edges[it];

                if (e.mPrevIn != last) {
                    return false;
                }

                last = it;
            }

            if (v.mLastIn != last) {
                return false;
            }
        }

        for (graph_idx_t i = 0; i < vertices.size(); ++i) {
            const auto& v = vertices[i];

            auto first = v.mFirstOut;
            auto last = invalid_graph_vertex;

            for (graph_idx_t it = v.mFirstOut; it != invalid_graph_edge; it = edges[it].mNextOut) {
                const auto& e = edges[it];

                if (e.mPrevOut != last) {
                    return false;
                }

                last = it;
            }

            if (v.mLastOut != last) {
                return false;
            }
        }
        
        for (auto [v_id, v_idx] : idToVertex) {
            if (vertices[v_idx].id != v_id)
                return false;
        }

        for (auto [e_v_id, e_idx] : idToEdge) {
            auto& edge = edges[e_idx];
            if (e_v_id.first != vertices[edge.source].id)
                return false;
            if (e_v_id.second != vertices[edge.dest].id)
                return false;
        }

        return true;
    }
}