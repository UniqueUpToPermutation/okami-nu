#pragma once

#include <okami/collection.hpp>

#include <algorithm>
#include <queue>

namespace okami {
    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    void Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::SwapEdges(graph_idx_t idx1, graph_idx_t idx2) {
        edge_impl_t& e1 = mEdges[idx1];
        edge_impl_t& e2 = mEdges[idx2];

        vertex_impl_t& e1s = mVertices[e1.mSource];
        vertex_impl_t& e1d = mVertices[e1.mDest];

        vertex_impl_t& e2s = mVertices[e2.mSource];
        vertex_impl_t& e2d = mVertices[e2.mDest];

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
            mEdges[e1pi].mNextIn = idx2;
        if (e1ni != invalid_graph_edge)
            mEdges[e1ni].mPrevIn = idx2;
        if (e1po != invalid_graph_edge)
            mEdges[e1po].mNextOut = idx2;
        if (e1no != invalid_graph_edge)
            mEdges[e1no].mPrevOut = idx2;

        if (e2pi != invalid_graph_edge)
            mEdges[e2pi].mNextIn = idx1;
        if (e2ni != invalid_graph_edge)
            mEdges[e2ni].mPrevIn = idx1;
        if (e2po != invalid_graph_edge)
            mEdges[e2po].mNextOut = idx1;
        if (e2no != invalid_graph_edge)
            mEdges[e2no].mPrevOut = idx1;

        mIdToEdge[std::make_pair(e1s.mId, e1d.mId)] = idx2;
        mIdToEdge[std::make_pair(e2s.mId, e2d.mId)] = idx1;

        std::swap(e1, e2);
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    void Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::SwapVertices(graph_idx_t idx1, graph_idx_t idx2) {
        vertex_impl_t& v1 = mVertices[idx1];
        vertex_impl_t& v2 = mVertices[idx2];

        for (graph_idx_t it = v1.mFirstIn; it != invalid_graph_edge; it = mEdges[it].mNextIn) {
            mEdges[it].mDest = idx2;
        }

        for (graph_idx_t it = v1.mFirstOut; it != invalid_graph_edge; it = mEdges[it].mNextOut) {
            mEdges[it].mSource = idx2;
        }

        for (graph_idx_t it = v2.mFirstIn; it != invalid_graph_edge; it = mEdges[it].mNextIn) {
            mEdges[it].mDest = idx1;
        }

        for (graph_idx_t it = v2.mFirstOut; it != invalid_graph_edge; it = mEdges[it].mNextOut) {
            mEdges[it].mSource = idx1;
        }

        mIdToVertex[v1.mId] = idx2;
        mIdToVertex[v2.mId] = idx1;

        std::swap(v1, v2);
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    void Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::AddToInLinkedList(vertex_impl_t& v, edge_impl_t& e, graph_idx_t eidx) {
        if (v.mFirstIn == invalid_graph_edge) {
            v.mFirstIn = eidx;
            v.mLastIn = eidx;
        } else {
            edge_impl_t& le = mEdges[v.mLastIn];
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
            edge_impl_t& le = mEdges[v.mLastOut];
            le.mNextOut = eidx;
            e.mPrevOut = v.mLastOut;
            v.mLastOut = eidx;
        }
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    void Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::RemoveFromInLinkedList(edge_impl_t& e) {
        if (e.mPrevIn != invalid_graph_edge) {
            mEdges[e.mPrevIn].mNextIn = e.mNextIn;
        } else {
            mVertices[e.mDest].mFirstIn = e.mNextIn;
        }
        if (e.mNextIn != invalid_graph_edge) {
            mEdges[e.mNextIn].mPrevIn = e.mPrevIn;
        } else {
            mVertices[e.mDest].mLastIn = e.mPrevIn;
        }

        e.mNextIn = invalid_graph_edge;
        e.mPrevIn = invalid_graph_edge;
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    void Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::RemoveFromOutLinkedList(edge_impl_t& e) {
        if (e.mPrevOut != invalid_graph_edge) {
            mEdges[e.mPrevOut].mNextOut = e.mNextOut;
        } else {
            mVertices[e.mSource].mFirstOut = e.mNextOut;
        }
        if (e.mNextOut != invalid_graph_edge) {
            mEdges[e.mNextOut].mPrevOut = e.mPrevOut;
        } else {
            mVertices[e.mSource].mLastOut = e.mPrevOut;
        }

        e.mNextOut = invalid_graph_edge;
        e.mPrevOut = invalid_graph_edge;
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    std::tuple<graph_idx_t, graph_idx_t> Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::RemoveEdgeAtIdx(graph_idx_t idx) {
        if (idx != mEdges.size() - 1) {
            SwapEdges(idx, mEdges.size() - 1);
        }

        auto& e = mEdges.back();
        auto& s = mVertices[e.mSource];
        auto& d = mVertices[e.mDest];
        mIdToEdge.erase(std::make_pair(s.mId, d.mId));
        
        auto nextIn = e.mNextIn;
        auto nextOut = e.mNextOut;
        RemoveFromOutLinkedList(e);
        RemoveFromInLinkedList(e);
        mEdges.pop_back();

        return std::make_tuple(nextIn, nextOut);
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    void Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::RemoveVertexAtIdx(graph_idx_t idx) {
        const vertex_impl_t& v = mVertices[idx];

        for (graph_idx_t it = v.mFirstIn; it != invalid_graph_edge;) {
            it = std::get<0>(RemoveEdgeAtIdx(it));
        }

        for (graph_idx_t it = v.mFirstOut; it != invalid_graph_edge;) {
            it = std::get<1>(RemoveEdgeAtIdx(it));
        }

        if (idx != mVertices.size() - 1) {
            SwapVertices(idx, mVertices.size() - 1);
        }

        mIdToVertex.erase(mVertices.back().mId);
        mVertices.pop_back();
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    void Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::Clear() {
        mVertices.clear();
        mEdges.clear();
        mIdToVertex.clear();
        mIdToEdge.clear();
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    Collection<typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::template OutEdgeIterator<true>> 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::GetOutgoing(vertex_const_t vertex) const {

        return Collection<OutEdgeIterator<true>>{
            OutEdgeIterator<true>{vertex.mVertex->mFirstOut, *vertex.mVertex, *this},
            OutEdgeIterator<true>{invalid_graph_edge, *vertex.mVertex, *this}};
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    Collection<typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::template InEdgeIterator<true>> 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::GetIngoing(vertex_const_t vertex) const {
        
        return Collection<InEdgeIterator<true>>{
            InEdgeIterator<true>{vertex.mVertex->mFirstIn, *vertex.mVertex, *this},
            InEdgeIterator<true>{invalid_graph_edge, *vertex.mVertex, *this}};
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    Collection<typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::template OutEdgeIterator<false>> 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::GetOutgoing(vertex_t vertex) {

        return Collection<OutEdgeIterator<false>>{
            OutEdgeIterator<false>{vertex.mVertex->mFirstOut, *vertex.mVertex, *this},
            OutEdgeIterator<false>{invalid_graph_edge, *vertex.mVertex, *this}};
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    Collection<typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::template InEdgeIterator<false>> 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::GetIngoing(vertex_t vertex) {

        return Collection<InEdgeIterator<false>>{
            InEdgeIterator<false>{vertex.mVertex->mFirstIn, *vertex.mVertex, *this},
            InEdgeIterator<false>{invalid_graph_edge, *vertex.mVertex, *this}};
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
        auto it = mIdToVertex.find(id);
        if (it != mIdToVertex.end()) {
            return vertex_t{
                mVertices[it->second]
            };
        }
        else {
            return std::optional<vertex_t>{};
        }
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    std::optional<typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::vertex_const_t>
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::TryGetVertex(VertexId id) const {
        auto it = mIdToVertex.find(id);
        if (it != mIdToVertex.end()) {
            return vertex_const_t{
                mVertices[it->second]
            };
        }
        else {
            return std::optional<vertex_const_t>{};
        }
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    std::optional<typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::edge_t> 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::TryGetEdge(VertexId source, VertexId dest) {
        auto it = mIdToEdge.find(std::make_pair(source, dest));
        if (it != mIdToEdge.end()) {
            return edge_t{
                mEdges[it->second],
                mVertices[mEdges[it->second].mSource],
                mVertices[mEdges[it->second].mDest]
            };
        } else {
            return std::optional<edge_t>{};
        }
    }
    
    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    std::optional<typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::edge_const_t> 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::TryGetEdge(VertexId source, VertexId dest) const {
        auto it = mIdToEdge.find(std::make_pair(source, dest));
        if (it != mIdToEdge.end()) {
            return edge_const_t{
                mEdges[it->second],
                mVertices[mEdges[it->second].mSource],
                mVertices[mEdges[it->second].mDest]
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
        mIdToVertex.emplace(id, mVertices.size());
        mVertices.emplace_back(vertex_impl_t(id, std::move(data)));
        return vertex_t{
            mVertices.back()
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
        auto idx = mEdges.size();
        mIdToEdge.emplace(std::make_pair(source.Id(), dest.Id()), idx);

        graph_idx_t sidx = source.mVertex - &mVertices[0];
        graph_idx_t didx = dest.mVertex - &mVertices[0];

        mEdges.emplace_back(edge_impl_t(std::move(data), sidx, didx));

        vertex_impl_t& s = *source.mVertex;
        vertex_impl_t& d = *dest.mVertex;

        edge_impl_t& e = mEdges[idx];

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
                ingoing.emplace(edge.mSource);
            }
            for (auto edge : GetOutgoing(*it)) {
                outgoing.emplace(edge.mDest);
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
        RemoveEdgeAtIdx(edge.mEdge - &mEdges[0]);
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    void Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::RemoveEdge(edge_const_t edge) {
        RemoveEdgeAtIdx(edge.mEdge - &mEdges[0]);
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
        RemoveVertexAtIdx(vertex.mVertex - &mVertices[0]);
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    void Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::RemoveVertex(vertex_const_t vertex) {
        RemoveVertexAtIdx(vertex.mVertex - &mVertices[0]);
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
            EdgeIterator<true>{mEdges.size(), *this}
        };
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    Collection<typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::template EdgeIterator<false>> 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::GetEdges() {
        return Collection<EdgeIterator<false>>{
            EdgeIterator<false>{0, *this},
            EdgeIterator<false>{mEdges.size(), *this}
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
        return VertexIterator<false>{mVertices.size(), *this};
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::template VertexIterator<true> 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::begin() const {
        return VertexIterator<true>{0, *this};
    }
    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    typename Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::template VertexIterator<true> 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::end() const {
        return VertexIterator<true>{mVertices.size(), *this};
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    size_t Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::GetVertexCount() const {
        return mVertices.size();
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    size_t Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::GetEdgeCount() const {
        return mEdges.size();
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    Digraph<VertexId, NoGraphData, NoGraphData, VertexIdHash> 
        Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::CloneWithoutData() const {
        
        Digraph<VertexId, NoGraphData, NoGraphData, VertexIdHash> result;

        result.mVertices.reserve(mVertices.size());
        result.mEdges.reserve(mEdges.size());

        for (auto& v : mVertices) {
            DigraphVertexImpl<VertexId, NoGraphData> newVert{
                v.mId,
                NoGraphData{}
            };

            newVert.mFirstOut = v.mFirstOut;
            newVert.mFirstIn = v.mFirstIn;
            newVert.mLastOut = v.mLastOut;
            newVert.mLastIn = v.mLastIn;
            
            result.mVertices.emplace_back(std::move(newVert));
        }

        for (auto& e : mEdges) {
            DigraphEdgeImpl<NoGraphData> newEdge{
                NoGraphData{},
                e.mSource,
                e.mDest,
            };

            newEdge.mNextIn = e.mNextIn;
            newEdge.mPrevIn = e.mPrevIn;
            newEdge.mNextOut = e.mNextOut;
            newEdge.mPrevOut = e.mPrevOut;

            result.mEdges.emplace_back(std::move(newEdge));
        }

        result.mIdToVertex = mIdToVertex;
        result.mIdToEdge = mIdToEdge;

        return result;
    }

    template <typename VertexId, typename VertexData, typename EdgeData, typename VertexIdHash>
    bool Digraph<VertexId, VertexData, EdgeData, VertexIdHash>::Verify() const {
        for (graph_idx_t i = 0; i < mVertices.size(); ++i) {
            const auto& v = mVertices[i];

            auto first = v.mFirstIn;
            auto last = invalid_graph_vertex;

            for (graph_idx_t it = v.mFirstIn; it != invalid_graph_edge; it = mEdges[it].mNextIn) {
                const auto& e = mEdges[it];

                if (e.mPrevIn != last) {
                    return false;
                }

                last = it;
            }

            if (v.mLastIn != last) {
                return false;
            }
        }

        for (graph_idx_t i = 0; i < mVertices.size(); ++i) {
            const auto& v = mVertices[i];

            auto first = v.mFirstOut;
            auto last = invalid_graph_vertex;

            for (graph_idx_t it = v.mFirstOut; it != invalid_graph_edge; it = mEdges[it].mNextOut) {
                const auto& e = mEdges[it];

                if (e.mPrevOut != last) {
                    return false;
                }

                last = it;
            }

            if (v.mLastOut != last) {
                return false;
            }
        }
        
        for (auto [v_id, v_idx] : mIdToVertex) {
            if (mVertices[v_idx].mId != v_id)
                return false;
        }

        for (auto [e_v_id, e_idx] : mIdToEdge) {
            auto& edge = mEdges[e_idx];
            if (e_v_id.first != mVertices[edge.mSource].mId)
                return false;
            if (e_v_id.second != mVertices[edge.mDest].mId)
                return false;
        }

        return true;
    }
}