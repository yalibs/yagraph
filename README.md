# YA-GRAPH
Yet another C/C++ directed graph implementation. Header-only directed graph structure.

## Example Usage
```c++
#include <graph> // include the library
// ...
/// build a graph with nodes, which hold std::string's, and are keyed on an int 
/// and edges, which also hold std::string's 
auto g = ya::graph_builder<std::string,std::string,int>{}
            .add_node(0,"node1") // add nodes individually
            .add_nodes({{1,"node2"},{2,"node3"}}) // or add nodes via initializer_lists
            .add_edge(0,1,"edge1")
            .add_edges({{2,1,"edge2"},{2,0,"edge3"}}) // You can add edges via initializer_lists
            .add_node(5,"node5") // You dont need to provide the objects in order
            // ... add more of your graph with .add_node(s) and .add_edge(s)
            //.optimize() /* Optionally optimize the data before construction */
            //.validate() // optional manual validation NOTE: This throws exceptions if your construct is not valid (i.e. edges pointing to non-existent nodes) 
            .build(); // Will call .validate for you (also throws exception if .validate does)
```
