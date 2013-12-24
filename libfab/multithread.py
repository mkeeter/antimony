import threading
from thread import LockType

def __monitor(interrupt, halt):
    """ @brief Waits for interrupt, then sets halt to 1
        @param interrupt threading.Event on which we wait
        @param halt ctypes.c_int used as a flag elsewhere
    """
    interrupt.wait()
    halt.value = 1


def multithread(target, args, interrupt=None, halt=None):
    """ @brief Runs a process on multiple threads.
        @details Must be called with both interrupt and halt or neither.  Interrupt is cleared before returning.
        @param target Callable function
        @param args List of argument tuples (one tuple per thread)
        @param interrupt threading.Event to halt thread or None
        @param halt ctypes.c_int used as an interrupt flag by target
    """

    if (halt is None) ^ (interrupt is None):
        raise ValueError('multithread must be invoked with both halt and interrupt (or neither)')

    threads = [threading.Thread(target=target, args=a) for a in args]

    if interrupt:
        m = threading.Thread(target=__monitor, args=(interrupt, halt))
        m.daemon = True
        m.start()

    for t in threads:   t.daemon = True
    for t in threads:   t.start()
    for t in threads:   t.join()

    if interrupt:
        interrupt.set()
        m.join()
        interrupt.clear()


def monothread(target, args, interrupt=None, halt=None):
    """ @brief Runs a process on a single thread
        @details Must be called with both interrupt and halt or neither.  Interrupt is cleared before returning.
        @param target Callable function
        @param args Argument tuples
        @param interrupt threading.Event to halt thread or None
        @param halt ctypes.c_int used as an interrupt flag by target
    """
    if (halt is None) ^ (interrupt is None):
        raise ValueError('monothread must be invoked with both halt and interrupt (or neither)')

    if interrupt:
        m = threading.Thread(target=__monitor, args=(interrupt, halt))
        m.daemon = True
        m.start()

    result = target(*args)

    if interrupt:
        interrupt.set()
        m.join()
        interrupt.clear()

    return result

def threadsafe(f):
    ''' A decorator that locks the arguments to a function,
        invokes the function, then unlocks the arguments and
        returns.'''
    def wrapped(*args, **kwargs):
        for a in set(list(args) + kwargs.values()):
            if hasattr(a, 'lock') and LockType and isinstance(a.lock, LockType):
                a.lock.acquire()
        result = f(*args, **kwargs)
        for a in set(list(args) + kwargs.values()):
            if hasattr(a, 'lock') and LockType and isinstance(a.lock, LockType):
                a.lock.release()
        return result
    return wrapped
