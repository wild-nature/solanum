#ifndef PARSER_H
#define PARSER_H

#include "types.h"
#include "lexer.h"

typedef enum {
	STRING,
	FLOAT,
	INTEGER,
	BOOLEAN
} literal_type;

typedef enum {
	UNARY,
	BINARY,
	LITERAL,
	GROUPING,
} expr_type;

typedef struct {
	expr_type type;
	union {
		struct unary_expr *unary;
		struct binary_expr *binary;
		struct literal_expr *literal;
		struct group_expr *grouping;
	};
} expr;

typedef struct group_expr {
	expr *inner;
} group_expr;

typedef struct binary_expr {
	token_type op; // operator
	expr *left;
	expr *right;
} binary_expr;

typedef struct unary_expr {
	token_type op;
	expr *operand;
} unary_expr;

typedef struct literal_expr {
	literal_type type;
	union {
		f64 decimal;
		char *string;
		i64 integer;
		bool boolean;
	};
} literal_expr;

group_expr *new_group(expr *expr);
unary_expr *new_unary(token_type op, expr *expr);
binary_expr *new_binary(token_type op, expr *left, expr *right);
literal_expr *new_literal(literal_type type, void *value);
expr *new_expr(expr_type type, void* wrapped_expr);

void free_group(group_expr *ge);
void free_unary(unary_expr *expr);
void free_binary(binary_expr *expr);
void free_literal(literal_expr *expr);
void free_expr(expr *expr);

expr *parse(token *tokens, i32 total_tokens);

void print_ast(expr* root);

#endif
