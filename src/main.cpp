#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <cstring>
#include <map>
#include <set>
#include <stack>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include "assert.h"  
#include "AST.h"

using namespace std;

extern FILE *yyin;
extern int yyparse(unique_ptr<BaseAST> &ast);
extern void parse_string(const char* str);

int main(int argc, const char *argv[]) {
  // 解析命令行参数. 测试脚本/评测平台要求你的编译器能接收如下参数:
  // compiler 模式 输入文件 -o 输出文件
  assert(argc == 5);
  assert(argc == 5);
  auto mode = argv[1];
  auto input = argv[2];
  auto output = argv[4];

  // 打开输入文件, 并且指定 lexer 在解析的时候读取这个文件
  yyin = fopen(input, "r");
  assert(yyin);
  // 调用 parser 函数, parser 函数会进一步调用 lexer 解析输入文件的
  // parse input file
  unique_ptr<BaseAST> ast;
  auto ret = yyparse(ast);
  assert(!ret);

  int old = dup(1);

  if (strcmp(mode, "-koopa") == 0)
  {
    // Dump Koopa IR
    freopen(output, "w", stdout);
    ast->dump();
    dup2(old, 1);
  }
  else
  {
    // Dump Koopa IR
    FILE *fp = freopen((string(output) + ".koopa").c_str(), "w", stdout);
    ast->dump();
    fflush(fp);
    dup2(old, 1);
    // Koopa IR -> RISC-V
    FILE* koopaio = fopen((string(output) + ".koopa").c_str(), "r");
    char *str=(char *)malloc(1 << 30);
    memset(str, 0, 1 << 30);
    int len = fread(str, 1, 1 << 30, koopaio);
    str[len] = 0;
    // cout<<str<<endl;
    // parse_string(str);
    freopen(output, "w", stdout);
    parse_string(str);
    dup2(old, 1);
  }

  return 0;
}