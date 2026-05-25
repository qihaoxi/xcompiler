#include "parser.h"

#include <string.h>

#define XC_PARSER_MAX_AST_NODES 256
#define XC_PARSER_MAX_LABEL_LENGTH 64

typedef struct xc_parser_cursor {
	xc_lexer_t       lexer;
	bool             has_peek;
	xc_token_t       peek_token;
	xc_status_code_t peek_status;
	size_t           last_end;
} xc_parser_cursor_t;

static xc_ast_node_t g_parser_nodes[XC_PARSER_MAX_AST_NODES];
static char          g_parser_labels[XC_PARSER_MAX_AST_NODES][XC_PARSER_MAX_LABEL_LENGTH];

static bool xc_parser_token_matches(const xc_token_t* token, const char* text) {
	if (!token || !text || !token->lexeme) {
		return false;
	}
	size_t text_length = strlen(text);
	return token->lexeme_length == text_length && memcmp(token->lexeme, text, text_length) == 0;
}

static bool xc_parser_token_is_symbol(const xc_token_t* token, const char* text) {
	return token && token->kind == XC_TOKEN_SYMBOL && xc_parser_token_matches(token, text);
}

static bool xc_parser_token_is_keyword(const xc_token_t* token, const char* text) {
	return token && token->kind == XC_TOKEN_KEYWORD && xc_parser_token_matches(token, text);
}

static bool xc_parser_is_declaration_start(const xc_token_t* token) {
	return xc_parser_token_is_keyword(token, "let") || xc_parser_token_is_keyword(token, "fun") ||
	       xc_parser_token_is_keyword(token, "class") || xc_parser_token_is_keyword(token, "import") ||
	       xc_parser_token_is_keyword(token, "package") || xc_parser_token_is_keyword(token, "prefer") ||
	       xc_parser_token_is_keyword(token, "micro") || xc_parser_token_is_keyword(token, "neta") ||
	       xc_parser_token_is_keyword(token, "note") || xc_parser_token_is_keyword(token, "extends");
}

static bool xc_parser_is_statement_start(const xc_token_t* token) {
	return xc_parser_token_is_keyword(token, "return") || xc_parser_token_is_keyword(token, "if") ||
	       xc_parser_token_is_keyword(token, "while") || xc_parser_token_is_keyword(token, "for") ||
	       xc_parser_token_is_keyword(token, "throw") || xc_parser_token_is_keyword(token, "break") ||
	       xc_parser_token_is_keyword(token, "continue") || xc_parser_token_is_keyword(token, "case") ||
	       xc_parser_token_is_keyword(token, "else") || xc_parser_token_is_keyword(token, "end");
}

static xc_ast_node_kind_t xc_parser_classify_start(const xc_token_t* token) {
	if (xc_parser_is_declaration_start(token)) {
		return XC_AST_DECLARATION;
	}
	if (xc_parser_is_statement_start(token)) {
		return XC_AST_STATEMENT;
	}
	return XC_AST_EXPRESSION;
}

static xc_status_code_t xc_parser_cursor_peek(xc_parser_cursor_t* cursor, xc_token_t* out_token) {
	if (!cursor) {
		return XC_STATUS_INVALID_ARGUMENT;
	}
	if (!cursor->has_peek) {
		cursor->peek_status = xc_lexer_next_token(&cursor->lexer, &cursor->peek_token);
		cursor->has_peek    = true;
	}
	if (out_token) {
		*out_token = cursor->peek_token;
	}
	return cursor->peek_status;
}

static xc_status_code_t xc_parser_cursor_consume(xc_parser_cursor_t* cursor, xc_token_t* out_token) {
	xc_status_code_t status = xc_parser_cursor_peek(cursor, out_token);
	if (status == XC_STATUS_INVALID_ARGUMENT) {
		return status;
	}
	cursor->last_end = cursor->peek_token.range.offset + cursor->peek_token.range.length;
	cursor->has_peek = false;
	return status;
}

