/* MIT License
 *
 * Copyright (c) 2022 Asger Gitz-Johansen
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef YAGRAPH_GRAPH_H
#define YAGRAPH_GRAPH_H
#include <vector>
#include <unordered_map>

namespace ya {
    template<typename node_data_t, typename edge_data_t, typename node_key_t> struct edge;
    template<typename node_data_t, typename edge_data_t, typename node_key_t> using edge_collection = std::unordered_map<edge_data_t, edge<node_data_t,edge_data_t,node_key_t>>;
    template<typename node_data_t, typename edge_data_t, typename node_key_t> using edge_refference = typename edge_collection<node_data_t,edge_data_t,node_key_t>::iterator;
    template<typename node_data_t, typename edge_data_t, typename node_key_t> struct node;
    template<typename node_data_t, typename edge_data_t, typename node_key_t> using node_collection = std::unordered_map<node_key_t, node<node_data_t,edge_data_t,node_key_t>>;
    template<typename node_data_t, typename edge_data_t, typename node_key_t> using node_refference = typename node_collection<node_data_t,edge_data_t,node_key_t>::iterator;
    template<typename node_data_t, typename edge_data_t, typename node_key_t> using node_const_ref  = typename ya::node_collection<node_data_t,edge_data_t,node_key_t>::const_iterator;
    template<typename node_data_t, typename edge_data_t, typename node_key_t> struct graph;

    template<typename node_data_t, typename edge_data_t, typename node_key_t>
    struct edge {
        edge_data_t data;
        node_refference<node_data_t, edge_data_t, node_key_t> source;
        node_refference<node_data_t, edge_data_t, node_key_t> target;
    };

    template<typename node_data_t, typename edge_data_t, typename node_key_t>
    struct node {
        node_data_t data;
        std::vector<edge_refference<node_data_t,edge_data_t,node_key_t>> outgoing_edges;
        std::vector<edge_refference<node_data_t,edge_data_t,node_key_t>> ingoing_edges;
    };

    template<typename node_data_t, typename edge_data_t, typename node_key_t>
    struct graph {
        node_collection<node_data_t,edge_data_t,node_key_t> nodes;
        edge_collection<node_data_t,edge_data_t,node_key_t> edges;
    };

    template<typename node_data_t, typename edge_data_t, typename node_key_t = node_data_t>
    struct graph_builder {
        graph_builder() : nodes{}, edges{} {}
        struct node_construction_object_separate_key {
            const node_key_t key;
            const node_data_t data;
            auto get_key() -> node_key_t { return key; }
            auto get_data() -> node_data_t { return data; }
        };
        struct node_construction_object_only_data {
            const node_key_t data;
            auto get_key() -> node_key_t { return data; }
            auto get_data() -> node_data_t { return data; }
        };
        using node_construction_object = std::conditional_t<std::is_same_v<node_data_t,node_key_t>, node_construction_object_only_data, node_construction_object_separate_key>;
        struct edge_construction_object {
            const node_key_t source;
            const node_key_t target;
            const edge_data_t data;
        };
        auto add_node(const node_construction_object& data) -> graph_builder<node_data_t, edge_data_t, node_key_t>&;
        auto add_nodes(const std::initializer_list<node_construction_object>& data) -> graph_builder<node_data_t, edge_data_t, node_key_t>&;
        auto add_edge(const node_key_t& source, const node_key_t& target, const edge_data_t& data) -> graph_builder<node_data_t, edge_data_t, node_key_t>&;
        auto add_edges(const std::initializer_list<edge_construction_object> &data) -> graph_builder<node_data_t, edge_data_t, node_key_t>&;
        auto validate() -> graph_builder<node_data_t, edge_data_t, node_key_t>&;
        auto is_valid() -> bool;
        auto optimize() -> graph_builder<node_data_t, edge_data_t, node_key_t>&;
        auto build() -> graph<node_data_t, edge_data_t, node_key_t>;
        auto build_heap() -> std::unique_ptr<graph<node_data_t, edge_data_t, node_key_t>>;

        std::vector<node_construction_object> nodes;
        std::vector<edge_construction_object> edges;
    };

    template<typename node_data_t, typename edge_data_t, typename node_key_t>
    auto graph_builder<node_data_t,edge_data_t,node_key_t>::add_node(const node_construction_object& data) -> graph_builder<node_data_t,edge_data_t,node_key_t>& {
        nodes.push_back(data);
        return *this;
    }

    template<typename node_data_t, typename edge_data_t, typename node_key_t>
    auto graph_builder<node_data_t,edge_data_t,node_key_t>::add_nodes(const std::initializer_list<node_construction_object>& data) -> graph_builder<node_data_t,edge_data_t,node_key_t>& {
        for(const auto& d : data)
            nodes.push_back(d);
        return *this;
    }

    template<typename node_data_t, typename edge_data_t, typename node_key_t>
    auto graph_builder<node_data_t,edge_data_t,node_key_t>::add_edge(const node_key_t& source, const node_key_t& target, const edge_data_t &data) -> graph_builder<node_data_t,edge_data_t,node_key_t>& {
        edges.push_back({source, target, data});
        return *this;
    }

    template<typename node_data_t, typename edge_data_t, typename node_key_t>
    auto graph_builder<node_data_t,edge_data_t,node_key_t>::add_edges(const std::initializer_list<edge_construction_object> &data) -> graph_builder<node_data_t,edge_data_t,node_key_t>& {
        for(const auto& d : data)
            edges.push_back(d);
        return *this;
    }

    template<typename node_data_t, typename edge_data_t, typename node_key_t>
    auto graph_builder<node_data_t,edge_data_t,node_key_t>::validate() -> graph_builder<node_data_t,edge_data_t,node_key_t>& {
        node_collection<node_data_t, edge_data_t, node_key_t> check_nodes{};
        for(auto& nco : nodes)
            check_nodes[nco.get_key()] = {nco.get_data(), {}};
        for(auto& eco : edges) {
            bool source_and_target_exist = check_nodes.contains(eco.source) && check_nodes.contains(eco.target);
#ifndef NDEBUG
            if (!source_and_target_exist)
                throw std::range_error("invalid edge");
#else
            assert(source_and_target_exist);
#endif
        }
        return *this;
    }

    template<typename node_data_t, typename edge_data_t, typename node_key_t>
    auto graph_builder<node_data_t,edge_data_t,node_key_t>::optimize() -> graph_builder<node_data_t,edge_data_t,node_key_t>& {
        return *this; // TODO: Call injectable optimizer
    }

    template<typename node_data_t, typename edge_data_t, typename node_key_t>
    auto graph_builder<node_data_t,edge_data_t,node_key_t>::is_valid() -> bool {
        return false; // TODO: Implement
    }

    template<typename node_data_t, typename edge_data_t, typename node_key_t>
    auto graph_builder<node_data_t,edge_data_t,node_key_t>::build() -> graph<node_data_t, edge_data_t, node_key_t> {
        validate();
        graph<node_data_t,edge_data_t,node_key_t> result{};
        for(auto& nco : nodes)
            result.nodes[nco.get_key()] = {nco.get_data(), {}};
        for(auto& eco : edges) {
            auto source_it = result.nodes.find(eco.source);
            auto target_it = result.nodes.find(eco.target);
            result.edges[eco.data] = {eco.data, source_it, target_it};
            auto edge_it = result.edges.find(eco.data);
            if(edge_it == result.edges.end())
                throw std::logic_error("edge construction failed");
            source_it->second.outgoing_edges.push_back(edge_it);
            target_it->second.ingoing_edges.push_back(edge_it);
        }
        return result;
    }

    template<typename node_data_t, typename edge_data_t, typename node_key_t>
    auto graph_builder<node_data_t,edge_data_t,node_key_t>::build_heap() -> std::unique_ptr<graph<node_data_t,edge_data_t,node_key_t>> {
        validate();
        auto result = std::make_unique<graph<node_data_t,edge_data_t,node_key_t>>();
        for(auto& nco : nodes)
            result->nodes[nco.get_key()] = {nco.get_data(), {}};
        for(auto& eco : edges) {
            auto source_it = result->nodes.find(eco.source);
            auto target_it = result->nodes.find(eco.target);
            result->edges[eco.data] = {eco.data, target_it};
            auto edge_it = result->edges.find(eco.data);
            if(edge_it == result->edges.end())
                throw std::logic_error("edge construction failed");
            source_it->second.outgoing_edges.push_back(edge_it);
        }
        return result;
    }
}

namespace ya {
    template <typename T, typename = std::void_t<>>
    struct is_std_hashable : std::false_type { };

    template <typename T>
    struct is_std_hashable<T, std::void_t<decltype(std::declval<std::hash<T>>()(std::declval<T>()))>> : std::true_type { };

    template <typename T>
    constexpr bool is_std_hashable_v = is_std_hashable<T>::value;

    template<typename node_data_t, typename edge_data_t, typename node_key_t>
    struct node_ref_hasher {
        static_assert(is_std_hashable_v<node_key_t>, "Node key type must be std hashable. Either implement an std::hash overload or provide a different node key type");
        auto operator()(const ya::node_refference<node_data_t,edge_data_t,node_key_t>& r) const -> size_t {
            return std::hash<node_key_t>{}(r->first);
        }
    };
}

#endif //YAGRAPH_GRAPH_H
