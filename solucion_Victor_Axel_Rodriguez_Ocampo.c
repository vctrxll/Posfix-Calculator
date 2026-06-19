#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

typedef struct TuNodo {
    char valor[16];
    struct TuNodo *izq;
    struct TuNodo *der;
} TuNodo;

static int igual(const char *a, const char *b) {
    int i = 0;

    if (a == NULL || b == NULL) {
        return 0;
    }

    while (a[i] != '\0' && b[i] != '\0') {
        if (tolower((unsigned char)a[i]) != tolower((unsigned char)b[i])) {
            return 0;
        }

        i++;
    }

    return a[i] == '\0' && b[i] == '\0';
}

static int es_binario(const char *token) {
    return igual(token, "and") || igual(token, "or");
}

static int es_unario(const char *token) {
    return igual(token, "not");
}

static TuNodo *crear_nodo(const char *valor) {
    TuNodo *nuevo = (TuNodo *) malloc(sizeof(TuNodo));

    if (nuevo == NULL) {
        return NULL;
    }

    strncpy(nuevo->valor, valor, 15);
    nuevo->valor[15] = '\0';

    nuevo->izq = NULL;
    nuevo->der = NULL;

    return nuevo;
}

static char *obtener_token(char **tokens, int i, int usar_matriz) {
    if (usar_matriz) {
        char (*matriz)[16] = (char (*)[16]) tokens;
        return matriz[i];
    }

    return tokens[i];
}

TuNodo *construir(char **tokens, int n) {
    TuNodo *pila[81];
    int tope = -1;
    int i;
    int usar_matriz = 0;

    if (tokens == NULL || n <= 0 || n > 81) {
        return NULL;
    }

    /*
        Si tokens realmente viene de char tokens[81][16],
        entonces tokens[0] no será una dirección válida.
        Por ejemplo, si el primer token es "a", tokens[0] parece 0x61.
    */
    if ((uintptr_t)tokens[0] < 0x100000000ULL) {
        usar_matriz = 1;
    }

    for (i = 0; i < n; i++) {
        char *token = obtener_token(tokens, i, usar_matriz);

        if (token == NULL) {
            return NULL;
        }

        if (es_binario(token)) {
            TuNodo *nuevo;
            TuNodo *izquierdo;
            TuNodo *derecho;

            if (tope < 1) {
                return NULL;
            }

            derecho = pila[tope];
            tope--;

            izquierdo = pila[tope];
            tope--;

            nuevo = crear_nodo(token);

            if (nuevo == NULL) {
                return NULL;
            }

            nuevo->izq = izquierdo;
            nuevo->der = derecho;

            tope++;
            pila[tope] = nuevo;
        }
        else if (es_unario(token)) {
            TuNodo *nuevo;
            TuNodo *hijo;

            if (tope < 0) {
                return NULL;
            }

            hijo = pila[tope];
            tope--;

            nuevo = crear_nodo(token);

            if (nuevo == NULL) {
                return NULL;
            }

            /*
                not c se coloca así:

                    not
                   /
                  c

                Es decir, el hijo va a la izquierda.
            */
            nuevo->izq = hijo;
            nuevo->der = NULL;

            tope++;
            pila[tope] = nuevo;
        }
        else {
            TuNodo *nuevo;

            if (tope >= 80) {
                return NULL;
            }

            nuevo = crear_nodo(token);

            if (nuevo == NULL) {
                return NULL;
            }

            tope++;
            pila[tope] = nuevo;
        }
    }

    if (tope != 0) {
        return NULL;
    }

    return pila[tope];
}

int evaluar(const TuNodo *raiz, const int *valores) {
    int indice;

    if (raiz == NULL || valores == NULL) {
        return 0;
    }

    if (igual(raiz->valor, "and")) {
        return evaluar(raiz->izq, valores) && evaluar(raiz->der, valores);
    }

    if (igual(raiz->valor, "or")) {
        return evaluar(raiz->izq, valores) || evaluar(raiz->der, valores);
    }

    if (igual(raiz->valor, "not")) {
        return !evaluar(raiz->izq, valores);
    }

    if (raiz->valor[0] == '0') {
        return 0;
    }

    if (raiz->valor[0] == '1') {
        return 1;
    }

    indice = tolower((unsigned char)raiz->valor[0]) - 'a';

    if (indice < 0 || indice > 25) {
        return 0;
    }

    return valores[indice] != 0;
}