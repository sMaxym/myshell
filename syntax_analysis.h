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
    NT_VARCALL, NT_LITERAL, NT_VALNULLABLE,
    // Terminals
    T_COM, T_LITERAL, T_FLAG, T_DIR, T_KEY, T_EOS, T_EQ, T_DOT,
    T_SLASH, T_DOLLAR, T_EPS,
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

static inline void ltrim(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}
static inline void rtrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}
static inline void trim(std::string& s)
{
    ltrim(s);
    rtrim(s);
}

char* dec_text(const char* cstr, TextParam fg, TextParam bg)
{
    char *dec_txt = (char*) malloc(sizeof(cstr) + 13);
    sprintf(dec_txt, "\033[%d;%dm%s\033[0m", fg, bg, cstr);
    return dec_txt;
}

void init_clean(std::string& str)
{
    trim(str);
    char grps_n = 1, prevc = str[0];
    if (prevc == '#') return;
    for (const char c: str)
    {
        if (c == '=')
        {
            prevc = c;
            break;
        }
        grps_n += isspace(prevc) && !isspace(c) ? 1 : 0;
        prevc = c;
    }
    if (grps_n > 1 || prevc != '=') str.insert(0, "&");
}

Tree<Symbol,std::string>* ll1_parser(const char* data)
{
    Tree<Symbol,std::string>* cursor;
    GrammarTbl table;
    size_t index = 0, iter = 0;

    std::stack<Symbol> ssmbl;
    ssmbl.push(T_EOS);
    ssmbl.push(NT_LINE);

    auto syntax_tree = new Tree<Symbol,std::string>(NT_LINE,std::string());
    cursor = syntax_tree;
    init_parsing_table(table);

    while ( !ssmbl.empty() ) {
        while ( isspace(data[index]) ) ++index;
        Token tok = lexer(data, index);
        if ( tok.terminal == ssmbl.top() || tok.terminal == T_EPS )
        {
            std::cout << "Matched symbols: " << tok.val << std::endl;
            index += tok.terminal != T_EPS ? tok.val.size() : 0;
            if ( tok.terminal != T_EOS )
            {
                Tree<Symbol,std::string>* lookup = cursor->find(tok.terminal);
                while ( lookup == nullptr && !cursor->is_root() )
                {
                    cursor = cursor->parent;
                    lookup = cursor->find(tok.terminal);
                }
                cursor = lookup;
                cursor->terminal = tok.val;
            }
            ssmbl.pop();
        }
        else
        {
            Prod prod = table[ssmbl.top()][tok.terminal];
            if ( iter != 0 )
            {
                Tree<Symbol,std::string>* lookup = cursor->find(ssmbl.top());
                while ( lookup == nullptr && !cursor->is_root() )
                {
                    cursor = cursor->parent;
                    lookup = cursor->find(ssmbl.top());
                }
                cursor = lookup;
            }

            ssmbl.pop();
            while (!prod.empty())
            {
                ssmbl.push(prod.back());
                cursor->push_front(new Tree<Symbol,std::string>(prod.back(),std::string()));
                prod.pop_back();
            }
        }
        ++iter;
    }
    return syntax_tree;
}

