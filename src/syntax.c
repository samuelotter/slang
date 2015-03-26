#include <syntax.h>

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

int is_lowercase(int symbol);
int is_uppercase(int symbol);
int is_underscore(int symbol);
int is_hash(int symbol);
int is_operator(int symbol);
int is_whitespace(int symbol);
int is_alphanumeric(int symbol);
int is_newline(int symbol);
int is_semicolon(int symbol);
int is_paren(int symbol);
int is_doublequote(int symbol);
int is_digit(int symbol);

void start_token(TokenizerStateContext *);
void add_token(TokenizerStateContext *, TokenType type);

void make_atom(TokenizerStateContext *context);
void make_variable(TokenizerStateContext *context);
void make_operator(TokenizerStateContext *context);
void make_comment(TokenizerStateContext *context);
void make_string(TokenizerStateContext *context);
void make_number(TokenizerStateContext *context);

TokenizerStateEdge state_start_edges[] = {
  { STATE_ATOM,     is_lowercase,     start_token },
  { STATE_VARIABLE, is_uppercase,     start_token },
  { STATE_VARIABLE, is_underscore,    start_token },
  { STATE_COMMENT,  is_hash,          start_token },
  { STATE_OPERATOR, is_operator,      start_token },
  { STATE_STRING,   is_doublequote,   start_token },
  { STATE_START,    is_semicolon,     make_operator },
  { STATE_NUMBER,   is_digit,         start_token },
  { STATE_START,    is_paren,         make_operator },
  { STATE_START,    is_whitespace,    NULL },
};

TokenizerStateEdge state_atom_edges[] = {
  { STATE_ATOM,     is_alphanumeric,  NULL },
  { STATE_START,    NULL,             make_atom },
};

TokenizerStateEdge state_variable_edges[] = {
  { STATE_VARIABLE, is_alphanumeric,  NULL },
  { STATE_START,    NULL,             make_variable }
};

TokenizerStateEdge state_operator_edges[] = {
  { STATE_OPERATOR, is_operator,      NULL },
  { STATE_START,    NULL,             make_operator }
};

TokenizerStateEdge state_comment_edges[] = {
  { STATE_START,    is_newline,       make_comment },
  { STATE_COMMENT,  NULL,             NULL }
};

TokenizerStateEdge state_string_edges[] = {
  { STATE_START,    is_doublequote,   make_string },
  { STATE_STRING,   NULL,             NULL }
};

TokenizerStateEdge state_number_edges[] = {
  { STATE_NUMBER,   is_digit,         NULL },
  { STATE_STRING,   NULL,             make_number }
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
  },
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

void add_token(TokenizerStateContext *context, TokenType type) {
  Binary *symbol       = binary(context->scope,
                                (uint8_t*)context->token_start,
                                context->buffer - context->token_start - 1);
  Token *token         = (Token*)scope_alloc_ref(context->scope, sizeof(Token),
                                         TYPEID_RECORD, 0).ptr;
  token->symbol        = symbol;
  token->type          = type;
  context->head        = list_cons(context->head, (Ref)(void*)token);
  context->token_start = context->buffer;
}

void make_atom(TokenizerStateContext *context) {
  add_token(context, TOKEN_TYPE_ATOM);
}

void make_variable(TokenizerStateContext *context) {
  add_token(context, TOKEN_TYPE_VARIABLE);
}

void make_operator(TokenizerStateContext *context) {
  add_token(context, TOKEN_TYPE_OPERATOR);
}

void make_comment(TokenizerStateContext *context) {
  add_token(context, TOKEN_TYPE_COMMENT);
}

void make_string(TokenizerStateContext *context) {
  add_token(context, TOKEN_TYPE_STRING);
}

