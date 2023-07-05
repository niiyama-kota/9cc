#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
    TK_RESERVED, // symbol
    TK_IDENT,    // identifier
    TK_NUM,      // integer token
    TK_EOF,      // end of file token
} TokenKind;

typedef struct Token Token;
struct Token
{
    TokenKind kind; // type of token
    Token *next;    // token to input next
    int val;        // integer if kind is TOK_NUM
    char *str;      // string of token
    int len;        // length of token
};

typedef enum
{
    ND_ADD,    // +
    ND_SUB,    // -
    ND_MUL,    // *
    ND_DIV,    // /
    ND_NUM,    // integer
    ND_EQ,     // ==
    ND_NEQ,    // !=
    ND_ST,     // <
    ND_SEQ,    // <=
    ND_ASSIGN, // =
    ND_LVAR,   // local variable
    ND_STMT,   // statement
} NodeKind;

typedef struct Node Node;
struct Node
{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;    // use if kind is ND_NUM
    int offset; // use if kind is ND_LVAR
};

typedef struct LVar LVar;
struct LVar
{
    LVar *next;
    char *name;
    int len;
    int offset;
};

extern char *user_input;
extern Token *token;
extern LVar *locals;

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
Token *consume_ident();
void expect(char *op);
int expect_number();
bool at_eof();
Token *tokenize();
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
void *program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();
extern Node *code[100];

void gen(Node *node);

LVar *find_lvar(Token *tok);