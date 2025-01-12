#pragma once
#include <cassert>
#include <fstream>
#include <unordered_map>
#include <bits/stdc++.h>

//定义一个结构体Symbol用来标识函数或变量特性
typedef struct 
{
  /*
    首先是函数返回类型
    其中0表示常量，1表示变量
    2表示无返回值，3表示int
  */
  int type;

  //然后是表示函数中参数个数以及常量值的变量
  int value;

  //最后是储存的函数、变量，常量等标识符(也就是名称)
  std::string str;
}Symbol;

//定义一个可以储存表示Symbol的map
typedef std::map<std::string, Symbol> Symbolmap;

//定义一个用来查找Symbolmap中Symbol的函数
typedef struct func_symbol
{
  int dep;
  int end;
  std::stack<int> loopstack;
  std::vector<Symbolmap> smap;
  std::set<std::string> nameset;
  func_symbol()
  {
    dep = 0;
    end = 0;
  }
}func_symbol;

typedef struct
{
  Symbolmap globalsymbol;
  std::map<std::string, std::unique_ptr<func_symbol>> funcsymbolmap;
} Symboltable;

//定义完结构体后一些静态变量
static Symboltable symbt;
static func_symbol *currentsymbt = NULL;
static std::string globalstr;

static int IFcount = 0;
static int othercount = 0;
static int whilecount = 0;
static int entrycount = 0;

static int debug = 0;
static int now = 0;    
static int curr = 0;


// 所有 AST 的基类
class BaseAST {
 public:
  virtual ~BaseAST() = default;

  virtual std::string dump() const = 0;
  virtual std::string ddump() {
    return std::string("Arrived a wrong place!");
  }

  virtual int cal() {
    return 0;
  }

  virtual std::string ArrayCal() {
    std::string s;
    return s;
  }

  virtual int Assign(std::string s){

  }

  virtual int Allocpara() {
    return 0;
  }

  virtual std::string Fetch(std::vector<int> dim, std::string Ident, int index, int k) {
    if (k == 0)
    {
      std::cout << "  %" << now + 1 << " = getelemptr " << Ident << ", " << index % dim[k] << std::endl;
      now++;
      return std::string("%") + std::to_string(now); 
    }
    std::string str = BaseAST::Fetch(dim, Ident, index/dim[k], k - 1);
    std::cout << "  %" << now + 1 << " = getelemptr " << str << ", " << index % dim[k] << std::endl;
    now++;
    return std::string("%") + std::to_string(now); 
  }
  virtual void init(std::vector<int> dim, int * a, int depth) {};
  virtual void init(std::vector<int> dim, std::string Ident, int depth) {};
  
  virtual void dumparr(std::vector<int> dim, int * a, int depth){
    if (depth == dim.size() - 1)
    {

      std::cout <<"{" << a[curr++];
      for (int i = 1; i < dim[depth]; ++i)
      {
        std::cout << "," << a[curr++];
      }
      std::cout << "}";
      return;
    }
    std::cout << "{" ;
    BaseAST::dumparr(dim, a, depth + 1);
    for (int i = 1; i < dim[depth]; ++i)
    {
      std::cout << ",";
      BaseAST::dumparr(dim, a, depth + 1);
    }
    std::cout << "}" ;
  }
};

//对顶层的解析
class CompUnitAST : public BaseAST
{
 public:
  std::unique_ptr<BaseAST> compunits;
  
  //重构一下dump来输出顶层类型
  std::string dump() const override
  {
    if (debug) std::cout << "CompUnitAST:" << std::endl;
    // 库函数
    std::cout << "decl @getint(): i32" << std::endl;
    std::cout << "decl @getch(): i32" << std::endl;
    std::cout << "decl @getarray(*i32): i32" << std::endl;
    std::cout << "decl @putint(i32)" << std::endl;
    std::cout << "decl @putch(i32)" << std::endl;
    std::cout << "decl @putarray(i32, *i32)" << std::endl;
    std::cout << "decl @starttime()" << std::endl;
    std::cout << "decl @stoptime()" << std::endl;

    symbt.globalsymbol["getint"] = {3, 0, "getint"};
    symbt.globalsymbol["getch"] = {3, 0, "getch"};
    symbt.globalsymbol["getarray"] = {3, 1, "getarray"};
    symbt.globalsymbol["putint"] = {2, 1, "putint"};
    symbt.globalsymbol["putch"] = {2, 1, "putch"};
    symbt.globalsymbol["putarray"] = {2, 2, "putarray"};
    symbt.globalsymbol["starttime"] = {2, 0, "starttime"};
    symbt.globalsymbol["stoptime"] = {2, 0, "stoptime"};

    return compunits->dump();
  }
};

class CompUnitsAST : public BaseAST
{
 public:
  // 用智能指针管理对象
  std::unique_ptr<BaseAST> compunits;
  std::unique_ptr<BaseAST> func_def;
  std::unique_ptr<BaseAST> decl;
  //根据不同指针类型输出
  std::string dump() const override
  {
    if (debug) std::cout << "CompUnitsAST:" << std::endl;
    if (compunits)
    {
      compunits->dump();
    }
    if (func_def)
    {
      func_def->dump();
    }
    if (decl)
    {
      decl->dump();
    }
    std::string str;
    return str;
  }
};

// Functype
class FuncTypeAST : public BaseAST
{
 public:
  std::string type;
  std::string dump() const override
  {
    if (debug) std::cout << "FuncTypeAST" << std::endl;
    if (type == "int")
      std::cout << ": i32 ";
    else
      std::cout << " ";
    std::string str;
    return str;
  }
  int cal() override
  {
    if (type == "int")
      return 1;
    else
      return 0;
  }
};

//BType
class BTypeAST: public BaseAST
{
 public:
  std::string type;
  std::string dump() const override
  {
    std::cout << type << std::endl;
    return type;
  }
};

// 函数名
class FuncDefAST : public BaseAST
{
 public:
  int funcType;
  std::string Ident;
  std::unique_ptr<BaseAST> funcp;
  std::unique_ptr<BaseAST> block;
  std::string dump() const override
  {
    std::cout << "fun " << "@" << Ident << "(";
    if (funcp)
      funcp->dump();
    std::cout << ")";
    if (funcType == 1)
      std::cout << ": i32";
    std::cout << " {" << std::endl;
    std::cout << "%entry_" << entrycount++ << ":" << std::endl;
    symbt.funcsymbolmap[Ident] = std::make_unique<func_symbol>();
    currentsymbt = symbt.funcsymbolmap[Ident].get();
    Symbolmap m;
    currentsymbt->smap.push_back(m);
    currentsymbt->end = 0;
    if (funcp)
    {
      if (funcType == 1)
        symbt.globalsymbol[Ident] = {3, funcp->Allocpara(), Ident + "_00"};
      else
        symbt.globalsymbol[Ident] = {2, funcp->Allocpara(), Ident + "_00"};
    }
    else
    {
      if (funcType == 1)
        symbt.globalsymbol[Ident] = {3, 0, Ident + "_00"};
      else
        symbt.globalsymbol[Ident] = {2, 0, Ident + "_00"};
    }
    block->dump();
    if (currentsymbt->end == 0)
    {
      if (funcType == 1)
      {
        now++;
        std::cout << "  %" << now << " = add 0, 0" << std::endl;
        std::cout << "  ret %" << now << std::endl;
      }
      else
        std::cout << "  ret" << std::endl;
      currentsymbt->end = 1;
    }
    std::cout << "}" << std::endl;
    std::cout << std::endl;
    currentsymbt->smap.pop_back();
    currentsymbt = NULL;
    std::string str;
    return str;
  }
};

class FunorVarAST : public BaseAST
{
 public:
  std::unique_ptr<BaseAST> func_type;
  std::unique_ptr<BaseAST> funcdef;
  std::unique_ptr<BaseAST> vardef;
  std::string dump() const override
  {
    if (debug)
      std::cout << "FunorVarAST" << std::endl;
    if (funcdef)
    {
      ((FuncDefAST *)funcdef.get())->funcType = func_type->cal();
      funcdef->dump();
    }
    if (vardef)
    {
      vardef->dump();
    }
    std::string str;
    return str;
  }
};

