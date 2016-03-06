#pragma once

#include "graph/node.h"

class Graph;

class GraphNode : public Node
{
public:
    explicit GraphNode(std::string name, Graph* root);
    explicit GraphNode(std::string name, uint32_t uid, Graph* root);

    Graph* getGraph() const { return subgraph.get(); }

    Datum* makeDatum(std::string name, PyTypeObject* type,
                     bool output);
    void removeDatum(Datum* d);

    /*
     *  Override triggerWatchers so that subgraph watchers
     *  are also triggered (for subdatum proxies)
     */
    void triggerWatchers() const override;

protected:
    std::unique_ptr<Graph> subgraph;
};
