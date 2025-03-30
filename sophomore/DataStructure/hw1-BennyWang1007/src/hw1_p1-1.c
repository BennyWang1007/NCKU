#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Stack {
    int top;
    int capacity;
    char *array;

} Stack;

void stack_resize(Stack *stack)
{
    stack->capacity *= 2;
    stack->array =
        (char *) realloc(stack->array, stack->capacity * sizeof(char));
}

void stack_push(Stack *stack, char data)
{
    if (stack->top == stack->capacity - 1) {
        stack_resize(stack);
    }
    stack->array[++stack->top] = data;
    return;
}

char stack_pop(Stack *stack)
{
    if (stack->top == -1) {
        printf("Stack is empty\n");
        return -1;
    }
    int temp = stack->array[stack->top];
    stack->top--;
    return temp;
}

void init_stack(Stack *stack, int capacity)
{
    stack->top = -1;
    stack->capacity = capacity;
    stack->array = (char *) malloc(capacity * sizeof(char));
    return;
}

void stack_destroy(Stack *stack)
{
    free(stack->array);
    free(stack);
    return;
}

bool isOperator(char c)
{
    return (c == '+' || c == '-' || c == '*' || c == '/');
}

int precedence(char c)
{
    if (c == '+' || c == '-')
        return 1;
    else if (c == '*' || c == '/')
        return 2;
    else
        return 0;
}

void infix_to_postfix(char *str, char *buffer, size_t str_len)
{
    Stack *stack = malloc(sizeof(Stack));
    init_stack(stack, 20);

    size_t buffer_index = 0, left_bracket_count = 0;

    for (size_t i = 0; i < str_len; ++i) {
        if (str[i] == ' ')
            continue;
        if (str[i] == '(') {
            stack_push(stack, str[i]);
            continue;
        }
        if (str[i] == ')') {
            while (stack->top != -1 && stack->array[stack->top] != '(') {
                buffer[buffer_index++] = stack_pop(stack);
            }
            stack_pop(stack);
            continue;
        }
        if (isOperator(str[i])) {
            if (stack->top == -1 ||
                precedence(str[i]) > precedence(stack->array[stack->top])) {
                stack_push(stack, str[i]);
            } else {
                while (stack->top != -1 &&
                       precedence(str[i]) <=
                           precedence(stack->array[stack->top])) {
                    buffer[buffer_index++] = stack_pop(stack);
                }
                stack_push(stack, str[i]);
            }
        } else {
            buffer[buffer_index++] = str[i];
        }
    }
    while (stack->top != -1) {
        buffer[buffer_index++] = stack_pop(stack);
    }

    return;
}

int main(int argc, char *argv[])
{
    Stack *stack = malloc(sizeof(Stack));
    size_t MAX_LEN = 9999, str_len;
    init_stack(stack, 20);
    char *str = malloc(sizeof(char) * MAX_LEN);
    char *ans = malloc(sizeof(char) * MAX_LEN);
    scanf("%s", str);
    str_len = strlen(str);
    infix_to_postfix(str, ans, str_len);
    printf("%s", ans);
    return 0;
}
