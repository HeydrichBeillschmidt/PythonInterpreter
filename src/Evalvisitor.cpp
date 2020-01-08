#include "Evalvisitor.h"
#include "Object.h"

using namespace std;

EvalVisitor::~EvalVisitor() {
    variable_GLOBAL.clear();
    func.clear();
}

Object EvalVisitor::Evaluation(const Any &t) const {
    if (t.is<Var>()) {
        const Var p = t.as<Var>();
        if (!variable_STACK.empty()) {
            const auto &c = variable_STACK.top().find(p);
            if (c!=variable_STACK.top().end()) return c->second;
        }
        return variable_GLOBAL.find(p)->second;
    }
    else if (t.is<Object>()) return t.as<Object>();
    else return Object();
}

vector<Object> EvalVisitor::List_Evaluation(const vector<Any> &t) const {
    vector<Object> ans;
    for (size_t i = 0, it = t.size(); i < it; ++i) {ans.push_back(Evaluation(t[i]));}
    return ans;
}

Object & EvalVisitor::get_Content(const class EvalVisitor::Var & t) {
    if (!variable_STACK.empty()) {
        const auto &c = variable_STACK.top().find(t);
        if (c!=variable_STACK.top().end()) return c->second;
    }
    return variable_GLOBAL[t];
}

antlrcpp::Any EvalVisitor::visitFile_input(Python3Parser::File_inputContext *ctx) {
    for (const auto &c: ctx->stmt()) visit(c);
    return 0;
}

antlrcpp::Any EvalVisitor::visitFuncdef(Python3Parser::FuncdefContext *ctx) {
    string name = ctx->NAME()->toString();
    func[name] = ctx;
    return 0;
}

antlrcpp::Any EvalVisitor::visitParameters(Python3Parser::ParametersContext *ctx) {
    parameter_STACK.push(vector<Var>{});
    if (ctx->typedargslist()!= nullptr) visit(ctx->typedargslist());
    return 0;
}

antlrcpp::Any EvalVisitor::visitTypedargslist(Python3Parser::TypedargslistContext *ctx) {
    for (size_t i = 0, it = ctx->tfpdef().size(), ir = it - ctx->test().size(); i < it; ++i) {
        const Var c = visit(ctx->tfpdef()[i]).as<Var>();
        parameter_STACK.top().push_back(c);
        if (i >= ir) {
            variable_BUFFER[c] = Evaluation(visit(ctx->test()[i-ir]));
        }
    }
    return 0;
}

antlrcpp::Any EvalVisitor::visitTfpdef(Python3Parser::TfpdefContext *ctx) {
    return Var(ctx->NAME()->toString());
}

antlrcpp::Any EvalVisitor::visitStmt(Python3Parser::StmtContext *ctx) {
    if (ctx->simple_stmt()!= nullptr) visit(ctx->simple_stmt());
    else visit(ctx->compound_stmt());
    return 0;
}

antlrcpp::Any EvalVisitor::visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx) {
    visit(ctx->small_stmt());
    return 0;
}

antlrcpp::Any EvalVisitor::visitSmall_stmt(Python3Parser::Small_stmtContext *ctx) {
    if (ctx->expr_stmt()!= nullptr) visit(ctx->expr_stmt());
    else visit(ctx->flow_stmt());
    return 0;
}

antlrcpp::Any EvalVisitor::visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) {
    if (ctx->augassign()!=nullptr) {
        const auto op = ctx->augassign()->getText();
        const Var lexp = visit(ctx->testlist()[0]->test()[0]).as<Var>();
        const Object rexp = Evaluation(visit(ctx->testlist()[1]->test()[0]));
        if (op=="+=") {get_Content(lexp) += rexp;}
        else if (op=="-=") {get_Content(lexp) -= rexp;}
        else if (op=="*=") {get_Content(lexp) *= rexp;}
        else if (op=="/=") {get_Content(lexp).self_devision(rexp);}
        else if (op=="//=") {get_Content(lexp) /= rexp;}
        else {get_Content(lexp) %= rexp;}
    }
    else {
        if (!ctx->ASSIGN().empty()) {
            size_t it = ctx->testlist().size()-1;
            vector<Object> rexplist = List_Evaluation(visit(ctx->testlist().back()).as<vector<Any>>());
            for (size_t i = 0; i < it; ++i) {
                const vector<Any> lexplist = visit(ctx->testlist()[i]).as<vector<Any>>();
                for (size_t j = 0, jt = rexplist.size(); j < jt; ++j) {
                    const Var lexp = lexplist[j].as<Var>();
                    if (!variable_STACK.empty()) {
                        const auto &c = variable_STACK.top().find(lexp);
                        if (c!=variable_STACK.top().end()) variable_STACK.top()[lexp] = rexplist[j];
                        else {
                            const auto &p = variable_GLOBAL.find(lexp);
                            if (p!=variable_GLOBAL.end()) variable_GLOBAL[lexp] = rexplist[j];
                            else variable_STACK.top()[lexp] = rexplist[j];
                        }
                    }
                    else {variable_GLOBAL[lexp] = rexplist[j];}
                }
            }
        }
        else {visit(ctx->testlist().back());}
    }
    return 0;
}