class FuncFParamsAST : public BaseAST
{
 public:
  std::unique_ptr<BaseAST> para;
  std::unique_ptr<BaseAST> paras;
  std::string dump() const override
  {
    para->dump();
    if (paras)
    {
      std::cout << ", ";
      paras->dump();
    }
    std::string str;
    return str;
  }
  int Allocpara() override
  {
    para->Allocpara();
    if (paras)
      return 1 + paras->Allocpara();
    return 1;
  }
};

class FuncFParamAST : public BaseAST
{
public:
  int isarray;
  std::string Ident;
  std::unique_ptr<BaseAST> ArrayDef;
  std::string dump() const override
  {
    if (isarray == 0)
    {
      std::cout << "@" << Ident << ": " << "i32";
    }
    else
    {
      if (ArrayDef == NULL)
        std::cout << "@" << Ident << ": " << "*i32";
      else
      {
        std::string arrdim = ArrayDef->ArrayCal();
        std::cout << "@" << Ident << ": *" << globalstr;
      }
    }
    std::string str;
    return str;
  }
  int Allocpara() override
  {
    if (isarray == 0)
    {
      currentsymbt->nameset.insert(Ident + std::string("_0"));
      std::cout << "  @" << Ident << "_0" << " = alloc i32" << std::endl;
      currentsymbt->smap[0][Ident] = {0, 0, Ident + std::string("_0")};
      std::cout << "  store @" << Ident << ", @" << Ident << "_0" << std::endl;
      return 0;
    }
    if (ArrayDef == NULL)
    {
      currentsymbt->nameset.insert(Ident + std::string("_0"));
      std::cout << "  @" << Ident << "_0" << " = alloc *i32" << std::endl;
      currentsymbt->smap[0][Ident] = {4, 1, Ident + std::string("_0")};
      std::cout << "  store @" << Ident << ", @" << Ident << "_0" << std::endl;

      return 0;
    }
    else
    {
      currentsymbt->nameset.insert(Ident + std::string("_0"));
      std::string arrdim = ArrayDef->ArrayCal();
      char *spl = std::strtok((char *)arrdim.c_str(), ",");
      std::vector<int> dim;
      while(spl)
      {
        dim.push_back(atoi(spl));
        spl = strtok(NULL, ",");
      }
      std::cout << "  @" << Ident << "_0" << " = alloc *" << globalstr << std::endl;
      currentsymbt->smap[0][Ident] = {4, (int)dim.size() + 1, Ident + std::string("_0")};
      std::cout << "  store @" << Ident << ", @" << Ident << "_0" << std::endl;
      return 0;
    }
    return 0;
  }
};

// 基本块分析
class BlockAST : public BaseAST
{
public:
  std::unique_ptr<BaseAST> blockitem;
  std::string dump() const override
  {
    if (debug) std::cout << "BlockAST" << std::endl;
    currentsymbt->dep++;
    Symbolmap m;
    currentsymbt->smap.push_back(m);
    std::string str = blockitem->dump();
    currentsymbt->smap.pop_back();
    currentsymbt->dep--;
    return str;
  }
};

class BlockItemAST : public BaseAST
{
public:
  std::unique_ptr<BaseAST> stmt;
  std::unique_ptr<BaseAST> decl;
  std::unique_ptr<BaseAST> blockitem;
  std::string dump() const override
  {
    if (debug) std::cout << "BlockItemAST:" << std::endl;
    if (stmt || decl)
    {
      if (currentsymbt->end == 1)
      {
        currentsymbt->end = 0;
        std::cout << "%other_" << othercount++ << ":" << std::endl;
      }
    }
    if (stmt != NULL)
      stmt->dump();
    if (decl != NULL)
      decl->dump();
    if (blockitem != NULL)
      blockitem->dump();
    std::string str;
    return str;
  }
};

//Decl(声明)部分
class DeclAST : public BaseAST
{
 public:
  std::unique_ptr<BaseAST> constdecl;
  std::unique_ptr<BaseAST> vardecl;
  std::string dump() const override
  {
    if (debug) std::cout << "DECL" << std::endl;
    if (vardecl != NULL)
      return vardecl->dump();
    else
      return constdecl->dump();
  }
};

class ConstExpAST: public BaseAST
{
 public:
  std::unique_ptr<BaseAST> exp;
  std::string dump() const override
  {
    if (debug) std::cout << "ConstExpAST:" << std::endl;
    std::string str = exp->dump();
    return str;
  }

  int cal() override
  {
    return exp->cal();
  }
};

// 对应ConstArrayInitValAST
class ConstArrayInitValAST: public BaseAST
{
 public:
  std::unique_ptr<BaseAST> constinitvalue;
  std::unique_ptr<BaseAST> constarrinitvalue;
  std::string dump() const override
  {
    std::string str;
    return str;
  }
  void init(std::vector<int> dim, int *a, int depth) override
  {
    constinitvalue->init(dim, a, depth);
    if (constarrinitvalue)
    {
      constarrinitvalue->init(dim, a, depth);
    }
  }
  void init(std::vector<int> dim, std::string Ident, int depth) override
  {
    constinitvalue->init(dim, Ident, depth);
    if (constarrinitvalue)
    {
      constarrinitvalue->init(dim, Ident, depth);
    }
  }
};

class ConstInitValAST: public BaseAST
{
 public:
  std::unique_ptr<BaseAST> ConstExp;
  std::unique_ptr<BaseAST> ConstArrayInitValue;
  std::string dump() const override
  {
    if (ConstExp)
      return ConstExp->dump();
    std::string str;
    return str;
  }
  int cal() override
  {
    if (ConstExp)
      return ConstExp->cal();
    return 0;
  }
  std::string ArrayCal() override
  {
    std::string str;
    if (ConstArrayInitValue)
    {
      str =  ConstArrayInitValue->ArrayCal();
    }
    return str;
  }
  void init(std::vector<int> dim, int *a, int depth) override
  {
    if (ConstExp)
    {
      a[curr++] = ConstExp->cal();
      return;
    }
    int sz = 1;
    for (int i = depth; i < dim.size(); ++i)
    {
      sz = sz * dim[i];
    }
    int sz1 = 1;
    int temp = curr;
    for (int i = dim.size() - 1; i >= 0 ; i--)
    {
      if (temp % dim[i] == 0)
      {
        sz1 *= dim[i];
        temp = temp / dim[i];
      }
      else
        break;
    }
    if (sz > sz1)
      sz = sz1;
    if (ConstArrayInitValue == NULL)
    {
      curr += sz;
      return;
    }
    if (ConstArrayInitValue)
    {
      ConstArrayInitValue->init(dim, a, depth + 1);
      curr = (curr + sz - 1) / sz * sz;
    }
  }

  void init(std::vector<int> dim, std::string Ident, int depth) override
  {
    if (ConstExp)
    {
      std::string dst = BaseAST::Fetch(dim, Ident, curr, dim.size() - 1);
      curr++;
      std::cout << "  store " << ConstExp->cal() << ", " << dst << std::endl;
      return;
    }
    int sz = 1;
    for (int i = depth; i < dim.size(); ++i)
    {
      sz = sz * dim[i];
    }
    int temp = curr;
    int sz1 = 1;
    for (int i = dim.size() - 1; i >= 0 ; i--)
    {
      if (temp % dim[i] == 0)
      {
        sz1 *= dim[i];
        temp = temp / dim[i];
      }
      else
        break;
    }
    if (sz > sz1)
      sz = sz1;
    if (ConstArrayInitValue == NULL)
    {
      for (int i = 0; i < sz; ++i)
      {
        std::string dst = BaseAST::Fetch(dim, Ident, curr, dim.size() - 1);
        curr++;
        std::cout << "  store " << "0" << ", " << dst << std::endl;
      }
      return;
    }
    if (ConstArrayInitValue)
    {
      ConstArrayInitValue->init(dim, Ident, depth + 1);
      int repeat = (curr + sz - 1) / sz * sz - curr;
      for (int i = 0; i < repeat; ++i)
      {
        std::string dst = BaseAST::Fetch(dim, Ident, curr, dim.size() - 1);
        curr++;
        std::cout << "  store " << "0" << ", " << dst << std::endl;
      }
    }
  }
};

