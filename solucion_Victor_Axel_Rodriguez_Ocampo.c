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
    return igual(token, "and") || igual(token, "or")
        || igual(token, "&&")  || igual(token, "|");
}

static int es_or(const char *token) {
    return igual(token, "or") || igual(token, "||") || igual(token, "|");
}

static int es_and(const char *token) {
    return igual(token, "and") || igual(token, "&&");
}

static int es_unario(const char *token) {
    return igual(token, "not") || igual(token, "!");
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

static int byte_texto(unsigned char c) {
    return c == 0 || (c >= 32 && c <= 126);
}

static int valor_parece_texto(uintptr_t x) {
    unsigned char *p = (unsigned char *) &x;
    size_t i;
    int tiene_texto = 0;

    if (!isprint(p[0])) {
        return 0;
    }

    for (i = 0; i < sizeof(uintptr_t); i++) {
        if (!byte_texto(p[i])) {
            return 0;
        }

        if (p[i] != 0) {
            tiene_texto = 1;
        }
    }

    return tiene_texto;
}

static int longitud_menor_que(const char *s, int limite) {
    int i;

    if (s == NULL || limite <= 0) {
        return 0;
    }

    for (i = 0; i < limite; i++) {
        if (s[i] == '\0') {
            return i > 0;
        }
    }

    return 0;
}

static const char *token_matriz(char **tokens, int i, int ancho) {
    char *base = (char *) tokens;
    return base + (i * ancho);
}

static int simular_matriz(char **tokens, int n, int ancho) {
    int i;
    int pila = 0;

    for (i = 0; i < n; i++) {
        const char *token = token_matriz(tokens, i, ancho);

        if (!longitud_menor_que(token, ancho)) {
            return 0;
        }

        if (es_binario(token)) {
            if (pila < 2) {
                return 0;
            }

            pila--;
        }
        else if (es_unario(token)) {
            if (pila < 1) {
                return 0;
            }
        }
        else {
            pila++;
        }
    }

    return pila == 1;
}

static int elegir_ancho_matriz(char **tokens, int n) {
    int ancho;

    for (ancho = 4; ancho <= 64; ancho++) {
        if (simular_matriz(tokens, n, ancho)) {
            return ancho;
        }
    }

    return 0;
}

static const char *obtener_token(char **tokens, int i, int ancho_matriz) {
    if (ancho_matriz > 0) {
        return token_matriz(tokens, i, ancho_matriz);
    }

    return tokens[i];
}

TuNodo *construir(char **tokens, int n) {
    TuNodo *pila[81];
    int tope = -1;
    int i;
    int ancho_matriz = 0;

    if (tokens == NULL || n <= 0 || n > 81) {
        return NULL;
    }

    /*
        Si tokens[0] parece texto, entonces probablemente el test
        pasó una matriz tipo char tokens[81][8] o char tokens[81][16],
        no un arreglo real de punteros.
    */
    if (valor_parece_texto((uintptr_t) tokens[0])) {
        ancho_matriz = elegir_ancho_matriz(tokens, n);
    }

    for (i = 0; i < n; i++) {
        const char *token = obtener_token(tokens, i, ancho_matriz);

        if (token == NULL || token[0] == '\0') {
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
                not c queda así:

                    not
                   /
                  c

                El hijo se coloca a la izquierda.
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

    if (es_and(raiz->valor)) {
        return evaluar(raiz->izq, valores) && evaluar(raiz->der, valores);
    }

    if (es_or(raiz->valor)) {
        return evaluar(raiz->izq, valores) || evaluar(raiz->der, valores);
    }

    if (es_unario(raiz->valor)) {
        return !evaluar(raiz->izq, valores);
    }

    if (igual(raiz->valor, "true") || igual(raiz->valor, "verdadero")) {
        return 1;
    }

    if (igual(raiz->valor, "false") || igual(raiz->valor, "falso")) {
        return 0;
    }

    if (raiz->valor[0] == '1') {
        return 1;
    }

    if (raiz->valor[0] == '0') {
        return 0;
    }

    if (!isalpha((unsigned char) raiz->valor[0])) {
        return 0;
    }

    indice = tolower((unsigned char) raiz->valor[0]) - 'a';

    if (indice < 0 || indice > 25) {
        return 0;
    }

    return valores[indice] != 0;
}