antlrcpp::Any EvalVisitor::visitFlow_stmt(Python3Parser::Flow_stmtContext *ctx) {
    if (ctx->break_stmt()!= nullptr) visit(ctx->break_stmt());
    else if (ctx->continue_stmt()!= nullptr) visit (ctx->continue_stmt());
    else visit(ctx->return_stmt());
    return 0;
}

antlrcpp::Any EvalVisitor::visitBreak_stmt(Python3Parser::Break_stmtContext *ctx) {
    throw EvalVisitor::Break_Exception();
}

antlrcpp::Any EvalVisitor::visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx) {
    throw EvalVisitor::Continue_Exception();
}

antlrcpp::Any EvalVisitor::visitReturn_stmt(Python3Parser::Return_stmtContext *ctx) {
    if (ctx->testlist()!= nullptr) {
        vector<Any> tmp = visit(ctx->testlist()).as<vector<Any>>();
        throw EvalVisitor::Return_Exception(List_Evaluation(tmp));
    }
    else throw EvalVisitor::Return_Exception();
}

antlrcpp::Any EvalVisitor::visitCompound_stmt(Python3Parser::Compound_stmtContext *ctx) {
    if (ctx->if_stmt()!= nullptr) visit(ctx->if_stmt());
    else if (ctx->while_stmt()!= nullptr) visit(ctx->while_stmt());
    else visit(ctx->funcdef());
    return 0;
}

antlrcpp::Any EvalVisitor::visitIf_stmt(Python3Parser::If_stmtContext *ctx) {
    for (size_t i = 0, it = ctx->test().size(); i < it; ++i) {
        bool condition = bool(Evaluation(visit(ctx->test()[i])));
        if (condition) {visit(ctx->suite()[i]); return 0;}
    }
    if (ctx->ELSE()!= nullptr) visit(ctx->suite().back());
    return 0;
}

antlrcpp::Any EvalVisitor::visitWhile_stmt(Python3Parser::While_stmtContext *ctx) {
    try {
        while (bool(Evaluation(visit(ctx->test())))) {
            try {visit(ctx->suite());}
            catch (EvalVisitor::Continue_Exception) {}
        }
    }
    catch (EvalVisitor::Break_Exception) {}
    return 0;
}

antlrcpp::Any EvalVisitor::visitSuite(Python3Parser::SuiteContext *ctx) {
    if (ctx->simple_stmt()!=nullptr) {visit(ctx->simple_stmt());}
    else {
        for (const auto &c:ctx->stmt()) {visit(c);}
    }
    return 0;
}

antlrcpp::Any EvalVisitor::visitTest(Python3Parser::TestContext *ctx) {
    return visit(ctx->or_test());
}

antlrcpp::Any EvalVisitor::visitOr_test(Python3Parser::Or_testContext *ctx) {
    if (ctx->and_test().size()==1) {return visit(ctx->and_test().front());}
    else {
        for (const auto &p: ctx->and_test()) {if (bool(Evaluation(visit(p)))) return Object(true);}
        return Object(false);
    }
}

antlrcpp::Any EvalVisitor::visitAnd_test(Python3Parser::And_testContext *ctx) {
    if (ctx->not_test().size()==1) {return visit(ctx->not_test().front());}
    else {
        for (const auto &p: ctx->not_test()) {if (!bool(Evaluation(visit(p)))) return Object(false);}
        return Object(true);
    }
}

antlrcpp::Any EvalVisitor::visitNot_test(Python3Parser::Not_testContext *ctx) {
    if (ctx->NOT()!= nullptr) {
        Object tmp = Object(!bool(Evaluation(visit(ctx->not_test()))));
        return tmp;
    }
    else {return visit(ctx->comparison());}
}

