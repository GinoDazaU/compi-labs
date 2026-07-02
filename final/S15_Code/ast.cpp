// =============================================================================
// ast.cpp — Implementación de los nodos del AST
// =============================================================================

#include "ast.h"
#include "visitor.h"
#include <iostream>

// =============================================================================
// Exp
// =============================================================================

Exp::~Exp() {}

std::string Exp::binopToChar(BinaryOp op) {
  switch (op) {
  case PLUS_OP:
    return "+";
  case MINUS_OP:
    return "-";
  case MUL_OP:
    return "*";
  case DIV_OP:
    return "/";
  case POW_OP:
    return "**";
  case LE_OP:
    return "<";
  case GT_OP:
    return ">";
  case LEQ_OP:
    return "<=";
  case GEQ_OP:
    return ">=";
  case EQ_OP:
    return "==";
  case NE_OP:
    return "!=";
  case AND_OP:
    return "&&";
  case OR_OP:
    return "||";
  default:
    return "?";
  }
}

// =============================================================================
// BinaryExp
// =============================================================================

BinaryExp::BinaryExp(Exp *l, Exp *r, BinaryOp o) : left(l), right(r), op(o) {}

BinaryExp::~BinaryExp() {
  delete left;
  delete right;
}

// =============================================================================
// UnaryExp
// =============================================================================

UnaryExp::UnaryExp(Exp *o) : operand(o) {}

UnaryExp::~UnaryExp() { delete operand; }

// =============================================================================
// NumberExp
// =============================================================================

NumberExp::NumberExp(int v) : value(v) {}

NumberExp::~NumberExp() {}

// =============================================================================
// IdExp
// =============================================================================

IdExp::IdExp(const std::string &v) : value(v) {}

IdExp::~IdExp() {}

// =============================================================================
// IndexExp
// =============================================================================
IndexExp::~IndexExp() { delete index; }
IndexExp::IndexExp(const std::string &name, Exp *index)
    : name(name), index(index) {}

// =============================================================================
// FieldExp
// =============================================================================

FieldExp::~FieldExp() {}
FieldExp::FieldExp(const std::string &object, const std::string &field)
    : object(object), field(field) {}

// =============================================================================
// Stm
// =============================================================================

Stm::~Stm() {}

// =============================================================================
// AssignStm
// =============================================================================

AssignStm::AssignStm(Exp *target, Exp *expr) : target(target), e(expr) {}

AssignStm::~AssignStm() {}

// =============================================================================
// ExpListSize
// =============================================================================

ExpListSize::ExpListSize(std::string t, Exp *s) : type(t), size(s) {}

ExpListSize::~ExpListSize() {}

// =============================================================================
// ExpListVals
// =============================================================================

ExpListVals::ExpListVals(std::string t) : type(t) {}

ExpListVals::~ExpListVals() {}

// =============================================================================
// ExpMatrixSize
// =============================================================================

ExpMatrixSize::ExpMatrixSize(std::string t, Exp *r, Exp *c)
    : type(t), rows(r), cols(c) {}

ExpMatrixSize::~ExpMatrixSize() {
  delete rows;
  delete cols;
}

// =============================================================================
// ExpMatrixVals
// =============================================================================

ExpMatrixVals::ExpMatrixVals(std::string t, Exp *r, Exp *c)
    : type(t), rows(r), cols(c) {}

ExpMatrixVals::~ExpMatrixVals() {
  delete rows;
  delete cols;
}

// =============================================================================
// MatrixExp
// =============================================================================

MatrixExp::~MatrixExp() {
  delete row;
  delete col;
}

MatrixExp::MatrixExp(const std::string &name, Exp *row, Exp *col)
    : name(name), row(row), col(col) {}

// =============================================================================
// PrintStm
// =============================================================================

PrintStm::PrintStm(Exp *expresion) : e(expresion) {}

PrintStm::~PrintStm() {}

// =============================================================================
// IfStm / WhileStm
// =============================================================================

IfStm::IfStm(Exp *c, Body *t, Body *e) : condition(c), then(t), els(e) {}

WhileStm::WhileStm(Exp *c, Body *t) : condition(c), b(t) {}

// =============================================================================
// DoWhileStm
// =============================================================================

DoWhileStm::DoWhileStm(Body *body, Exp *cond) : b(body), condition(cond) {}

// =============================================================================
// BreakStm
// =============================================================================

// Constructor generado implícitamente

// =============================================================================
// SwitchStm
// =============================================================================

SwitchStm::SwitchStm(Exp *expr) : e(expr) {}

// =============================================================================
// VarDec
// =============================================================================

VarDec::VarDec() {}

VarDec::~VarDec() {}
// =============================================================================
// Body
// =============================================================================

Body::Body()
    : declarations(std::list<VarDec *>()), StmList(std::list<Stm *>()) {}

Body::~Body() {}