class ArrayDefAST: public BaseAST
{
public:
  std::unique_ptr<BaseAST> ConstExp;
  std::unique_ptr<BaseAST> ArrayDef;
  std::string dump() const override
  {
    std::string str = ConstExp->dump();
    if (ArrayDef)
      str += ArrayDef->dump();
    return str;
  }
  std::string ArrayCal() override
  {
    std::string str;
    if (ArrayDef)
    {
      str = std::to_string(ConstExp->cal()) + ", " + ArrayDef->ArrayCal();
      globalstr = std::string("[") + globalstr + ", " + std::to_string(ConstExp->cal()) + "]";
    }
    else
    {
      str = std::to_string(ConstExp->cal());
      globalstr = std::string("[i32, ") + std::to_string(ConstExp->cal()) + "]";
    }
    return str;
  }
};

class ConstDefAST: public BaseAST
{
 public:
  std::string Ident;
  std::unique_ptr<BaseAST> ArrayDef;
  std::unique_ptr<BaseAST> constinitvalue;
  std::unique_ptr<BaseAST> constdef;
  std::string dump() const override
  {
    if (debug) std::cout << "ConstDefAST" << std::endl;
    if (currentsymbt == NULL)   
    {
      if (ArrayDef == NULL)   
      {
        symbt.globalsymbol[Ident] = {1, constinitvalue->cal(), Ident + "_00"};
      }
      else      
      {
        std::string arrdim = ArrayDef->ArrayCal();
        char *spl = std::strtok((char *)arrdim.c_str(), ",");
        std::vector<int> dim;
        while(spl)
        {
          dim.push_back(atoi(spl));
          spl = strtok(NULL, ",");
        }
        symbt.globalsymbol[Ident] = {3, (int)dim.size(), Ident + "_00"};
        std::cout << "global @" << Ident + "_00" << " = alloc " << globalstr << ", ";
        int k = 1;
        for (int i = 0; i < dim.size(); ++i)
        {
          k = k * dim[i];
        }

        int *a = new int[k];
        memset(a, 0, sizeof(int) * k);
        curr = 0;
        constinitvalue->init(dim, a, 0);
        curr = 0;
        constinitvalue->dumparr(dim, a, 0);
        std::cout << std::endl;
      }
    }
    else     
    {
      if (ArrayDef == NULL)   
      {
        currentsymbt->smap[currentsymbt->dep][Ident] = {1, constinitvalue->cal(), Ident + std::string("_") + std::to_string(currentsymbt->dep)};
      }
      else    
      {
        std::string arrdim = ArrayDef->ArrayCal();

        char *spl = std::strtok((char *)arrdim.c_str(), ",");
        std::vector<int> dim;
        while(spl)
        {
          dim.push_back(atoi(spl));
          spl = strtok(NULL, ",");
        }
        currentsymbt->smap[currentsymbt->dep][Ident] = {3, (int)dim.size(), Ident + std::string("_") + std::to_string(currentsymbt->dep)};
        std::cout << "  @" << Ident << "_" << currentsymbt->dep << " = alloc " << globalstr << std::endl;
        curr = 0;
        constinitvalue->init(dim, std::string("@") + Ident + "_" + std::to_string(currentsymbt->dep), 0);
      }
    }
    if (constdef)
      constdef->dump();
    std::string str;
    return str;
  }
};

class ConstDeclAST: public BaseAST
{
public:
  std::string const_;
  std::unique_ptr<BaseAST> btype;
  std::unique_ptr<BaseAST> constdef;
  std::string dump() const override
  {
    if (debug)
      std::cout << "ConstDeclAST" << std::endl;
    return constdef->dump();
  }
};


class ArrInitValAST: public BaseAST
{
 public:
  std::unique_ptr<BaseAST> initvalue;
  std::unique_ptr<BaseAST> arrayinitvalue;
  std::string dump() const override
  {
    std::string str;
    if (initvalue)
      str = initvalue->dump();
    if (arrayinitvalue)
      str = str + "," + arrayinitvalue->dump();
    return str;
  }
  std::string ArrayCal() override
  {
    std::string str;
    str = std::to_string(initvalue->cal());
    if (arrayinitvalue)
    {
      str = str + "," + arrayinitvalue->ArrayCal();
    }
    return str;
  }
  void init(std::vector<int> dim, int *a, int depth) override
  {
    initvalue->init(dim, a, depth);
    if (arrayinitvalue)
    {
      arrayinitvalue->init(dim, a, depth);
    }
  }
  void init(std::vector<int> dim, std::string Ident, int depth) override
  {
    initvalue->init(dim, Ident, depth);
    if (arrayinitvalue)
    {
      arrayinitvalue->init(dim, Ident, depth);
    }
  }
};

class InitValAST: public BaseAST
{
public:
  int zeroinit;
  std::unique_ptr<BaseAST> exp;
  std::unique_ptr<BaseAST> arrayinitvalue;
  std::string dump() const override
  {
    std::string str;
    if (exp)
      str = exp->dump();
    if (arrayinitvalue)
      str = arrayinitvalue->dump();
    return str;
  }
  int cal() override
  {
    if (exp)
      return exp->cal();
    return 0;
  }
  std::string ArrayCal() override
  {
    std::string str;
    if (arrayinitvalue)
    {
      str = arrayinitvalue->ArrayCal();
    }
    return str;
  }
  void init(std::vector<int> dim, int *a, int depth) override
  {
    if (exp)
    {
      a[curr++] = exp->cal();
      return;
    }
    int sz = 1;
    for (int i = depth; i < dim.size(); ++i)
    {
      sz = sz * dim[i];
    }
    int temp = curr;
    int sz1 = 1;
    for (int i = dim.size() - 1; i >= 0 ; i--)
    {
      if (temp % dim[i] == 0)
      {
        sz1 *= dim[i];
        temp = temp / dim[i];
      }
      else
        break;
    }
    if (sz > sz1)
      sz = sz1;
    if (arrayinitvalue == NULL)
    {
      curr += sz;
      return;
    }
    if (arrayinitvalue)
    {
      arrayinitvalue->init(dim, a, depth + 1);
      curr = (curr + sz - 1) / sz * sz;
    }
  }
  void init(std::vector<int> dim, std::string Ident, int depth) override
  {
    if (exp)
    {
      std::string src = exp->dump();
      std::string dst = BaseAST::Fetch(dim, Ident, curr, dim.size() - 1);
      curr++;
      std::cout << "  store " << src << ", " << dst << std::endl;
      return;
    }
    int sz = 1;
    for (int i = depth; i < dim.size(); ++i)
    {
      sz = sz * dim[i];
    }
    int temp = curr;
    int sz1 = 1;
    for (int i = dim.size() - 1; i >= 0 ; i--)
    {
      if (temp % dim[i] == 0)
      {
        sz1 *= dim[i];
        temp = temp / dim[i];
      }
      else
        break;
    }
    if (sz > sz1)
      sz = sz1;
    if (arrayinitvalue == NULL)
    {
      for (int i = 0; i < sz; ++i)
      {
        std::string dst = BaseAST::Fetch(dim, Ident, curr, dim.size() - 1);
        curr++;
        std::cout << "  store " << "0" << ", " << dst << std::endl;
      }
      return;
    }
    if (arrayinitvalue)
    {
      arrayinitvalue->init(dim, Ident, depth + 1);
      int repeat = (curr + sz - 1) / sz * sz - curr;
      for (int i = 0; i < repeat; ++i)
      {
        std::string dst = BaseAST::Fetch(dim, Ident, curr, dim.size() - 1);
        curr++;
        std::cout << "  store " << "0" << ", " << dst << std::endl;
      }
    }
  }
};

