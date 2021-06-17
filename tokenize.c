#include"9cc.h"

// インプットを保存する変数
static char *user_input;

// 現在着目しているトークン
static Token *token;

// Reports an error and exit.
void error(char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

static void verror_at(char *loc, char *fmt, va_list ap) {
	int pos = loc - user_input;
	fprintf(stderr, "%s\n", user_input);
	fprintf(stderr, "%*s\n", pos, "");
	fprintf(stderr, "^ ");
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

//void error_tok(Token *tok, char *fmt, ...) {
//	va_list ap;
//	va_start(ap, fmt);
//	verror_at(tok->loc, fmt, ap);
//}

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume(char *op) {
	if (token->kind != TK_RESERVED || strlen(op) != token->len || 
			memcmp(token->str, op, token->len) )
		return false;
	token = token->next;
	return true;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進める。
// それ以外の場合にはエラーを報告する。
void expect(char *op) {
	if (token->kind != TK_RESERVED || strlen(op) != token->len ||
			memcmp(token->str, op, token->len))
		error_at(token->str, "\'%s\'ではありません", op);
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

// 新しいトークンを作成してcurにつなげる
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	tok->len = len;
	cur->next = tok;
	return tok;
}

// スタートをtokenizeする
bool startswith(char *p, char *q) {
	return memcmp(p, q, strlen(q)) == 0;
}


// 入力文字列pをトークナイズしてそれを返す
Token *tokenize() {
	// user_inputをトークナイズする
	char *p = user_input;
	Token head;
	head.next = NULL;
	Token *cur = &head;

	while (*p) {
		// 空白文字をスキップ
		if (isspace(*p)) {
			p++;
			continue;
		}

		// Multi-letter punctuator
		if (startswith(p, "==") || startswith(p, "!=") ||
				startswith(p, "<=") || startswith(p, ">=")) { 
			cur = new_token(TK_RESERVED, cur, p, 2);
			p += 2;
			continue;
		}

		// Single-letter punctuator
		if(strchr("+-*/()<>", *p)) {
			cur = new_token(TK_RESERVED, cur, p++, 1);
			continue;
		}

		// Integer literal
		if (isdigit(*p)) {
			cur = new_token(TK_NUM, cur, p, 0);
			char *q = p;
			cur->val = strtol(p, &p, 10);
			cur->len = p-q;
			continue;
		}

		error_at(p, "無効なトークンです");
	}

	new_token(TK_EOF, cur, p, 0);
	return head.next;
}