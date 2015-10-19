#pragma once

#include "graph/node.h"

class Graph;

class GraphNode : public Node
{
public:
    explicit GraphNode(std::string name, Graph* root);
    explicit GraphNode(std::string name, uint32_t uid, Graph* root);

    Graph* getGraph() const { return subgraph.get(); }

    bool makeDatum(std::string name, PyTypeObject* type,
                   bool output);

    /*
     *  pyGetAttr is overloaded to add __subgraph as a lookup.
     */
    PyObject* pyGetAttr(std::string name, Downstream* caller,
                        uint8_t flags) const override;
protected:
    std::unique_ptr<Graph> subgraph;
};