antlrcpp::Any EvalVisitor::visitComparison(Python3Parser::ComparisonContext *ctx) {
    if (ctx->arith_expr().size()==1) return visit(ctx->arith_expr().front());
    else {
        vector<Object> compare_list;
        compare_list.push_back(Evaluation(visit(ctx->arith_expr()[0])));
        for (size_t i = 1, it = ctx->arith_expr().size(); i < it; ++i) {
            compare_list.push_back(Evaluation(visit(ctx->arith_expr()[i])));
            const Object &a = compare_list[i-1], &b = compare_list[i];
            const auto op = ctx->comp_op()[i-1]->getText();
            if (op=="==") {
                if (!(a==b)) {compare_list.clear(); return Object(false);}
            }
            else if (op=="!=") {
                if (!(a!=b)) {compare_list.clear(); return Object(false);}
            }
            else if (op=="<") {
                if (!(a<b)) {compare_list.clear(); return Object(false);}
            }
            else if (op==">") {
                if (!(a>b)) {compare_list.clear(); return Object(false);}
            }
            else if (op=="<=") {
                if (!(a<=b)) {compare_list.clear(); return Object(false);}
            }
            else {
                if (!(a>=b)) {compare_list.clear(); return Object(false);}
            }
        }
        compare_list.clear(); return Object(true);
    }
}

antlrcpp::Any EvalVisitor::visitArith_expr(Python3Parser::Arith_exprContext *ctx) {
    if (ctx->term().size()==1) return visit(ctx->term().front());
    else {
        Object ans = Evaluation(visit(ctx->term().front()));
        vector<int> add, minus;
        for (const auto &c: ctx->ADD()) {add.push_back(c->getSymbol()->getTokenIndex());}
        add.push_back(1e9);
        for (const auto &c: ctx->MINUS()) {minus.push_back(c->getSymbol()->getTokenIndex());}
        minus.push_back(1e9);
        size_t j=0, k=0;
        for (size_t i = 1, it = ctx->term().size(); i < it; ++i) {
            Object tmp = Evaluation(visit(ctx->term()[i]));
            if (add[j]<minus[k]) {ans = ans + tmp; ++j;}
            else {ans = ans - tmp; ++k;}
        }
        add.clear(); minus.clear();
        return ans;
    }
}

antlrcpp::Any EvalVisitor::visitTerm(Python3Parser::TermContext *ctx) {
    if (ctx->factor().size()==1) return visit(ctx->factor().front());
    else {
        Object ans = Evaluation(visit(ctx->factor().front()));
        vector<int> star, div, idiv, mod;
        for (const auto &c: ctx->STAR()) {star.push_back(c->getSymbol()->getTokenIndex());}
        star.push_back(1e9);
        for (const auto &c: ctx->DIV()) {div.push_back(c->getSymbol()->getTokenIndex());}
        div.push_back(1e9);
        for (const auto &c: ctx->IDIV()) {idiv.push_back(c->getSymbol()->getTokenIndex());}
        idiv.push_back(1e9);
        for (const auto &c: ctx->MOD()) {mod.push_back(c->getSymbol()->getTokenIndex());}
        mod.push_back(1e9);
        size_t j=0, k=0, m=0, n=0;
        for (size_t i = 1, it = ctx->factor().size(); i < it; ++i) {
            Object tmp = Evaluation(visit(ctx->factor()[i]));
            if (star[j]<div[k]&&star[j]<idiv[m]&&star[j]<mod[n]) {ans = ans * tmp; ++j;}
            else if (div[k]<star[j]&&div[k]<idiv[m]&&div[k]<mod[n]) {ans = float_devision(ans, tmp); ++k;}
            else if (idiv[m]<star[j]&&idiv[m]<div[k]&&idiv[m]<mod[n]) {ans = ans / tmp; ++m;}
            else {ans = ans % tmp; ++n;}
        }
        star.clear(); div.clear(); idiv.clear(); mod.clear();
        return ans;
    }
}

antlrcpp::Any EvalVisitor::visitFactor(Python3Parser::FactorContext *ctx) {
    if (ctx->atom_expr()!= nullptr) {return visit(ctx->atom_expr());}
    else {
        if (ctx->ADD()!= nullptr) {return Evaluation(visit(ctx->factor()));}
        else {
            Object tmp = Evaluation(visit(ctx->factor()));
            return tmp * Object(BigInteger(-1));
        }
    }
}

