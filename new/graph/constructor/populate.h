#pragma once

#include <QMenu>
#include <functional>
#include "graph/graph.h"

void emptyCallback(Node* n) { (void)n; }

void populateNodeMenu(QMenu* menu, Graph* g,
                      std::function<void(Node*)> callback=emptyCallback);
