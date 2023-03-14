#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILE_SIZE 8196000000 // 8mb

char* pretty_tokens[] = {
	"(", ")", "[", "]", "{", "}", 
	"Equals", "Minus", "Plus", "Star", "Slash",
	"Identifier", "String", "Integer", "Float", 
	"let"
};

typedef enum {
	L_PAREN,
	R_PAREN,

	L_BRACKET,
	R_BRACKET,

	L_BRACES,
	R_BRACES,

	// operators
	EQUAL,
	MINUS,
	PLUS,
	STAR,
	SLASH,

	IDENTIFIER,

	// values
	STRING,
	INTEGER,
	FLOAT,

	// keywords
	KW_LET
} token_type;

typedef struct {
	token_type type;
	union {
		i64 integer;
		f64 decimal;
		char *string;
	};
} token;

typedef struct {
	token *tokens;
	i32 total_tokens;
} token_list;

// ---------------------------------------------------------

token_list tokenize(char *text) {
	token *tokens = malloc(sizeof(token) * 1000); // TODO: exponentially increase it

	tokens[0] = (token){ .type = KW_LET };
	tokens[1] = (token){ .type = IDENTIFIER, .string = "codando" };
	tokens[2] = (token){ .type = EQUAL };
	tokens[3] = (token){ .type = INTEGER, .integer = 10 };

	return (token_list){
		.tokens = tokens,
		.total_tokens = 4,
	};
}

// ---------------------------------------------------------

char *format_token_type(token_type type) {
	return pretty_tokens[type];
}

// ---------------------------------------------------------

// TODO: pass allocation responsibility to caller
char *format_token(token tk) {
	char *buffer = malloc(1000);
	char *helper = malloc(50);

	sprintf(buffer, "<Token type=%s", format_token_type(tk.type));

	switch (tk.type) {
		case IDENTIFIER:
		case STRING:
			sprintf(helper, " value=%s", tk.string);
			strcat(buffer, helper);
			break;
		case INTEGER:
			sprintf(helper, " value=%d", tk.integer);
			strcat(buffer, helper);
			break;
		case FLOAT:
			sprintf(helper, " value=%f", tk.decimal);
			strcat(buffer, helper);
			break;
	}

	strcat(buffer, ">");
	free(helper);
	return buffer;
}

// ---------------------------------------------------------

char *parse_file(FILE *file) {
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

    char *text = parse_file(file);
	token_list tl = tokenize(text);

	for (i32 i = 0; i < tl.total_tokens; i++) {
		char *pretty_token = format_token(tl.tokens[i]);
		printf("%s\n", pretty_token);
		free(pretty_token);
	}


	puts(text);
	printf("length: %d\n", strlen(text));

	free(text);
    fclose(file);
	exit(EXIT_SUCCESS);
}
