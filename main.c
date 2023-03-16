#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define ADD_SIMPLE_TOKEN(x) tokens[idx++] = (token){ .type = x };
#define ADD_INT_TOKEN(value) tokens[idx++] = (token){ .type = LITERAL_INT, .integer = value };
#define ADD_FLOAT_TOKEN(value) tokens[idx++] = (token){ .type = LITERAL_FLOAT, .decimal = value };
#define ADD_STRING_TOKEN(value) tokens[idx++] = (token){ .type = LITERAL_STRING, .string = value };
#define ADD_IDENTIFIER_TOKEN(value) tokens[idx++] = (token){ .type = IDENTIFIER, .string = value };
#define ADD_BOOL_TOKEN(value) tokens[idx++] = (token){ .type = LITERAL_BOOL, .boolean = value };

#define PREVIOUS text[i - 1]
#define CURRENT text[i]
#define NEXT text[i + 1]
#define NEXT_NEXT text[i + 2]

#define MAX_FILE_SIZE 8196000000 // 8mb
#define MAX_IDENTIFIER_LEN 256

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
	OP_OR, 
	OP_AND, 

	IDENTIFIER,

	// values
	LITERAL_STRING,
	LITERAL_INT,
	LITERAL_FLOAT,
	LITERAL_BOOL,

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
	[OP_OR] = "||", 
	[OP_AND] = "&&", 

	// Others
	[IDENTIFIER] = "Identifier",
	[LITERAL_STRING] = "String",
	[LITERAL_INT] = "Int",
	[LITERAL_FLOAT] = "Float",
	[LITERAL_BOOL] = "Boolean",

	// keywords
	[KW_LET] = "let",
	[KW_RECORD] = "record",
	[KW_IF] = "if",
	[KW_DO] = "do",
	[KW_ELSE] = "else",
};

