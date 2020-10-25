#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <map>
#include <stack>
#include <vector>
#include "tree.h"

typedef enum
{
    fBlack = 30, fRed, fGreen, fYellow, fBlue, fMagenta, fCyan, fWhite,
    bBlack = 40, bRed, bGreen, bYellow, bBlue, bMagenta, bCyan, bWhite,
    reset = 0, fBold
} TextParam;

typedef enum
{
    // Non-terminals
    NT_LINE, NT_CMD, NT_COM, NT_PRG, NT_DIR, NT_PRGNAME,
    NT_ARGS, NT_ARG, NT_KEY, NT_FLAG, NT_VAR, NT_VAL,
    NT_VARCALL, NT_LITERAL, NT_VALNULLABLE, NT_RDCTS, NT_RDCT, NT_P_TO, NT_BG_PRC,
    // Terminals
    T_COM, T_LITERAL, T_FLAG, T_DIR, T_KEY, T_EOS, T_EQ, T_DOT, T_P_TO,
    T_SLASH, T_DOLLAR, T_EPS, T_RDCT, T_P_FROM, T_BG_PRC,
    // Misha
    PROG, KEY, VALUE, ARGS, VARS, CURRENT_PATH, FILE_ARGS
} Symbol;


typedef struct
{
    std::string val = "";
    Symbol terminal = T_EOS;
} Token;

typedef std::vector<Symbol> Prod;
typedef std::map<Symbol,std::map<Symbol,Prod>> GrammarTbl;

static inline void ltrim(std::string &s);
static inline void rtrim(std::string &s);
static inline void trim(std::string &s);
char* dec_text(const char* cstr, TextParam fg, TextParam bg);
void init_clean(std::string& str);
Tree<Symbol,std::string>* ll1_parser(const char* data);
Token lexer(const char* data, size_t index);
void init_parsing_table(GrammarTbl& tbl);

