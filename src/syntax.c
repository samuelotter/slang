#include <syntax.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <atom.h>
#include <tuple.h>

typedef enum {
  STATE_START,
  STATE_ATOM,
  STATE_COMMENT,
  STATE_NUMBER,
  STATE_OPERATOR,
  STATE_STRING,
  STATE_VARIABLE,
  STATE_MAX
} TokenizerState;

typedef enum {
  TOKEN_TYPE_ATOM,
  TOKEN_TYPE_COMMENT,
  TOKEN_TYPE_NUMBER,
  TOKEN_TYPE_OPERATOR,
  TOKEN_TYPE_STRING,
  TOKEN_TYPE_VARIABLE,
  TOKEN_TYPE_PAREN,
  TOKEN_TYPE_SEMICOLON,
  TOKEN_TYPE_MAX
} TokenType;

reftype(Token, struct {
    Binary    *symbol;
    TokenType  type;
  });

typedef struct TokenizerStateContext {
  char       *buffer;
  char       *token_start;
  List       *head;
  Scope      *scope;
} TokenizerStateContext;

typedef int  TokenizerStateCondition(int symbol);
typedef void TokenizerStateAction(TokenizerStateContext *data);

typedef struct TokenizerStateEdge {
  TokenizerState                  target;
  TokenizerStateCondition * const condition;
  TokenizerStateAction    * const action;
} TokenizerStateEdge;

static int is_lowercase(int symbol);
static int is_uppercase(int symbol);
static int is_underscore(int symbol);
static int is_hash(int symbol);
static int is_operator(int symbol);
static int is_whitespace(int symbol);
static int is_alphanumeric(int symbol);
static int is_newline(int symbol);
static int is_semicolon(int symbol);
static int is_paren(int symbol);
static int is_doublequote(int symbol);
static int is_digit(int symbol);

static void start_token(TokenizerStateContext *);
static void add_token(TokenizerStateContext *,
               const char *buffer,
               size_t length,
               TokenType type);
static void next_symbol(TokenizerStateContext *);
static void report_error(TokenizerStateContext *);

static void make_atom(TokenizerStateContext *context);
static void make_variable(TokenizerStateContext *context);
static void make_operator(TokenizerStateContext *context);
static void make_comment(TokenizerStateContext *context);
static void make_string(TokenizerStateContext *context);
static void make_number(TokenizerStateContext *context);
static void handle_paren(TokenizerStateContext *context);
static void handle_semicolon(TokenizerStateContext *context);

TokenizerStateEdge state_start_edges[] = {
  { STATE_ATOM,      is_lowercase,     start_token },
  { STATE_VARIABLE,  is_uppercase,     start_token },
  { STATE_VARIABLE,  is_underscore,    start_token },
  { STATE_COMMENT,   is_hash,          start_token },
  { STATE_OPERATOR,  is_operator,      start_token },
  { STATE_STRING,    is_doublequote,   start_token },
  { STATE_START,     is_semicolon,     handle_semicolon },
  { STATE_NUMBER,    is_digit,         start_token },
  { STATE_START,     is_paren,         handle_paren },
  { STATE_START,     is_whitespace,    next_symbol },
  { STATE_START,     NULL,             report_error }
};

TokenizerStateEdge state_atom_edges[] = {
  { STATE_ATOM,     is_alphanumeric,  next_symbol },
  { STATE_START,    NULL,             make_atom },
};

TokenizerStateEdge state_variable_edges[] = {
  { STATE_VARIABLE, is_alphanumeric,  next_symbol },
  { STATE_START,    NULL,             make_variable }
};

TokenizerStateEdge state_operator_edges[] = {
  { STATE_OPERATOR, is_operator,      next_symbol },
  { STATE_START,    NULL,             make_operator }
};

TokenizerStateEdge state_comment_edges[] = {
  { STATE_START,    is_newline,       make_comment },
  { STATE_COMMENT,  NULL,             next_symbol }
};

TokenizerStateEdge state_string_edges[] = {
  { STATE_START,    is_doublequote,   make_string },
  { STATE_STRING,   NULL,             next_symbol }
};

TokenizerStateEdge state_number_edges[] = {
  { STATE_NUMBER,   is_digit,         next_symbol },
  { STATE_START,    NULL,             make_number }
};

