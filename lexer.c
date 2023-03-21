#include "lexer.h"

#define ADD_SIMPLE_TOKEN(x) tokens[idx++] = (token){ .type = x, .line = line, .column = column };
#define ADD_INT_TOKEN(value) tokens[idx++] = (token){ .type = LITERAL_INT, .integer = value, .line = line, .column = column };
#define ADD_FLOAT_TOKEN(value) tokens[idx++] = (token){ .type = LITERAL_FLOAT, .decimal = value, .line = line, .column = column };
#define ADD_STRING_TOKEN(value) tokens[idx++] = (token){ .type = LITERAL_STRING, .string = value, .line = line, .column = column };
#define ADD_IDENTIFIER_TOKEN(value) tokens[idx++] = (token){ .type = IDENTIFIER, .string = value, .line = line, .column = column };
#define ADD_BOOL_TOKEN(value) tokens[idx++] = (token){ .type = LITERAL_BOOL, .boolean = value, .line = line, .column = column };

#define PREVIOUS text[i - 1]
#define CURRENT text[i]
#define NEXT text[i + 1]
#define NEXT_NEXT text[i + 2]

#define MAX_IDENTIFIER_LEN 256

// ---------------------------------------------------------
//
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


bool valid_identifier_prefix(char c) {
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

bool valid_identifier_char(char c) {
	return valid_identifier_prefix(c) || isdigit(c);
} 

i32 try_parse_int(char *buffer, i32 *number) {
	i32 length = 0;
	char *value = calloc(65, sizeof(char));

	while (isdigit(buffer[length])) {
		if (buffer[length + 1] == '.') {
			return 0; // float, so we don't parse it as int
		}
		length++;
	}

	strncat(value, buffer, length);

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
			strncat(value, ".", 2);
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

void reset_line(u32 *line, u32 *column) {
	*line += 1;
	*column = 1;
}

// ---------------------------------------------------------

i32 tokenize(char *text, token *tokens) {
	u32 idx = 0;
	u32 column = 1;
	u32 line = 1;

	for (i32 i = 0; i < strlen(text); i++) {
		switch (CURRENT) {
			case '(':
				ADD_SIMPLE_TOKEN(L_PAREN);
				column++;
				break;
			case ')':
				ADD_SIMPLE_TOKEN(R_PAREN);
				column++;
				break;
			case '[':
				ADD_SIMPLE_TOKEN(L_BRACKET);
				column++;
				break;
			case ']':
				ADD_SIMPLE_TOKEN(R_BRACKET);
				column++;
				break;
			case '{':
				ADD_SIMPLE_TOKEN(L_BRACE);
				column++;
				break;
			case '}':
				ADD_SIMPLE_TOKEN(R_BRACE);
				column++;
				break;
			case ';':
				ADD_SIMPLE_TOKEN(SEMICOLON);
				column++;
				break;
			case '|':
				if (NEXT == '|') {
					ADD_SIMPLE_TOKEN(OP_OR);
					column += 2;
					i++;
				} else {
					ADD_SIMPLE_TOKEN(PIPE);
					column++;
				}
				break;
			case '&':
				if (NEXT == '&') {
					ADD_SIMPLE_TOKEN(OP_AND);
					column += 2;
					i++;
				} 
				break;
			case '.':
				ADD_SIMPLE_TOKEN(DOT);
				column++;
				break;
			case ':':
				if (NEXT == ':') {
					ADD_SIMPLE_TOKEN(DOUBLE_COLON);
					column += 2;
					i++;
				} else {
					ADD_SIMPLE_TOKEN(COLON);
					column++;
				}

				break;
			case ',':
				ADD_SIMPLE_TOKEN(COMMA);
				column++;
				break;
			case '>':
				if (NEXT == '=') {
					ADD_SIMPLE_TOKEN(OP_GREATER_EQUAL);
					column += 2;
					i++;
				} else {
					ADD_SIMPLE_TOKEN(OP_GREATER);
					column++;
				}

				break;
			case '<':
				if (NEXT  == '=') {
					ADD_SIMPLE_TOKEN(OP_LESS_EQUAL);
					column += 2;
					i++;
				} else {
					ADD_SIMPLE_TOKEN(OP_LESS);
					column++;
				}

				break;
			case '!':
				if (NEXT == '=') {
					ADD_SIMPLE_TOKEN(OP_DIFFERENT);
					column += 2;
					i++;
				} 
				// TODO: add 'not'
				break;
			case '=':
				if (NEXT == '=') {
					ADD_SIMPLE_TOKEN(OP_EQUALS);
					column += 2;
					i++;
				} else {
					ADD_SIMPLE_TOKEN(OP_ASSIGNMENT);
					column++;
				}
				break;
			case '*':
				ADD_SIMPLE_TOKEN(OP_STAR);
				column++;
				break;
			case '-':
				if (NEXT == '>') {
					ADD_SIMPLE_TOKEN(ARROW);
					column += 2;
					i++;
				} else {
					ADD_SIMPLE_TOKEN(OP_MINUS);
					column++;
				}
				break;
			case '+':
				ADD_SIMPLE_TOKEN(OP_PLUS);
				column++;
				break;
			case '/':
				{
					// multiline comment
					if (NEXT == '*') {
						do {
							i++;
							column++;

							if (CURRENT == '\n') {
								reset_line(&line, &column);
							}
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
						reset_line(&line, &column);
						i++;
						continue;
					}

					ADD_SIMPLE_TOKEN(OP_SLASH);
					column++;
				}
				break;
			case '\n':
				reset_line(&line, &column);
				break;
			case '"':
				{
					i32 buf_len = 32;
					char *value = malloc(buf_len);
					char len = 1;

					// TODO: support backslash for escaping
					while (text[i + len] != '"') {
						if (len > buf_len) {
							buf_len *= 2;
							value = realloc(value, buf_len);
						}

						strncat(value, &text[i+len], 1);
						len++;
					}

					ADD_STRING_TOKEN(value)
					column += len;
					i += len;
				}
				break;
			default:
				{
					if (isspace(CURRENT)) {
						column++;
						continue;
					}

					i32 len = 0;
					i32 int_literal = 0;
					f64 float_literal = 0;

					if ((len = try_parse_int(&CURRENT, &int_literal))) {
						ADD_INT_TOKEN(int_literal);
						column += len;
						i += len - 1;
					} else if ((len = try_parse_float(&CURRENT, &float_literal))) {
						ADD_FLOAT_TOKEN(float_literal);
						column += len;
						i += len - 1;
					}

					// Keywords
					if ((len = match_word("let", &CURRENT))) {
						ADD_SIMPLE_TOKEN(KW_LET);
						column += len;
						i += len - 1;
					} else if ((len = match_word("record", &CURRENT))) {
						ADD_SIMPLE_TOKEN(KW_RECORD);
						column += len;
						i += len - 1;
					} else if ((len = match_word("if", &CURRENT))) {
						ADD_SIMPLE_TOKEN(KW_IF);
						column += len;
						i += len - 1;
					} else if ((len = match_word("do", &CURRENT))) {
						ADD_SIMPLE_TOKEN(KW_DO);
						column += len;
						i += len - 1;
					} else if ((len = match_word("else", &CURRENT))) {
						ADD_SIMPLE_TOKEN(KW_ELSE);
						column += len;
						i += len - 1;
					}
					// Booleans
					else if ((len = match_word("true", &CURRENT))) {
						ADD_BOOL_TOKEN(true);
						column += len;
						i += len - 1;
					} else if ((len = match_word("false", &CURRENT))) {
						ADD_BOOL_TOKEN(false);
						column += len;
						i += len - 1;
					} 
					// Identifiers
					else if (valid_identifier_prefix(CURRENT)) {
						char *identifier = malloc(MAX_IDENTIFIER_LEN);
						len = parse_identifier(&CURRENT, identifier);
						ADD_IDENTIFIER_TOKEN(identifier);
						column += len;
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

	sprintf(buffer, "<Token type=%s line=%d col=%d", format_token_type(tk.type), tk.line, tk.column);

	switch (tk.type) {
		case IDENTIFIER:
			sprintf(helper, " value=%s", tk.string);
			strcat(buffer, helper);
			break;
		case LITERAL_STRING:
			sprintf(helper, " value=\"%s\"", tk.string);
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

void print_tokens(token *tokens, int total_tokens) {
	for (i32 i = 0; i < total_tokens; i++) {
		char *pretty_token = malloc(1000);
		format_token(pretty_token, tokens[i]);
		printf("%s\n", pretty_token);
		free(pretty_token);
	}
}

void free_tokens(token *tokens, int total_tokens) {
	for (i32 i = 0; i < total_tokens; i++) {
		token t = tokens[i];
		if (t.type == IDENTIFIER || t.type == LITERAL_STRING) {
			free(t.string);
		}
	}

	free(tokens);
}
