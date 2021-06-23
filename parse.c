#include "9cc.h"

//
// パーサ
//



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
	locals = calloc(1, sizeof(LVar));
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

		LVar *lvar = find_lvar(tok);
		if (lvar) {
			node->offset = lvar->offset;
		} else {
			lvar = calloc(1, sizeof(LVar));
			lvar->next = locals;
			lvar->name = tok->str;
			lvar->len = tok->len;
			lvar->offset = locals->offset +8;
			node->offset = lvar->offset;
			locals = lvar;
		}
		return node;
	}
	
	return new_node_num(expect_number());
}