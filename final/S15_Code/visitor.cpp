// =============================================================================
// visitor.cpp — Implementación de TypeCheckerVisitor y GenCodeVisitor
// =============================================================================

#include "visitor.h"
#include "ast.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

namespace {
int constNumberValue(Exp *exp) {
  if (exp && exp->isConstant)
    return exp->constantValue;
  if (auto number = dynamic_cast<NumberExp *>(exp))
    return number->value;
  return 0;
}

bool constantExpValue(Exp *exp, int &value) {
  if (!exp)
    return false;
  if (exp->isConstant) {
    value = exp->constantValue;
    return true;
  }
  if (auto number = dynamic_cast<NumberExp *>(exp)) {
    value = number->value;
    return true;
  }
  return false;
}

int powInt(int base, int exponent) {
  if (exponent == 0)
    return 1;
  if (exponent == 1)
    return base;

  int half = powInt(base * base, exponent / 2);
  if (exponent % 2 == 0)
    return half;
  return half * base;
}

bool evalConstantBinary(BinaryOp op, int left, int right, int &result) {
  switch (op) {
  case PLUS_OP:
    result = left + right;
    return true;
  case MINUS_OP:
    result = left - right;
    return true;
  case MUL_OP:
    result = left * right;
    return true;
  case DIV_OP:
    if (right == 0)
      return false;
    result = left / right;
    return true;
  case POW_OP:
    result = powInt(left, right);
    return true;
  case LE_OP:
    result = left < right;
    return true;
  case GT_OP:
    result = left > right;
    return true;
  case LEQ_OP:
    result = left <= right;
    return true;
  case GEQ_OP:
    result = left >= right;
    return true;
  case EQ_OP:
    result = left == right;
    return true;
  case NE_OP:
    result = left != right;
    return true;
  case AND_OP:
    result = (left != 0) && (right != 0);
    return true;
  case OR_OP:
    result = (left != 0) || (right != 0);
    return true;
  }
  return false;
}

void emitPowerHelper(std::ostream &out) {
  out << "\n.globl potencia\n";
  out << "potencia:\n";
  out << "  pushq %rbp\n";
  out << "  movq %rsp, %rbp\n";
  out << "  cmpq $0, %rsi\n";
  out << "  je potencia_n_zero\n";
  out << "  cmpq $1, %rsi\n";
  out << "  je potencia_n_one\n";
  out << "  pushq %rdi\n";
  out << "  movq %rsi, %rdx\n";
  out << "  andq $1, %rdx\n";
  out << "  pushq %rdx\n";
  out << "  movq %rdi, %rax\n";
  out << "  imulq %rdi, %rax\n";
  out << "  movq %rax, %rdi\n";
  out << "  sarq $1, %rsi\n";
  out << "  call potencia\n";
  out << "  popq %rdx\n";
  out << "  popq %rcx\n";
  out << "  cmpq $0, %rdx\n";
  out << "  je potencia_end\n";
  out << "  imulq %rcx, %rax\n";
  out << "  jmp potencia_end\n";
  out << "potencia_n_zero:\n";
  out << "  movq $1, %rax\n";
  out << "  jmp potencia_end\n";
  out << "potencia_n_one:\n";
  out << "  movq %rdi, %rax\n";
  out << "  jmp potencia_end\n";
  out << "potencia_end:\n";
  out << "  leave\n";
  out << "  ret\n";
}

} // namespace

// =============================================================================
// Despacho del patrón Visitor (accept en cada nodo del AST)
// =============================================================================

int BinaryExp::accept(Visitor *v) { return v->visit(this); }
int NumberExp::accept(Visitor *v) { return v->visit(this); }
int IdExp::accept(Visitor *v) { return v->visit(this); }
int IdExp::computeAddress(Visitor *v) { return v->computeAddress(this); }
int Program::accept(Visitor *v) { return v->visit(this); }
int PrintStm::accept(Visitor *v) { return v->visit(this); }
int AssignStm::accept(Visitor *v) { return v->visit(this); }
int ExpListSize::accept(Visitor *v) { return v->visit(this); }
int ExpListVals::accept(Visitor *v) { return v->visit(this); }
int ExpMatrixSize::accept(Visitor *v) { return v->visit(this); }
int ExpMatrixVals::accept(Visitor *v) { return v->visit(this); }
int IfStm::accept(Visitor *v) { return v->visit(this); }
int WhileStm::accept(Visitor *v) { return v->visit(this); }
int Body::accept(Visitor *v) { return v->visit(this); }
int VarDec::accept(Visitor *v) { return v->visit(this); }
int FcallExp::accept(Visitor *v) { return v->visit(this); }
int FunDec::accept(Visitor *v) { return v->visit(this); }
int ReturnStm::accept(Visitor *v) { return v->visit(this); }
int DoWhileStm::accept(Visitor *v) { return v->visit(this); }
int BreakStm::accept(Visitor *v) { return v->visit(this); }
int SwitchStm::accept(Visitor *v) { return v->visit(this); }
int UnaryExp::accept(Visitor *v) { return v->visit(this); }
int IndexExp::accept(Visitor *v) { return v->visit(this); }
int IndexExp ::computeAddress(Visitor *v) { return v->computeAddress(this); }
int MatrixExp::accept(Visitor *v) { return v->visit(this); }
int MatrixExp::computeAddress(Visitor *v) { return v->computeAddress(this); }
int FieldExp::accept(Visitor *v) { return v->visit(this); }
int FieldExp::computeAddress(Visitor *v) { return v->computeAddress(this); }
int StructDec::accept(Visitor *v) { return v->visit(this); }

// =============================================================================
// Implementación de accept()
// =============================================================================

// =============================================================================
// TypeCheckerVisitor — Análisis semántico
// =============================================================================
// Responsabilidades:
//   1. Registrar las funciones y su aridad antes de analizar los cuerpos.
//   2. Verificar que cada variable usada haya sido declarada en el scope.
//   3. Verificar que cada función llamada exista y reciba el número correcto
//      de argumentos.
//   4. Contar las variables locales de cada función para que GenCodeVisitor
//      pueda reservar el espacio correcto en el stack frame.
// =============================================================================

// -----------------------------------------------------------------------------
// TypeChecker — punto de entrada
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::TypeChecker(Program *program) {
  for (auto sd : program->sdlist) {
    sd->accept(this);
  }

  // Primera pasada: registrar todas las funciones y su aridad.
  // Esto permite llamadas hacia adelante (funciones que se usan antes de
  // ser definidas en el orden textual del fuente).
  for (auto fd : program->fdlist) {
    funAridad[fd->nombre] = static_cast<int>(fd->Pnombres.size());
  }

  // Segunda pasada: analizar cada función
  for (auto fd : program->fdlist) {
    fd->accept(this);
  }

  return 0;
}

// -----------------------------------------------------------------------------
// visit(FunDec) — analiza una declaración de función
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(FunDec *fd) {
  funcionActual = fd->nombre;
  locales = 0;
  int parametros = static_cast<int>(fd->Pnombres.size());

  // Abrir scope de la función
  entorno.add_level();
  tiposVar.add_level();

  // Registrar los parámetros como variables del scope actual
  for (size_t i = 0; i < fd->Pnombres.size(); ++i) {
    entorno.add_var(fd->Pnombres[i], 0);
    tiposVar.add_var(fd->Pnombres[i], fd->Ptipos[i]);
  }

  // Analizar el cuerpo
  fd->cuerpo->accept(this);

  // Cerrar scope
  tiposVar.remove_level();
  entorno.remove_level();

  // Guardar el tamaño total del frame (parámetros + locales)
  funcontador[fd->nombre] = parametros + locales;
  return 0;
}

// -----------------------------------------------------------------------------
// visit(Body) — analiza declaraciones y sentencias del cuerpo
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(Body *body) {
  entorno.add_level();
  tiposVar.add_level();

  for (auto dec : body->declarations)
    dec->accept(this);
  for (auto stm : body->StmList)
    stm->accept(this);

  tiposVar.remove_level();
  entorno.remove_level();
  return 0;
}