class VarDefAST: public BaseAST
{
public:
  std::string Ident;
  std::unique_ptr<BaseAST> ArrayDef;
  std::unique_ptr<BaseAST> initvalue;
  std::unique_ptr<BaseAST> vardef;
  std::string dump() const override
  {
    if (debug) std::cout << "VarDefAST:" << std::endl;
    if (currentsymbt == NULL)  //global var
    {
      if (ArrayDef == NULL)    // var
      {
        if (initvalue == NULL)
        {
          symbt.globalsymbol[Ident] = {0, 0, Ident + "_00"};
          std::cout << "global @" << Ident + "_00" << " = alloc i32, zeroinit" << std::endl;
        }
        else
        {
          int init = initvalue->cal();
          symbt.globalsymbol[Ident] = {0, init, Ident + "_00"};
          std::cout << "global @" << Ident + "_00" << " = alloc i32, " << init << std::endl;
        }
      }
      else   // array
      {
        if (initvalue == NULL || ((InitValAST *)initvalue.get())->zeroinit == 1)
        {
          std::string arrdim = ArrayDef->ArrayCal();
          char *spl = std::strtok((char *)arrdim.c_str(), ",");
          std::vector<int> dim;
          while(spl)
          {
            dim.push_back(atoi(spl));
            spl = strtok(NULL, ",");
          }
          symbt.globalsymbol[Ident] = {2, (int)dim.size(), Ident + "_00"};
          std::cout << "global @" << Ident + "_00" << " = alloc " << globalstr << ", zeroinit" << std::endl;
        }
        else
        {
          std::string arrdim = ArrayDef->ArrayCal();
          char *spl = std::strtok((char *)arrdim.c_str(), ",");
          std::vector<int> dim;
          while(spl)
          {
            dim.push_back(atoi(spl));
            spl = strtok(NULL, ",");
          }
          symbt.globalsymbol[Ident] = {2, (int)dim.size(), Ident + "_00"};
          std::cout << "global @" << Ident + "_00" << " = alloc " << globalstr << ", ";


          int k = 1;
          for (int i = 0; i < dim.size(); ++i)
          {
            k = k * dim[i];
          }
          int *a = new int[k];
          memset(a, 0, sizeof(int) * k);
          curr = 0;
          initvalue->init(dim, a, 0);
          curr = 0;
          initvalue->dumparr(dim, a, 0);
          std::cout << std::endl;
        }
      }
    }
    else     // local
    {
      if (ArrayDef == NULL)   // var
      {
        if (initvalue == NULL)
        {
          if (currentsymbt->nameset.count(Ident + std::string("_") + std::to_string(currentsymbt->dep)) == 0)
          {
            currentsymbt->nameset.insert(Ident + std::string("_") + std::to_string(currentsymbt->dep));
            std::cout << "  @" << Ident << "_" << currentsymbt->dep << " = alloc i32" << std::endl;
          }
          currentsymbt->smap[currentsymbt->dep][Ident] = {0, 0, Ident + std::string("_") + std::to_string(currentsymbt->dep)};
          std::cout << "  store 0, @" << Ident << "_" << currentsymbt->dep << std::endl;
        }
        else
        {
          if (currentsymbt->nameset.count(Ident + std::string("_") + std::to_string(currentsymbt->dep)) == 0)
          {
            currentsymbt->nameset.insert(Ident + std::string("_") + std::to_string(currentsymbt->dep));
            std::cout << "  @" << Ident << "_" << currentsymbt->dep << " = alloc i32" << std::endl;
          }
          currentsymbt->smap[currentsymbt->dep][Ident] = {0, 0, Ident + std::string("_") + std::to_string(currentsymbt->dep)};
          initvalue->dump();
          std::cout << "  store %" << now << ", @" << Ident << "_" << currentsymbt->dep << std::endl;
        }
      }
      else     //array
      {
        if (initvalue == NULL)
        {
          if (currentsymbt->nameset.count(Ident + std::string("_") + std::to_string(currentsymbt->dep)) == 0)
          {
            currentsymbt->nameset.insert(Ident + std::string("_") + std::to_string(currentsymbt->dep));
            std::string arrdim = ArrayDef->ArrayCal();
            std::cout << "  @" << Ident << "_" << currentsymbt->dep << " = alloc " << globalstr << std::endl;
          }
          std::string arrdim = ArrayDef->ArrayCal();
          char *spl = std::strtok((char *)arrdim.c_str(), ",");
          std::vector<int> dim;
          while(spl)
          {
            dim.push_back(atoi(spl));
            spl = strtok(NULL, ",");
          }
          currentsymbt->smap[currentsymbt->dep][Ident] = {2, (int)dim.size(), Ident + std::string("_") + std::to_string(currentsymbt->dep)};
        }
        else
        {
          if (currentsymbt->nameset.count(Ident + std::string("_") + std::to_string(currentsymbt->dep)) == 0)
          {
            currentsymbt->nameset.insert(Ident + std::string("_") + std::to_string(currentsymbt->dep));
            std::string arrdim = ArrayDef->ArrayCal();
            std::cout << "  @" << Ident << "_" << currentsymbt->dep << " = alloc " << globalstr << std::endl;

            char *spl = std::strtok((char *)arrdim.c_str(), ",");
            std::vector<int> dim;
            while(spl)
            {
              dim.push_back(atoi(spl));
              spl = strtok(NULL, ",");
            }
            curr = 0;
            initvalue->init(dim, std::string("@") + Ident + "_" + std::to_string(currentsymbt->dep), 0);
            currentsymbt->smap[currentsymbt->dep][Ident] = {2, (int)dim.size(), Ident + std::string("_") + std::to_string(currentsymbt->dep)};
          }
          else
          {
            std::string arrdim = ArrayDef->ArrayCal();
            char *spl = std::strtok((char *)arrdim.c_str(), ",");
            std::vector<int> dim;
            while(spl)
            {
              dim.push_back(atoi(spl));
              spl = strtok(NULL, ",");
            }
            currentsymbt->smap[currentsymbt->dep][Ident] = {2, (int)dim.size(), Ident + std::string("_") + std::to_string(currentsymbt->dep)};
          }
        }
      }
    }
    if (vardef != NULL)
      vardef->dump();
    std::string str;
    return str;
  }
};

class VarDeclAST: public BaseAST
{
public:
  std::unique_ptr<BaseAST> btype;
  std::unique_ptr<BaseAST> vardef;
  std::string dump() const override
  {
    return vardef->dump();
  }
};

// Statement
class StmtAST : public BaseAST
{
public:
  std::unique_ptr<BaseAST> ms;
  std::unique_ptr<BaseAST> ums;
  std::string dump() const override
  {
    if (ms)
      return ms->dump();
    else
      return ums->dump();
  }
};

class MSAST: public BaseAST
{
public:
  int type;
  std::unique_ptr<BaseAST> exp;
  std::unique_ptr<BaseAST> ms;
  std::unique_ptr<BaseAST> ms2;

  std::string dump() const override
  {
    if (debug) std::cout << "MSAST" << std::endl;
    if (type == 0) {}
    if (type == 1)     // Exp ';'
    {
      return exp->dump();
    }
    else if (type == 2)      
    {
      std::cout << "  ret" << std::endl;
      currentsymbt->end = 1;
    }
    else if (type == 3)      
    {
      std::string str = exp->dump();
      std::cout << "  ret " << str << std::endl;
      currentsymbt->end = 1;
      return str;
    }
    else if (type == 4)      // LVal '=' Exp ';'
    {
      std::string s0 = exp->dump();
      ms->Assign(s0);
      std::string str;
      return str;
    }
    else if (type == 5)     // Block
    {
      return ms->dump();
    }
    else if (type == 6)      // IF '(' Exp ')' MS ELSE MS
    {
      int ifcount = IFcount++;
      std::string s0 = exp->dump();
      std::cout << "  br " << s0 << ", %" << "then_" << ifcount << ", %" << "else_" << ifcount << std::endl;

      std::cout << "%" << "then_" << ifcount << ":" << std::endl;
      currentsymbt->end = 0;
      ms->dump();

      if (currentsymbt->end == 0)
      {
        currentsymbt->end = 1;
        std::cout << "  jump " << "%" << "end_" << ifcount << std::endl;
      }

      std::cout << "%" << "else_" << ifcount << ":" << std::endl;
      currentsymbt->end = 0;
      ms2->dump();

      if (currentsymbt->end == 0)
      {
        currentsymbt->end = 1;
        std::cout << "  jump " << "%" << "end_" << ifcount << std::endl;
      }

      std::cout << "%" << "end_" << ifcount << ":" << std::endl;
      currentsymbt->end = 0;
    }

    else if (type == 7)      // WHILE '(' Exp ')' MS
    {
      int W = whilecount++;
      currentsymbt->loopstack.push(W);
      std::cout << "  jump %while_entry_" << W << std::endl;
      std::cout << "%while_entry_" << W << ":" << std::endl;
      currentsymbt->end = 0;
      std::string s0 = exp->dump();

      std::cout << "  br " << s0 << ", %while_body_" << W << ", %while_end_" << W << std::endl;

      std::cout << "%while_body_" << W << ":" << std::endl;
      currentsymbt->end = 0;

      ms->dump();

      if (currentsymbt->end == 0)
        std::cout << "  jump %while_entry_" << W << std::endl;

      std::cout << "%while_end_" << W << ":" << std::endl;
      currentsymbt->end = 0;
      currentsymbt->loopstack.pop();
    }

    else if (type == 8)    // BREAK
    {
      if (currentsymbt->end == 0)
      {
        currentsymbt->end = 1;
        std::cout << "  jump %while_end_" << currentsymbt->loopstack.top() << std::endl;
      }
    }

    else if (type == 9)    // CONTINUE
    {
      if (currentsymbt->end == 0)
      {
        currentsymbt->end = 1;
        std::cout << "  jump %while_entry_" << currentsymbt->loopstack.top() << std::endl;
      }
    }
    std::string str;
    return str;
  }
};

