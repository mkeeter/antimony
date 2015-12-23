#include "graph/proxy/base_datum.h"
#include "canvas/connection/connection.h"

BaseDatumProxy::~BaseDatumProxy()
{
    // I don't understand why this is necessary, but doing the deletions
    // directly from the QHash causes crashes.
    QList<Connection*> cs;
    for (auto c : connections)
        cs.push_back(c);
    for (auto c : cs)
        delete c;
}
