enum Ops {Add, Sub, Mul};

typedef struct 
{
    float storage;
    float grad;
    enum Ops ops;
    struct Variable *operands;
} Variable;