static const char* xc_parser_store_label(size_t slot, const char* text, size_t length) {
	if (!text) {
		return "unknown";
	}
	if (slot >= XC_PARSER_MAX_AST_NODES) {
		return "unknown";
	}
	size_t copy_length = length;
	if (copy_length >= XC_PARSER_MAX_LABEL_LENGTH) {
		copy_length = XC_PARSER_MAX_LABEL_LENGTH - 1;
	}
	memcpy(g_parser_labels[slot], text, copy_length);
	g_parser_labels[slot][copy_length] = '\0';
	return g_parser_labels[slot];
}

static xc_status_code_t xc_parser_append_node(xc_ast_node_t* nodes, size_t capacity, size_t* count,
                                              xc_ast_node_kind_t kind, size_t start, size_t end, const char* label,
                                              size_t label_length, size_t* out_index) {
	if (!nodes || !count || !label) {
		return XC_STATUS_INVALID_ARGUMENT;
	}
	if (*count >= capacity) {
		return XC_STATUS_INTERNAL_ERROR;
	}
	size_t      slot         = *count;
	const char* stored_label = xc_parser_store_label(slot, label, label_length);
	nodes[slot].kind         = kind;
	nodes[slot].range.offset = start;
	nodes[slot].range.length = (end > start) ? (end - start) : 0;
	nodes[slot].label        = stored_label;
	if (out_index) {
		*out_index = slot;
	}
	(*count)++;
	return XC_STATUS_OK;
}

static void xc_parser_update_node_range(xc_ast_node_t* nodes, size_t index, size_t start, size_t end) {
	if (!nodes) {
		return;
	}
	nodes[index].range.offset = start;
	nodes[index].range.length = (end > start) ? (end - start) : 0;
}

static xc_status_code_t xc_parser_collect_stream(xc_parser_cursor_t* cursor, bool stop_on_closing_brace,
                                                 xc_ast_node_t* nodes, size_t capacity, size_t* count);

static xc_status_code_t xc_parser_parse_construct(xc_parser_cursor_t* cursor, const xc_token_t* start_token,
                                                  bool stop_on_closing_brace, xc_ast_node_t* nodes, size_t capacity,
                                                  size_t* count) {
	xc_ast_node_kind_t kind         = xc_parser_classify_start(start_token);
	const char*        label        = start_token->lexeme ? start_token->lexeme : "unknown";
	size_t             label_length = start_token->lexeme_length;
	size_t             start        = start_token->range.offset;
	size_t             end          = start_token->range.offset + start_token->range.length;
	size_t             node_index   = 0;
	xc_status_code_t   result       = XC_STATUS_OK;
	xc_status_code_t   status =
	    xc_parser_append_node(nodes, capacity, count, kind, start, start, label, label_length, &node_index);
	if (status != XC_STATUS_OK) {
		return status;
	}

	if (xc_parser_token_is_keyword(start_token, "let") || xc_parser_token_is_keyword(start_token, "fun")) {
		xc_token_t next_token;
		status = xc_parser_cursor_peek(cursor, &next_token);
		if (status == XC_STATUS_INVALID_ARGUMENT || status == XC_STATUS_UNSUPPORTED) {
			return status;
		}
		if (status == XC_STATUS_OK && next_token.kind == XC_TOKEN_IDENTIFIER) {
			(void)xc_parser_cursor_consume(cursor, &next_token);
			label                   = next_token.lexeme ? next_token.lexeme : label;
			label_length            = next_token.lexeme_length;
			nodes[node_index].label = xc_parser_store_label(node_index, label, label_length);
			end                     = next_token.range.offset + next_token.range.length;
		}
	}

	for (;;) {
		xc_token_t token;
		status = xc_parser_cursor_peek(cursor, &token);
		if (status == XC_STATUS_INVALID_ARGUMENT || status == XC_STATUS_UNSUPPORTED) {
			result = status;
			break;
		}
		if (status == XC_STATUS_NOT_READY) {
			break;
		}
		if (token.kind == XC_TOKEN_COMMENT) {
			(void)xc_parser_cursor_consume(cursor, NULL);
			continue;
		}
		if (xc_parser_token_is_symbol(&token, "{")) {
			(void)xc_parser_cursor_consume(cursor, NULL);
			status = xc_parser_collect_stream(cursor, true, nodes, capacity, count);
			if (status != XC_STATUS_OK) {
				result = status;
				break;
			}
			end = cursor->last_end;
			xc_parser_update_node_range(nodes, node_index, start, end);
			continue;
		}
		if (xc_parser_token_is_symbol(&token, ";")) {
			(void)xc_parser_cursor_consume(cursor, NULL);
			end = cursor->last_end;
			xc_parser_update_node_range(nodes, node_index, start, end);
			break;
		}
		if (xc_parser_token_is_symbol(&token, "}")) {
			break;
		}
		(void)xc_parser_cursor_consume(cursor, NULL);
		end = cursor->last_end;
		xc_parser_update_node_range(nodes, node_index, start, end);
	}

	(void)stop_on_closing_brace;
	xc_parser_update_node_range(nodes, node_index, start, end);
	return result;
}