typedef struct TokenizerStateTableEntry {
  size_t              size;
  TokenizerStateEdge *edges;
} TokenizerStateTableEntry;

TokenizerStateTableEntry state_table[] = {
  { sizeof(state_start_edges) / sizeof(TokenizerStateEdge),
    state_start_edges
  },
  { sizeof(state_atom_edges) / sizeof(TokenizerStateEdge),
    state_atom_edges
  },
  { sizeof(state_comment_edges) / sizeof(TokenizerStateEdge),
    state_comment_edges
  },
  { sizeof(state_number_edges) / sizeof(TokenizerStateEdge),
    state_number_edges
  },
  { sizeof(state_operator_edges) / sizeof(TokenizerStateEdge),
    state_operator_edges
  },
  { sizeof(state_string_edges) / sizeof(TokenizerStateEdge),
    state_string_edges
  },
  { sizeof(state_variable_edges) / sizeof(TokenizerStateEdge),
    state_variable_edges
  }
};

int is_lowercase(int symbol) {
  return symbol >= 'a' && symbol <= 'z';
}

int is_uppercase(int symbol) {
  return symbol >= 'A' && symbol <= 'Z';
}

int is_underscore(int symbol) {
  return symbol == '_';
}

int is_hash(int symbol) {
  return symbol == '#';
}

int is_operator(int symbol) {
  switch (symbol) {
  case '+':
  case '-':
  case '/':
  case '*':
  case '=':
  case ':':
  case '?':
  case '!':
  case '@':
  case '$':
  case '%':
  case '\\':
  case '|':
  case '^':
  case '&':
  case '>':
  case '<':
  case ',':
    return 1;
  default:
    return 0;
  }
}

int is_whitespace(int symbol) {
  return symbol == ' ' || symbol == '\n' || symbol == '\r' || symbol == '\t';
}

int is_alphanumeric(int symbol) {
  return is_lowercase(symbol) ||
    is_uppercase(symbol) ||
    is_underscore(symbol) ||
    is_digit(symbol);
}

int is_newline(int symbol) {
  return symbol == '\n';
}

int is_semicolon(int symbol) {
  return symbol == ';';
}

int is_paren(int symbol) {
  return symbol == '(' || symbol == ')' || symbol == '{' || symbol == '}';
}

int is_doublequote(int symbol) {
  return symbol == '"';
}

int is_digit(int symbol) {
  return symbol >= '0' && symbol <= '9';
}

void start_token(TokenizerStateContext *context) {
  context->token_start = context->buffer;
}

void next_symbol(TokenizerStateContext *context) {
  context->buffer++;
}

void report_error(TokenizerStateContext *context) {
  // TODO: Add real error handling
  printf("Unexpected character %.20s...\n", context->buffer);
  exit(1);
}

void add_token(TokenizerStateContext *context,
               const char *buffer,
               size_t length,
               TokenType type) {
#if 0
  printf("context->buffer '%.1s', context->token_start: '%.20s', length: %zd\n",
         context->buffer,
         context->token_start,
         length);
#endif
  assert(length > 0);
  Binary *symbol       = binary(context->scope,
                                (uint8_t*)buffer,
                                length);
  Token *token         = (Token*)scope_alloc_ref(context->scope, sizeof(Token),
                                         TYPEID_RECORD, 0).ptr;
  token->symbol        = symbol;
  token->type          = type;
  context->head        = list_cons(context->head, (Ref)(void*)token);
  context->token_start = context->buffer;
}

void make_atom(TokenizerStateContext *context) {
  add_token(context,
            context->token_start,
            context->buffer - context->token_start,
            TOKEN_TYPE_ATOM);
}

void make_variable(TokenizerStateContext *context) {
  add_token(context,
            context->token_start,
            context->buffer - context->token_start,
            TOKEN_TYPE_VARIABLE);
}

void make_operator(TokenizerStateContext *context) {
  size_t length = context->buffer - context->token_start;
  add_token(context,
            context->token_start,
            length,
            TOKEN_TYPE_OPERATOR);
}

void make_comment(TokenizerStateContext *context) {
  add_token(context,
            context->token_start,
            context->buffer - context->token_start,
            TOKEN_TYPE_COMMENT);
}

void make_string(TokenizerStateContext *context) {
  add_token(context,
            context->token_start,
            context->buffer  - context->token_start,
            TOKEN_TYPE_STRING);
}

