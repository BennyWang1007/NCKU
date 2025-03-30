#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct Stack {
    int top;
    int capacity;
    char **array;

} Stack;

void stack_resize(Stack *stack)
{
    stack->capacity *= 2;
    stack->array =
        (char **) realloc(stack->array, stack->capacity * sizeof(char));
}

void stack_push(Stack *stack, char *str)
{
    if (stack->top == stack->capacity - 1) {
        stack_resize(stack);
    }
    stack->array[++stack->top] = str;
    return;
}

char *stack_pop(Stack *stack)
{
    if (stack->top == -1) {
        printf("Stack is empty\n");
        return (char *) (0);
    }
    char *temp = stack->array[stack->top];
    stack->top--;
    return temp;
}

void init_stack(Stack *stack, int capacity)
{
    stack->top = -1;
    stack->capacity = capacity;
    stack->array = (char **) malloc(capacity * sizeof(char));
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

void postfix_to_prefix(char *str, char *buffer, size_t str_len)
{
    Stack *stack = malloc(sizeof(Stack));
    init_stack(stack, 20);

    size_t buffer_idx = 0;

    for (size_t i = 0; i < str_len; ++i) {
        if (str[i] == ' ')
            continue;
        if (isOperator(str[i])) {
            char *temp = stack_pop(stack), *temp2 = stack_pop(stack);
            char *temp3 =
                calloc(strlen(temp) + strlen(temp2) + 2, sizeof(char));

            temp3[0] = str[i];
            strcat(temp3, temp2);
            strcat(temp3, temp);
            temp3[strlen(temp) + strlen(temp2) + 1] = '\0';

            stack_push(stack, temp3);
            free(temp);
            free(temp2);
        } else {
            char *temp = malloc(sizeof(char) * 2);
            temp[0] = str[i];
            temp[1] = '\0';
            stack_push(stack, temp);
        }
    }
    strcpy(buffer, stack_pop(stack));
    stack_destroy(stack);
    return;
}

int main(int argc, char *argv[])
{
    Stack *stack = malloc(sizeof(Stack));
    size_t MAX_LEN = 9999, str_len;
    init_stack(stack, 20);

    char *ans = malloc(sizeof(char) * MAX_LEN);
    char *str = malloc(sizeof(char) * MAX_LEN);

    scanf("%s", str);
    str_len = strlen(str);
    postfix_to_prefix(str, ans, str_len);
    printf("%s", ans);

    return 0;
}
