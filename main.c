#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_FILE_SIZE 8196000000 // 8mb

typedef enum {
	L_PAREN,
	R_PAREN,

	L_BRACKET,
	R_BRACKET,

	L_BRACE,
	R_BRACE,

	SEMICOLON,
	DOUBLE_COLON,
	COLON,
	COMMA,
	ARROW,
	PIPE,
	DOT,

	// operators
	OP_ASSIGNMENT,
	OP_EQUALS,
	OP_MINUS,
	OP_PLUS,
	OP_STAR,
	OP_SLASH,
	OP_LESS_EQUAL, 
	OP_GREATER_EQUAL, 
	OP_LESS, 
	OP_GREATER, 
	OP_DIFFERENT, 

	IDENTIFIER,

	// values
	LITERAL_STRING,
	LITERAL_INT,
	LITERAL_FLOAT,
	// TODO: add booleans

	// keywords
	KW_LET,
	KW_RECORD,
	KW_IF,
	KW_DO,
	KW_ELSE,
} token_type;

char* pretty_tokens[] = {
	[L_PAREN] = "(", 
	[R_PAREN] = ")", 
	[L_BRACKET] = "[", 
	[R_BRACKET] = "]", 
	[L_BRACE] = "{", 
	[R_BRACE] = "}", 
	[SEMICOLON] = ";", 
	[DOUBLE_COLON] = "::", 
	[COLON] = ":", 
	[COMMA] = ",", 
	[ARROW] = "Arrow", 
	[PIPE] = "|", 
	[DOT] = ".", 

	// Operators
	[OP_ASSIGNMENT] = "Assignment", 
	[OP_EQUALS] = "Equals", 
	[OP_MINUS] = "Minus", 
	[OP_PLUS] = "Plus", 
	[OP_STAR] = "Star", 
	[OP_SLASH] = "Slash", 
	[OP_LESS_EQUAL] = "<=", 
	[OP_GREATER_EQUAL] = ">=", 
	[OP_LESS] = "<", 
	[OP_GREATER] = ">", 
	[OP_DIFFERENT] = "!=", 

	// Others
	[IDENTIFIER] = "Identifier",
	[LITERAL_STRING] = "String",
	[LITERAL_INT] = "Int",
	[LITERAL_FLOAT] = "Float",

	// keywords
	[KW_LET] = "let",
	[KW_RECORD] = "record",
	[KW_IF] = "if",
	[KW_DO] = "do",
	[KW_ELSE] = "else",
};

typedef struct {
	token_type type;
	union {
		i64 integer;
		f64 decimal;
		char *string;
	};
} token;

// ---------------------------------------------------------

bool valid_identifier_char(char c) {
	if (c >= 65 && c <= 90) {
		return true;
	} 

	if (c >= 97 && c <= 122) {
		return true;
	} 

	if (c == '_') {
		return true;
	}

	return false;
} 

// ---------------------------------------------------------

// TODO: replace this function with macro
u32 peek(char target, char next) {
	return target == next;
} 

// ---------------------------------------------------------

// TODO: fix
u32 match_keyword(char *keyword, char *target) {
	u32 idx = 0;

	while (keyword[idx] == target[idx]) {
		idx++;
	} 

	if (idx == strlen(keyword)) {
		return 1;
	}

	/* if (strspn(keyword, target) == strlen(keyword)) { */
	/* 	printf("%s -------- %s\n", keyword, target); */
	/* 	return 1; */
	/* } */

/* 	printf("%s -------- %s\n", keyword, target); */

	return 0;
}

// ---------------------------------------------------------

