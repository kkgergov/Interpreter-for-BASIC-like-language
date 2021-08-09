#ifndef AST_SOURCE
#define AST_SOURCE

inline void Bin_OP::accept(Visitor &v)
{
    v.visit(this);
}

inline void Num::accept(Visitor &v)
{
    v.visit(this);
}

inline void Un_OP::accept(Visitor &v)
{
    v.visit(this);
}

inline void Var::accept(Visitor &v)
{
    v.visit(this);
}

inline void BlockCode::accept(Visitor &v)
{
    v.visit(this);
}

inline void VarAssign::accept(Visitor &v)
{
    v.visit(this);
}

inline void ArrAssign::accept(Visitor &v)
{
    v.visit(this);
}

inline void VarDecl::accept(Visitor &v)
{
    v.visit(this);
}

inline void ArrDecl::accept(Visitor &v)
{
    v.visit(this);
}

inline void IfElse::accept(Visitor &v)
{
    v.visit(this);
}

inline void While::accept(Visitor &v)
{
    v.visit(this);
}

inline void GoTo::accept(Visitor &v)
{
    v.visit(this);
}

inline void Label::accept(Visitor &v)
{
    v.visit(this);
}

inline void Print::accept(Visitor &v)
{
    v.visit(this);
}

inline void ReadVar::accept(Visitor &v)
{
    v.visit(this);
}

inline void ReadArr::accept(Visitor &v)
{
    v.visit(this);
}

inline void NO_OP::accept(Visitor &v)
{
    v.visit(this);
}

inline void Array::accept(Visitor &v)
{
    v.visit(this);
}

#endif