Token lexer(const char* data, size_t index)
{
    Token tok;
    std::string val;
    bool done = false;
    size_t state = 0, st_i = index;

    while (!done)
    {
        char c = *(data + index++);
        switch (state)
        {
            case 0:
                if 		( c == '-' ) { val += c; state = 1; }
                else if ( c == '#' ) { val += c; state = 3; }
                else if ( c == '=' ) { val += c; state = 6; done = true; break; }
                else if ( c == '/' ) { val += c; state = 7; done = true; break; }
                else if ( c == '$' ) { val += c; state = 8; done = true; break; }
                else if ( c == '&' ) { val += c; state = 9; done = true; break; }
                else if ( isalpha(c) || isdigit(c) || c == '_' || c == '.'|| c == '~' ) { val += c; state = 4; }
                else if ( c == '~' || c == '.' ) { val += c; state = 5; }
                else { done = true; break; };
                break;
            case 1:
                if ( isalpha(c) || isdigit(c) || c == '_' || c == '-' ) { val += c; state = 2; }
                else { done = true; break; };
                break;
            case 2:
                if ( isalpha(c) || isdigit(c) || c == '_' ) { val += c; state = 2; }
                else { done = true; break; };
                break;
            case 3:
                if ( c != '\0' ) { val += c; state = 3; }
                else { done = true; break; };
                break;
            case 4:
                if ( isalpha(c) || isdigit(c) || c == '_' || c == '.' || c == '~' || c=='-' ) { val += c; state = 4; }
                else { done = true; break; };
                break;
            case 5:
                if ( c == '.' ) val += c;
                done = true; break;
        }
    }
    tok.val = val;
    switch (state)
    {
        case 2: tok.terminal = T_FLAG;			break;
        case 3: tok.terminal = T_COM; 			break;
        case 4:
            if 		( *(data + index - 1) == '/' ) 			tok.terminal = T_DIR;
            else if	( *(data + index - 1) == '=' && st_i ) 	tok.terminal = T_KEY;
            else											tok.terminal = T_LITERAL;
            break;
        case 5:	tok.terminal = T_DIR; 			break;
        case 6: tok.terminal = T_EQ;			break;
        case 7: tok.terminal = T_SLASH;			break;
        case 8: tok.terminal = T_DOLLAR; 		break;
        case 9: tok.terminal = T_DOT;			break;
        default: break;
    }
    return tok;
}

void init_parsing_table(GrammarTbl& tbl)
{
    tbl[NT_LINE][T_DOT]=tbl[NT_LINE][T_LITERAL]=												{NT_CMD,NT_COM};
    tbl[NT_LINE][T_EOS]=tbl[NT_LINE][T_COM]=													{NT_COM};
    tbl[NT_CMD][T_DOT]=																			{T_DOT,NT_PRG,NT_ARGS};
    tbl[NT_CMD][T_LITERAL]=																		{NT_VAR,T_EQ,NT_VALNULLABLE};
    tbl[NT_COM][T_EOS]=																			{T_EPS};
    tbl[NT_COM][T_COM]=																			{T_COM};
    tbl[NT_PRG][T_DIR]=																			{NT_DIR,T_SLASH,NT_PRG};
    tbl[NT_PRG][T_LITERAL]=																		{NT_PRGNAME};
    tbl[NT_PRGNAME][T_LITERAL]=																	{T_LITERAL};
    tbl[NT_DIR][T_DIR]=																			{T_DIR};
    tbl[NT_ARGS][T_EOS]=tbl[NT_ARGS][T_COM]=													{T_EPS};
    tbl[NT_ARGS][T_DIR]=tbl[NT_ARGS][T_KEY]=tbl[NT_ARGS][T_FLAG]=
            tbl[NT_ARGS][T_DOLLAR]=tbl[NT_ARGS][T_LITERAL]=										{NT_ARG,NT_ARGS};
    tbl[NT_ARG][T_KEY]=																			{NT_KEY,T_EQ,NT_VAL};
    tbl[NT_ARG][T_FLAG]=																		{NT_FLAG};
    tbl[NT_ARG][T_DIR]=tbl[NT_ARG][T_DOLLAR]=tbl[NT_ARG][T_LITERAL]=							{NT_VAL};
    tbl[NT_KEY][T_KEY]=																			{T_KEY};
    tbl[NT_FLAG][T_FLAG]=																		{T_FLAG};
    tbl[NT_VAR][T_LITERAL]=																		{T_LITERAL};
    tbl[NT_VAL][T_DOLLAR]=																		{NT_VARCALL};
    tbl[NT_VAL][T_DIR]=tbl[NT_VAL][T_LITERAL]=													{NT_PRG};
    tbl[NT_VALNULLABLE][T_EOS]=tbl[NT_VALNULLABLE][T_COM]=										{T_EPS};
    tbl[NT_VALNULLABLE][T_DIR]=tbl[NT_VALNULLABLE][T_DOLLAR]=tbl[NT_VALNULLABLE][T_LITERAL]=	{NT_VAL};
    tbl[NT_VARCALL][T_DOLLAR]=																	{T_DOLLAR,NT_VAR};
    tbl[NT_LITERAL][T_LITERAL]=																	{T_LITERAL};
}