typedef struct {
	// TODO: track token's position
	token_type type;
	union {
		i64 integer;
		f64 decimal;
		char *string;
		bool boolean;
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

i32 try_parse_int(char *buffer, i32 *number) {
	i32 length = 0;
	char *value = calloc(65, sizeof(char));

	while (isdigit(buffer[length])) {
		if (buffer[length + 1] == '.') {
			return 0; // float, so we don't parse it as int
		}

		strncat(value, &buffer[length], 1);
		length++;
	}

	if (length > 0) {
		*number = atol(value);
	}

	free(value);
	return length;
}

i32 try_parse_float(char *buffer, f64 *number) {
	i32 length = 0;
	char *value = calloc(65, sizeof(char));

	while (isdigit(buffer[length])) {
		strncat(value, &buffer[length], 1);
		length++;

		if (buffer[length + 1] == '.' && isdigit(buffer[length + 2])) {
			strncat(value, &buffer[length], 1);
			strncat(value, ".", 1);
			length += 2;
			continue;
		}

	}

	if (length > 0) {
		*number = atof(value);
	}

	free(value);
	return length;
}

i32 parse_identifier(char *buffer, char *identifier) {
	i32 length = 0;

	while (valid_identifier_char(buffer[length])) {
		length++;
	}

	strncat(identifier, buffer, length);
	return length;
}

// ---------------------------------------------------------

u32 match_word(char *keyword, char *target) {
	u32 idx = 0;

	while (keyword[idx] == target[idx]) {
		idx++;
	} 

	if (idx == strlen(keyword)) {
		return idx;
	}

	return 0;
}

// ---------------------------------------------------------

i32 tokenize(char *text, token *tokens) {
	i32 idx = 0;

	for (i32 i = 0; i < strlen(text); i++) {
		switch (CURRENT) {
			case '(':
				ADD_SIMPLE_TOKEN(L_PAREN);
				break;
			case ')':
				ADD_SIMPLE_TOKEN(R_PAREN);
				break;
			case '[':
				ADD_SIMPLE_TOKEN(L_BRACKET);
				break;
			case ']':
				ADD_SIMPLE_TOKEN(R_BRACKET);
				break;
			case '{':
				ADD_SIMPLE_TOKEN(L_BRACE);
				break;
			case '}':
				ADD_SIMPLE_TOKEN(R_BRACE);
				break;
			case ';':
				ADD_SIMPLE_TOKEN(SEMICOLON);
				break;
			case '|':
				if (NEXT == '|') {
					ADD_SIMPLE_TOKEN(OP_OR);
					i++;
				} else {
					ADD_SIMPLE_TOKEN(PIPE);
				}
				break;
			case '&':
				if (NEXT == '&') {
					ADD_SIMPLE_TOKEN(OP_AND);
					i++;
				} 
				break;
			case '.':
				ADD_SIMPLE_TOKEN(DOT);
				break;
			case ':':
				if (NEXT == ':') {
					ADD_SIMPLE_TOKEN(DOUBLE_COLON);
					i++;
				} else {
					ADD_SIMPLE_TOKEN(COLON);
				}

				break;
			case ',':
				ADD_SIMPLE_TOKEN(COMMA);
				break;
			case '>':
				if (NEXT == '=') {
					ADD_SIMPLE_TOKEN(OP_GREATER_EQUAL);
					i++;
				} else {
					ADD_SIMPLE_TOKEN(OP_GREATER);
				}

				break;
			case '<':
				if (NEXT  == '=') {
					ADD_SIMPLE_TOKEN(OP_LESS_EQUAL);
					i++;
				} else {
					ADD_SIMPLE_TOKEN(OP_LESS);
				}

				break;
			case '!':
				if (NEXT == '=') {
					ADD_SIMPLE_TOKEN(OP_DIFFERENT);
					i++;
				} 
				break;
			case '=':
				if (NEXT == '=') {
					ADD_SIMPLE_TOKEN(OP_EQUALS);
					i++;
				} else {
					ADD_SIMPLE_TOKEN(OP_ASSIGNMENT);
				}
				break;
			case '*':
				ADD_SIMPLE_TOKEN(OP_STAR);
				break;
			case '-':
				if (NEXT == '>') {
					ADD_SIMPLE_TOKEN(ARROW);
					i++;
				} else {
					ADD_SIMPLE_TOKEN(OP_MINUS);
				}
				break;
			case '+':
				ADD_SIMPLE_TOKEN(OP_PLUS);
				break;
			case '/':
				{
					// multiline comment
					if (NEXT == '*') {
						do {
							i++;
						}
						while(!(CURRENT == '*' && NEXT == '/'));
						i++;
						continue;
					}

					if (NEXT == '/') {
						do {
							i++;
						}
						while(NEXT != '\n');
						continue;
					}

					ADD_SIMPLE_TOKEN(OP_SLASH);
				}
				break;
			case '"':
				{
					i32 buf_len = 32;
					char *value = malloc(buf_len);
					char len = 1;

					// TODO: support backslash for escaping
					while (text[i + len] != '"' && &NEXT != NULL) {
						if (len > buf_len) {
							buf_len *= 2;
							value = realloc(value, buf_len);
						}

						strncat(value, &text[i+len], 1);
						len++;
					}

					tokens[idx++] = (token){ .type = LITERAL_STRING, .string = value };
					i += len;
				}
				break;
			default:
				{
					i32 len = 0;
					i32 int_literal = 0;
					f64 float_literal = 0;

					if ((len = try_parse_int(&CURRENT, &int_literal))) {
						ADD_INT_TOKEN(int_literal);
						i += len - 1;
					} else if ((len = try_parse_float(&CURRENT, &float_literal))) {
						ADD_FLOAT_TOKEN(float_literal);
						i += len - 1;
					}

					// Keywords
					if ((len = match_word("let", &CURRENT))) {
						ADD_SIMPLE_TOKEN(KW_LET);
						i += len - 1;
					} else if ((len = match_word("record", &CURRENT))) {
						ADD_SIMPLE_TOKEN(KW_RECORD);
						i += len - 1;
					} else if ((len = match_word("if", &CURRENT))) {
						ADD_SIMPLE_TOKEN(KW_IF);
						i += len - 1;
					} else if ((len = match_word("do", &CURRENT))) {
						ADD_SIMPLE_TOKEN(KW_DO);
						i += len - 1;
					} else if ((len = match_word("else", &CURRENT))) {
						ADD_SIMPLE_TOKEN(KW_ELSE);
						i += len - 1;
					}
					// Booleans
					else if ((len = match_word("true", &CURRENT))) {
						ADD_BOOL_TOKEN(true);
						i += len - 1;
					} else if ((len = match_word("false", &CURRENT))) {
						ADD_BOOL_TOKEN(false);
						i += len - 1;
					} 
					// Identifiers
					else if (valid_identifier_char(CURRENT)) {
						char *identifier = malloc(MAX_IDENTIFIER_LEN);
						len = parse_identifier(&CURRENT, identifier);
						ADD_IDENTIFIER_TOKEN(identifier);
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
		case LITERAL_BOOL:
			sprintf(helper, " value=%s", tk.boolean ? "true" : "false");
			strcat(buffer, helper);
			break;
		default:
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
