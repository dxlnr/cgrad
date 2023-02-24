#define PY_SSIZE_T_CLEAN
#include <Python.h>

/* enum Ops {Add, Sub, Mul}; */

typedef struct 
{
    PyObject_HEAD
    float storage;
    /* float grad; */
    /* enum Ops ops; */
    /* struct Value *operands; */
} Value;

static PyObject *
Value_new(PyTypeObject *type, PyObject *args)
{
    /* float x; */
    /* PyArg_ParseTuple(args, "i", &x); */
    
    Value *self;
    self = (Value *) type->tp_alloc(type, 0);
    if (self != NULL) {
        self -> storage = 0.0;
    }
    return (PyObject *) self;
}

static PyTypeObject ValueType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "engine.Engine",
    .tp_doc = PyDoc_STR("Value, which wraps a scalar value,\
    and also stores more information (AutogradMeta), which is needed for performing autograd."),
    .tp_basicsize = sizeof(Value),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = Value_new,
};

static PyModuleDef cmodule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "engine",
    .m_doc = "autograd engine as c module.",
    .m_size = -1,
};

PyMODINIT_FUNC
PyInit_engine(void)
{
    PyObject *m;
    if (PyType_Ready(&ValueType) < 0)
        return NULL;

    m = PyModule_Create(&cmodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&ValueType);
      
    PyModule_AddObject(m, "Value", (PyObject *) &ValueType);

    return m;
};
