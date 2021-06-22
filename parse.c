#include "9cc.h"

//
// パーサ
//

// Reports an error and exit.
void error(char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

// エラー箇所を報告する
void error_at(char *loc, char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);

	int pos = loc - user_input;
	fprintf(stderr, "%s\n", user_input);
	fprintf(stderr, "%*s", pos, " ");
	fprintf(stderr, "^ ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}


// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume(char *op) {
	if (token->kind != TK_RESERVED || 
			strlen(op) != token->len || 
			memcmp(token->str, op, token->len) )
		return false;
	token = token->next;
	return true;
}

// トークンが変数の場合、そのトークンを1つ読み進めて
//  トークンを返す。そうでない場合、NULLを返す
Token *consume_ident() {
	if (token->kind != TK_IDENT)
		return NULL;
	Token *t = token;
	token = token->next;
	return t;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進める。
// それ以外の場合にはエラーを報告する。
void expect(char *op) {
	if (token->kind != TK_RESERVED || strlen(op) != token->len ||
			memcmp(token->str, op, token->len))
		error_at(token->str, "\'%s\'ではありませんyo", op);
	token = token->next;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number() {
	if (token->kind != TK_NUM)
		error_at(token->str, "数ではありません");
	int val = token->val;
	token = token->next;
	return val;
}

bool at_eof() {
	return token->kind == TK_EOF;
}

// スタートをtokenizeする
bool startswith(char *p, char *q) {
	return memcmp(p, q, strlen(q)) == 0;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
	Node *node = calloc(1, sizeof(Node));
	node->kind = kind;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

//Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
//	Node *node = new_node(kind);
//	node->lhs = lhs;
//	node->rhs = rhs;
//	return node;
// }

Node *new_node_num(int val) {
	Node *node = calloc(1, sizeof(Node));
	node->kind = ND_NUM;
	node->val = val;
	return node;
}

static Node *stmt();
static Node *expr();
static Node *assign();
static Node *equality();
static Node *relational();
static Node *add();
static Node *mul();
static Node *unary();
static Node *primary();

Node *code[100];

// program = 一番上位概念
void program() {
	int i = 0;
	while (!at_eof())
		code[i++] = stmt();
	code[i] = NULL;
}

// stmt = expr ";"
static Node *stmt() {
	Node *node = expr();
	expect(";");
	return node;
}

// expr = equality
static Node *expr() {
	return assign();
}

// assign = equality ("=" assign)?
static Node *assign() {
	Node *node = equality();

	if (consume("="))
		node = new_node(ND_ASSIGN, node, assign());
	return node;
}

// equality = relational ("==" relational | "!=" relational)*
static Node *equality() {
	Node *node = relational();

	for (;;) {
		if (consume("=="))
			node = new_node(ND_EQ, node, relational());
		else if (consume("!="))
			node = new_node(ND_NE, node, relational());
		else
			return node;
	}
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
static Node *relational() {
	Node *node = add();

	for (;;) {
		if (consume("<"))
			node = new_node(ND_LT, node, add());
		else if (consume("<="))
			node = new_node(ND_LE, node, add());
		else if (consume(">"))
			node = new_node(ND_LT, add(), node);
		else if (consume(">="))
			node = new_node(ND_LE, add(), node);
		else
			return node;
	}
}

// add = mul ("+" mul | "-" mul)*
static Node *add() {
	Node *node = mul();

	for(;;) {
		if (consume("+"))
			node = new_node(ND_ADD, node, mul());
		else if (consume("-"))
			node = new_node(ND_SUB, node, mul());
		else
			return node;
	}
}

// mul = unary ("*" unary || "/" unary)*
static Node *mul() {
	Node *node = unary();

	for(;;) {
		if (consume("*"))
			node = new_node(ND_MUL, node, unary());
		else if (consume("/"))
			node = new_node(ND_DIV, node, unary());
		else
			return node;
	}
}

// unary = ("+" | "-" )? unary
static Node *unary() {
		if (consume("+"))
			return unary();
		if (consume("-"))
			return new_node(ND_SUB, new_node_num(0), unary());
		return primary();
}


// primary = "(" expr ")" | ident | num
static Node *primary() {
	if (consume("(")) {
		Node *node = expr();
		expect(")");
		return node;
	}

	Token *tok = consume_ident(); 
	if (tok) {
		Node *node = calloc(1, sizeof(Node));
		node->kind = ND_LVAR;
		node->offset = (tok->str[0] - 'a' + 1) * 8; // offset はコンパイラで勝手に定義してよいので、この数値リテラルに意味はない
		return node;
	}
	
	return new_node_num(expect_number());
}