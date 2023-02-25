#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <math.h>

/* enum Ops {Add, Sub, Mul}; */

typedef struct 
{
    PyObject_HEAD
    double data;
    double grad;
    /* enum Ops ops; */
    /* struct Value *operands; */
} Value;

static PyTypeObject ValueType;

static void Value_dealloc(Value *self)
{
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static PyObject *
Value_new(PyTypeObject *type, PyObject *args)
{
    double data;
    if (!PyArg_ParseTuple(args, "d", &data))
        return NULL;
    
    Value *self;
    self = (Value *) type->tp_alloc(type, 0);
    if (self != NULL) {
        self -> data = data;
        self -> grad = 0.0;
    }
    return (PyObject *) self;
}

/* 
 * Implement Operand Options for Value
 *
 * Addition, Subtraction, Mulitplication, Power, Negative
*/
PyObject *value_add(PyObject *self, PyObject *other)
{
    Value *res = (Value *) ValueType.tp_alloc(&ValueType, 0); 
    res->data = ((Value *) self)->data + ((Value *) other)->data;
    res->grad = 0.0;
    return (PyObject *) res;
}

PyObject *value_sub(PyObject *self, PyObject *other)
{
    Value *res = (Value *) ValueType.tp_alloc(&ValueType, 0); 
    res->data = ((Value *) self)->data - ((Value *) other)->data;
    res->grad = 0.0;
    return (PyObject *) res;
}

PyObject *value_mul(PyObject *self, PyObject *other)
{
    Value *res = (Value *) ValueType.tp_alloc(&ValueType, 0); 
    res->data = ((Value *) self)->data * ((Value *) other)->data;
    res->grad = 0.0;
    return (PyObject *) res;
}

PyObject *value_pow(PyObject *self, PyObject *power)
{
    Value *res = (Value *) ValueType.tp_alloc(&ValueType, 0);
    res->data = pow(((Value *)self)->data, PyFloat_AsDouble(power));
    res->grad = 0.0;
    return (PyObject *) res;
}

PyObject *value_neg(PyObject *self)
{
    ((Value *) self)->data = ((Value *) self)->data * (double) (-1);
    return (PyObject *) self;
}

static PyNumberMethods Value_as_number_module = {
    value_add,
    value_sub,
    value_mul,
    0,
    0,
    value_pow,
    value_neg,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0, 
    0,
    0,
    0,
    0, 
};

/*
 * Getter & Setter for Value
 */
static PyObject * value_get_data(Value *self, void *closure)
{
    return (PyObject *) PyFloat_FromDouble((double) self->data);
}

static PyGetSetDef Value_getsetters[] = {
    {"data", (getter) value_get_data, "data stored in value", NULL},
    {NULL}  
};


/* 
 * Custom Methods for Value
 */
static PyObject *value_relu(Value *self, PyObject *Py_UNUSED(ignored))
{
    if (self->data < 0)
        self->data = 0.0;
    return (PyObject *) self;
}

static PyMethodDef Value_methods[] = {
    {"relu", (PyCFunction) value_relu, METH_NOARGS,
     "ReLU (rectified linear unit) activation function."
    },
    {NULL} 
};

/*
 * Compose the meta data of Value type & define the module.
 */
static PyTypeObject ValueType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "Value",
    .tp_doc = PyDoc_STR("Value, which wraps a scalar value,\
    and also stores more information (AutogradMeta), which is needed for performing autograd."),
    .tp_basicsize = sizeof(Value),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = Value_new,
    .tp_dealloc = (destructor) Value_dealloc,
    /* .tp_members = Value_members, */
    .tp_getset = Value_getsetters,
    .tp_methods = Value_methods,
    .tp_as_number = &Value_as_number_module,
};

static PyModuleDef engine_module = {
    PyModuleDef_HEAD_INIT,
    .m_name = "engine",
    .m_doc = "autograd engine as c module.",
    .m_size = -1,
};

/*
 * Initialize the module and ship the bindings.
 */ 
PyMODINIT_FUNC
PyInit_engine(void)
{
    PyObject *m;
    if (PyType_Ready(&ValueType) < 0)
        return NULL;

    m = PyModule_Create(&engine_module);
    if (m == NULL)
        return NULL;

    Py_INCREF(&ValueType);
      
    PyModule_AddObject(m, "Value", (PyObject *) &ValueType);

    return m;
};