void make_number(TokenizerStateContext *context) {
  add_token(context,
            context->token_start,
            context->buffer - context->token_start,
            TOKEN_TYPE_NUMBER);
}

void handle_paren(TokenizerStateContext *context) {
  add_token(context,
            context->buffer,
            1,
            TOKEN_TYPE_PAREN);
  next_symbol(context);
}

void handle_semicolon(TokenizerStateContext *context) {
  add_token(context,
            context->buffer,
            1,
            TOKEN_TYPE_SEMICOLON);
  next_symbol(context);
}

List *syntax_tokenize(Binary *binary) {
  List *head = list_new(scopeof(binary));
  TokenizerStateContext context = {
    (char*)binary->data,
    (char*)binary->data,
    head,
    scopeof(head)
  };
  TokenizerState state = STATE_START;
  while (context.buffer < (char *)binary->data + binary->size) {
    TokenizerStateEdge *edges = state_table[state].edges;
#if 0
    printf("state: %d, *buffer: %.1s, start: %.1s, edges: %zd\n",
           state,
           context.buffer,
           context.token_start,
           state_table[state].size);
#endif
    for (size_t j = 0; j < state_table[state].size; j++) {
      if (edges[j].condition == NULL || edges[j].condition(*context.buffer)) {
        if (edges[j].action != NULL) {
          edges[j].action(&context);
        }
        state = edges[j].target;
        break;
      }
    }
  }
  return list_reverse(context.head);
}

/* Parser *********************************************************************/

typedef struct ParserContext {
  List  *tokens;
  Scope *scope;
} ParserContext;

typedef Ref InfixParser(Ref left, ParserContext *context);

typedef struct InfixRule {
  int          lbp;
  const char  *symbol;
  int          type;
  InfixParser *parse_fun;
} InfixRule;

typedef Ref PrefixParser(ParserContext *context);

typedef struct PrefixRule {
  const char   *symbol;
  int           type;
  PrefixParser *parse_fun;
} PrefixRule;

Token *next_token(ParserContext *context) {
  if (list_is_empty(context->tokens->next)) {
    return NULL;
  }
  return (Token *)context->tokens->next->value.ptr;
}

Token *current_token(ParserContext *context) {
  return context->tokens != NULL
    ? (Token *)context->tokens->value.ptr
    : NULL;
}

Token *consume_token(ParserContext *context) {
  if (list_is_empty(context->tokens->next)) {
    context->tokens = NULL;
    return NULL;
  }
  Token *token    = current_token(context);
  context->tokens = context->tokens->next;
#if 0
  printf("consumed: '%s', current_token -> %.2d '%s'\n",
         token->symbol->data,
         ((Token *)context->tokens->value.ptr)->type,
         ((Token *)context->tokens->value.ptr)->symbol->data);
#endif
  return token;
}

Ref parse_sequence(ParserContext *context, PrefixParser *parser) {
  List *head = list_new(context->scope);
  Ref expr = parser(context);
  while (expr.ptr != NULL) {
    head = list_cons(head, expr);
    expr = parser(context);
  }
  return (Ref)list_reverse(head);
}

Ref expression(ParserContext *context, int rbp);

Ref expression_parser(ParserContext *context) {
  return expression(context, 0);
}

Ref list_parser(ParserContext *context) {
  List *lists = list_new(context->scope);
  List *head  = list_new(context->scope);
  Token *current = current_token(context);
  if (current != NULL && current->symbol->data[0] == '(') {
    consume_token(context);
    current = current_token(context);
  }
  while (current != NULL) {
    if (current->symbol->data[0] == ')') {
      current = consume_token(context);
      break;
    }
    if (current->symbol->data[0] == ';') {
      current = consume_token(context);
      lists = list_cons(lists, (Ref)list_reverse(head));
      head  = list_new(context->scope);
    }
    if (current == NULL) {
      break;
    }
    Ref expr = expression(context, 0);
    head = list_cons(head, expr);
    current = current_token(context);
  }
  if (!list_is_empty(lists)) {
    lists = list_cons(lists, (Ref)list_reverse(head));
    head  = lists;
  }
  return (Ref)list_reverse(head);
}

Ref atom_parser(ParserContext *context) {
  Token *token = consume_token(context);
  return (Ref)atom((char *)token->symbol->data);
}

