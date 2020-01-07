#ifndef PYTHON_INTERPRETER_EVALVISITOR_H
#define PYTHON_INTERPRETER_EVALVISITOR_H


#include "Python3BaseVisitor.h"
#include <stack>
#include <map>
#include <utility>
#include <exception>
#include <vector>
#include <deque>
#include "Object.h"
using namespace std;
using namespace antlrcpp;


class EvalVisitor: public Python3BaseVisitor {

//todo:override all methods of Python3BaseVisitor
private:
    class Var {
    public:
        string name;
        explicit Var(string s) {name = s;}
        bool operator<(const Var &oth) const {return name < oth.name;}
        Var(const Var &oth) = default;
        Var &operator=(const Var &oth) = default;
        friend ostream &operator<<(ostream &os, const Var &obj) {os << obj.name; return os;}
    };
    class Break_Exception: public exception {};
    class Continue_Exception: public exception {};
    class Return_Exception: public exception {
    public:
        vector<Object> Return_List;
        bool Return_Nothing;
        explicit Return_Exception(vector<Object> rl) {Return_List = std::move(rl); Return_Nothing = false;}
        Return_Exception() {Return_Nothing = true;};
        ~Return_Exception() override {Return_List.clear();}
    };
  stack<map<Var, Object>> variable_STACK;
  stack<vector<Var>> parameter_STACK;
  map<Var, Object> variable_GLOBAL, variable_BUFFER;
  map<string, Python3Parser::FuncdefContext*> func;
public:
  ~EvalVisitor() override;
  Object Evaluation(const Any &t) const;
  vector<Object> List_Evaluation(const vector<Any> &t) const;
  Object &get_Content(const Var &t);

  virtual antlrcpp::Any visitFile_input(Python3Parser::File_inputContext *ctx) override;

  virtual antlrcpp::Any visitFuncdef(Python3Parser::FuncdefContext *ctx) override;

  virtual antlrcpp::Any visitParameters(Python3Parser::ParametersContext *ctx) override;

  virtual antlrcpp::Any visitTypedargslist(Python3Parser::TypedargslistContext *ctx) override;

  virtual antlrcpp::Any visitTfpdef(Python3Parser::TfpdefContext *ctx) override;

  virtual antlrcpp::Any visitStmt(Python3Parser::StmtContext *ctx) override;

  virtual antlrcpp::Any visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx) override;

  virtual antlrcpp::Any visitSmall_stmt(Python3Parser::Small_stmtContext *ctx) override;

  virtual antlrcpp::Any visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) override;

  virtual antlrcpp::Any visitAugassign(Python3Parser::AugassignContext *ctx) override {return 0;}

  virtual antlrcpp::Any visitFlow_stmt(Python3Parser::Flow_stmtContext *ctx) override;

  virtual antlrcpp::Any visitBreak_stmt(Python3Parser::Break_stmtContext *ctx) override;

  virtual antlrcpp::Any visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx) override;

  virtual antlrcpp::Any visitReturn_stmt(Python3Parser::Return_stmtContext *ctx) override;

  virtual antlrcpp::Any visitCompound_stmt(Python3Parser::Compound_stmtContext *ctx) override;

  virtual antlrcpp::Any visitIf_stmt(Python3Parser::If_stmtContext *ctx) override;

  virtual antlrcpp::Any visitWhile_stmt(Python3Parser::While_stmtContext *ctx) override;

  virtual antlrcpp::Any visitSuite(Python3Parser::SuiteContext *ctx) override;

  virtual antlrcpp::Any visitTest(Python3Parser::TestContext *ctx) override;

  virtual antlrcpp::Any visitOr_test(Python3Parser::Or_testContext *ctx) override;

  virtual antlrcpp::Any visitAnd_test(Python3Parser::And_testContext *ctx) override;

  virtual antlrcpp::Any visitNot_test(Python3Parser::Not_testContext *ctx) override;

  virtual antlrcpp::Any visitComparison(Python3Parser::ComparisonContext *ctx) override;

  virtual antlrcpp::Any visitComp_op(Python3Parser::Comp_opContext *ctx) override {return 0;}

  virtual antlrcpp::Any visitArith_expr(Python3Parser::Arith_exprContext *ctx) override;

  virtual antlrcpp::Any visitTerm(Python3Parser::TermContext *ctx) override;

  virtual antlrcpp::Any visitFactor(Python3Parser::FactorContext *ctx) override;

  virtual antlrcpp::Any visitAtom_expr(Python3Parser::Atom_exprContext *ctx) override;

  virtual antlrcpp::Any visitTrailer(Python3Parser::TrailerContext *ctx) override;

  virtual antlrcpp::Any visitAtom(Python3Parser::AtomContext *ctx) override;

  virtual antlrcpp::Any visitTestlist(Python3Parser::TestlistContext *ctx) override;

  virtual antlrcpp::Any visitArglist(Python3Parser::ArglistContext *ctx) override;

  virtual antlrcpp::Any visitArgument(Python3Parser::ArgumentContext *ctx) override;
  };


#endif //PYTHON_INTERPRETER_EVALVISITOR_H