static xc_status_code_t xc_parser_collect_stream(xc_parser_cursor_t* cursor, bool stop_on_closing_brace,
                                                 xc_ast_node_t* nodes, size_t capacity, size_t* count) {
	for (;;) {
		xc_token_t       token;
		xc_status_code_t status = xc_parser_cursor_peek(cursor, &token);
		if (status == XC_STATUS_INVALID_ARGUMENT || status == XC_STATUS_UNSUPPORTED) {
			return status;
		}
		if (status == XC_STATUS_NOT_READY) {
			return stop_on_closing_brace ? XC_STATUS_UNSUPPORTED : XC_STATUS_OK;
		}
		if (token.kind == XC_TOKEN_COMMENT) {
			(void)xc_parser_cursor_consume(cursor, NULL);
			continue;
		}
		if (xc_parser_token_is_symbol(&token, ";")) {
			(void)xc_parser_cursor_consume(cursor, NULL);
			continue;
		}
		if (xc_parser_token_is_symbol(&token, "}")) {
			if (stop_on_closing_brace) {
				(void)xc_parser_cursor_consume(cursor, NULL);
				return XC_STATUS_OK;
			}
			(void)xc_parser_cursor_consume(cursor, NULL);
			return XC_STATUS_UNSUPPORTED;
		}
		(void)xc_parser_cursor_consume(cursor, NULL);
		if (xc_parser_token_is_symbol(&token, "{")) {
			status = xc_parser_collect_stream(cursor, true, nodes, capacity, count);
			if (status != XC_STATUS_OK) {
				return status;
			}
			continue;
		}
		status = xc_parser_parse_construct(cursor, &token, stop_on_closing_brace, nodes, capacity, count);
		if (status != XC_STATUS_OK) {
			return status;
		}
	}
}

void xc_parser_init(xc_parser_t* parser, const xc_lexer_t* lexer) {
	if (!parser) {
		return;
	}
	parser->lexer = lexer;
}

void xc_parser_reset(xc_parser_t* parser) {
	if (!parser) {
		return;
	}
	parser->lexer = NULL;
}

bool xc_parser_is_ready(const xc_parser_t* parser) {
	return parser && parser->lexer != NULL;
}

xc_status_code_t xc_parser_parse(const xc_parser_t* parser, xc_ast_t* out_ast) {
	if (!parser || !out_ast || !xc_parser_is_ready(parser)) {
		return XC_STATUS_INVALID_ARGUMENT;
	}

	xc_ast_init(out_ast);
	out_ast->nodes       = g_parser_nodes;
	out_ast->node_count  = 0;
	xc_ast_node_t* nodes = (xc_ast_node_t*)out_ast->nodes;

	xc_parser_cursor_t cursor;
	cursor.lexer       = *parser->lexer;
	cursor.has_peek    = false;
	cursor.peek_status = XC_STATUS_OK;
	cursor.last_end    = 0;

	xc_status_code_t status =
	    xc_parser_append_node(nodes, XC_PARSER_MAX_AST_NODES, &out_ast->node_count, XC_AST_PROGRAM, 0,
	                          cursor.lexer.source.length, "program", strlen("program"), NULL);
	if (status != XC_STATUS_OK) {
		return status;
	}

	status = xc_parser_collect_stream(&cursor, false, nodes, XC_PARSER_MAX_AST_NODES, &out_ast->node_count);
	if (status != XC_STATUS_OK) {
		return status;
	}

	return XC_STATUS_OK;
}
