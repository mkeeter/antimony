#ifndef UNDO_STACK
#define UNDO_STACK

#include <QUndoStack>

class UndoStack : public QUndoStack
{
public:
    UndoStack(QObject* parent=NULL);

    template<typename T> void swapPointer(T* a, T* b)
    {
        for (auto p : findChildren<QPointer<T>*>())
            if (p->data() == a)
                *p = b;
    }
};

#endif
