#include"9cc.h"
	//
	// code generator
	//
	void gen(Node *node) {
		if (node -> kind == ND_NUM) {
			printf("	push %d\n", node->val);
			return;
		}
	
		gen(node->lhs);
		gen(node->rhs);

		printf("	pop rdi\n");
		printf("	pop rax\n");

		switch (node->kind) {
		case ND_ADD:
			printf("	add rax, rdi\n");
			break;
		case ND_SUB:
			printf("	sub rax, rdi\n");
			break;
		case ND_MUL:
			printf("	imul rax, rdi\n");
			break;
		case ND_DIV:
			printf("	cqo\n");
			printf("	idiv rdi\n");
			break;
		case ND_EQ:
			printf("	cmp rax, rdi\n");
			printf("	sete al\n");
			printf("	movzb rax, al\n");
			break;
		case ND_NE:
			printf("	cmp rax, rdi\n");
			printf("	setne al\n");
			printf("	movzb rax, al\n");
			break;
		case ND_LT:
			printf("	cmp rax, rdi\n");
			printf("	setl al\n");
			printf("	movzb rax, al\n");
			break;
		case ND_LE:
			printf("	cmp rax, rdi\n");
			printf("	setle al\n");
			printf("	movzb rax, al\n");
			break;
		}
			
	printf("	push rax\n");

	} // end of gen	

int main(int argc, char **argv) {
	if (argc != 2) 
		error("%s: invalid number of arguments", argv[0]);

	// トークナイズしてパースする
	// user_input = argv[1];
	Token *token = tokenize();
	Node *node = parse(token);

	// アセンブリの前半部分を出力
	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");

	// 抽象構文木を下りながらコード生成
	gen(node);

	
	// スタックトップに式全体の値が残っているはずなので
	// それをRAXにロードして関数からの返り値とする
	printf("	pop rax\n");
	printf("	ret\n");
	return 0;
}

