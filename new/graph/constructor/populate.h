#pragma once

#include <QMenu>

#include <functional>

#include "graph/graph.h"

void emptyNodeCallback(Node* n);
void emptyDatumCallback(Datum* d);

void populateNodeMenu(
        QMenu* menu, Graph* g,
        std::function<void(Node*)> callback=emptyNodeCallback,
        std::function<void(Datum*)> datum_callback=emptyDatumCallback);
