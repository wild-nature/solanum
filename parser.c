#include "parser.h"

/*
	expr => 
		| fun
		| var
		| lit
		| call
		| unary
		| binary
	 
	var => "let" identifier = expr
	fun => "let" identifier+ = expr
	lit => 
		| float
		| integer
		| boolean
		| string
	call => identifier expr
	unary => ("-" | "!") expression
	binary => expression operator expression
	grouping => "(" expr ")"

	operator => 
		"+" | "-" | "*" | "/" 
		| "==" | ">" | ">=" | "<" | "<=" 
		| "&&" | "||"
*/


// TODO: figure out if it would be nice to make specific enums for binary and
// unary operators here
binary_expr *new_binary(token_type op, expr *left, expr *right) {
	binary_expr *bexpr = malloc(sizeof(binary_expr));
	bexpr->op = op;
	bexpr->left = left;
	bexpr->right = right;
	return bexpr;
}

void free_binary(binary_expr *expr) {
	free_expr(expr->left);
	free_expr(expr->right);
	free(expr);
}

literal_expr *new_literal(literal_type type, void *value) {
	literal_expr *literal = malloc(sizeof(literal));
	literal->type = type;

	// TODO: fix warnings caused here
	switch (type) {
		case STRING:
			literal->string = (char*)value;
			break;
		case FLOAT:
			literal->decimal = *(f64*)value;
			break;
		case INTEGER:
			literal->integer = *(i64*)value;
			break;
		case BOOLEAN:
			literal->boolean = *(bool*)value;
			break;
	}

	return literal;
}

expr *new_expr(expr_type type, void* wrapped_expr) {
	// TODO: fix warnings caused here
	expr *expr = malloc(sizeof(expr));
	expr->type = type;

	switch (type) {
		case UNARY:
			expr->unary = (unary_expr*)wrapped_expr;
			break;
		case BINARY:
			expr->binary = (binary_expr*)wrapped_expr;
			break;
		case LITERAL:
			expr->literal = (literal_expr*)wrapped_expr;
			break;
	}

	return expr;
}

void free_expr(expr *expr) {
	switch (expr->type) {
		case UNARY:
			free_unary(expr->unary);
			break;
		case BINARY:
			free_binary(expr->binary);
			break;
		case LITERAL:
			free_literal(expr->literal);
			break;
	}

	free(expr);
}

void free_literal(literal_expr *expr) {
	if (expr->type == STRING) {
		free(expr->string);
	}
	
	free(expr);
}

unary_expr *new_unary(token_type op, expr *expr) {
	unary_expr *unexpr = malloc(sizeof(unary_expr));
	unexpr->op = op;
	unexpr->operand = expr;
	return unexpr;
}

void free_unary(unary_expr *expr) {
	free_expr(expr->operand);
	free(expr);
}

expr *parse(token *tokens, i32 total_tokens) {
	i64 a = 10;
	expr *left = new_expr(LITERAL, new_literal(INTEGER, &a));

	// right
	i64 b = 20;
	expr *right = new_expr(LITERAL, new_literal(INTEGER, &b));
	
	// expr
	expr *sum = new_expr(BINARY, new_binary(OP_PLUS, left, right));

	expr *sub = 
		new_expr(BINARY, 
			new_binary(OP_MINUS, sum, 
			new_expr(LITERAL, new_literal(INTEGER, &b))
		));

	return sub;
}

void leftpad(int depth, int length) {
	for (i32 i = 0; i < depth; i++) {
		if (i % length == 0) {
			putchar('|');
			continue;
	 	}

		putchar(' ');
	}
}

void format_literal(char *buffer, literal_expr *expr) {
	switch (expr->type) {
		case STRING:
			sprintf(buffer, "String %s", expr->string);
			break;
		case FLOAT:
			sprintf(buffer, "Float %.4f", expr->decimal);
			break;
		case INTEGER:
			sprintf(buffer, "Integer %ld", expr->integer);
			break;
		case BOOLEAN:
			sprintf(buffer, "Boolean %s", expr->boolean ? "true" : "false");
			break;
	}
}

void print_ast_rec(expr* root, int depth) {
	int length = 2;
	leftpad(depth, length);

	switch (root->type) {
		case LITERAL:
			{
				char buffer[256];
				format_literal(buffer, root->literal);
				printf("%s\n", buffer);
			}
			break;

		case UNARY:
			// TODO: different type for ast unary operators, for now lets use
			// this workaround
			printf("Unary %s\n", "-"); // TODO: print operator
			print_ast_rec(root->unary->operand, depth + length);
			break;

		case BINARY:
			// TODO: different type for ast binary operators, for now lets use
			// this workaround
			printf("Binary %s\n", "+"); // TODO: print operator
			print_ast_rec(root->binary->left, depth + length);
			print_ast_rec(root->binary->right, depth + length);
			break;
	}
}

void print_ast(expr* root) {
	print_ast_rec(root, 0);
}