antlrcpp::Any EvalVisitor::visitAtom_expr(Python3Parser::Atom_exprContext *ctx) {
    if (ctx->trailer()!=nullptr) {
        string Name = visit(ctx->atom()).as<Var>().name;
        if (Name=="print") {
            if (ctx->trailer()->arglist()!=nullptr) {
                for (const auto &p: ctx->trailer()->arglist()->argument()) {
                    Any t = visit(p);
                    if (t.is<Object>()) {
                        Object tmp = t.as<Object>();
                        if (tmp.type==FLOAT) cout << fixed << tmp << ' ';
                        else cout << tmp << ' ';
                    }
                    else cout << "None ";
                }
                cout << endl;
            }
            else {cout << endl;}
            return Object();
        }
        else if (Name=="str"||Name=="bool"||Name=="int"||Name=="float") {
            Object tmp = visit(ctx->trailer()->arglist()->argument().front()).as<Object>();
            if (Name=="str") return Object(string(tmp));
            else if (Name=="bool") return Object(bool(tmp));
            else if (Name=="int") return Object(BigInteger(tmp));
            else return Object(double(tmp));
        }
        else {
            visit(func[Name]->parameters());
            visit(ctx->trailer());
            variable_STACK.push(variable_BUFFER); variable_BUFFER.clear();
            vector<Object> t;
            try {
                visit(func[Name]->suite());
            }
            catch (EvalVisitor::Return_Exception &re) {
                if (!re.Return_Nothing) {t = re.Return_List;}
            }
            parameter_STACK.top().clear(); parameter_STACK.pop();
            variable_STACK.top().clear(); variable_STACK.pop();
            if (!t.empty()) {
                if (t.size()==1) {return t.front();}
                else return t;
            }
            else return Object();
        }
    }
    else {return visit(ctx->atom());}
}

antlrcpp::Any EvalVisitor::visitTrailer(Python3Parser::TrailerContext *ctx) {
    if (ctx->arglist()!= nullptr) visit(ctx->arglist());
    return 0;
}

antlrcpp::Any EvalVisitor::visitAtom(Python3Parser::AtomContext *ctx) {
    if (ctx->NAME()!=nullptr) {
        return Var(ctx->NAME()->toString());
    }
    else if (!(ctx->STRING().empty())) {
        string ss;
        for (const auto &c:ctx->STRING()) {
            string tmp = c->toString();
            tmp.erase(tmp.begin());
            auto e = tmp.end(); --e;
            tmp.erase(e);
            ss += tmp;
        }
        return Object(ss);
    }
    else if (ctx->NUMBER()!=nullptr) {
        string tmp = ctx->NUMBER()->toString();
        for (const auto &c:tmp) {
            if (c=='.') {return Object(stod(tmp));}
        }
        return Object(BigInteger(tmp));
    }
    else if (ctx->TRUE()!=nullptr) return Object(true);
    else if (ctx->FALSE()!=nullptr) return Object(false);
    else if (ctx->test()!=nullptr) return visit(ctx->test());
    else if (ctx->NONE()!= nullptr) return Object();
    else return NULL;
}

antlrcpp::Any EvalVisitor::visitTestlist(Python3Parser::TestlistContext *ctx) {
    vector<Any> ans;
    if (ctx->test().size()==1) {
        Any tmp = visit(ctx->test().front());
        if (tmp.is<Object>() || tmp.is<Var>()) {ans.push_back(tmp); return ans;}
        else if (tmp.is<vector<Any>>()) {
            for (const auto p: tmp.as<vector<Any>>()) {ans.push_back(p);}
            return ans;
        }
        else if (tmp.is<vector<Object>>()) {
            for (const auto p: tmp.as<vector<Object>>()) {ans.push_back(p);}
            return ans;
        }
        else {return vector<Any>{Object()};}
    }
    else {
        for (const auto &c: ctx->test()) {
            Any tmp = visit(c);
            if (tmp.is<Object>() || tmp.is<Var>()) ans.push_back(tmp);
            else if (tmp.is<vector<Any>>()) {
                for (const auto p: tmp.as<vector<Any>>()) {ans.push_back(p);}
            }
            else if (tmp.is<vector<Object>>()) {
                for (const auto p: tmp.as<vector<Object>>()) {ans.push_back(p);}
            }
            else {return vector<Any>{Object()};}
        }
        return ans;
    }
}

antlrcpp::Any EvalVisitor::visitArglist(Python3Parser::ArglistContext *ctx) {
    for (size_t i = 0, it = ctx->argument().size(); i < it; ++i) {
        Object tmp = visit(ctx->argument()[i]).as<Object>();
        if (ctx->argument()[i]->NAME()==nullptr) {
            Var Name = parameter_STACK.top()[i];
            variable_BUFFER[Name] = tmp;
        }
        else {
            Var Name = Var(ctx->argument()[i]->NAME()->toString());
            variable_BUFFER[Name] = tmp;
        }
    }
    return 0;
}

antlrcpp::Any EvalVisitor::visitArgument(Python3Parser::ArgumentContext *ctx) {
    Any tmp = visit(ctx->test());
    if (tmp.is<Object>()||tmp.is<Var>()) return Evaluation(tmp);
    else return Object();
}
