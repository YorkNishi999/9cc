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
	TK_IDENT,			// 識別子
	TK_NUM,				// 整数トークン
	TK_RETURN,		// return token
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

// 複数文字の変数
typedef struct LVar LVar;
struct LVar {
	LVar *next;			// 次の変数かNULL
	char *name; 	// 変数の名前
	int len; 			// 名前の長さ
	int offset; 		// RBPからのオフセット
};

// local variables
LVar *locals;

// インプットを保存する変数
char *user_input;

// 現在着目しているトークン
Token *token;

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
Token *consume_ident();
bool consume_return();
void expect(char *op);
int expect_number();
bool at_eof();
LVar *find_lvar(Token *tok);
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
bool startswith(char *p, char *q);
Token *tokenize(char *p);

//
//parse.c
//

typedef enum {
	ND_ADD, // +
	ND_SUB, // -
	ND_MUL, // *
	ND_DIV, // /
	ND_ASSIGN, // = 変数代入
	ND_LVAR, // ローカル変数
	ND_EQ, // ==
	ND_NE, // !=
	ND_LT, // < or >
	ND_LE, // <= or >=
	ND_NUM, // integer
	ND_RETURN, // return
} NodeKind;

// AST node type(抽象構文木のノード)
typedef struct Node Node;
struct Node {
	NodeKind kind;	// ノードの種類
	Node *lhs;			// 左サイドのノード
	Node *rhs;			// 右サイドのノード
	int val;				// kind == ND_NUM のとき使う
	int offset;				//kind がND_LVAR（ローカル変数）のときに使う
};

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
// Node *new_binary(NodeKind kind, Node *lhs, Node *rhs);
Node *new_num(int val);

Node *code[100]; 

void program();


//
// gen.c
//

void gen(Node *node);