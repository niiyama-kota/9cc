#include "9cc.h"

int labels = 0;

void gen_lval(Node *node)
{
    if (node->kind != ND_LVAR)
    {
        error("lvalue of substitution is not variable");
    }

    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n", node->offset);
    printf("    push rax\n");
}

void gen(Node *node)
{
    if (!node)
    {
        return;
    }
    switch (node->kind)
    {
    case ND_NUM:
        printf("    push %d\n", node->val);
        return;
    case ND_LVAR:
        gen_lval(node);
        printf("    pop rax\n");
        printf("    mov rax, [rax]\n");
        printf("    push rax\n");
        return;
    case ND_ASSIGN:
        gen_lval(node->children[0]);
        gen(node->children[1]);

        printf("    pop rdi\n");
        printf("    pop rax\n");
        printf("    mov [rax], rdi\n");
        printf("    push rdi\n");
        return;
    case ND_RETURN:
        gen(node->children[0]);
        printf("    pop rax\n");
        printf("    mov rsp, rbp\n");
        printf("    pop rbp\n");
        printf("    ret\n");
        return;
    case ND_IF:
        gen(node->children[0]);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je  .Lend%d\n", labels);
        gen(node->children[1]);
        printf(".Lend%d:\n", labels++);
        return;
    case ND_IFELSE:
        gen(node->children[0]);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je  .Lelse%d\n", labels);
        gen(node->children[1]);
        printf("    jmp .Lend%d\n", labels);
        printf(".Lelse%d:\n", labels);
        gen(node->children[2]);
        printf(".Lend%d:\n", labels++);
        return;
    case ND_WHILE:
        printf(".Lbegin%d:\n", labels);
        gen(node->children[0]);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je  .Lend%d\n", labels);
        gen(node->children[1]);
        printf("    jmp .Lbegin%d\n", labels);
        printf(".Lend%d:\n", labels++);
        return;
    }

    gen(node->children[0]);
    gen(node->children[1]);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node->kind)
    {
    case ND_ADD:
        printf("    add rax, rdi\n");
        break;
    case ND_SUB:
        printf("    sub rax, rdi\n");
        break;
    case ND_MUL:
        printf("    imul rax, rdi\n");
        break;
    case ND_DIV:
        printf("    cqo\n");
        printf("    idiv rax, rdi\n");
        break;
    case ND_EQ:
        printf("    cmp rax, rdi\n");
        printf("    sete al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_NEQ:
        printf("    cmp rax, rdi\n");
        printf("    setne al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_ST:
        printf("    cmp rax, rdi\n");
        printf("    setl al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_SEQ:
        printf("    cmp rax, rdi\n");
        printf("    setle al\n");
        printf("    movzb rax, al\n");
        break;
    }

    printf("    push rax\n");
}