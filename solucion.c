#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int aplicar_operador(int a, int b, char op) {
    switch(op) {
        case '+':
            return a + b;
        case '-':
            return a - b;
        case '*':
            return a * b;
        case '/':
            return a / b;
    }
    return 0;
}

int evaluar_posfijo(const char *expresion) {
    int pila[100];
    int tope = -1;

    while (*expresion) {

        if (isspace(*expresion)) {
            expresion++;
            continue;
        }

        if (isdigit(*expresion)) {

            int numero = 0;

            while (isdigit(*expresion)) {
                numero = numero * 10 + (*expresion - '0');
                expresion++;
            }

            pila[++tope] = numero;
        }
        else {

            int b = pila[tope--];
            int a = pila[tope--];

            int resultado = aplicar_operador(a, b, *expresion);

            pila[++tope] = resultado;

            expresion++;
        }
    }

    return pila[tope];
}

#ifndef MODO_TEST
int main(void) {

    char expresion[256];

    printf("Expresion postfix: ");
    fgets(expresion, sizeof(expresion), stdin);

    printf("Resultado: %d\n", evaluar_posfijo(expresion));

    return 0;
}
#endif