class UMSAST: public BaseAST
{
public:
  std::unique_ptr<BaseAST> exp;
  std::unique_ptr<BaseAST> ms;
  std::unique_ptr<BaseAST> ums;

  std::string dump() const override
  {
    if (ms == NULL)        // WHILE '(' Exp ')' UMS
    {
      int W = whilecount++;
      currentsymbt->loopstack.push(W);

      std::cout << "  jump %while_entry_" << W << std::endl;
      std::cout << "%while_entry_" << W << ":" << std::endl;
      currentsymbt->end = 0;
      std::string s0 = exp->dump();

      std::cout << "  br " << s0 << ", %while_body_" << W << ", %while_end_" << W << std::endl;
      std::cout << "%while_body_" << W << ":" << std::endl;
      currentsymbt->end = 0;
      ums->dump();

      if (currentsymbt->end == 0)
        std::cout << "  jump %while_entry_" << W << std::endl;

      std::cout << "%while_end_" << W << ":" << std::endl;
      currentsymbt->end = 0;
      currentsymbt->loopstack.pop();
    }

    else if (ums == NULL)      // IF '(' Exp ')' Stmt
    {
      int ifcount = IFcount++;
      std::string s0 = exp->dump();
      std::cout << "  br " << s0 << ", %" << "then_" << ifcount << ", %" << "end_" << ifcount << std::endl;

      std::cout << "%" << "then_" << ifcount << ":" << std::endl;
      currentsymbt->end = 0;
      ms->dump();

      if (currentsymbt->end == 0)
      {
        currentsymbt->end = 1;
        std::cout << "  jump " << "%" << "end_" << ifcount << std::endl;
      }

      std::cout << "%" << "end_" << ifcount << ":" << std::endl;
      currentsymbt->end = 0;
    }
    else        // IF '(' Exp ')' MS ELSE UMS
    {
      int ifcount = IFcount++;
      std::string s0 = exp->dump();
      std::cout << "  br " << s0 << ", %" << "then_" << ifcount << ", %" << "else_" << ifcount << std::endl;

      std::cout << "%" << "then_" << ifcount << ":" << std::endl;
      currentsymbt->end = 0;
      ms->dump();

      if (currentsymbt->end == 0)
      {
        currentsymbt->end = 1;
        std::cout << "  jump " << "%" << "end_" << ifcount << std::endl;
      }

      std::cout << "%" << "else_" << ifcount << ":" << std::endl;
      currentsymbt->end = 0;
      ums->dump();

      if (currentsymbt->end == 0)
      {
        currentsymbt->end = 1;
        std::cout << "  jump " << "%" << "end_" << ifcount << std::endl;
      }

      std::cout << "%" << "end_" << ifcount << ":" << std::endl;
      currentsymbt->end = 0;
    }
    std::string str;
    return str;
  }
};

// 表达式
class NumberAST: public BaseAST
{
public:
  int number;
  std::string dump() const override
  {
    if (debug) std::cout << "NumberExpAST" << std::endl;
    std::cout << "  %" << ++now;
    std::cout << " = add 0, " << number << std::endl;
    std::string s0 = "%" + std::to_string(now);
    return s0;
  }
  int cal() override
  {
    return number;
  }
};

class ArrayExpAST:public BaseAST{
public:
    std::unique_ptr<BaseAST> exp;
    std::unique_ptr<BaseAST> arrayexp;
    std::string dump() const override{
      std::string str;
      if (arrayexp == NULL)
      {
        str = exp->dump();
      }
      else
      {
        str = exp->dump() + "," + arrayexp->dump();
      }
      return str;
    }
};

class LValAST: public BaseAST
{
public:
  std::string Ident;
  std::unique_ptr<BaseAST> arrayexp;
  std::string dump() const override
  {
    std::map<std::string, Symbol>::iterator it;
    if (currentsymbt == NULL)
    {
      it = symbt.globalsymbol.find(Ident);
    }
    else
    {
      for (int d = currentsymbt->dep; d >= 0; d--)
      {
        if ((it = currentsymbt->smap[d].find(Ident)) != currentsymbt->smap[d].end())
          break;
      }
      if (it == currentsymbt->smap[0].end())
        it = symbt.globalsymbol.find(Ident);
    }
    if (arrayexp == NULL)
    {
      if(it->second.type == 0)    // const
        std::cout << "  %" << now + 1 << " = load @" << it->second.str << std::endl;
      else if (it->second.type == 1)    // var
        std::cout << "  %" << now + 1 << " = add 0, " << it->second.value << std::endl;
      else if (it->second.type == 2)      // var array
        std::cout << "  %" << now + 1 << " = getelemptr @" << it->second.str << ", 0" << std::endl;
      else if (it->second.type == 3)      // const array
        std::cout << "  %" << now + 1 << " = getelemptr @" << it->second.str << ", 0" << std::endl;
      else if (it->second.type == 4)
      {
        std::cout << "  %" << now + 1 << " = load @" << it->second.str << std::endl;
        now++;
        std::cout << "  %" << now + 1 << " = getelemptr %" << now << ", 0" << std::endl;
      }
      now++;

    }
    else
    {

      if (it->second.type == 4)
      {
        std::string str = arrayexp->dump();
        char *spl = std::strtok((char *)str.c_str(), ",");
        int count = 0;
        std::cout << "  %" << now + 1 << " = load @" << it->second.str << std::endl;
        now++;
        std::cout << "  %" << now + 1 << " = getptr %" << now << ", " << spl << std::endl;
        now++;
        spl = strtok(NULL, ",");
        while(spl)
        {
          count++;
          std::cout << "  %" << now + 1 << " = getelemptr %" << now << ", " << spl << std::endl;
          now++;
          spl = strtok(NULL, ",");
        }
        if (it->second.value == count + 1)
        {
          std::cout << "  %" << now + 1 << " = load %" << now << std::endl;
          now++;
        }
      }
      else
      {
        std::string str = arrayexp->dump();
        char *spl = std::strtok((char *)str.c_str(), ",");
        int count = 0;
        std::cout << "  %" << now + 1 << " = getelemptr @" << it->second.str << ", " << spl << std::endl;
        now++;
        spl = strtok(NULL, ",");
        while(spl)
        {
          count++;
          std::cout << "  %" << now + 1 << " = getelemptr %" << now << ", " << spl << std::endl;
          now++;
          spl = strtok(NULL, ",");
        }
        if (it->second.value == count + 1)
        {
          std::cout << "  %" << now + 1 << " = load %" << now << std::endl;
          now++;
        }
      }

    }
    std::string s0 = "%" + std::to_string(now);
    return s0;
  }

