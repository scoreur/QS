#include "pywrap.h"
#include "Python.h"
#include <QDebug>
Pywrap::Pywrap()
{
    Py_Initialize();

    if ( !Py_IsInitialized() )
    {
        qDebug()<<"init python failed";
        return;
    }
    PyRun_SimpleString("import sys");




    qDebug()<<"start python";


}
void Pywrap::load(char **&argv){
    PyRun_SimpleString("sys.path.append('/Users/user/Documents/compile/Eigen/wrap/')");
        PyObject *pName,*pModule,*pDict,*pFunc,*pArgs;
        pName = PyString_FromString("api");
        pModule = PyImport_Import(pName);
        if ( !pModule )
        {
            qDebug()<< "can't find .py";
            return;
        }
        qDebug()<<"load dict";
        pDict = PyModule_GetDict(pModule);
        if ( !pDict )
        {
            return;
        }
        pFunc = PyDict_GetItemString(pDict, "fCQT");
        if ( !pFunc || !PyCallable_Check(pFunc) )
        {
            qDebug()<<"can't find function";
            return;
        }
        pArgs = PyTuple_New(5);
        PyTuple_SetItem(pArgs, 0, Py_BuildValue("s",argv[1]));
        PyTuple_SetItem(pArgs, 1, Py_BuildValue("s",argv[2]));
        PyTuple_SetItem(pArgs, 2, Py_BuildValue("s",argv[3]));
        PyTuple_SetItem(pArgs, 3, Py_BuildValue("i", 0));
        PyTuple_SetItem(pArgs, 4, Py_BuildValue("i", 1));


        PyObject_CallObject(pFunc, pArgs);

}

Pywrap::~Pywrap(){
    if(Py_IsInitialized())
        Py_Finalize();
    qDebug()<<"finalize python";

}

