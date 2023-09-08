#include "pythreadstatelock.h"

PyThreadStateLock::PyThreadStateLock()
{
    _save = nullptr;
    nStatus = 0;
    nStatus = PyGILState_Check();
    PyGILState_STATE gstate;
    if(!nStatus) {
        gstate = PyGILState_Ensure();
        nStatus = 1;
    }
    _save = PyEval_SaveThread();
    PyEval_RestoreThread(_save);
    //gstate = PyGILState_Ensure();
}

PyThreadStateLock::~PyThreadStateLock() {
    _save = PyEval_SaveThread();
    PyEval_RestoreThread(_save);
    if(nStatus) {
        PyGILState_Release(gstate);
    }
    //PyGILState_Release(gstate);
}