void make_number(TokenizerStateContext *context) {
  add_token(context, TOKEN_TYPE_NUMBER);
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
  for (size_t i = 0; i < binary->size; i++) {
    TokenizerStateEdge *edges = state_table[state].edges;
    /* printf("state: %d, *buffer: %.1s, edges: %zd\n", */
    /*        state, */
    /*        context.buffer, */
    /*        state_table[state].size); */
    for (size_t j = 0; j < state_table[state].size; j++) {
      if (edges[j].condition == NULL || edges[j].condition(*context.buffer)) {
        if (edges[j].action != NULL) {
          edges[j].action(&context);
        }
        state = edges[j].target;
        goto next;
      }
    }
    // TODO: Add real error handling
    printf("State: %d; Unexpected character %.20s...\n", state, context.buffer);
    exit(1);
  next:
    context.buffer++;
  }
  return list_reverse(context.head);
}

/* Parser *********************************************************************/

typedef List *InfixParser(List *left, List *tokens);

typedef struct InfixRule {
  int          lbp;
  const char  *symbol;
  InfixParser *parse_fun;
} InfixRule;

typedef List *PrefixParser(List *tokens);

typedef struct PrefixRule {
  const char   *symbol;
  PrefixParser *parse_fun;
} PrefixRule;

List *expression(List *tokens, int rbp);

List *list_parser(List *tokens) {
  List *head = list_new(scopeof(tokens));
  while (!list_is_empty(tokens)) {
    Token *current = (Token*)tokens->value.ptr;
    if (current->symbol->data[0] == ';') {
      break;
    }
    List *expr = expression(tokens, 0);
    list_cons(head, (Ref)expr);
    tokens = tokens->next;
  }
  return head;
}

List *comment_parser(List *tokens) {
  Token *token = (Token *)tokens->value.ptr;
  List *comment = list_new(scopeof(tokens));
  comment       = list_cons(comment, (Ref)token->symbol);
  comment       = list_cons(comment, (Ref)atom("#"));
  return comment;
}

static PrefixRule prefix_rules[] = {
  { "(", list_parser },
  { "#", comment_parser }
};

static InfixRule infix_rules[] = {
  { 0, "~", NULL }
};

InfixRule* find_infix_rule(Binary *symbol) {
  printf("find_infix_rule(\"%s\")\n", symbol->data);
  for (size_t i = 0; i < sizeof(infix_rules) / sizeof(InfixRule); i++) {
    if (strncmp(infix_rules[i].symbol, (char*)symbol->data, symbol->size) == 0) {
      return &infix_rules[i];
    }
  }
  return NULL;
}

PrefixRule* find_prefix_rule(Binary *symbol) {
  printf("find_prefix_rule(\"%s\")\n", symbol->data);
  for (size_t i = 0; i < sizeof(prefix_rules) / sizeof(PrefixRule); i++) {
    size_t symbol_len = strlen(prefix_rules[i].symbol);
    size_t len        = symbol_len < symbol->size ? symbol_len : symbol->size;
    if (strncmp(prefix_rules[i].symbol, (char *)symbol->data, len) == 0) {
      return &prefix_rules[i];
    }
  }
  return NULL;
}

List *expression(List *tokens, int rbp) {
  Token *current = (Token*)tokens->value.ptr;
  printf("expression: current = '%s'\n", current->symbol->data);
  PrefixRule *prefix_rule = find_prefix_rule(current->symbol);
  if (prefix_rule == NULL) {
    printf("No rule found for token %s\n", current->symbol->data);
    exit(1);
  }

  List *left = prefix_rule->parse_fun(tokens);

  while (!list_is_empty(tokens->next)) {
    tokens = tokens->next;
    Token *next = (Token*)tokens->value.ptr;
    InfixRule *infix_rule = find_infix_rule(next->symbol);
    if (infix_rule != NULL && rbp < infix_rule->lbp) {
      left = infix_rule->parse_fun(left, tokens);
    } else {
      break;
    }
  }
  return left;
}

List *syntax_parse(List *tokens) {
  return list_parser(tokens);
}
