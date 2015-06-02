#ifndef UTIL_HASH_H
#define UTIL_HASH_H

#include <QPointer>

// Qt doesn't provide a qHash function for arbitrary QPointers,
// so we'll define our own here.
template <class T>
uint qHash(const QPointer<T>& p) {
    return qHash(p.operator->());
}

#endif
