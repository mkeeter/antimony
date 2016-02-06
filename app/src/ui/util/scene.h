template <class A, class N, class T>
void pruneHash(const QSet<A*>& set, QHash<N*, T>* hash)
{
    auto itr = hash->begin();
    while (itr != hash->end())
        if (!set.contains(itr.key()))
            itr = hash->erase(itr);
        else
            itr++;
}
