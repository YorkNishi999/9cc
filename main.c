#include "9cc.h"

int main(int argc, char **argv) {
	if (argc != 2) 
		error("%s: invalid number of arguments", argv[0]);

	// トークナイズしてパースする
	user_input = argv[1];
	token = tokenize(user_input);
	program(); // parse

	// アセンブリの前半部分を出力
	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");

	// プロローグ　変数26個分の領域を確保
	printf("	push rbp\n");
	printf("	mov rbp, rsp\n");
	printf("	sub rsp, 208\n");

	// 先頭の式から順にコード生成
	for (int i = 0; code[i]; i++) {
		gen(code[i]);

		// 式の評価結果としてスタックに1つの値が残っているはずなので
		// スタックが溢れないようにPOPする
		printf("	pop rax\n");
	}
	
	// エピローグ　最後の式の結果がRAXに残っているのでそれをリターン
	printf("	mov rsp, rbp\n");
	printf("	pop rbp\n");
	printf("	ret\n");
	return 0;
}

