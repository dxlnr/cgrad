#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <math.h>

typedef struct t_graph t_graph;

typedef struct 
{
    PyObject_HEAD
    double data;
    double grad;
    PyObject *operands;
    int ops;
    int visited;
    t_graph *topology;
} Value;

// Doubly Linked List 
typedef struct node {
    Value *value;
    struct node *next;
    struct node *prev;
} node_t;

// Topology graph storing a pointer to the head and tail of doubly linked list.
struct t_graph
{
    struct node *head;
    struct node *tail;
};

static PyTypeObject ValueType;

static int Value_traverse(Value *self, visitproc visit, void *arg)
{
    if (self->operands) {
        int visited = visit(self->operands, arg);
        if (visited != 0)
            return visited;
    }
    return 0;
}

static int Value_clear(Value *self) {
    // Clear out the operands.
    PyObject *ops;
    ops = self->operands;
    self->operands = NULL;
    Py_XDECREF(ops);

    return 0;
}

static void Value_dealloc(Value *self)
{
    Value_clear(self);

    if (((Value *) self)->topology) {
        node_t *node = ((Value *) self)->topology->tail;
        node_t *t;
        while (node) {
            t = node;
            node = node->prev;
            free(t);
        }
        free(((Value *) self)->topology);
    }
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
        self -> operands = PyTuple_New(0);
        self -> ops = -1;
        self -> visited = 0;
        self -> topology = NULL;
    }
    return (PyObject *) self;
}

/* 
 * Implement Operand Options for Value
 *
 * Addition, Subtraction, Mulitplication, (True) Division, Power, Negative
*/
PyObject *value_add(PyObject *self, PyObject *other)
{
    Value *res = (Value *) ValueType.tp_alloc(&ValueType, 0); 
    res->data = ((Value *) self)->data + ((Value *) other)->data;
    res->grad = 0.0;
    res->operands = PyTuple_Pack(2, self, other);
    res->ops = 0;
    return (PyObject *) res;
}

PyObject *value_mul(PyObject *self, PyObject *other)
{
    Value *res = (Value *) ValueType.tp_alloc(&ValueType, 0); 
    res->data = ((Value *) self)->data * ((Value *) other)->data;
    res->grad = 0.0;
    res->operands = PyTuple_Pack(2, self, other);
    res->ops = 1;
    return (PyObject *) res;
}

PyObject *value_pow(PyObject *self, PyObject *power)
{
    Value *res = (Value *) ValueType.tp_alloc(&ValueType, 0);
    res->data = pow(((Value *)self)->data, PyFloat_AsDouble(power));
    res->grad = 0.0;
    res->ops = 2;
    return (PyObject *) res;
}

PyObject *value_neg(PyObject *self)
{
    ((Value *) self)->data = ((Value *) self)->data * (double) (-1.0);
    return (PyObject *) self;
}

PyObject *value_sub(PyObject *self, PyObject *other) {
    return value_add(self, value_neg(other));
}

PyObject *value_truediv(PyObject *self, PyObject *other) {
    return value_mul(self, value_pow(other, PyFloat_FromDouble(-1.0)));
}


static PyNumberMethods Value_as_number_module = {
    // 
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
    // Core but inplace
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
    // Divide section (with inplace)
    0,
    value_truediv, 
    0,
    0,
    // Indexing
    0,
    // Matrix Mulitply
    0,
    0,
};

/*
 * Backward Compute
 */
static void *_backward_add(PyObject *self) {
    Value *op1 = ((Value *)PyTuple_GetItem(((Value *) self)->operands, 0));
    Value *op2 = ((Value *)PyTuple_GetItem(((Value *) self)->operands, 1));
  
    op1->grad += ((Value *) self)->grad;
    op2->grad += ((Value *) self)->grad;
    Py_RETURN_NONE;
}

static void *_backward_mul(PyObject *self) {
    Value *op1 = ((Value *)PyTuple_GetItem(((Value *) self)->operands, 0));
    Value *op2 = ((Value *)PyTuple_GetItem(((Value *) self)->operands, 1));
  
    op1->grad += (op2->data) * ((Value *) self)->grad;
    op2->grad += (op1->data) * ((Value *) self)->grad;
    Py_RETURN_NONE;
}

static void set_t_graph(PyObject *s, t_graph *topology)
{
    node_t *node = malloc(sizeof(node_t));
    node->value = ((Value *) s);
    node->next = NULL;

    if (!(topology->head)) {
        node->prev = NULL;
        topology->head = node;
        topology->tail = node;
    } else {
        node->prev = topology->tail;
        topology->tail->next = node;
        topology->tail = node;
    } 
}

static void build_topological_graph(PyObject *start_v, t_graph *topology)
{
    Value *v = ((Value *) start_v);
    if (!(v->visited)) {
        v->visited = 1;
        for (int i = 0; i < PyTuple_Size(v->operands); ++i) {
            PyObject *child = PyTuple_GetItem(((Value *) start_v)->operands, i);
            build_topological_graph(child, topology); 
        }
        set_t_graph(start_v, topology);
    }
}

// Define the function dispatch table for various backward functions.
typedef void backward_t(PyObject *self);

backward_t *_backward[2] = {
    _backward_add,
    _backward_mul,
};

/*
 * Getter & Setter for Value
 */
static PyObject * value_get_data(Value *self, void *closure)
{
    return (PyObject *) PyFloat_FromDouble((double) self->data);
}

static PyObject * value_get_grad(Value *self, void *closure)
{
    return (PyObject *) PyFloat_FromDouble((double) self->grad);
}

static PyGetSetDef Value_getsetters[] = {
    {"data", (getter) value_get_data, "data stored in Value", NULL},
    {"grad", (getter) value_get_grad, "gradient stored in Value", NULL},
    {NULL}  
};

/* 
 * Custom Methods for Value
 */
static PyObject *value_relu(Value *self, PyObject *Py_UNUSED(ignored))
{
    Value *res = (Value *) ValueType.tp_alloc(&ValueType, 0);

    if (self->data < 0) {
        res->data = (double) 0.0; 
    } else {
        res->data = (double) self->data; 
    }
    res->grad = 0.0;
    return (PyObject *) res;
}

static PyObject *backward(Value *self, PyObject *Py_UNUSED(ignored))
{
    
    if (!(self->topology)) {
        ((Value *) self)->topology = malloc(sizeof(t_graph));
        ((Value *) self)->topology->head = NULL;
        ((Value *) self)->topology->tail = NULL;
        build_topological_graph(self, ((Value *) self)->topology);
    }
    self->grad = 1.0;
    
    node_t *node = ((Value *) self)->topology->tail;
    while (node)
    {
        if (node->value->ops >= 0)
            _backward[node->value->ops](node->value);
        node = node->prev;
    }
    Py_RETURN_NONE;
}

static PyMethodDef Value_methods[] = {
    {"relu", (PyCFunction) value_relu, METH_NOARGS,
     "ReLU (rectified linear unit) activation function."
    },
    {"backward", (PyCFunction) backward, METH_NOARGS,
     "Computes the whole backward pass of a Value."
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
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = Value_new,
    .tp_dealloc = (destructor) Value_dealloc,
    .tp_traverse = (traverseproc) Value_traverse,
    .tp_clear = (inquiry) Value_clear,
    /* .tp_members = Value_members, */
    .tp_getset = Value_getsetters,
    .tp_methods = Value_methods,
    .tp_as_number = &Value_as_number_module,
    /* .tp_repr = (reprfunc) Value_repr, */
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