  std::string ddump() override
  {
    std::map<std::string, Symbol>::iterator it;
    if (currentsymbt == NULL)
    {
      it = symbt.globalsymbol.find(Ident);
    }
    else
    {
      for (int d = currentsymbt->dep; d >= 0; d--)
      {
        if ((it = currentsymbt->smap[d].find(Ident)) != currentsymbt->smap[d].end())
          break;
      }
      if (it == currentsymbt->smap[0].end())
        it = symbt.globalsymbol.find(Ident);
    }
    if (arrayexp == NULL)
    {
      if(it->second.type == 0)    // const
        std::cout << "  %" << now + 1 << " = load @" << it->second.str << std::endl;
      else if (it->second.type == 1)    // var
        std::cout << "  %" << now + 1 << " = add 0, " << it->second.value << std::endl;
      else if (it->second.type == 2)      // var array
        std::cout << "  %" << now + 1 << " = getelemptr @" << it->second.str << ", 0" << std::endl;
      else if (it->second.type == 3)      // const array
        std::cout << "  %" << now + 1 << " = getelemptr @" << it->second.str << ", 0" << std::endl;
      else if (it->second.type == 4)
      {

        std::cout << "  %" << now + 1 << " = load @" << it->second.str << std::endl;
        now++;
        std::cout << "  %" << now + 1 << " = getptr %" << now << ", 0" << std::endl;
      }
      now++;

    }
    else
    {

      if (it->second.type == 4)
      {
        std::string str = arrayexp->dump();
        char *spl = std::strtok((char *)str.c_str(), ",");
        std::cout << "  %" << now + 1 << " = load @" << it->second.str << std::endl;
        now++;
        std::cout << "  %" << now + 1 << " = getptr %" << now << ", " << spl << std::endl;
        now++;
        spl = strtok(NULL, ",");
        int count = 0;
        while(spl)
        {
          count++;
          std::cout << "  %" << now + 1 << " = getelemptr %" << now << ", " << spl << std::endl;
          now++;
          spl = strtok(NULL, ",");
        }
        if (it->second.value == count + 1)
        {
          std::cout << "  %" << now + 1 << " = load %" << now << std::endl;
          now++;
        }
        else
        {
          std::cout << "  %" << now + 1 << " = getelemptr %" << now << ", 0" << std::endl;
          now++;
        }
      }
      else
      {
        std::string str = arrayexp->dump();
        char *spl = std::strtok((char *)str.c_str(), ",");
        std::cout << "  %" << now + 1 << " = getelemptr @" << it->second.str << ", " << spl << std::endl;
        now++;
        spl = strtok(NULL, ",");
        int count = 0;
        while(spl)
        {
          count++;
          std::cout << "  %" << now + 1 << " = getelemptr %" << now << ", " << spl << std::endl;
          now++;
          spl = strtok(NULL, ",");
        }
        if (it->second.value == count + 1)
        {
          std::cout << "  %" << now + 1 << " = load %" << now << std::endl;
          now++;
        }
        else
        {
          std::cout << "  %" << now + 1 << " = getelemptr %" << now << ", 0" << std::endl;
          now++;
        }
      }

    }
    std::string s0 = "%" + std::to_string(now);
    return s0;
  }

  int cal() override
  {
    std::map<std::string, Symbol>::iterator it;
    if (currentsymbt == NULL)
    {
      it = symbt.globalsymbol.find(Ident);
      return it->second.value;
    }
    for (int d = currentsymbt->dep; d >= 0; d--)
    {
      if ((it = currentsymbt->smap[d].find(Ident)) != currentsymbt->smap[d].end())
        break;
    }
    if (it == currentsymbt->smap[0].end())
    {
      it = symbt.globalsymbol.find(Ident);
    }
    return it->second.value;
  }

  int Assign(std::string s) override
  {
    std::map<std::string, Symbol>::iterator it;
    for (int d = currentsymbt->dep; d >= 0; d--)
    {
      if ((it = currentsymbt->smap[d].find(Ident)) != currentsymbt->smap[d].end())
        break;
    }
    if (it == currentsymbt->smap[0].end())
    {
      it = symbt.globalsymbol.find(Ident);
    }
    if (arrayexp == NULL)
      std::cout << "  store " << s << ", @" << it->second.str << std::endl;
    else
    {
      std::string str = arrayexp->dump();
      char *spl = std::strtok((char *)str.c_str(), ",");
      std::vector<int> dim;

      if (it->second.type == 4)
      {
        std::cout << "  %" << now + 1 << " = load @" << it->second.str << std::endl;
        now++;
        std::cout << "  %" << now + 1 << " = getptr %" << now << ", " << spl << std::endl;
        now++;
      }
      else
      {
        std::cout << "  %" << now + 1 << " = getelemptr @" << it->second.str << ", " << spl << std::endl;
        now++;
      }
      spl = strtok(NULL, ",");
      while(spl)
      {
        std::cout << "  %" << now + 1 << " = getelemptr %" << now << ", " << spl << std::endl;
        now++;
        spl = strtok(NULL, ",");
      }
      std::cout << "  store " << s << ", %" << now << std::endl;
      now++;
    }
    return 0;
  }
};

class PrimaryExpAST: public BaseAST
{
public:
  std::unique_ptr<BaseAST> num;
  std::unique_ptr<BaseAST> exp;
  std::unique_ptr<BaseAST> lval;
  std::string dump() const override
  {
    if (debug) std::cout << "PrimaryExpAST" << std::endl;
    if (num)
      return num->dump();
    else if (exp)
      return exp->dump();
    else
      return lval->dump();
  }
  std::string ddump() override
  {
    if (debug)
      std::cout << "PrimaryExpAST" << std::endl;
    if (num)
      return num->dump();
    else if (exp)
      return exp->dump();
    else
      return lval->ddump();

  }
  int cal() override
  {
    if (num)
      return num->cal();
    else if(exp)
      return exp->cal();
    else
      return lval->cal();
  }
};

class FuncRParamsAST:public BaseAST{
public:
  std::unique_ptr<BaseAST> exp;
  std::unique_ptr<BaseAST> paras;
  std::string dump() const override{
    std::string str;

    str = exp->ddump();
    if (paras){
      str += ", ";
      str += paras->dump();
    }
    return str;
  }
  std::string ddump() override{
    std::string str;

    str = exp->ddump();
    if (paras){
      str += ", ";
      str += paras->dump();
    }
    return str;
  }
};

class UnaryExpAST: public BaseAST
{
public:
  int type;
  std::string op_Ident;
  std::unique_ptr<BaseAST> unaryexp_paras;
  std::string dump() const override
  {
    if (debug) std::cout << "UnaryExpAST" << std::endl;
    if (type == 0)
    {
      return unaryexp_paras->dump();
    }
    else if (type == 1)
    {
      unaryexp_paras->dump();
      if (op_Ident == "!")
      {
        std::cout << "  %" << now + 1;
        std::cout << " = eq %" << now++ << ", 0" << std::endl;
      }
      else if (op_Ident == "-")
      {
        std::cout << "  %" << now + 1;
        std::cout << " = sub 0, %" << now++ << std::endl;
      }
    }
    else if (type == 2)
    {
      if (symbt.globalsymbol[op_Ident].type == 3)
      {
        std::cout << "  %" << now + 1 << " = call @" << op_Ident << "()" << std::endl;
        now++;
      }
      else
      {
        std::cout << "  call @" << op_Ident << "()" << std::endl;
      }
    }
    else if (type == 3)
    {
      std::string s0 = unaryexp_paras->dump();
      if (symbt.globalsymbol[op_Ident].type == 3)
      {
        std::cout << "  %" << now + 1 << " = call @" << op_Ident << "(" << s0 << ")" << std::endl;
        now++;
      }
      else
      {
        std::cout << "  call @" << op_Ident << "(" << s0 << ")" << std::endl;
      }
    }
    std::string s1 = "%" + std::to_string(now);
    return s1;
  }

