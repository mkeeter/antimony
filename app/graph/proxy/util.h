#include <QSet>
#include <QHash>
#include <QSharedPointer>

/*
 *  Updates a proxy hash-map
 */
template <class Container, class Object, class Proxy, class Parent>
void updateHash(const Container& input, QHash<Object*, Proxy*>* hash,
                Parent* parent, bool null_on_destroyed=false)
{
    QSet<Object*> set;
    for (auto i : input)
        set.insert(i);

    {
        auto itr = hash->begin();
        while (itr != hash->end())
            if (!set.contains(itr.key()))
            {
                Proxy* ptr = *itr;
                itr = hash->erase(itr);
                delete ptr;
            }
            else
            {
                itr++;
            }
    }

    for (auto s : set)
        if (!hash->contains(s))
        {
            auto proxy = new Proxy(s, parent);
            (*hash)[s] = proxy;

            // Automatically prune hash when the proxy is destroyed
            // (in cases where the parent doesn't quite take ownership,
            // like Connections from a DatumProxy)
            if (null_on_destroyed)
                proxy->connect(proxy, &QObject::destroyed,
                        [=]{ hash->remove(s); });
        }
}