// -----------------------------------------------------------------------------
// visit(VarDec) — registra las variables declaradas y cuenta los locales
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(VarDec *vd) {
  for (auto &nombre : vd->vars) {
    if (entorno.check(nombre)) {
      std::cerr << "[TypeChecker] Advertencia: la variable '" << nombre
                << "' ya fue declarada en este scope"
                << " (en función '" << funcionActual << "').\n";
    }
    entorno.add_var(nombre, 0);
    tiposVar.add_var(nombre, vd->type);
    locales++;
  }
  return 0;
}

int TypeCheckerVisitor::visit(StructDec *sd) {
  if (structFields.count(sd->name)) {
    throw std::runtime_error("[TypeChecker] Estructura duplicada: '" + sd->name + "'");
  }

  int totalFields = 0;
  std::unordered_map<std::string, bool> names;
  std::unordered_map<std::string, int> offsets;
  for (auto field : sd->fields) {
    for (auto &name : field->vars) {
      if (names.count(name)) {
        throw std::runtime_error("[TypeChecker] Campo duplicado '" + name +
                                 "' en estructura '" + sd->name + "'");
      }
      names[name] = true;
      offsets[name] = totalFields * 8;
      totalFields++;
    }
  }

  structFields[sd->name] = totalFields;
  structFieldOffsets[sd->name] = offsets;
  return 0;
}

// -----------------------------------------------------------------------------
// visit(IdExp) — verifica que la variable esté declarada
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(IdExp *exp) {
  if (!entorno.check(exp->value)) {
    throw std::runtime_error("[TypeChecker] Variable no declarada: '" +
                             exp->value + "' usada en la función '" +
                             funcionActual + "'");
  }
  return 0;
}

// -----------------------------------------------------------------------------
// (IdExp) — verifica que la variable esté declarada
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::computeAddress(IdExp *exp) { return 0; }

// -----------------------------------------------------------------------------
// visit(AssignStm) — verifica variable y evalúa expresión
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(AssignStm *stm) {
  stm->target->accept(this);
  stm->e->accept(this);
  return 0;
}

// -----------------------------------------------------------------------------
// visit(ExpListSize) — verifica variable y evalúa expresión
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(ExpListSize *StmListSize) {
  StmListSize->size->accept(this);
  return 0;
}

// -----------------------------------------------------------------------------
// visit(ExpListVals) — verifica variable y evalúa expresión
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(ExpListVals *StmListVals) {
  if (structFields.count(StmListVals->type)) {
    int esperados = structFields[StmListVals->type];
    int recibidos = static_cast<int>(StmListVals->values.size());
    if (recibidos != 0 && recibidos != esperados) {
      throw std::runtime_error("[TypeChecker] La estructura '" +
                               StmListVals->type + "' espera " +
                               std::to_string(esperados) +
                               " valor(es), pero se pasaron " +
                               std::to_string(recibidos));
    }
  }
  for (auto value : StmListVals->values)
    value->accept(this);
  return 0;
}

int TypeCheckerVisitor::visit(ExpMatrixSize *stm) {
  stm->rows->accept(this);
  stm->cols->accept(this);
  return 0;
}

int TypeCheckerVisitor::visit(ExpMatrixVals *stm) {
  stm->rows->accept(this);
  stm->cols->accept(this);
  for (auto value : stm->values)
    value->accept(this);
  return 0;
}

// -----------------------------------------------------------------------------
// visit(FcallExp) — verifica existencia de la función y aridad
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(FcallExp *fcall) {
  // Verificar que la función exista
  if (funAridad.find(fcall->nombre) == funAridad.end()) {
    throw std::runtime_error("[TypeChecker] Función no definida: '" +
                             fcall->nombre + "' llamada en '" + funcionActual +
                             "'");
  }

  // Verificar número de argumentos
  int esperados = funAridad[fcall->nombre];
  int recibidos = static_cast<int>(fcall->argumentos.size());
  if (recibidos != esperados) {
    throw std::runtime_error("[TypeChecker] La función '" + fcall->nombre +
                             "' espera " + std::to_string(esperados) +
                             " argumento(s), pero se pasaron " +
                             std::to_string(recibidos));
  }

  // Verificar cada argumento
  for (auto arg : fcall->argumentos)
    arg->accept(this);

  return 0;
}

// -----------------------------------------------------------------------------
// visit(IfStm) — cuenta máximo de locales entre ramas
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(IfStm *stm) {
  stm->condition->accept(this);

  int base = locales;

  // Rama then
  locales = 0;
  stm->then->accept(this);
  int maxLocales = locales;

  // Rama else (opcional)
  if (stm->els) {
    locales = 0;
    stm->els->accept(this);
    maxLocales = std::max(maxLocales, locales);
  }

  // El frame necesita el máximo de ambas ramas
  locales = base + maxLocales;
  return 0;
}

// -----------------------------------------------------------------------------
// visit(WhileStm)
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(WhileStm *stm) {
  stm->condition->accept(this);
  stm->b->accept(this);
  return 0;
}

// -----------------------------------------------------------------------------
// visit(PrintStm)
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(PrintStm *stm) {
  stm->e->accept(this);
  return 0;
}

// -----------------------------------------------------------------------------
// visit(ReturnStm)
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(ReturnStm *r) {
  r->e->accept(this);
  return 0;
}

// -----------------------------------------------------------------------------
// Nodos que no requieren verificación semántica adicional
// -----------------------------------------------------------------------------

int TypeCheckerVisitor::visit(BinaryExp *exp) {
  exp->left->accept(this);
  exp->right->accept(this);
  return 0;
}

int TypeCheckerVisitor::visit(UnaryExp *exp) {
  exp->operand->accept(this);
  return 0;
}
int TypeCheckerVisitor::visit(IndexExp *exp) {
  if (!entorno.check(exp->name)) {
    throw std::runtime_error("[TypeChecker] Variable no declarada: '" +
                             exp->name + "' usada en la función '" +
                             funcionActual + "'");
  }
  exp->index->accept(this);
  return 0;
}

int TypeCheckerVisitor::computeAddress(IndexExp *exp) { return 0; }
int TypeCheckerVisitor::visit(MatrixExp *exp) {
  if (!entorno.check(exp->name)) {
    throw std::runtime_error("[TypeChecker] Variable no declarada: '" +
                             exp->name + "' usada en la función '" +
                             funcionActual + "'");
  }
  exp->row->accept(this);
  exp->col->accept(this);
  return 0;
}
int TypeCheckerVisitor::computeAddress(MatrixExp *exp) { return 0; }
int TypeCheckerVisitor::visit(FieldExp *exp) {
  std::string type;
  if (!tiposVar.lookup(exp->object, type)) {
    throw std::runtime_error("[TypeChecker] Variable no declarada: '" +
                             exp->object + "' usada en la función '" +
                             funcionActual + "'");
  }
  if (!structFieldOffsets.count(type)) {
    throw std::runtime_error("[TypeChecker] La variable '" + exp->object +
                             "' no es una estructura");
  }
  if (!structFieldOffsets[type].count(exp->field)) {
    throw std::runtime_error("[TypeChecker] La estructura '" + type +
                             "' no tiene campo '" + exp->field + "'");
  }
  return 0;
}
int TypeCheckerVisitor::computeAddress(FieldExp *exp) { return 0; }

int TypeCheckerVisitor::visit(NumberExp *exp) { return 0; }
int TypeCheckerVisitor::visit(Program *p) { return 0; }

int TypeCheckerVisitor::visit(DoWhileStm *stm) {
  stm->condition->accept(this);
  stm->b->accept(this);
  return 0;
}

int TypeCheckerVisitor::visit(BreakStm *stm) { return 0; }

int TypeCheckerVisitor::visit(SwitchStm *stm) {
  stm->e->accept(this);
  for (auto c : stm->cases) {
    for (auto s : c->body)
      s->accept(this);
  }
  for (auto s : stm->default_body)
    s->accept(this);
  return 0;
}

// =============================================================================
// ConstantTaggingVisitor - Etiquetado para plegado de constantes
// =============================================================================

Program *ConstantTaggingVisitor::tag(Program *program) {
  if (!program)
    return nullptr;
  program->accept(this);
  return program;
}

