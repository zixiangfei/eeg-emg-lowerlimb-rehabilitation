#include "emgdataget.h"

EmgDataGet::EmgDataGet()
{
    running = true;
}

/*
 * 初始化python解释器
 */
void EmgDataGet::PythonInit() {
    //python只需要初始化一次
    if(!Py_IsInitialized()) {
        //1.初始化python解释器，调用操作的第一步
        Py_Initialize();
        if(!Py_IsInitialized()) {
            qDebug("Python初始化失败");
        }
        else {
            // 初始化线程支持
            PyEval_InitThreads();
            // 启动子线程前执行，为了释放PyEval_InitThreads获得的全局锁，否则子线程可能无法获取到全局锁
            PyEval_ReleaseThread(PyThreadState_Get());
            qDebug("初始化Python解释器成功");
        }
   }
}

/*
 * 连接肌电数据采集设备，并且获取
 */
void EmgDataGet::run() {
    PythonInit();
    //PyErr_Print();
    class PyThreadStateLock PyThreadLock;
    TEST
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('/home/pi/Desktop/RecoveredEquipment/PythonCode')"); //环境路径
    PyObject *establishConnect = PyImport_ImportModule("establishConnect"); //找到对应文件
    if(!establishConnect) {
        qDebug() << "not find module establishConnect\n";
        return;
    }
    PyObject *funConnectEmgDevice = PyObject_GetAttrString(establishConnect, "connectEmgDevice"); //设备连接函数
    if(!funConnectEmgDevice) {
        qDebug() << "not find func ConnectEmgDevice\n";
        return;
    }
    PyObject *PyConnectState = PyObject_CallObject(funConnectEmgDevice, NULL);
    int connectState = PyLong_AsLong(PyConnectState); //检查设备连接情况返回-1连接失败，返回0连接成功
    if(connectState == CONNECT_SUCC) {
        PyObject *onlineFetchData = PyImport_ImportModule("online_fetch_data"); //分析解析数据
        if(!onlineFetchData) {
            qDebug() << "not find module online_fetch_data";
            return;
        }
        PyObject *funAnalysisData = PyObject_GetAttrString(onlineFetchData, "analysisData");
        running = true;
        while(running){
            PyObject *PyPotentialState = PyObject_CallObject(funAnalysisData, NULL);
            int potentialState = PyLong_AsLong(PyPotentialState);
            //qDebug() << potentialState << '\n';
            if(potentialState == RESTING_POTENTIAL) {
                emit RestingPotentialSignal();
            }
            if(potentialState == BEND_POTENTIAL) {
                emit BendPotentialSignal();
            }
            if(potentialState == STRETCH_POTENTIAL) {
                emit StretchPotentialSignal();
            }
        }
    }
    emit RestingPotentialSignal();
    //Py_Finalize();
}