Ref number_parser(ParserContext *context) {
  Token *token = consume_token(context);
  // TODO: Handle conversion..
  return (Ref)token->symbol;
}

Ref comment_parser(ParserContext *context) {
  Token *token  = consume_token(context);
  List *comment = list_new(context->scope);
  comment       = list_cons(comment, (Ref)token->symbol);
  comment       = list_cons(comment, (Ref)atom("#"));
  return (Ref)comment;
}

Ref struct_parser(ParserContext *context) {
  consume_token(context);
  List *struct_expr = list_new(context->scope);
  Ref exprs         = parse_sequence(context, expression_parser);
  consume_token(context); // TODO proper error handling, hopefully eats '}'
  struct_expr       = list_cons(struct_expr, exprs);
  struct_expr       = list_cons(struct_expr, (Ref)atom("struct"));
  return (Ref)struct_expr;
}

Ref infix_operator_parser(Ref left, ParserContext *context) {
  Token *token = consume_token(context);
  List *op     = list_new(context->scope);
  Ref right    = expression(context, 0); //TODO handle binding power..
  op           = list_cons(op, right);
  op           = list_cons(op, left);
  op           = list_cons(op, (Ref)atom_from_binary(token->symbol));
  return (Ref)op;
}

static PrefixRule prefix_rules[] = {
  { "(",  -1,                  list_parser },
  { "{",  -1,                  struct_parser },
  { NULL, TOKEN_TYPE_ATOM,     atom_parser },
  { NULL, TOKEN_TYPE_COMMENT,  comment_parser },
  { NULL, TOKEN_TYPE_NUMBER,   number_parser },
  { NULL, TOKEN_TYPE_VARIABLE, atom_parser }
};

static InfixRule infix_rules[] = {
  { 10, "/",  TOKEN_TYPE_OPERATOR, infix_operator_parser },
  { 10, NULL, TOKEN_TYPE_OPERATOR, infix_operator_parser },
};

InfixRule* find_infix_rule(Token *token) {
  Binary *symbol = token->symbol;
  for (size_t i = 0; i < sizeof(infix_rules) / sizeof(InfixRule); i++) {
    if (infix_rules[i].symbol != NULL) {
      size_t symbol_len = strlen(infix_rules[i].symbol);
      size_t len        = symbol_len < symbol->size ? symbol_len : symbol->size;
      if (strncmp(infix_rules[i].symbol, (char *)symbol->data, len) == 0) {
        return &infix_rules[i];
      }
    } else {
      if (infix_rules[i].type == token->type) {
        return &infix_rules[i];
      }
    }
  }
  return NULL;
}

PrefixRule* find_prefix_rule(Token *token) {
  Binary *symbol = token->symbol;
  for (size_t i = 0; i < sizeof(prefix_rules) / sizeof(PrefixRule); i++) {
    if (prefix_rules[i].symbol != NULL) {
      size_t symbol_len = strlen(prefix_rules[i].symbol);
      size_t len        = symbol_len < symbol->size ? symbol_len : symbol->size;
      if (strncmp(prefix_rules[i].symbol, (char *)symbol->data, len) == 0) {
        return &prefix_rules[i];
      }
    } else {
      if (prefix_rules[i].type == token->type) {
        return &prefix_rules[i];
      }
    }
  }
  return NULL;
}

Ref expression(ParserContext *context, int rbp) {
  Token *current          = current_token(context);
  if (current == NULL) {
    printf("Unexpected end of token list\n");
    exit(1);
  }
  PrefixRule *prefix_rule = find_prefix_rule(current);
  if (prefix_rule == NULL) {
    return (Ref)NULL;
  }

  Ref left = prefix_rule->parse_fun(context);

  Token *next = current_token(context);
  while (next != NULL) {
    InfixRule *infix_rule = find_infix_rule(next);
    if (infix_rule != NULL && rbp < infix_rule->lbp) {
      left = infix_rule->parse_fun(left, context);
    } else {
      break;
    }
    next = current_token(context);
  }
  return left;
}

Ref syntax_parse(List *tokens) {
  ParserContext context = {
    .tokens = tokens,
    .scope  = scopeof(tokens)
  };
  return list_parser(&context); //parse_sequence(&context, list_parser);
}