int ConstantTaggingVisitor::visit(NumberExp *exp) {
  exp->isConstant = true;
  exp->constantValue = exp->value;
  return 0;
}

int ConstantTaggingVisitor::visit(IdExp *exp) {
  exp->isConstant = false;
  exp->constantValue = 0;
  return 0;
}

int ConstantTaggingVisitor::visit(BinaryExp *exp) {
  exp->left->accept(this);
  exp->right->accept(this);
  exp->constantValue = 0;
  exp->isConstant =
      exp->left->isConstant && exp->right->isConstant &&
      evalConstantBinary(exp->op, exp->left->constantValue,
                         exp->right->constantValue, exp->constantValue);
  return 0;
}

int ConstantTaggingVisitor::visit(UnaryExp *exp) {
  exp->operand->accept(this);
  exp->isConstant = exp->operand->isConstant;
  exp->constantValue = exp->isConstant ? !exp->operand->constantValue : 0;
  return 0;
}

int ConstantTaggingVisitor::visit(IndexExp *exp) {
  exp->index->accept(this);
  exp->isConstant = false;
  exp->constantValue = 0;
  return 0;
}

int ConstantTaggingVisitor::visit(MatrixExp *exp) {
  exp->row->accept(this);
  exp->col->accept(this);
  exp->isConstant = false;
  exp->constantValue = 0;
  return 0;
}

int ConstantTaggingVisitor::visit(FieldExp *exp) {
  exp->isConstant = false;
  exp->constantValue = 0;
  return 0;
}

int ConstantTaggingVisitor::visit(FcallExp *fc) {
  for (auto arg : fc->argumentos)
    arg->accept(this);
  fc->isConstant = false;
  fc->constantValue = 0;
  return 0;
}

int ConstantTaggingVisitor::visit(ExpListSize *stm) {
  stm->size->accept(this);
  stm->isConstant = false;
  stm->constantValue = 0;
  return 0;
}

int ConstantTaggingVisitor::visit(ExpListVals *stm) {
  for (auto value : stm->values)
    value->accept(this);
  stm->isConstant = false;
  stm->constantValue = 0;
  return 0;
}

int ConstantTaggingVisitor::visit(ExpMatrixSize *stm) {
  stm->rows->accept(this);
  stm->cols->accept(this);
  stm->isConstant = false;
  stm->constantValue = 0;
  return 0;
}

int ConstantTaggingVisitor::visit(ExpMatrixVals *stm) {
  stm->rows->accept(this);
  stm->cols->accept(this);
  for (auto value : stm->values)
    value->accept(this);
  stm->isConstant = false;
  stm->constantValue = 0;
  return 0;
}

int ConstantTaggingVisitor::visit(PrintStm *stm) {
  stm->e->accept(this);
  return 0;
}

int ConstantTaggingVisitor::visit(AssignStm *stm) {
  stm->target->accept(this);
  stm->e->accept(this);
  return 0;
}

int ConstantTaggingVisitor::visit(ReturnStm *r) {
  r->e->accept(this);
  return 0;
}

int ConstantTaggingVisitor::visit(IfStm *stm) {
  stm->condition->accept(this);
  stm->then->accept(this);
  if (stm->els)
    stm->els->accept(this);
  return 0;
}

int ConstantTaggingVisitor::visit(WhileStm *stm) {
  stm->condition->accept(this);
  stm->b->accept(this);
  return 0;
}

int ConstantTaggingVisitor::visit(DoWhileStm *stm) {
  stm->b->accept(this);
  stm->condition->accept(this);
  return 0;
}

int ConstantTaggingVisitor::visit(BreakStm *stm) {
  (void)stm;
  return 0;
}

int ConstantTaggingVisitor::visit(SwitchStm *stm) {
  stm->e->accept(this);
  for (auto c : stm->cases) {
    for (auto s : c->body)
      s->accept(this);
  }
  for (auto s : stm->default_body)
    s->accept(this);
  return 0;
}

int ConstantTaggingVisitor::visit(Body *body) {
  for (auto dec : body->declarations)
    dec->accept(this);
  for (auto stm : body->StmList)
    stm->accept(this);
  return 0;
}

int ConstantTaggingVisitor::visit(VarDec *vd) {
  (void)vd;
  return 0;
}

int ConstantTaggingVisitor::visit(StructDec *sd) {
  for (auto field : sd->fields)
    field->accept(this);
  return 0;
}

int ConstantTaggingVisitor::visit(FunDec *fd) {
  fd->cuerpo->accept(this);
  return 0;
}

int ConstantTaggingVisitor::visit(Program *p) {
  for (auto sd : p->sdlist)
    sd->accept(this);
  for (auto vd : p->vdlist)
    vd->accept(this);
  for (auto fd : p->fdlist)
    fd->accept(this);
  return 0;
}

// =============================================================================
// CascadeFunctionEvalVisitor - Evaluacion en cascada de llamadas constantes
// =============================================================================

Program *CascadeFunctionEvalVisitor::optimize(Program *program) {
  if (!program)
    return nullptr;
  funciones.clear();
  pilaLlamadas.clear();
  program->accept(this);
  return program;
}

void CascadeFunctionEvalVisitor::markConstant(Exp *exp, int value) {
  exp->isConstant = true;
  exp->constantValue = value;
}

void CascadeFunctionEvalVisitor::markNonConstant(Exp *exp) {
  exp->isConstant = false;
  exp->constantValue = 0;
}

ReturnStm *CascadeFunctionEvalVisitor::findReturn(Body *body) {
  if (!body)
    return nullptr;
  for (auto stm : body->StmList) {
    if (auto ret = dynamic_cast<ReturnStm *>(stm))
      return ret;
  }
  return nullptr;
}

bool CascadeFunctionEvalVisitor::evalExp(
    Exp *exp, const std::unordered_map<std::string, int> &bindings,
    int &result) {
  if (!exp)
    return false;

  if (auto number = dynamic_cast<NumberExp *>(exp)) {
    result = number->value;
    return true;
  }

  if (auto id = dynamic_cast<IdExp *>(exp)) {
    auto it = bindings.find(id->value);
    if (it == bindings.end())
      return false;
    result = it->second;
    return true;
  }

  if (auto binary = dynamic_cast<BinaryExp *>(exp)) {
    int left = 0;
    int right = 0;
    return evalExp(binary->left, bindings, left) &&
           evalExp(binary->right, bindings, right) &&
           evalConstantBinary(binary->op, left, right, result);
  }

  if (auto unary = dynamic_cast<UnaryExp *>(exp)) {
    int value = 0;
    if (!evalExp(unary->operand, bindings, value))
      return false;
    result = !value;
    return true;
  }

  if (auto call = dynamic_cast<FcallExp *>(exp))
    return evalCall(call, bindings, result);

  return false;
}

bool CascadeFunctionEvalVisitor::evalCall(
    FcallExp *fc, const std::unordered_map<std::string, int> &bindings,
    int &result) {
  auto funIt = funciones.find(fc->nombre);
  if (funIt == funciones.end())
    return false;

  FunDec *fd = funIt->second;
  if (fd->Pnombres.size() != fc->argumentos.size())
    return false;

  if (std::find(pilaLlamadas.begin(), pilaLlamadas.end(), fc->nombre) !=
      pilaLlamadas.end())
    return false;

  std::unordered_map<std::string, int> callBindings;
  for (size_t i = 0; i < fc->argumentos.size(); ++i) {
    int value = 0;
    if (!evalExp(fc->argumentos[i], bindings, value))
      return false;
    callBindings[fd->Pnombres[i]] = value;
  }

  ReturnStm *ret = findReturn(fd->cuerpo);
  if (!ret)
    return false;

  pilaLlamadas.push_back(fc->nombre);
  bool ok = evalExp(ret->e, callBindings, result);
  pilaLlamadas.pop_back();
  return ok;
}

int CascadeFunctionEvalVisitor::visit(NumberExp *exp) {
  markConstant(exp, exp->value);
  return 0;
}

int CascadeFunctionEvalVisitor::visit(IdExp *exp) {
  markNonConstant(exp);
  return 0;
}

