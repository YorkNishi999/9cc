#include "9cc.h"

//
// tokenizer
//

// 新しいトークンを作成してcurにつなげる
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	tok->len = len;
	cur->next = tok;
	return tok;
}

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p) {
	// user_inputをトークナイズする
	// char *p = user_input;
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
		if(strchr("+-*/()<>;=", *p)) {
			cur = new_token(TK_RESERVED, cur, p++, 1);
			continue;
		}

		// Ident （アルファベット小文字なら、IDENTを作る）
		if ('a' <= *p && *p <= 'z') {
			cur = new_token(TK_IDENT, cur, p++, 1);
			cur->len = 1;
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