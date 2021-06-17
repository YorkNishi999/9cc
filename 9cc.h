#include<ctype.h>
#include<stdarg.h>
#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//
// tokenize.c
//

// トークン（意味のあるヒトカタマリの単語）の種類
typedef enum {
	TK_RESERVED,	// 記号
	TK_NUM,				// 整数トークン
	TK_EOF,				// 入力の終わりを表すトークン
} TokenKind;


// トークン型 連結リストで表現されている
typedef struct Token Token;
struct Token {
	TokenKind kind;		// トークンの型
	Token *next;			// 次の入力トークン（nextはトークン型のポインタ）
	int val;					// kind が TK_NUMの場合、その数値
	char *str;				// トークン文字列（strはchar型のポインタ）
	int len;				// トークンの長さ
};

// report error 
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

// トークナイザ
bool consume(char *op);
void expect(char *op);
int expect_number();
bool et_eof();
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
bool startswith(char *p, char *q);

Token *tokenize(); 

//
// parse.c
//

// 演算子の種類
typedef enum {
	ND_ADD, // +
	ND_SUB, // -
	ND_MUL, // *
	ND_DIV, // /
	ND_EQ, // ==
	ND_NE, // !=
	ND_LT, // < or >
	ND_LE, // <= or >=
	ND_NUM, // integer
} NodeKind;

// AST node type
typedef struct Node Node;
struct Node {
	NodeKind kind;	// ノードの種類
	Node *lhs;			// 左サイドのノード
	Node *rhs;			// 右サイドのノード
	int val;				// kind == ND_NUM のとき使う
};

Node *parse(Token *token);