#include <QSet>
#include <QHash>
#include <QSharedPointer>

/*
 *  Updates a proxy hash-map
 */
template <class Container, class Object, class Proxy, class Parent>
void updateHash(const Container& input, QHash<Object*, Proxy*>* hash,
                Parent* parent)
{
    QSet<Object*> set;
    for (auto i : input)
        set.insert(i);

    {
        auto itr = hash->begin();
        while (itr != hash->end())
            if (!set.contains(itr.key()))
            {
                (*itr)->deleteLater();
                itr = hash->erase(itr);
            }
            else
            {
                itr++;
            }
    }

    for (auto s : set)
        if (!hash->contains(s))
            (*hash)[s] = new Proxy(s, parent);
}

#define NULL_ON_DESTROYED(s) connect(s, &QObject::destroyed, [=]{ this->s = NULL; })
