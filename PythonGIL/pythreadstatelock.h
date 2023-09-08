#ifndef PYTHREADSTATELOCK_H
#define PYTHREADSTATELOCK_H
#include <Python.h>

class PyThreadStateLock
{
public:
    PyThreadStateLock();
    ~PyThreadStateLock();

private:
    PyGILState_STATE gstate;
    PyThreadState *_save;
    int nStatus;
};

#endif // PYTHREADSTATELOCK_H
