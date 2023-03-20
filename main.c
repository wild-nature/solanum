#include "types.h"
#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_FILE_SIZE 8196000000 // 8mb

// ---------------------------------------------------------

char *read_file(FILE *file) {
	char *buffer = malloc(MAX_FILE_SIZE);

    i32 result = fread(buffer, MAX_FILE_SIZE, 1, file);
    if (result != 0) {
	    perror("could not read file");
	    exit(EXIT_FAILURE);
    }

    return buffer;
}

// ---------------------------------------------------------

int main() {
    FILE *file = fopen("sample.sn", "r");

    char *text = read_file(file);
	token *tokens = malloc(sizeof(token) * 1000);
	int total_tokens = tokenize(text, tokens);
	free(text);

	for (i32 i = 0; i < total_tokens; i++) {
		char *pretty_token = malloc(1000);
		format_token(pretty_token, tokens[i]);
		printf("%s\n", pretty_token);
		free(pretty_token);
	}

	expr *ast = parse(tokens, total_tokens);
	free(tokens);

	print_ast(ast);
	free(ast);

    fclose(file);
	exit(EXIT_SUCCESS);
}