int CascadeFunctionEvalVisitor::visit(BinaryExp *exp) {
  exp->left->accept(this);
  exp->right->accept(this);

  int value = 0;
  if (exp->left->isConstant && exp->right->isConstant &&
      evalConstantBinary(exp->op, exp->left->constantValue,
                         exp->right->constantValue, value)) {
    markConstant(exp, value);
  } else {
    markNonConstant(exp);
  }
  return 0;
}

int CascadeFunctionEvalVisitor::visit(UnaryExp *exp) {
  exp->operand->accept(this);
  if (exp->operand->isConstant)
    markConstant(exp, !exp->operand->constantValue);
  else
    markNonConstant(exp);
  return 0;
}

int CascadeFunctionEvalVisitor::visit(IndexExp *exp) {
  exp->index->accept(this);
  markNonConstant(exp);
  return 0;
}

int CascadeFunctionEvalVisitor::visit(MatrixExp *exp) {
  exp->row->accept(this);
  exp->col->accept(this);
  markNonConstant(exp);
  return 0;
}

int CascadeFunctionEvalVisitor::visit(FieldExp *exp) {
  markNonConstant(exp);
  return 0;
}

int CascadeFunctionEvalVisitor::visit(FcallExp *fc) {
  for (auto arg : fc->argumentos)
    arg->accept(this);

  std::unordered_map<std::string, int> bindings;
  int value = 0;
  if (evalCall(fc, bindings, value))
    markConstant(fc, value);
  else
    markNonConstant(fc);
  return 0;
}

int CascadeFunctionEvalVisitor::visit(ExpListSize *stm) {
  stm->size->accept(this);
  markNonConstant(stm);
  return 0;
}

int CascadeFunctionEvalVisitor::visit(ExpListVals *stm) {
  for (auto value : stm->values)
    value->accept(this);
  markNonConstant(stm);
  return 0;
}

int CascadeFunctionEvalVisitor::visit(ExpMatrixSize *stm) {
  stm->rows->accept(this);
  stm->cols->accept(this);
  markNonConstant(stm);
  return 0;
}

int CascadeFunctionEvalVisitor::visit(ExpMatrixVals *stm) {
  stm->rows->accept(this);
  stm->cols->accept(this);
  for (auto value : stm->values)
    value->accept(this);
  markNonConstant(stm);
  return 0;
}

int CascadeFunctionEvalVisitor::visit(PrintStm *stm) {
  stm->e->accept(this);
  return 0;
}

int CascadeFunctionEvalVisitor::visit(AssignStm *stm) {
  stm->target->accept(this);
  stm->e->accept(this);
  return 0;
}

int CascadeFunctionEvalVisitor::visit(ReturnStm *r) {
  r->e->accept(this);
  return 0;
}

int CascadeFunctionEvalVisitor::visit(IfStm *stm) {
  stm->condition->accept(this);
  stm->then->accept(this);
  if (stm->els)
    stm->els->accept(this);
  return 0;
}

int CascadeFunctionEvalVisitor::visit(WhileStm *stm) {
  stm->condition->accept(this);
  stm->b->accept(this);
  return 0;
}

int CascadeFunctionEvalVisitor::visit(DoWhileStm *stm) {
  stm->b->accept(this);
  stm->condition->accept(this);
  return 0;
}

int CascadeFunctionEvalVisitor::visit(BreakStm *stm) {
  (void)stm;
  return 0;
}

int CascadeFunctionEvalVisitor::visit(SwitchStm *stm) {
  stm->e->accept(this);
  for (auto c : stm->cases) {
    for (auto s : c->body)
      s->accept(this);
  }
  for (auto s : stm->default_body)
    s->accept(this);
  return 0;
}

int CascadeFunctionEvalVisitor::visit(Body *body) {
  for (auto dec : body->declarations)
    dec->accept(this);
  for (auto stm : body->StmList)
    stm->accept(this);
  return 0;
}

int CascadeFunctionEvalVisitor::visit(VarDec *vd) {
  (void)vd;
  return 0;
}

int CascadeFunctionEvalVisitor::visit(StructDec *sd) {
  for (auto field : sd->fields)
    field->accept(this);
  return 0;
}

int CascadeFunctionEvalVisitor::visit(FunDec *fd) {
  fd->cuerpo->accept(this);
  return 0;
}

int CascadeFunctionEvalVisitor::visit(Program *p) {
  for (auto fd : p->fdlist)
    funciones[fd->nombre] = fd;

  for (auto sd : p->sdlist)
    sd->accept(this);
  for (auto vd : p->vdlist)
    vd->accept(this);
  for (auto fd : p->fdlist)
    fd->accept(this);
  return 0;
}

// =============================================================================
// SethiUllmanLabelVisitor - Etiquetado de registros minimos
// =============================================================================

Program *SethiUllmanLabelVisitor::label(Program *program) {
  if (!program)
    return nullptr;
  program->accept(this);
  return program;
}

int SethiUllmanLabelVisitor::visit(NumberExp *exp) {
  exp->label = 1;
  return 0;
}

int SethiUllmanLabelVisitor::visit(IdExp *exp) {
  exp->label = 1;
  return 0;
}

int SethiUllmanLabelVisitor::visit(BinaryExp *exp) {
  exp->left->accept(this);
  exp->right->accept(this);

  int l1 = exp->left->label;
  int l2 = exp->right->label;
  exp->label = (l1 == l2) ? l1 + 1 : std::max(l1, l2);
  return 0;
}

int SethiUllmanLabelVisitor::visit(UnaryExp *exp) {
  exp->operand->accept(this);
  exp->label = exp->operand->label;
  return 0;
}

int SethiUllmanLabelVisitor::visit(IndexExp *exp) {
  exp->index->accept(this);
  exp->label = 1;
  return 0;
}

int SethiUllmanLabelVisitor::visit(MatrixExp *exp) {
  exp->row->accept(this);
  exp->col->accept(this);
  exp->label = 1;
  return 0;
}

int SethiUllmanLabelVisitor::visit(FieldExp *exp) {
  exp->label = 1;
  return 0;
}

int SethiUllmanLabelVisitor::visit(FcallExp *fc) {
  for (auto arg : fc->argumentos)
    arg->accept(this);
  fc->label = 1;
  return 0;
}

int SethiUllmanLabelVisitor::visit(ExpListSize *stm) {
  stm->size->accept(this);
  stm->label = 1;
  return 0;
}

int SethiUllmanLabelVisitor::visit(ExpListVals *stm) {
  for (auto value : stm->values)
    value->accept(this);
  stm->label = 1;
  return 0;
}

int SethiUllmanLabelVisitor::visit(ExpMatrixSize *stm) {
  stm->rows->accept(this);
  stm->cols->accept(this);
  stm->label = 1;
  return 0;
}

int SethiUllmanLabelVisitor::visit(ExpMatrixVals *stm) {
  stm->rows->accept(this);
  stm->cols->accept(this);
  for (auto value : stm->values)
    value->accept(this);
  stm->label = 1;
  return 0;
}

int SethiUllmanLabelVisitor::visit(PrintStm *stm) {
  stm->e->accept(this);
  return 0;
}

int SethiUllmanLabelVisitor::visit(AssignStm *stm) {
  stm->target->accept(this);
  stm->e->accept(this);
  return 0;
}

int SethiUllmanLabelVisitor::visit(ReturnStm *r) {
  r->e->accept(this);
  return 0;
}

int SethiUllmanLabelVisitor::visit(IfStm *stm) {
  stm->condition->accept(this);
  stm->then->accept(this);
  if (stm->els)
    stm->els->accept(this);
  return 0;
}

int SethiUllmanLabelVisitor::visit(WhileStm *stm) {
  stm->condition->accept(this);
  stm->b->accept(this);
  return 0;
}

int SethiUllmanLabelVisitor::visit(DoWhileStm *stm) {
  stm->b->accept(this);
  stm->condition->accept(this);
  return 0;
}

int SethiUllmanLabelVisitor::visit(BreakStm *stm) {
  (void)stm;
  return 0;
}

