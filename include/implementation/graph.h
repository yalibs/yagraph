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
    template<typename node_data_t, typename edge_data_t, typename node_key_t> struct graph;

    template<typename node_data_t, typename edge_data_t, typename node_key_t>
    struct edge {
        edge_data_t data;
        node_refference<node_data_t, edge_data_t, node_key_t> target;
    };

    template<typename node_data_t, typename edge_data_t, typename node_key_t>
    struct node {
        node_data_t data;
        std::vector<edge_refference<node_data_t,edge_data_t,node_key_t>> outgoing_edges;
    };

    template<typename node_data_t, typename edge_data_t, typename node_key_t>
    struct graph {
        node_collection<node_data_t,edge_data_t,node_key_t> nodes;
        edge_collection<node_data_t,edge_data_t,node_key_t> edges;
    };

    template<typename node_data_t, typename edge_data_t, typename node_key_t = node_data_t>
    struct graph_builder {
        graph_builder() : nodes{}, edges{} {}
        struct node_construction_object {
            const node_key_t key;
            const node_data_t data;
        };
        struct edge_construction_object {
            const node_key_t source;
            const node_key_t target;
            const edge_data_t data;
        };
        auto add_node(const node_key_t& key, const node_data_t& data) -> graph_builder<node_data_t, edge_data_t, node_key_t>&;
        auto add_nodes(const std::initializer_list<node_construction_object>& data) -> graph_builder<node_data_t, edge_data_t, node_key_t>&;
        auto add_edge(const node_key_t& source, const node_key_t& target, const edge_data_t& data) -> graph_builder<node_data_t, edge_data_t, node_key_t>&;
        auto add_edges(const std::initializer_list<edge_construction_object> &data) -> graph_builder<node_data_t, edge_data_t, node_key_t>&;
        auto validate() -> graph_builder<node_data_t, edge_data_t, node_key_t>&;
        auto is_valid() -> bool;
        auto optimize() -> graph_builder<node_data_t, edge_data_t, node_key_t>&;
        auto build() -> graph<node_data_t, edge_data_t, node_key_t>;

        std::vector<node_construction_object> nodes;
        std::vector<edge_construction_object> edges;
    };

    template<typename node_data_t, typename edge_data_t, typename node_key_t>
    auto graph_builder<node_data_t,edge_data_t,node_key_t>::add_node(const node_key_t& key, const node_data_t& data) -> graph_builder<node_data_t,edge_data_t,node_key_t>& {
        nodes.push_back({key, data});
        return *this;
    }

    template<typename node_data_t, typename edge_data_t, typename node_key_t>
    auto graph_builder<node_data_t,edge_data_t,node_key_t>::add_nodes(const std::initializer_list<node_construction_object>& data) -> graph_builder<node_data_t,edge_data_t,node_key_t>& {
        for(const auto& d : data)
            nodes.push_back({d.key, d.data});
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
            check_nodes[nco.key] = {nco.data, {}};
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
        return *this; // TODO: Call injectable optimizer, TODO: Implement injectable optimizer(s)
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
            result.nodes[nco.key] = {nco.data, {}};
        for(auto& eco : edges) {
            auto source_it = result.nodes.find(eco.source);
            auto target_it = result.nodes.find(eco.target);
            result.edges[eco.data] = {eco.data, target_it};
            auto edge_it = result.edges.find(eco.data);
            if(edge_it == result.edges.end())
                throw std::logic_error("edge construction failed");
            source_it->second.outgoing_edges.push_back(edge_it);
        }
        return result;
    }
}

#endif //YAGRAPH_GRAPH_H
