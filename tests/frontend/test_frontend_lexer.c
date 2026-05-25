#include "xcompiler/frontend/frontend.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

typedef struct expected_token {
	xc_token_kind_t kind;
	const char*     text;
} expected_token_t;

static void expect_token(const xc_token_t* token, const expected_token_t* expected) {
	assert(token->kind == expected->kind);
	assert(token->lexeme != NULL);
	assert(token->lexeme_length == strlen(expected->text));
	assert(memcmp(token->lexeme, expected->text, token->lexeme_length) == 0);
}

int main(void) {
	const char* source = "fun main() {\n"
	                     "case value -> result end\n"
	                     "// line comment\n"
	                     "/* block comment */\n"
	                     "ident 123 0x1f 0b1010 0o755 3.14e-2 \"str\\n\" $/ab\\/c/\n"
	                     "+= -= *= /= %= .= ..= __ _= .. ... := :: |> .? !.? ?? !?? %% !% == != >= <= =~ !~ && || ,. , "
	                     ". ; : ( ) [ ] { } @ # $ ^ ~ < > ?\n";

	xc_lexer_t lexer;
	xc_lexer_init(&lexer, source);
	assert(xc_lexer_is_ready(&lexer));

	const expected_token_t expected[] = {
		{ XC_TOKEN_KEYWORD,    "fun"                 },
		{ XC_TOKEN_IDENTIFIER, "main"                },
		{ XC_TOKEN_SYMBOL,     "("                   },
		{ XC_TOKEN_SYMBOL,     ")"                   },
		{ XC_TOKEN_SYMBOL,     "{"                   },
		{ XC_TOKEN_KEYWORD,    "case"                },
		{ XC_TOKEN_IDENTIFIER, "value"               },
		{ XC_TOKEN_SYMBOL,     "->"                  },
		{ XC_TOKEN_IDENTIFIER, "result"              },
		{ XC_TOKEN_KEYWORD,    "end"                 },
		{ XC_TOKEN_COMMENT,    "// line comment"     },
		{ XC_TOKEN_COMMENT,    "/* block comment */" },
		{ XC_TOKEN_IDENTIFIER, "ident"               },
		{ XC_TOKEN_NUMBER,     "123"                 },
		{ XC_TOKEN_NUMBER,     "0x1f"                },
		{ XC_TOKEN_NUMBER,     "0b1010"              },
		{ XC_TOKEN_NUMBER,     "0o755"               },
		{ XC_TOKEN_NUMBER,     "3.14e-2"             },
		{ XC_TOKEN_STRING,     "\"str\\n\""          },
		{ XC_TOKEN_REGEX,      "$/ab\\/c/"           },
		{ XC_TOKEN_SYMBOL,     "+="                  },
		{ XC_TOKEN_SYMBOL,     "-="                  },
		{ XC_TOKEN_SYMBOL,     "*="                  },
		{ XC_TOKEN_SYMBOL,     "/="                  },
		{ XC_TOKEN_SYMBOL,     "%="                  },
		{ XC_TOKEN_SYMBOL,     ".="                  },
		{ XC_TOKEN_SYMBOL,     "..="                 },
		{ XC_TOKEN_SYMBOL,     "__"                  },
		{ XC_TOKEN_SYMBOL,     "_="                  },
		{ XC_TOKEN_SYMBOL,     ".."                  },
		{ XC_TOKEN_SYMBOL,     "..."                 },
		{ XC_TOKEN_SYMBOL,     ":="                  },
		{ XC_TOKEN_SYMBOL,     "::"                  },
		{ XC_TOKEN_SYMBOL,     "|>"                  },
		{ XC_TOKEN_SYMBOL,     ".?"                  },
		{ XC_TOKEN_SYMBOL,     "!.?"                 },
		{ XC_TOKEN_SYMBOL,     "??"                  },
		{ XC_TOKEN_SYMBOL,     "!??"                 },
		{ XC_TOKEN_SYMBOL,     "%%"                  },
		{ XC_TOKEN_SYMBOL,     "!%"                  },
		{ XC_TOKEN_SYMBOL,     "=="                  },
		{ XC_TOKEN_SYMBOL,     "!="                  },
		{ XC_TOKEN_SYMBOL,     ">="                  },
		{ XC_TOKEN_SYMBOL,     "<="                  },
		{ XC_TOKEN_SYMBOL,     "=~"                  },
		{ XC_TOKEN_SYMBOL,     "!~"                  },
		{ XC_TOKEN_SYMBOL,     "&&"                  },
		{ XC_TOKEN_SYMBOL,     "||"                  },
		{ XC_TOKEN_SYMBOL,     ",."                  },
		{ XC_TOKEN_SYMBOL,     ","                   },
		{ XC_TOKEN_SYMBOL,     "."                   },
		{ XC_TOKEN_SYMBOL,     ";"                   },
		{ XC_TOKEN_SYMBOL,     ":"                   },
		{ XC_TOKEN_SYMBOL,     "("                   },
		{ XC_TOKEN_SYMBOL,     ")"                   },
		{ XC_TOKEN_SYMBOL,     "["                   },
		{ XC_TOKEN_SYMBOL,     "]"                   },
		{ XC_TOKEN_SYMBOL,     "{"                   },
		{ XC_TOKEN_SYMBOL,     "}"                   },
		{ XC_TOKEN_SYMBOL,     "@"                   },
		{ XC_TOKEN_SYMBOL,     "#"                   },
		{ XC_TOKEN_SYMBOL,     "$"                   },
		{ XC_TOKEN_SYMBOL,     "^"                   },
		{ XC_TOKEN_SYMBOL,     "~"                   },
		{ XC_TOKEN_SYMBOL,     "<"                   },
		{ XC_TOKEN_SYMBOL,     ">"                   },
		{ XC_TOKEN_SYMBOL,     "?"                   },
	};

	for (size_t i = 0; i < sizeof(expected) / sizeof(expected[0]); ++i) {
		xc_token_t       token;
		xc_status_code_t status = xc_lexer_next_token(&lexer, &token);
		assert(status == XC_STATUS_OK);
		expect_token(&token, &expected[i]);
	}

	xc_token_t eof_token;
	assert(xc_lexer_next_token(&lexer, &eof_token) == XC_STATUS_NOT_READY);
	assert(eof_token.kind == XC_TOKEN_EOF);
	assert(eof_token.lexeme_length == 0);

	xc_lexer_reset(&lexer);
	assert(xc_lexer_next_token(&lexer, &eof_token) == XC_STATUS_OK);
	assert(eof_token.kind == XC_TOKEN_KEYWORD);
	assert(memcmp(eof_token.lexeme, "fun", eof_token.lexeme_length) == 0);

	return 0;
}
