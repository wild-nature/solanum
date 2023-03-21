#ifndef LEXER_H
#define LEXER_H

#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

typedef struct {
	token_type type;
	union {
		i64 integer;
		f64 decimal;
		char *string;
		bool boolean;
	};
	u32 line;
	u32 column;
} token;

bool valid_identifier_prefix(char c);
bool valid_identifier_char(char c);
i32 try_parse_int(char *buffer, i32 *number);
i32 try_parse_float(char *buffer, f64 *number);
i32 parse_identifier(char *buffer, char *identifier);
u32 match_word(char *keyword, char *target);
void reset_line(u32 *line, u32 *column);
i32 tokenize(char *text, token *tokens);
char *format_token_type(token_type type);
char *format_token(char *buffer, token tk);
void print_tokens(token *tokens, int total_tokens);
void free_tokens(token *tokens, int total_tokens);

extern char* pretty_tokens[];

#endif