int SethiUllmanLabelVisitor::visit(SwitchStm *stm) {
  stm->e->accept(this);
  for (auto c : stm->cases) {
    for (auto s : c->body)
      s->accept(this);
  }
  for (auto s : stm->default_body)
    s->accept(this);
  return 0;
}

int SethiUllmanLabelVisitor::visit(Body *body) {
  for (auto dec : body->declarations)
    dec->accept(this);
  for (auto stm : body->StmList)
    stm->accept(this);
  return 0;
}

int SethiUllmanLabelVisitor::visit(VarDec *vd) {
  (void)vd;
  return 0;
}

int SethiUllmanLabelVisitor::visit(StructDec *sd) {
  for (auto field : sd->fields)
    field->accept(this);
  return 0;
}

int SethiUllmanLabelVisitor::visit(FunDec *fd) {
  fd->cuerpo->accept(this);
  return 0;
}

int SethiUllmanLabelVisitor::visit(Program *p) {
  for (auto sd : p->sdlist)
    sd->accept(this);
  for (auto vd : p->vdlist)
    vd->accept(this);
  for (auto fd : p->fdlist)
    fd->accept(this);
  return 0;
}

// =============================================================================
// GenCodeVisitor — Generación de código ensamblador x86-64 (AT&T syntax)
// =============================================================================
// Convenciones usadas:
//   · Registros de argumentos: %rdi, %rsi, %rdx, %rcx, %r8, %r9
//   · Resultado de expresiones en %rax
//   · Variables locales: offsets negativos desde %rbp
//   · Variables globales: símbolos en .data con acceso RIP-relativo
//   · printf para print: formato en print_fmt ("%ld \n")
// =============================================================================

// -----------------------------------------------------------------------------
// generar — punto de entrada de la generación
// -----------------------------------------------------------------------------

int GenCodeVisitor::generar(Program *program) {
  tipos.TypeChecker(program);
  funcontador = tipos.funcontador;
  structFields = tipos.structFields;
  structFieldOffsets = tipos.structFieldOffsets;
  for (auto fd : program->fdlist) {
    funParamNames[fd->nombre] = fd->Pnombres;
    funParamTypes[fd->nombre] = fd->Ptipos;
  }
  program->accept(this);
  return 0;
}

// -----------------------------------------------------------------------------
// visit(Program)
// -----------------------------------------------------------------------------

void GenCodeVisitor::requestFunctionEmission(const std::string &name) {
  if (!funciones.count(name) || funcionesEmitidas.count(name))
    return;

  if (std::find(funcionesPendientes.begin(), funcionesPendientes.end(), name) ==
      funcionesPendientes.end()) {
    funcionesPendientes.push_back(name);
  }
}

void GenCodeVisitor::emitPendingFunctions() {
  while (!funcionesPendientes.empty()) {
    std::string name = funcionesPendientes.front();
    funcionesPendientes.erase(funcionesPendientes.begin());

    if (!funciones.count(name) || funcionesEmitidas.count(name))
      continue;

    funciones[name]->accept(this);
  }
}

int GenCodeVisitor::visit(Program *program) {
  funciones.clear();
  funcionesEmitidas.clear();
  funcionesPendientes.clear();
  necesitaPotencia = false;

  for (auto fd : program->fdlist)
    funciones[fd->nombre] = fd;
  // Sección de datos
  out << ".data\n";
  out << "print_fmt: .string \"%ld \\n\"\n";

  // Recolectar nombres de variables globales
  for (auto dec : program->vdlist)
    dec->accept(this);

  // Emitir las etiquetas .quad para las globales
  for (auto &[var, _] : memoriaGlobal)
    out << var << ": .quad 0\n";
  for (auto fd : program->fdlist) {
    for (size_t i = 0; i < fd->Ptipos.size(); ++i) {
      if (fd->Ptipos[i] == "matrix")
        out << "__cols_" << fd->nombre << "_" << fd->Pnombres[i]
            << ": .quad 0\n";
    }
  }

  // Sección de texto (código)
  out << "\n.text\n";
  if (funciones.count("main")) {
    funciones["main"]->accept(this);
  } else {
    for (auto fd : program->fdlist) {
      fd->accept(this);
      break;
    }
  }

  emitPendingFunctions();

  if (necesitaPotencia)
    emitPowerHelper(out);

  // Marca el stack como no ejecutable (requerido por el linker moderno)
  out << "\n.section .note.GNU-stack,\"\",@progbits\n";
  return 0;
}

int GenCodeVisitor::visit(StructDec *sd) { return 0; }

// -----------------------------------------------------------------------------
// visit(VarDec) — registra variables en memoria (global o local)
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(VarDec *stm) {
  for (auto &var : stm->vars) {
    if (!entornoFuncion) {
      memoriaGlobal[var] = true;
      variableTypes[var] = stm->type;
      if (structFields.count(stm->type))
        structAllocated[var] = false;
    } else {
      memoria[var] = offset;
      variableTypes[var] = stm->type;
      if (structFields.count(stm->type))
        structAllocated[var] = false;
      offset -= 8;
    }
  }
  return 0;
}

int GenCodeVisitor::visit(FieldExp *exp) {
  std::string type = variableTypes[exp->object];
  int fieldIndex = structFieldOffsets[type][exp->field] / 8;

  out << "  movq $" << fieldIndex << ", %rdi\n";
  if (memoriaGlobal.count(exp->object)) {
    out << "  movq " << exp->object << "(%rip), %rax\n";
  } else {
    int off = memoria[exp->object];
    out << "  movq " << off << "(%rbp), %rax\n";
  }
  out << "  movq (%rax, %rdi, 8), %rax\n";
  return 0;
}

// -----------------------------------------------------------------------------
// visit(NumberExp) — carga un inmediato en %rax
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(NumberExp *exp) {
  out << "  movq $" << exp->value << ", %rax\n";
  return 0;
}

// -----------------------------------------------------------------------------
// visit(ExpListSize) — evalúa la expresión para reservar el espacio apropiado y
// dejarlo en rax
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(ExpListSize *stm) {
  // TEST
  // Lista de Int: malloc de 8*n bytes
  stm->size->accept(this);
  out << "  movq $8, %rcx\n";
  out << "  imulq %rcx, %rax\n";
  out << "  movq %rax, %rdi\n"
      << "  call malloc@PLT\n";
  // El puntero se deja en rax
  return 0;
}

// -----------------------------------------------------------------------------
// visit(ExpListVals) — evalúa las expresiones expresión y las almacena en los
// espacios reservados de memoria y luego regresa el puntero
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(ExpListVals *stm) {
  // TEST
  // Lista de Int: malloc de 8*n bytes + almacenar cada entero
  int n = stm->values.size();
  out << "  movq $" << n << ", %rax\n";
  out << "  movq $8, %rcx\n";
  out << "  imulq %rcx, %rax\n";
  out << "  movq %rax, %rdi\n";
  out << "  call malloc@PLT\n";
  out << "  pushq %rax\n";
  for (size_t i = 0; i < n; ++i) {
    stm->values[i]->accept(this); // → %rax = valor entero
    out << "  movq %rax, %rcx\n";
    out << "  movq $" << i << ", %rdi\n";
    // Recupera el puntero base para escribir como en arreglo[index].
    out << "  popq %rax\n";
    out << "  movq %rcx, (%rax, %rdi, 8)\n";
    if (i + 1 < n)
      out << "  pushq %rax\n";
  }

  return 0;
}

int GenCodeVisitor::visit(ExpMatrixSize *stm) {
  stm->cols->accept(this);
  out << "  pushq %rax\n";
  stm->rows->accept(this);
  out << "  popq %rcx\n";
  out << "  imulq %rcx, %rax\n";
  out << "  salq $3, %rax\n";
  out << "  movq %rax, %rdi\n";
  out << "  call malloc@PLT\n";
  return 0;
}

