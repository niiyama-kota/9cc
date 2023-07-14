#include "9cc.h"

Node *code[100];
LVar *locals;

Node *new_node(NodeKind kind, int children_num, ...)
{
    va_list ap;
    va_start(ap, children_num);
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->children = calloc(children_num, sizeof(Node));
    for (int i = 0; i < children_num; i++)
    {
        node->children[i] = va_arg(ap, Node *);
    }
    return node;
}

Node *new_node_num(int val)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

void *program()
{
    int i = 0;
    while (!at_eof())
    {
        code[i++] = stmt();
    }
    code[i] = NULL;
}

Node *stmt()
{
    if (consume("return"))
    {
        Node *node;
        node = calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        node->children = calloc(1, sizeof(Node));
        node->children[0] = expr();
        expect(";");
        return node;
    }
    else if (consume("if"))
    {
        if (consume("("))
        {
            Node *node;
            node = calloc(1, sizeof(Node));
            node->kind = ND_IF;
            node->children = calloc(3, sizeof(Node));
            node->children[0] = expr();
            expect(")");
            node->children[1] = stmt();
            if (consume("else"))
            {
                node->kind = ND_IFELSE;
                node->children[2] = stmt();
            }
            return node;
        }
        error("expect '('\n");
    }
    else if (consume("while"))
    {
        Node *node;
        node = calloc(1, sizeof(Node));
        node->kind = ND_WHILE;
        node->children = calloc(2, sizeof(Node));
        if (consume("("))
        {
            node->children[0] = expr();
            expect(")");
            node->children[1] = stmt();
            return node;
        }
        error("expect '('\n");
    }
    else if (consume("for"))
    {
        Node *node = new_node(ND_FOR, 4, NULL, NULL, NULL, NULL);
        if (consume("("))
        {
            if (!consume(";"))
            {
                node->children[0] = expr();
                expect(";");
            }
            if (!consume(";"))
            {
                node->children[1] = expr();
                expect(";");
            }
            if (!consume(")"))
            {
                node->children[2] = expr();
                expect(")");
            }
            node->children[3] = stmt();

            return node;
        }
        error("expect '('\n");
    }
    else
    {
        Node *node = expr();
        expect(";");
        return node;
    }
}

Node *expr()
{
    return assign();
}

Node *assign()
{
    Node *node = equality();
    if (consume("="))
    {
        // node->children[1] = assign();
        node = new_node(ND_ASSIGN, 2, node, assign());
        return node;
    }
    return node;
}

Node *equality()
{
    Node *node = relational();

    for (;;)
    {
        if (consume("=="))
        {
            node = new_node(ND_EQ, 2, node, relational());
        }
        else if (consume("!="))
        {
            node = new_node(ND_NEQ, 2, node, relational());
        }
        return node;
    }
}

Node *relational()
{
    Node *node = add();
    for (;;)
    {
        if (consume("<"))
        {
            node = new_node(ND_ST, 2, node, add());
        }
        else if (consume("<="))
        {
            node = new_node(ND_SEQ, 2, node, add());
        }
        else if (consume(">"))
        {
            node = new_node(ND_ST, 2, add(), node);
        }
        else if (consume(">="))
        {
            node = new_node(ND_SEQ, 2, add(), node);
        }
        else
        {
            return node;
        }
    }
}

Node *add()
{
    Node *node = mul();
    for (;;)
    {
        if (consume("+"))
        {
            node = new_node(ND_ADD, 2, node, mul());
        }
        else if (consume("-"))
        {
            node = new_node(ND_SUB, 2, node, mul());
        }
        else
        {
            return node;
        }
    }
}

Node *mul()
{
    Node *node = unary();

    for (;;)
    {
        if (consume("*"))
        {
            node = new_node(ND_MUL, 2, node, unary());
        }
        else if (consume("/"))
        {
            node = new_node(ND_DIV, 2, node, unary());
        }
        else
        {
            return node;
        }
    }
}

Node *unary()
{
    if (consume("+"))
    {
        return primary();
    }
    if (consume("-"))
    {
        Node *node = new_node(ND_SUB, 2, new_node_num(0), primary());
        return node;
    }
    else
    {
        return primary();
    }
}

Node *primary()
{
    if (consume("("))
    {
        Node *node = expr();
        expect(")");
        return node;
    }

    Token *tok = consume_ident();
    if (tok)
    {
        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_LVAR;

        LVar *lvar = find_lvar(tok);
        if (lvar)
        {
            node->offset = lvar->offset;
        }
        else
        {
            lvar = calloc(1, sizeof(LVar));
            lvar->next = locals;
            lvar->name = tok->str;
            lvar->len = tok->len;
            lvar->offset = ((locals) ? locals->offset : 0) + 8;
            node->offset = lvar->offset;
            locals = lvar;
        }
        return node;
    }

    return new_node_num(expect_number());
}
