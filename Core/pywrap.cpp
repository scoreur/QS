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





    qDebug()<<"start python";


}

void printDict(PyObject* obj) {
    if (!PyDict_Check(obj))
        return;
    PyObject *k, *keys;
    keys = PyDict_Keys(obj);
    for (int i = 0; i < PyList_GET_SIZE(keys); i++) {
        k = PyList_GET_ITEM(keys, i);
        char* c_name = PyString_AsString(k);
        printf("%s/n", c_name);
    }
}
void Pywrap::load(char **&argv){//under testing, do not use
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('/Users/user/Documents/compile/Qt5/QS/Core/py/')");
        PyObject *pName,*pModule,*pDict,*pFunc,*pArgs;
        pName = PyString_FromString("onset");
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
        printDict(pDict);
        pFunc = PyDict_GetItemString(pDict, "onset_svm");
        if ( !pFunc || !PyCallable_Check(pFunc) )
        {
            qDebug()<<"can't find function";
            return;
        }
        pArgs = PyTuple_New(4);
        PyTuple_SetItem(pArgs, 0, Py_BuildValue("s","/Users/user/Documents/compile/Qt5/QS/Core/py/piano.wav"));
        PyTuple_SetItem(pArgs, 1, Py_BuildValue("s","/Users/user/Documents/compile/Qt5/QS/Core/py/testpy2.txt"));
        PyTuple_SetItem(pArgs, 2, Py_BuildValue("i",0));
        PyTuple_SetItem(pArgs, 3, Py_BuildValue("i", 1));
        qDebug()<<"pass params success";



        PyObject_CallObject(pFunc, pArgs);

}

Pywrap::~Pywrap(){
    if(Py_IsInitialized())
        Py_Finalize();
    qDebug()<<"finalize python";

}