int GenCodeVisitor::visit(ExpMatrixVals *stm) {
  stm->cols->accept(this);
  out << "  pushq %rax\n";
  stm->rows->accept(this);
  out << "  popq %rcx\n";
  out << "  imulq %rcx, %rax\n";
  out << "  salq $3, %rax\n";
  out << "  movq %rax, %rdi\n";
  out << "  call malloc@PLT\n";
  out << "  pushq %rax\n";
  for (size_t i = 0; i < stm->values.size(); ++i) {
    out << "  pushq %rax\n";
    stm->values[i]->accept(this);
    out << "  movq %rax, %rcx\n";
    out << "  popq %rax\n";
    out << "  movq %rcx, " << (i * 8) << "(%rax)\n";
  }
  out << "  popq %rax\n";
  return 0;
}

// -----------------------------------------------------------------------------
// visit(IdExp) — carga el valor de una variable en %rax
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(IdExp *exp) {
  if (memoriaGlobal.count(exp->value))
    out << "  movq " << exp->value << "(%rip), %rax\n";
  else
    out << "  movq " << memoria[exp->value] << "(%rbp), %rax\n";
  return 0;
}

// visit(UnaryExp) — operador unario NOT
int GenCodeVisitor::visit(UnaryExp *exp) {
  if (exp->isConstant) {
    out << "  movq $" << exp->constantValue << ", %rax\n";
    return 0;
  }

  exp->operand->accept(this);
  int lbl = labelcont++;
  out << "  cmpq $0, %rax\n";
  out << "  je not_true_" << lbl << "\n";
  out << "  movq $0, %rax\n";
  out << "  jmp not_end_" << lbl << "\n";
  out << "not_true_" << lbl << ":\n";
  out << "  movq $1, %rax\n";
  out << "not_end_" << lbl << ":\n";
  return 0;
}

// visit(IndexExp) — operador para indices de listas
int GenCodeVisitor::visit(IndexExp *exp) {
  // 1) Evaluar el índice → %rax
  exp->index->accept(this);
  out << "  movq %rax, %rdi\n";

  // 2) Cargar la dirección base del array en %rax
  if (memoriaGlobal.count(exp->name)) {
    out << "  movq " << exp->name << "(%rip), %rax\n"; // global
  } else {
    int off = memoria[exp->name];
    out << "  movq " << off << "(%rbp), %rax\n"; // ptr heap
  }

  // 3) Leer con el mismo modo de direccionamiento que structs.
  out << "  movq (%rax, %rdi, 8), %rax\n";
  return 0;
}

int GenCodeVisitor::visit(MatrixExp *exp) {
  exp->row->accept(this);
  out << "  pushq %rax\n";
  exp->col->accept(this);
  out << "  movq %rax, %rdi\n";
  out << "  popq %rax\n";

  if (matrixColumns.count(exp->name)) {
    out << "  movq $" << matrixColumns[exp->name] << ", %rcx\n";
  } else if (currentMatrixParamLabels.count(exp->name)) {
    out << "  movq " << currentMatrixParamLabels[exp->name] << "(%rip), %rcx\n";
  } else {
    out << "  movq $0, %rcx\n";
  }
  out << "  imulq %rcx, %rax\n";
  out << "  addq %rdi, %rax\n";
  out << "  movq %rax, %rdi\n";
  if (memoriaGlobal.count(exp->name)) {
    out << "  movq " << exp->name << "(%rip), %rax\n";
  } else {
    int off = memoria[exp->name];
    out << "  movq " << off << "(%rbp), %rax\n";
  }
  out << "  movq (%rax, %rdi, 8), %rax\n";
  return 0;
}

// -----------------------------------------------------------------------------
// visit(BinaryExp) — evalúa left y right, aplica operador
// Convención: left en %rax, right en %rcx
// -----------------------------------------------------------------------------

  int GenCodeVisitor::visit(BinaryExp *exp) {
    if (exp->isConstant) {
      out << "  movq $" << exp->constantValue << ", %rax\n";
      return 0;
    }

  if (exp->op == POW_OP) {
    int exponent = 0;
    if (constantExpValue(exp->right, exponent) &&
        (exponent == 2 || exponent == 4)) {
      exp->left->accept(this);
      out << "  imulq %rax, %rax\n";
      if (exponent == 4)
        out << "  imulq %rax, %rax\n";
      return 0;
    }
  }

  int l = exp->left ? exp->left->label : 0;
  int r = exp->right ? exp->right->label : 0;

  if (r == 0) {
    exp->left->accept(this);
    out << "  movq %rax, %rcx\n";
    exp->right->accept(this);
  } else if (l >= r) {
    exp->left->accept(this);
    out << "  pushq %rax\n";
    exp->right->accept(this);
    out << "  movq %rax, %rcx\n";
    out << "  popq %rax\n";
  } else {
    exp->right->accept(this);
    out << "  pushq %rax\n";
    exp->left->accept(this);
    out << "  movq %rax, %rcx\n";
    out << "  popq %rax\n";
    out << "  xchgq %rax, %rcx\n";
  }

  switch (exp->op) {
  case PLUS_OP:
    out << "  addq %rcx, %rax\n";
    break;
  case MINUS_OP:
    out << "  subq %rcx, %rax\n";
    break;
  case MUL_OP:
    out << "  imulq %rcx, %rax\n";
    break;
  case DIV_OP:
    // División entera con signo: idivq usa %rdx:%rax / %rcx
    out << "  cqto\n";       // sign-extend %rax → %rdx:%rax
    out << "  idivq %rcx\n"; // cociente en %rax
    break;
  case POW_OP:
    out << "  movq %rax, %rdi\n";
    out << "  movq %rcx, %rsi\n";
    out << "  call potencia\n";
    necesitaPotencia = true;
    break;
  case LE_OP:
    out << "  cmpq %rcx, %rax\n";
    out << "  movq $0, %rax\n";
    out << "  setl %al\n";
    out << "  movzbq %al, %rax\n";
    break;
  case GT_OP:
    out << "  cmpq %rcx, %rax\n";
    out << "  movq $0, %rax\n";
    out << "  setg %al\n";
    out << "  movzbq %al, %rax\n";
    break;
  case LEQ_OP:
    out << "  cmpq %rcx, %rax\n";
    out << "  movq $0, %rax\n";
    out << "  setle %al\n";
    out << "  movzbq %al, %rax\n";
    break;
  case GEQ_OP:
    out << "  cmpq %rcx, %rax\n";
    out << "  movq $0, %rax\n";
    out << "  setge %al\n";
    out << "  movzbq %al, %rax\n";
    break;
  case EQ_OP:
    out << "  cmpq %rcx, %rax\n";
    out << "  movq $0, %rax\n";
    out << "  sete %al\n";
    out << "  movzbq %al, %rax\n";
    break;
  case NE_OP:
    out << "  cmpq %rcx, %rax\n";
    out << "  movq $0, %rax\n";
    out << "  setne %al\n";
    out << "  movzbq %al, %rax\n";
    break;
  case AND_OP:
    out << "  andq %rcx, %rax\n";
    break;
  case OR_OP:
    out << "  orq %rcx, %rax\n";
    break;
  }
  return 0;
}

