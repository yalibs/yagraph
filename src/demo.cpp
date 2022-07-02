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
#include <iostream>
#include <graph>

/// Example structs for edge and graph-vertex (also called node) data:
/// A graph consists of nodes (containing a "name" property in this example)
/// and nodes are connected with edges (containing a "descriptor" property in this example)
struct node_data { std::string name; };
struct edge_data { std::string descriptor; };
/// Both nodes and edges are required to be hashable
namespace std {
    template<>
    struct hash<edge_data> {
        inline auto operator()(const edge_data& d) const -> size_t {
            return std::hash<std::string>{}(d.descriptor);
        }
    };
    template<>
    struct hash<node_data> {
        inline auto operator()(const node_data& d) const -> size_t {
            return std::hash<std::string>{}(d.name);
        }
    };
}
/// Hashable objects should also be comparable (to detect hash-collisions)
inline auto operator==(const edge_data& a, const edge_data& b) { return a.descriptor == b.descriptor; }
inline auto operator==(const node_data& a, const node_data& b) { return a.name == b.name; }

int main(int argc, char** argv) {
    /// Construct graphs with the ya::graph_builder factory
    /// the ya::graph_builder template arguments refer to <node_data_structure, edge_data_structure, node_key_type>
    /// The node_key_type is important for the construction of edges. In this simple example it looks a bit redundant
    /// but once your node's data-structure becomes more complicated, this setup makes more sense.
    auto g = ya::graph_builder<node_data,edge_data,std::string>{}
            .add_node("0", {"A"})                                   /// You can add nodes individually
            .add_node("1", {"B"})
            .add_edge("0","1",{"x:=1"})                             /// You can add edges individually (from, to, data)
            .add_nodes({{"2", {"C"}},{"3", {"D"}}})                 /// You can add nodes via initializer_lists
            .add_edges({{"2","1",{"x:=2"}},{"3","4",{"x:=3"}}})     /// You can add edges via initializer_lists
            .add_node("5", {"F"})                                   /// You dont need to provide the objects in order
            .add_node("4", {"E"})                                   /// You dont need to provide the objects in order
            //.optimize()                                           /// Optionally optimize the data before construction
            .build();                                               /// Finalize and construct the graph
    /// Now you have a graph object.
    /// This example graph should look something like so:
    ///
    /// [A] -{x:=1}-> [B] <-{x:=2}- [C]
    /// [D] -{x:=3}-> [E]           [F]
    ///
    /// Let's iterate through all the nodes and their associated edges
    for(auto& n : g.nodes) {
        std::cout << n.second.data.name << ":\n";
        for(auto& e : n.second.outgoing_edges)
            std::cout << "<" << n.second.data.name << ", '" << e->second.data.descriptor
                      << "', " << e->second.target->second.data.name << ">\n";
    }
    /// Expected output is something like this:   (order might be different, depending on your system's hashing implementation(s))
    // E:
    // D:
    // <D, 'x:=3', E>
    // C:
    // <C, 'x:=2', B>
    // B:
    // F:
    // A:
    // <A, 'x:=1', B>
    return 0;
}