  std::string ddump() override
  {
    if (debug)
      std::cout << "UnaryExpAST" << std::endl;
    if (type == 0)
    {
      return unaryexp_paras->ddump();
    }
    else if (type == 1)
    {
      unaryexp_paras->ddump();
      if (op_Ident[0] == '!')
      {
        std::cout << "  %" << now + 1;
        std::cout << " = eq %" << now++ << ", 0" << std::endl;
      }
      else if (op_Ident[0] == '-')
      {
        std::cout << "  %" << now + 1;
        std::cout << " = sub 0, %" << now++ << std::endl;
      }
    }
    else if (type == 2)
    {
      if (symbt.globalsymbol[op_Ident].type == 3)
      {
        std::cout << "  %" << now + 1 << " = call @" << op_Ident << "()" << std::endl;
        now++;
      }
      else
      {
        std::cout << "  call @" << op_Ident << "()" << std::endl;
      }
    }
    else if (type == 3)
    {
      std::string s0 = unaryexp_paras->ddump();
      if (symbt.globalsymbol[op_Ident].type == 3)
      {
        std::cout << "  %" << now + 1 << " = call @" << op_Ident << "(" << s0 << ")" << std::endl;
        now++;
      }
      else
      {
        std::cout << "  call @" << op_Ident << "(" << s0 << ")" << std::endl;
      }
    }
    std::string s1 = "%" + std::to_string(now);
    return s1;
  }

  int cal() override
  {
    if (type == 0 || type == 1)
    {
      if (op_Ident == "!")
        return !(unaryexp_paras->cal());
      else if (op_Ident == "-")
        return -(unaryexp_paras->cal());
      else
        return unaryexp_paras->cal();
    }
    else
    {
      return -1;
    }
  }
};

class MulAST: public BaseAST
{
public:
  std::unique_ptr<BaseAST> mulexp;
  std::string op;
  std::unique_ptr<BaseAST> unaryexp;
  std::string dump() const override
  {
    if (debug) std::cout << "MulAST" << std::endl;
    if (op == "")
    {
      return unaryexp->dump();
    }
    if (op == "*")
    {
      std::string l = mulexp->dump();
      std::string r = unaryexp->dump();
      std::cout << "  %" << now + 1 << " = mul " << l << ", " << r << std::endl;
      now++;
    }
    else if (op == "/")
    {
      std::string l = mulexp->dump();
      std::string r = unaryexp->dump();
      std::cout << "  %" << now + 1 << " = div " << l << ", " << r << std::endl;
      now++;
    }
    else if (op == "%")
    {
      std::string l = mulexp->dump();
      std::string r = unaryexp->dump();
      std::cout << "  %" << now + 1 << " = mod " << l << ", " << r << std::endl;
      now++;
    }
    std::string s0 = "%" + std::to_string(now);
    return s0;
  }

  std::string ddump() override
  {
    if (debug) std::cout << "MulAST" << std::endl;
    if (op == "")
    {
      return unaryexp->ddump();
    }
    if (op == "*")
    {
      std::string l = mulexp->ddump();
      std::string r = unaryexp->ddump();
      std::cout << "  %" << now + 1 << " = mul " << l << ", " << r << std::endl;
      now++;
    }
    else if (op == "/")
    {
      std::string l = mulexp->ddump();
      std::string r = unaryexp->ddump();
      std::cout << "  %" << now + 1 << " = div " << l << ", " << r << std::endl;
      now++;
    }
    else if (op == "%")
    {
      std::string l = mulexp->ddump();
      std::string r = unaryexp->ddump();
      std::cout << "  %" << now + 1 << " = mod " << l << ", " << r << std::endl;
      now++;
    }
    std::string s0 = "%" + std::to_string(now);
    return s0;
  }

  int cal() override
  {
    if (op == "*")
      return (mulexp->cal() * unaryexp->cal());
    else if (op == "/")
      return (mulexp->cal() / unaryexp->cal());
    else if (op == "%")
      return (mulexp->cal() % unaryexp->cal());
    return unaryexp->cal();
  }
};

class AddAST: public BaseAST
{
public:
  std::unique_ptr<BaseAST> addexp;
  std::string op;
  std::unique_ptr<BaseAST> mulexp;
  std::string dump() const override
  {
    if (debug) std::cout << "AddAST" << std::endl;
    if (op == "")
      return mulexp->dump();
    else if (op == "+")
    {
      std::string l = addexp->dump();
      std::string r = mulexp->dump();
      std::cout << "  %" << now + 1 << " = add " << l << ", " << r << std::endl;
      now++;
    }
    else if (op == "-")
    {
      std::string l = addexp->dump();
      std::string r = mulexp->dump();
      std::cout << "  %" << now + 1 << " = sub " << l << ", " << r << std::endl;
      now++;
    }
    std::string s0 = "%" + std::to_string(now);
    return s0;
  }

  std::string ddump() override
  {
    if (debug) std::cout << "AddAST" << std::endl;
    if (op == "")
      return mulexp->ddump();
    else if (op == "+")
    {
      std::string l = addexp->ddump();
      std::string r = mulexp->ddump();
      std::cout << "  %" << now + 1 << " = add " << l << ", " << r << std::endl;
      now++;
    }
    else if (op == "-")
    {
      std::string l = addexp->ddump();
      std::string r = mulexp->ddump();
      std::cout << "  %" << now + 1 << " = sub " << l << ", " << r << std::endl;
      now++;
    }
    std::string s0 = "%" + std::to_string(now);
    return s0;
  }

  int cal() override
  {
    if(op != "")
    {
      if (op == "+")
        return (addexp->cal() + mulexp->cal());
      else if (op == "-")
        return (addexp->cal() - mulexp->cal());
    }
    return mulexp->cal();
  }
};

class RelExpAST: public BaseAST
{
public:
  std::unique_ptr<BaseAST> relexp;
  std::unique_ptr<BaseAST> addexp;
  std::string op;
  std::string dump() const override
  {
    if (debug)
      std::cout << "RelExpAST" << std::endl;
    if(relexp != NULL)
    {
      std::string l = relexp->dump();
      std::string r = addexp->dump();
      if (op == "<=")
        std::cout << "  %" << now + 1 << " = le " << l << ", " << r << std::endl;
      else if (op == ">=")
        std::cout << "  %" << now + 1 << " = ge " << l << ", " << r << std::endl;
      else if (op == "<")
        std::cout << "  %" << now + 1 << " = lt " << l << ", " << r << std::endl;
      else  
        std::cout << "  %" << now + 1 << " = gt " << l << ", " << r << std::endl;
      now++;
      std::string s0 = "%" + std::to_string(now);
      return s0;
    }
    else
    {
      return addexp->dump();
    }
  }

  std::string ddump() override
  {
    if (debug)
      std::cout << "RelExpAST" << std::endl;
    if(relexp != NULL)
    {
      std::string l = relexp->ddump();
      std::string r = addexp->ddump();
      if (op == "<=")
        std::cout << "  %" << now + 1 << " = le " << l << ", " << r << std::endl;
      else if (op == ">=")
        std::cout << "  %" << now + 1 << " = ge " << l << ", " << r << std::endl;
      else if (op == "<")
        std::cout << "  %" << now + 1 << " = lt " << l << ", " << r << std::endl;
      else  // (op == ">")
        std::cout << "  %" << now + 1 << " = gt " << l << ", " << r << std::endl;
      now++;
      std::string s0 = "%" + std::to_string(now);
      return s0;
    }
    else
    {
      return addexp->ddump();
    }
  }

  int cal() override
  {
    if(relexp != NULL)
    {
      if (op == "<=")
        return (relexp->cal() <= addexp->cal());
      else if (op == ">=")
        return (relexp->cal() >= addexp->cal());
      else if (op == "<")
        return (relexp->cal() < addexp->cal());
      else if (op == ">")
        return (relexp->cal() > addexp->cal());
    }
    return addexp->cal();
  }
};

class EqExpAST: public BaseAST
{
public:
  std::unique_ptr<BaseAST> eqexp;
  std::unique_ptr<BaseAST> relexp;
  std::string op;
  std::string dump() const override
  {
    if (debug)
      std::cout << "EqExpAST" << std::endl;
    if(eqexp != NULL)
    {
      std::string l = eqexp->dump();
      std::string r = relexp->dump();
      if (op == "==")
        std::cout << "  %" << now + 1 << " = eq " << l << ", " << r << std::endl;
      else
        std::cout << "  %" << now + 1 << " = ne " << l << ", " << r << std::endl;
      now++;
      std::string s0 = "%" + std::to_string(now);
      return s0;
    }
    else
    {
      return relexp->dump();
    }
  }