// -----------------------------------------------------------------------------
// visit(AssignStm) — evalúa expresión y almacena resultado
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(AssignStm *stm) {
  if (auto id = dynamic_cast<IdExp *>(stm->target)) {
    if (auto vals = dynamic_cast<ExpListVals *>(stm->e)) {
      if (structFields.count(vals->type)) {
        int fields = structFields[vals->type];
        out << "  movq $" << (fields * 8) << ", %rdi\n";
        out << "  call malloc@PLT\n";
        if (memoriaGlobal.count(id->value)) {
          out << "  movq %rax, " << id->value << "(%rip)\n";
        } else {
          int off = memoria[id->value];
          out << "  movq %rax, " << off << "(%rbp)\n";
        }
        structAllocated[id->value] = true;

        for (size_t i = 0; i < vals->values.size(); ++i) {
          vals->values[i]->accept(this);
          out << "  pushq %rax\n";
          out << "  movq $" << i << ", %rax\n";
          out << "  movq %rax, %rdi\n";
          out << "  popq %rax\n";
          out << "  movq %rax, %rcx\n";
          if (memoriaGlobal.count(id->value)) {
            out << "  movq " << id->value << "(%rip), %rax\n";
          } else {
            int off = memoria[id->value];
            out << "  movq " << off << "(%rbp), %rax\n";
          }
          out << "  movq %rcx, (%rax, %rdi, 8)\n";
        }
        return 0;
      }

      int n = vals->values.size();
      out << "  movq $" << n << ", %rax\n";
      out << "  movq $8, %rcx\n";
      out << "  imulq %rcx, %rax\n";
      out << "  movq %rax, %rdi\n";
      out << "  call malloc@PLT\n";
      if (memoriaGlobal.count(id->value)) {
        out << "  movq %rax, " << id->value << "(%rip)\n";
      } else {
        int off = memoria[id->value];
        out << "  movq %rax, " << off << "(%rbp)\n";
      }

      for (size_t i = 0; i < vals->values.size(); ++i) {
        vals->values[i]->accept(this);
        out << "  pushq %rax\n";
        out << "  movq $" << i << ", %rax\n";
        out << "  movq %rax, %rdi\n";
        out << "  popq %rax\n";
        out << "  movq %rax, %rcx\n";
        if (memoriaGlobal.count(id->value)) {
          out << "  movq " << id->value << "(%rip), %rax\n";
        } else {
          int off = memoria[id->value];
          out << "  movq " << off << "(%rbp), %rax\n";
        }
        out << "  movq %rcx, (%rax, %rdi, 8)\n";
      }
      return 0;
    }

    if (auto mat = dynamic_cast<ExpMatrixSize *>(stm->e)) {
      int cols = constNumberValue(mat->cols);
      if (cols > 0)
        matrixColumns[id->value] = cols;
      mat->cols->accept(this);
      out << "  pushq %rax\n";
      mat->rows->accept(this);
      out << "  popq %rcx\n";
      out << "  imulq %rcx, %rax\n";
      out << "  salq $3, %rax\n";
      out << "  movq %rax, %rdi\n";
      out << "  call malloc@PLT\n";
      if (memoriaGlobal.count(id->value)) {
        out << "  movq %rax, " << id->value << "(%rip)\n";
      } else {
        int off = memoria[id->value];
          out << "  movq %rax, " << off << "(%rbp)\n";
      }
      return 0;
    }

    if (auto mat = dynamic_cast<ExpMatrixVals *>(stm->e)) {
      int cols = constNumberValue(mat->cols);
      if (cols > 0)
        matrixColumns[id->value] = cols;
      mat->cols->accept(this);
      out << "  pushq %rax\n";
      mat->rows->accept(this);
      out << "  popq %rcx\n";
      out << "  imulq %rcx, %rax\n";
      out << "  salq $3, %rax\n";
      out << "  movq %rax, %rdi\n";
      out << "  call malloc@PLT\n";
      if (memoriaGlobal.count(id->value)) {
        out << "  movq %rax, " << id->value << "(%rip)\n";
      } else {
        int off = memoria[id->value];
          out << "  movq %rax, " << off << "(%rbp)\n";
      }

      for (size_t i = 0; i < mat->values.size(); ++i) {
        mat->values[i]->accept(this);
        out << "  pushq %rax\n";
        if (cols > 0) {
          out << "  movq $" << (i / cols) << ", %rax\n";
          out << "  pushq %rax\n";
          out << "  movq $" << (i % cols) << ", %rax\n";
        } else {
          out << "  movq $0, %rax\n";
          out << "  pushq %rax\n";
          out << "  movq $" << i << ", %rax\n";
        }
        out << "  movq %rax, %rdi\n";
        out << "  popq %rax\n";
        if (matrixColumns.count(id->value)) {
          out << "  movq $" << matrixColumns[id->value] << ", %rcx\n";
        } else if (currentMatrixParamLabels.count(id->value)) {
          out << "  movq " << currentMatrixParamLabels[id->value]
              << "(%rip), %rcx\n";
        } else {
          out << "  movq $0, %rcx\n";
        }
        out << "  imulq %rcx, %rax\n";
        out << "  addq %rdi, %rax\n";
        out << "  movq %rax, %rdi\n";
        out << "  popq %rcx\n";
        if (memoriaGlobal.count(id->value)) {
          out << "  movq " << id->value << "(%rip), %rax\n";
        } else {
          int off = memoria[id->value];
          out << "  movq " << off << "(%rbp), %rax\n";
        }
        out << "  movq %rcx, (%rax, %rdi, 8)\n";
      }
      return 0;
    }
  }

  stm->e->accept(this);
  stm->target->computeAddress(this);
  return 0;
}

// -----------------------------------------------------------------------------
// computeAdress(IdExp) — Asigna el rax hacia la posicion correcta de memoria
// -----------------------------------------------------------------------------

int GenCodeVisitor::computeAddress(IdExp *id) {
  if (memoriaGlobal.count(id->value)) {
    out << "  movq %rax, " << id->value << "(%rip)\n";
  } else {
    int off = memoria[id->value];
    out << "  movq %rax, " << off << "(%rbp)\n";
  }
  return 0;
}

// -----------------------------------------------------------------------------
// computeAdress(IndexExp) — Asigna el rax hacia la posicion correcta de memoria
// -----------------------------------------------------------------------------

int GenCodeVisitor::computeAddress(IndexExp *idx) {
  // Not perfect as it usses a third register rdi
  // OPTIMIZE
  out << "  pushq %rax\n";

  idx->index->accept(this);

  out << "  movq %rax, %rdi\n"  // Saves the index in rdi
      << "  popq %rax\n"        // pops the index to have the value to assign
      << "  movq %rax, %rcx\n"; // Assigns the exp to rcx
  if (memoriaGlobal.count(idx->name)) {
    out << "  movq " << idx->name
        << "(%rip), %rax\n"; // Assigns the array in memory to the index
  } else {
    int off = memoria[idx->name];
    out << "  movq " << off
        << "(%rbp), %rax\n"; // Assigns the array in memory to the index
  }
  out << "  movq %rcx, (%rax, %rdi, 8)\n"; // asssigns the rcx wo the  array
                                           // in the correct offset

  return 0;
}

int GenCodeVisitor::computeAddress(MatrixExp *idx) {
  out << "  pushq %rax\n";
  idx->row->accept(this);
  out << "  pushq %rax\n";
  idx->col->accept(this);
  out << "  movq %rax, %rdi\n";
  out << "  popq %rax\n";

  if (matrixColumns.count(idx->name)) {
    out << "  movq $" << matrixColumns[idx->name] << ", %rcx\n";
  } else if (currentMatrixParamLabels.count(idx->name)) {
    out << "  movq " << currentMatrixParamLabels[idx->name] << "(%rip), %rcx\n";
  } else {
    out << "  movq $0, %rcx\n";
  }
  out << "  imulq %rcx, %rax\n";
  out << "  addq %rdi, %rax\n";
  out << "  movq %rax, %rdi\n";
  out << "  popq %rcx\n";
  if (memoriaGlobal.count(idx->name)) {
    out << "  movq " << idx->name << "(%rip), %rax\n";
  } else {
    int off = memoria[idx->name];
    out << "  movq " << off << "(%rbp), %rax\n";
  }
  out << "  movq %rcx, (%rax, %rdi, 8)\n";
  return 0;
}

int GenCodeVisitor::computeAddress(FieldExp *exp) {
  std::string type = variableTypes[exp->object];
  int fieldIndex = structFieldOffsets[type][exp->field] / 8;

  out << "  pushq %rax\n";
  out << "  movq $" << fieldIndex << ", %rax\n";
  out << "  movq %rax, %rdi\n";
  out << "  popq %rax\n";
  out << "  movq %rax, %rcx\n";

  if (structAllocated.count(exp->object) && !structAllocated[exp->object]) {
    int fields = structFields[type];
    out << "  pushq %rcx\n";
    out << "  pushq %rdi\n";
    out << "  movq $" << (fields * 8) << ", %rdi\n";
    out << "  call malloc@PLT\n";
    if (memoriaGlobal.count(exp->object)) {
      out << "  movq %rax, " << exp->object << "(%rip)\n";
    } else {
      int off = memoria[exp->object];
      out << "  movq %rax, " << off << "(%rbp)\n";
    }
    out << "  popq %rdi\n";
    out << "  popq %rcx\n";
    structAllocated[exp->object] = true;
  }

  if (memoriaGlobal.count(exp->object)) {
    out << "  movq " << exp->object << "(%rip), %rax\n";
  } else {
    int off = memoria[exp->object];
    out << "  movq " << off << "(%rbp), %rax\n";
  }
  out << "  movq %rcx, (%rax, %rdi, 8)\n";
  return 0;
}

