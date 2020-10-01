#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <map>
#include <stack>
#include <vector>

typedef enum
{
	fBlack = 30, fRed, fGreen, fYellow, fBlue, fMagenta, fCyan, fWhite,
	bBlack = 40, bRed, bGreen, bYellow, bBlue, bMagenta, bCyan, bWhite,
	reset = 0
} TextParam;
typedef enum
{
	// Non-terminals
	NT_LINE, NT_CMD, NT_COM, NT_PRG, NT_DIR, NT_PRGNAME,
	NT_ARGS, NT_ARG, NT_KEY, NT_FLAG, NT_VAR, NT_VAL,
	NT_VARCALL, NT_LITERAL, NT_VALNULLABLE,
	// Terminals
	T_COM, T_LITERAL, T_FLAG, T_DIR, T_KEY, T_EOS, T_EQ, T_DOT,
	T_SLASH, T_DOLLAR, T_EPS
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
void ll1_parser(const char* data);
Token lexer(const char* data, size_t index);
void init_parsing_table(GrammarTbl& tbl);

int main()
{
	std::string str = "dir1/dir_2/myprg text $varia_b1e___   _key= 13 -help # TODO";
	init_clean(str);
	ll1_parser(str.c_str());


	return 0;
}

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
	str.insert(0, "."); // TODO: if LINE -> PRG ARGS
}

void ll1_parser(const char* data)
{
	GrammarTbl table;
	size_t index = 0;
	std::stack<Symbol> ssmbl;
	ssmbl.push(T_EOS);
	ssmbl.push(NT_LINE);

	init_parsing_table(table);

	while ( !ssmbl.empty() )
	{
		while ( isspace(data[index]) ) ++index;
		Token tok = lexer(data, index);
		if ( tok.terminal == ssmbl.top() || tok.terminal == T_EPS )
		{
			std::cout << "Matched symbols: " << tok.val << std::endl;
			index += tok.terminal != T_EPS ? tok.val.size() : 0;
			ssmbl.pop();
		}
		else
		{
			Prod prod = table[ssmbl.top()][tok.terminal];
			ssmbl.pop();
			while (!prod.empty())
			{
				ssmbl.push(prod.back());
				prod.pop_back();
			}
		}
	}
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
				else if ( isalpha(c) || isdigit(c) || c == '_' ) { val += c; state = 4; }
				else if ( c == '~' || c == '.' ) { val += c; state = 5; }
				else { done = true; break; };
				break;
			case 1: case 2:
				if ( isalpha(c) || isdigit(c) || c == '_' ) { val += c; state = 2; }
				else { done = true; break; };
				break;
			case 3:
				if ( c != '\0' ) { val += c; state = 3; }
				else { done = true; break; };
				break;
			case 4:
				if ( isalpha(c) || isdigit(c) || c == '_' ) { val += c; state = 4; }
				else if ( c == '.' ) { val += c; state = 5; }
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
		case 5: tok.terminal = st_i ? T_DIR : T_DOT;
			break;
		case 6: tok.terminal = T_EQ;			break;
		case 7: tok.terminal = T_SLASH;			break;
		case 8: tok.terminal = T_DOLLAR; 		break;
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
	tbl[NT_ARGS][T_KEY]=tbl[NT_ARGS][T_FLAG]=tbl[NT_ARGS][T_DOLLAR]=tbl[NT_ARGS][T_LITERAL]=	{NT_ARG,NT_ARGS};
	tbl[NT_ARG][T_KEY]=																			{NT_KEY,T_EQ,NT_VAL};
	tbl[NT_ARG][T_FLAG]=																		{NT_FLAG};
	tbl[NT_ARG][T_DOLLAR]=tbl[NT_ARG][T_LITERAL]=												{NT_VAL};
	tbl[NT_KEY][T_KEY]=																			{T_KEY};
	tbl[NT_FLAG][T_FLAG]=																		{T_FLAG};
	tbl[NT_VAR][T_LITERAL]=																		{T_LITERAL};
	tbl[NT_VAL][T_DOLLAR]=																		{NT_VARCALL};
	tbl[NT_VAL][T_LITERAL]=																		{T_LITERAL};
	tbl[NT_VALNULLABLE][T_EOS]=tbl[NT_VALNULLABLE][T_COM]=										{T_EPS};
	tbl[NT_VALNULLABLE][T_DOLLAR]=tbl[NT_VALNULLABLE][T_LITERAL]=								{NT_VAL};
	tbl[NT_VARCALL][T_DOLLAR]=																	{T_DOLLAR,NT_VAR};
	tbl[NT_LITERAL][T_LITERAL]=																	{T_LITERAL};
}