  std::string ddump() override
  {
    if (debug)
      std::cout << "EqExpAST" << std::endl;
    if(eqexp != NULL)
    {
      std::string l = eqexp->ddump();
      std::string r = relexp->ddump();
      if (op == "==")
        std::cout << "  %" << now + 1 << " = eq " << l << ", " << r << std::endl;
      else
        std::cout << "  %" << now + 1 << " = ne " << l << ", " << r << std::endl;
      now++;
      std::string s0 = "%" + std::to_string(now);
      return s0;
    }
    else
    {
      return relexp->ddump();
    }
  }
  int cal() override
  {
    if(eqexp != NULL)
    {
      if (op == "==")
        return (eqexp->cal() == relexp->cal());
      else
        return (eqexp->cal() != relexp->cal());
    }
    else
    {
      return relexp->cal();
    }
  }
};

class LAndExpAST: public BaseAST
{
public:
  std::unique_ptr<BaseAST> eqexp;
  std::unique_ptr<BaseAST> landexp;
  std::string dump() const override
  {
    if (debug) std::cout << "LAndExpAST" << std::endl;
    if(landexp != NULL)
    {
      // LAndExp "&&" EqExp;
      int ifcount = IFcount++;
      std::string l = landexp->dump();
      std::cout << "  @int" << ifcount << " = alloc i32" << std::endl;
      std::cout << "  br " << l << ", %" << "then_" << ifcount << ", %" << "else_" << ifcount << std::endl;

      std::cout << "%" << "then_" << ifcount << ":" << std::endl;
      currentsymbt->end = 0;
      std::string r = eqexp->dump();
      std::cout << "  %" << now + 1 << " = ne " << r << ", 0" << std::endl;
      now++;
      std::cout << "  store %" << now << ", @int" << ifcount << std::endl;

      std::cout << "  jump %end_" << ifcount << std::endl;

      std::cout << "%" << "else_" << ifcount << ":" << std::endl;
      currentsymbt->end = 0;
      std::cout << "  %" << now + 1 << " = ne " << l << ", 0" << std::endl;
      now++;
      std::cout << "  store %" << now << ", @int" << ifcount << std::endl;

      std::cout << "  jump %end_" << ifcount << std::endl;

      std::cout << "%" << "end_" << ifcount << ":" << std::endl;
      currentsymbt->end = 0;
      std::cout << "  %" << ++now << " = load @int" << ifcount << std::endl;
      std::string s0 = "%" + std::to_string(now);
      return s0;
    }
    else
    {
      return eqexp->dump();
    }
  }

  std::string ddump() override
  {
    if (debug)
      std::cout << "LAndExpAST" << std::endl;
    if(landexp != NULL)
    {
      // LAndExp "&&" EqExp;
      int ifcount = IFcount++;
      std::string l = landexp->ddump();
      std::cout << "  @int" << ifcount << " = alloc i32" << std::endl;

      std::cout << "  br " << l << ", %" << "then_" << ifcount << ", %" << "else_" << ifcount << std::endl;

      std::cout << "%" << "then_" << ifcount << ":" << std::endl;
      currentsymbt->end = 0;
      std::string r = eqexp->ddump();
      std::cout << "  %" << now + 1 << " = ne " << r << ", 0" << std::endl;
      now++;
      std::cout << "  store %" << now << ", @int" << ifcount << std::endl;

      std::cout << "  jump %end_" << ifcount << std::endl;

      std::cout << "%" << "else_" << ifcount << ":" << std::endl;
      currentsymbt->end = 0;
      std::cout << "  %" << now + 1 << " = ne " << l << ", 0" << std::endl;
      now++;
      std::cout << "  store %" << now << ", @int" << ifcount << std::endl;

      std::cout << "  jump %end_" << ifcount << std::endl;

      std::cout << "%" << "end_" << ifcount << ":" << std::endl;
      currentsymbt->end = 0;
      std::cout << "  %" << ++now << " = load @int" << ifcount << std::endl;
      std::string s0 = "%" + std::to_string(now);
      return s0;
    }
    else
    {
      return eqexp->ddump();
    }
  }
  int cal() override
  {
    if(landexp != NULL)
      return (landexp->cal() and eqexp->cal());
    else
      return eqexp->cal();
  }
};

class LOrExpAST: public BaseAST
{
public:
  std::unique_ptr<BaseAST> lorexp;
  std::unique_ptr<BaseAST> landexp;
  std::string dump() const override
  {
    if (debug)
      std::cout << "LOrExpAST" << std::endl;
    if(lorexp != NULL)
    {
      int ifcount = IFcount++;
      // LOrExp "||" LAndExp;

      std::string l = lorexp->dump();
      std::cout << "  @int" << ifcount << " = alloc i32" << std::endl;
      std::cout << "  br " << l << ", %" << "then_" << ifcount << ", %" << "else_" << ifcount << std::endl;

      std::cout << "%" << "then_" << ifcount << ":" << std::endl;
      currentsymbt->end = 0;
      std::cout << "  %" << ++now << " = ne " << l << ", 0" << std::endl;
      std::cout << "  store %" << now << ", @int" << ifcount << std::endl;

      std::cout << "  jump %end_" << ifcount << std::endl;

      std::cout << "%" << "else_" << ifcount << ":" << std::endl;
      currentsymbt->end = 0;
      std::string r = landexp->dump();
      std::cout << "  %" << now + 1 << " = ne " << r << ", 0" << std::endl;
      now++;
      std::cout << "  store %" << now << ", @int" << ifcount << std::endl;

      std::cout << "  jump %end_" << ifcount << std::endl;

      std::cout << "%" << "end_" << ifcount << ":" << std::endl;
      currentsymbt->end = 0;
      std::cout << "  %" << ++now << " = load @int" << ifcount << std::endl;
      std::string s1 = "%" + std::to_string(now);
      return s1;
    }
    else
    {
      return landexp->dump();
    }
  }

  std::string ddump() override
  {
    if (debug)
      std::cout << "LOrExpAST" << std::endl;
    if(lorexp != NULL)
    {
      int ifcount = IFcount++;
      // LOrExp "||" LAndExp;
      std::string l = lorexp->ddump();
      std::cout << "  @int" << ifcount << " = alloc i32" << std::endl;
      std::cout << "  br " << l << ", %" << "then_" << ifcount << ", %" << "else_" << ifcount << std::endl;

      std::cout << "%" << "then_" << ifcount << ":" << std::endl;
      currentsymbt->end = 0;
      std::cout << "  %" << ++now << " = ne " << l << ", 0" << std::endl;
      std::cout << "  store %" << now << ", @int" << ifcount << std::endl;

      std::cout << "  jump %end_" << ifcount << std::endl;

      std::cout << "%" << "else_" << ifcount << ":" << std::endl;
      currentsymbt->end = 0;
      std::string r = landexp->ddump();
      std::cout << "  %" << now + 1 << " = ne " << r << ", 0" << std::endl;
      now++;
      std::cout << "  store %" << now << ", @int" << ifcount << std::endl;

      std::cout << "  jump %end_" << ifcount << std::endl;

      std::cout << "%" << "end_" << ifcount << ":" << std::endl;
      currentsymbt->end = 0;
      std::cout << "  %" << ++now << " = load @int" << ifcount << std::endl;
      std::string s1 = "%" + std::to_string(now);
      return s1;
    }
    else
    {
      return landexp->ddump();
    }
  }

  int cal() override
  {
    if(lorexp != NULL)
      return (lorexp->cal() or landexp->cal());
    else
      return landexp->cal();
  }
};

class ExpAST: public BaseAST
{
public:
  std::unique_ptr<BaseAST> lorexp;
  std::string dump() const override
  {
    return lorexp->dump();
  }
  std::string ddump() override
  {
    return lorexp->ddump();
  }
  int cal() override
  {
    return lorexp->cal();
  }
};