// -----------------------------------------------------------------------------
// visit(PrintStm) — imprime un entero con printf
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(PrintStm *stm) {
  stm->e->accept(this);
  out << "  movq %rax, %rsi\n";
  out << "  leaq print_fmt(%rip), %rdi\n";
  out << "  movq $0, %rax\n";
  out << "  call printf@PLT\n";
  return 0;
}

// -----------------------------------------------------------------------------
// visit(Body) — procesa declaraciones y sentencias
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(Body *b) {
  for (auto dec : b->declarations)
    dec->accept(this);
  for (auto stm : b->StmList)
    stm->accept(this);
  return 0;
}

// -----------------------------------------------------------------------------
// visit(IfStm) — emite bloque if-then-else con etiquetas únicas
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(IfStm *stm) {
  int lbl = labelcont++;
  stm->condition->accept(this);
  out << "  cmpq $0, %rax\n";
  out << "  je else_" << lbl << "\n";
  stm->then->accept(this);
  out << "  jmp endif_" << lbl << "\n";
  out << "else_" << lbl << ":\n";
  if (stm->els)
    stm->els->accept(this);
  out << "endif_" << lbl << ":\n";
  return 0;
}

// -----------------------------------------------------------------------------
// visit(WhileStm) — emite bucle while con etiquetas únicas
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(WhileStm *stm) {

  int lbl = labelcont++;

  std::string oldBreak = currentBreakLabel;
  currentBreakLabel = "endwhile_" + std::to_string(lbl);

  out << "while_" << lbl << ":\n";

  stm->condition->accept(this);

  out << "  cmpq $0, %rax\n";
  out << "  je endwhile_" << lbl << "\n";

  stm->b->accept(this);

  out << "  jmp while_" << lbl << "\n";

  out << "endwhile_" << lbl << ":\n";

  currentBreakLabel = oldBreak;

  return 0;
}

// -----------------------------------------------------------------------------
// visit(ReturnStm) — salta al epílogo de la función
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(ReturnStm *stm) {
  stm->e->accept(this);
  out << "  jmp .end_" << nombreFuncion << "\n";
  return 0;
}

// -----------------------------------------------------------------------------
// visit(FunDec) — emite prólogo, cuerpo y epílogo de una función
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(FunDec *f) {
  funcionesEmitidas.insert(f->nombre);
  entornoFuncion = true;
  memoria.clear();
  variableTypes.clear();
  structAllocated.clear();
  matrixColumns.clear();
  currentMatrixParamLabels.clear();
  offset = -8;
  nombreFuncion = f->nombre;

  const std::vector<std::string> argRegs = {"%rdi", "%rsi", "%rdx",
                                            "%rcx", "%r8",  "%r9"};

  // ---- Prólogo ----
  out << "\n.globl " << f->nombre << "\n";
  out << f->nombre << ":\n";
  out << "  pushq %rbp\n";
  out << "  movq %rsp, %rbp\n";
  out << "  subq $" << funcontador[f->nombre] * 8 << ", %rsp\n";

  // Guardar parámetros en el frame local
  int nParams = static_cast<int>(f->Pnombres.size());
  for (int i = 0; i < nParams; i++) {
    memoria[f->Pnombres[i]] = offset;
    variableTypes[f->Pnombres[i]] = f->Ptipos[i];
    if (f->Ptipos[i] == "matrix") {
      currentMatrixParamLabels[f->Pnombres[i]] =
          "__cols_" + f->nombre + "_" + f->Pnombres[i];
    }
    out << "  movq " << argRegs[i] << ", " << offset << "(%rbp)\n";
    offset -= 8;
  }

  // Registrar variables locales declaradas (ajusta 'offset' y 'memoria')
  for (auto dec : f->cuerpo->declarations)
    dec->accept(this);

  // ---- Cuerpo ----
  for (auto stm : f->cuerpo->StmList)
    stm->accept(this);

  // ---- Epílogo ----
  out << ".end_" << f->nombre << ":\n";
  out << "  leave\n";
  out << "  ret\n";

  entornoFuncion = false;
  return 0;
}

// -----------------------------------------------------------------------------
// visit(FcallExp) — emite una llamada a función
// Argumentos en registros según la ABI System V x86-64
// -----------------------------------------------------------------------------

int GenCodeVisitor::visit(FcallExp *exp) {
  if (exp->isConstant) {
    out << "  movq $" << exp->constantValue << ", %rax\n";
    return 0;
  }

  const std::vector<std::string> argRegs = {"%rdi", "%rsi", "%rdx",
                                            "%rcx", "%r8",  "%r9"};

  int nArgs = static_cast<int>(exp->argumentos.size());
  for (int i = 0; i < nArgs; i++) {
    exp->argumentos[i]->accept(this);
    out << "  movq %rax, " << argRegs[i] << "\n";
    if (funParamTypes.count(exp->nombre) &&
        i < static_cast<int>(funParamTypes[exp->nombre].size()) &&
        funParamTypes[exp->nombre][i] == "matrix") {
      std::string targetLabel =
          "__cols_" + exp->nombre + "_" + funParamNames[exp->nombre][i];
      if (auto id = dynamic_cast<IdExp *>(exp->argumentos[i])) {
        if (matrixColumns.count(id->value)) {
          out << "  movq $" << matrixColumns[id->value] << ", %r10\n";
          out << "  movq %r10, " << targetLabel << "(%rip)\n";
        } else if (currentMatrixParamLabels.count(id->value)) {
          out << "  movq " << currentMatrixParamLabels[id->value]
              << "(%rip), %r10\n";
          out << "  movq %r10, " << targetLabel << "(%rip)\n";
        }
      }
    }
  }
  out << "  call " << exp->nombre << "\n";
  requestFunctionEmission(exp->nombre);
  return 0;
}

int GenCodeVisitor::visit(DoWhileStm *stm) {

  int lbl = labelcont++;

  std::string oldBreak = currentBreakLabel;
  currentBreakLabel = "endwhile_" + std::to_string(lbl);

  out << "dowhile_" << lbl << ":\n";

  stm->b->accept(this);

  stm->condition->accept(this);

  out << "  cmpq $0, %rax\n";
  out << "  jne dowhile_" << lbl << "\n";

  out << "endwhile_" << lbl << ":\n";

  currentBreakLabel = oldBreak;

  return 0;
}

int GenCodeVisitor::visit(BreakStm *stm) {

  if (currentBreakLabel.empty()) {
    std::cerr << "Error: break fuera de while, do-while o switch\n";
    exit(1);
  }

  out << "  jmp " << currentBreakLabel << "\n";

  return 0;
}

int GenCodeVisitor::visit(SwitchStm *stm) {

  int lbl = labelcont++;

  stm->e->accept(this);

  out << "  movq %rax, %r10\n";

  for (auto c : stm->cases) {

    out << "  movq $" << c->value << ", %rax\n";
    out << "  cmpq %rax, %r10\n";
    out << "  je case_" << lbl << "_" << c->value << "\n";
  }

  if (!stm->default_body.empty())
    out << "  jmp default_" << lbl << "\n";
  else
    out << "  jmp endswitch_" << lbl << "\n";

  std::string oldBreak = currentBreakLabel;
  currentBreakLabel = "endswitch_" + std::to_string(lbl);

  for (auto c : stm->cases) {

    out << "case_" << lbl << "_" << c->value << ":\n";

    for (auto s : c->body)
      s->accept(this);

    out << "  jmp endswitch_" << lbl << "\n";
  }

  if (!stm->default_body.empty()) {

    out << "default_" << lbl << ":\n";

    for (auto s : stm->default_body)
      s->accept(this);
  }

  currentBreakLabel = oldBreak;

  out << "endswitch_" << lbl << ":\n";

  return 0;
}