i32 tokenize(char *text, token *tokens) {
	i32 idx = 0;

	for (i32 i = 0; i < strlen(text); i++) {
		switch (text[i]) {
			case '(':
				tokens[idx] = (token){ .type = L_PAREN};
				idx++;
				break;
			case ')':
				tokens[idx] = (token){ .type = R_PAREN};
				idx++;
				break;
			case '[':
				tokens[idx] = (token){ .type = L_BRACKET};
				idx++;
				break;
			case ']':
				tokens[idx] = (token){ .type = R_BRACKET};
				idx++;
				break;
			case '{':
				tokens[idx] = (token){ .type = L_BRACE};
				idx++;
				break;
			case '}':
				tokens[idx] = (token){ .type = R_BRACE};
				idx++;
				break;
			case ';':
				tokens[idx] = (token){ .type = SEMICOLON};
				idx++;
				break;
			case '|':
				tokens[idx] = (token){ .type = PIPE};
				idx++;
				break;
			case '.':
				tokens[idx] = (token){ .type = DOT };
				idx++;
				break;
			case ':':
				{
					token_type type;

					if (peek(':', text[i+1])) {
						type = DOUBLE_COLON;
						i++;
					} else {
						type = COLON;
					}

					tokens[idx] = (token){ .type = type};
					idx++;
				}
				break;
			case ',':
				tokens[idx] = (token){ .type = COMMA };
				idx++;
				break;
			case '>':
				{
					token_type type;
					if (peek('=', text[i+1])) {
						type = OP_GREATER_EQUAL;
						i++;
					} else {
						type = OP_GREATER;
					}

					tokens[idx] = (token){ .type = type };
					idx++;
				}
				break;
			case '<':
				{
					token_type type;
					if (peek('=', text[i+1])) {
						type = OP_LESS_EQUAL;
						i++;
					} else {
						type = OP_LESS;
					}

					tokens[idx] = (token){ .type = type };
					idx++;
				}
				break;
			case '!':
				if (peek('=', text[i+1])) {
					tokens[idx] = (token){ .type = OP_DIFFERENT };
					idx++;
					i++;
				} 
				break;
			case '=':
				{
					token_type type;
					if (peek('=', text[i+1])) {
						type = OP_EQUALS;
						i++;
					} else {
						type = OP_ASSIGNMENT;
					}

					tokens[idx] = (token){ .type = type };
					idx++;
				}
				break;
			case '*':
				tokens[idx] = (token){ .type = OP_STAR };
				idx++;
				break;
			case '-':
				{
					token_type type;
					if (peek('>', text[i+1])) {
						type = ARROW;
						i++;
					} else {
						type = OP_MINUS;
					}

					tokens[idx] = (token){ .type = type };
					idx++;
				}
				break;
			case '+':
				tokens[idx] = (token){ .type = OP_PLUS };
				idx++;
				break;
			case '/':
				{
					// multiline comment
					if (peek('*', text[i+1])) {
						do {
							i++;
						}
						while(!(peek('*', text[i]) && peek('/', text[i+1])));
						i++;
						continue;
					}

					if (peek('/', text[i+1])) {
						do {
							i++;
						}
						while(!peek('\n', text[i]));
						continue;
					}

					tokens[idx] = (token){ .type = OP_SLASH };
					idx++;
				}
				break;
			case '"':
				{
					i32 buf_len = 32;
					char *value = malloc(buf_len);
					char len = 1;
					strncat(value, &text[i], 1);

					// TODO: support backslash for escaping
					while (!peek('"', text[i+len]) && &text[i+len] != NULL) {
						if (len > buf_len) {
							buf_len *= 2;
							value = realloc(value, buf_len);
						}

						strncat(value, &text[i+len], 1);
						len++;
					}

					strncat(value, "\"", 1);
					tokens[idx] = (token){ .type = LITERAL_STRING, .string = value };
					idx++;
					i += len;
				}
				break;
			default:
				{
					if (isdigit(text[i])) {
						bool is_float = false;
						char *value = calloc(65, sizeof(char));

						while (isdigit(text[i])) {
							if (text[i + 1] == '.' && isdigit(text[i + 2])) {
								strncat(value, &text[i], 1);
								strncat(value, ".", 1);
								i += 2;
								is_float = true;
							} else {
								strncat(value, &text[i], 1);
								i++;
							}
						}

						if (is_float) {
							tokens[idx] = (token){ .type = LITERAL_FLOAT, .decimal = atof(value) };
						} else {
							tokens[idx] = (token){ .type = LITERAL_INT, .integer = atol(value) };
						}

						idx++;
						free(value);
					}

					if (match_keyword("let", &text[i])) {
						tokens[idx] = (token){ .type = KW_LET };
						i += strlen("let") - 1;
						idx++;
					} else if (match_keyword("record", &text[i])) {
						tokens[idx] = (token){ .type = KW_RECORD };
						i += strlen("record") - 1;
						idx++;
					} else if (match_keyword("if", &text[i])) {
						tokens[idx] = (token){ .type = KW_IF };
						i += strlen("if") - 1;
						idx++;
					} else if (match_keyword("do", &text[i])) {
						tokens[idx] = (token){ .type = KW_DO };
						i += strlen("do") - 1;
						idx++;
					} else if (match_keyword("else", &text[i])) {
						tokens[idx] = (token){ .type = KW_ELSE };
						i += strlen("else") - 1;
						idx++;
					} 
					// Identifiers
					else if (isalpha(text[i])) {
						i32 buf_len = 32;
						char *value = malloc(buf_len);
						char len = 1;
						strncat(value, &text[i], 1);

						while (valid_identifier_char(text[i + len])) {
							printf("chhar is %c\n", text[i + len]);
							if (len > buf_len) {
								buf_len *= 2;
								value = realloc(value, buf_len);
							}

							strncat(value, &text[i+len], 1);
							len++;
						}

						tokens[idx] = (token){ .type = IDENTIFIER, .string = value };
						idx++;
						i += len - 1;
					}

				}
		}
	}

	return idx;
}

// ---------------------------------------------------------

char *format_token_type(token_type type) {
	return pretty_tokens[type];
}

// ---------------------------------------------------------

char *format_token(char *buffer, token tk) {
	char *helper = malloc(50);

	sprintf(buffer, "<Token type=%s", format_token_type(tk.type));

	switch (tk.type) {
		case IDENTIFIER:
		case LITERAL_STRING:
			sprintf(helper, " value=%s", tk.string);
			strcat(buffer, helper);
			break;
		case LITERAL_INT:
			sprintf(helper, " value=%ld", tk.integer);
			strcat(buffer, helper);
			break;
		case LITERAL_FLOAT:
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
	token *tokens = malloc(sizeof(token) * 1000);
	int total_tokens = tokenize(text, tokens);

	for (i32 i = 0; i < total_tokens; i++) {
		char *pretty_token = malloc(1000);
		format_token(pretty_token, tokens[i]);
		printf("%s\n", pretty_token);
		free(pretty_token);
	}

	free(text);
    fclose(file);
	exit(EXIT_SUCCESS);
}
