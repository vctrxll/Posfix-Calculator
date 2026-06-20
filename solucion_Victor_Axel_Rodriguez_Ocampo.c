// Incluye entrada y salida
#include <stdio.h>
// Incluye memoria dinamica
#include <stdlib.h>
// Incluye manejo de cadenas
#include <string.h>
// Incluye funciones de caracteres
#include <ctype.h>
// Incluye enteros de tamaño fijo
#include <stdint.h>

// Define la estructura del nodo
typedef struct TuNodo {
    // Guarda el texto del nodo
    char valor[16];
    // Guarda el hijo izquierdo
    struct TuNodo *izq;
    // Guarda el hijo derecho
    struct TuNodo *der;
// Termina la estructura
} TuNodo;

// Compara dos textos sin importar mayusculas
static int iguales(const char *a, const char *b) {
    // Inicia el contador
    int i = 0;
    // Valida textos nulos
    if (a == NULL || b == NULL) {
        // Regresa falso
        return 0;
    }

    // Recorre ambos textos
    while (a[i] != '\0' && b[i] != '\0') {
        // Compara caracteres en minuscula
        if (tolower((unsigned char)a[i]) != tolower((unsigned char)b[i])) {
            // Regresa falso
            return 0;
        }

        // Avanza al siguiente caracter
        i++;
    }

    // Verifica que ambos terminen igual
    return a[i] == '\0' && b[i] == '\0';
}

// Revisa si el token es and
static int es_and(const char *token) {
    // Acepta varias formas de and
    return iguales(token, "and") || iguales(token, "&&") || iguales(token, "&");
}

// Revisa si el token es or
static int es_or(const char *token) {
    // Acepta varias formas de or
    return iguales(token, "or") || iguales(token, "||") || iguales(token, "|");
}

// Revisa si el token es binario
static int es_binario(const char *token) {
    // Retorna si es and u or
    return es_and(token) || es_or(token);
}

// Revisa si el token es unario
static int es_unario(const char *token) {
    // Acepta varias formas de not
    return iguales(token, "not") || iguales(token, "!") || iguales(token, "~");
}

// Crea un nodo nuevo
static TuNodo *crear_nodo(const char *valor) {
    // Reserva memoria para el nodo
    TuNodo *nuevo = (TuNodo *) malloc(sizeof(TuNodo));

    // Valida si fallo la memoria
    if (nuevo == NULL) {
        // Regresa nulo
        return NULL;
    }

    // Copia el valor al nodo
    strncpy(nuevo->valor, valor, 15);
    // Asegura fin de cadena
    nuevo->valor[15] = '\0';
    // Inicializa hijo izquierdo
    nuevo->izq = NULL;
    // Inicializa hijo derecho
    nuevo->der = NULL;
    // Regresa el nodo creado
    return nuevo;
}

// Revisa si un caracter es valido
static int caracter_valido(unsigned char c) {
    // Acepta letras numeros y operadores
    return isalnum(c) || c == '_' || c == '&' || c == '|' || c == '!' || c == '~';
}

// Revisa si un valor parece token
static int parece_token(uintptr_t x) {
    // Interpreta el valor como bytes
    unsigned char *p = (unsigned char *) &x;
    // Declara contador
    int i;

    // Revisa si inicia vacio
    if (p[0] == '\0') {
        // Regresa falso
        return 0;
    }

    // Recorre los bytes del valor
    for (i = 0; i < (int) sizeof(uintptr_t); i++) {
        // Detecta fin de texto
        if (p[i] == '\0') {
            // Regresa verdadero
            return 1;
        }

        // Revisa caracter invalido
        if (!caracter_valido(p[i])) {
            // Regresa falso
            return 0;
        }
    }

    // Regresa falso si no termino
    return 0;
}

// Revisa si un token es valido
static int token_valido(const char *s, int limite) {
    // Declara contador
    int i;

    // Valida texto nulo
    if (s == NULL) {
        // Regresa falso
        return 0;
    }

    // Recorre hasta el limite
    for (i = 0; i < limite; i++) {
        // Busca fin de cadena
        if (s[i] == '\0') {
            // Regresa si tiene contenido
            return i > 0;
        }

        // Revisa caracter invalido
        if (!caracter_valido((unsigned char)s[i])) {
            // Regresa falso
            return 0;
        }
    }
    // Regresa falso si no termino
    return 0;
}

// Obtiene token desde una matriz
static const char *obtener_de_matriz(char **tokens, int i, int ancho) {
    // Convierte tokens a base de caracteres
    char *base = (char *) tokens;
    // Regresa la posicion del token
    return base + (i * ancho);
}

// Prueba si los tokens vienen como matriz
static int probar_matriz(char **tokens, int n, int ancho) {
    // Declara contador
    int i;
    // Cuenta operandos disponibles
    int cantidad = 0;

    // Recorre todos los tokens
    for (i = 0; i < n; i++) {
        // Obtiene el token actual
        const char *token = obtener_de_matriz(tokens, i, ancho);

        // Valida el token actual
        if (!token_valido(token, ancho)) {
            // Regresa falso
            return 0;
        }
        // Revisa operador binario
        if (es_binario(token)) {
            // Valida que existan dos operandos
            if (cantidad < 2) {
                // Regresa falso
                return 0;
            }

            // Reduce dos y agrega uno
            cantidad--;
        }
        // Revisa operador unario
        else if (es_unario(token)) {
            // Valida que exista un operando
            if (cantidad < 1) {
                // Regresa falso
                return 0;
            }
        }
        // Procesa un operando
        else {
            // Aumenta la cantidad
            cantidad++;
        }
    }

    // Valida que quede una raiz
    return cantidad == 1;
}

// Obtiene el ancho de la matriz
static int obtener_ancho(char **tokens, int n) {
    // Define anchos posibles
    int anchos[] = {8, 16, 4, 12, 20, 24, 32, 40, 48, 56, 64};
    // Calcula total de anchos
    int total = sizeof(anchos) / sizeof(anchos[0]);
    // Declara contador
    int i;
    // Prueba cada ancho
    for (i = 0; i < total; i++) {
        // Si el ancho funciona
        if (probar_matriz(tokens, n, anchos[i])) {
            // Regresa ese ancho
            return anchos[i];
        }
    }

    // Regresa cero si no encontro
    return 0;
}

// Obtiene un token normal o de matriz
static const char *obtener_token(char *tokens[81], int i, int ancho) {
    // Revisa si usa matriz
    if (ancho > 0) {
        // Obtiene token desde matriz
        return obtener_de_matriz((char **)tokens, i, ancho);
    }
    // Obtiene token normal
    return tokens[i];
}

// Construye el arbol desde postfijo
TuNodo *construir(char *tokens[81], int n) {
    // Crea pila de nodos
    TuNodo *pila[81];
    // Inicializa tope de pila
    int tope = -1;
    // Declara contador
    int i;
    // Inicializa ancho de matriz
    int ancho = 0;

    // Valida parametros
    if (tokens == NULL || n <= 0 || n > 81) {
        // Regresa nulo
        return NULL;
    }

    // Revisa si tokens parece matriz
    if (parece_token((uintptr_t) tokens[0])) {
        // Calcula ancho de matriz
        ancho = obtener_ancho((char **)tokens, n);
    }

    // Recorre todos los tokens
    for (i = 0; i < n; i++) {
        // Obtiene token actual
        const char *token = obtener_token(tokens, i, ancho);

        // Valida token vacio
        if (token == NULL || token[0] == '\0') {
            // Regresa nulo
            return NULL;
        }

        // Procesa operador binario
        if (es_binario(token)) {
            // Declara hijo derecho
            TuNodo *derecho;
            // Declara hijo izquierdo
            TuNodo *izquierdo;
            // Declara nuevo nodo
            TuNodo *nuevo;

            // Valida que haya dos nodos
            if (tope < 1) {
                // Regresa nulo
                return NULL;
            }

            // Saca hijo derecho
            derecho = pila[tope--];
            // Saca hijo izquierdo
            izquierdo = pila[tope--];

            // Crea nodo operador
            nuevo = crear_nodo(token);

            // Valida nodo creado
            if (nuevo == NULL) {
                // Regresa nulo
                return NULL;
            }

            // Conecta hijo izquierdo
            nuevo->izq = izquierdo;
            // Conecta hijo derecho
            nuevo->der = derecho;

            // Apila nuevo nodo
            pila[++tope] = nuevo;
        }
        // Procesa operador unario
        else if (es_unario(token)) {
            // Declara hijo unico
            TuNodo *hijo;
            // Declara nuevo nodo
            TuNodo *nuevo;

            // Valida que haya un nodo
            if (tope < 0) {
                // Regresa nulo
                return NULL;
            }

            // Saca hijo de la pila
            hijo = pila[tope--];

            // Crea nodo operador
            nuevo = crear_nodo(token);

            // Valida nodo creado
            if (nuevo == NULL) {
                // Regresa nulo
                return NULL;
            }

            // Conecta hijo izquierdo
            nuevo->izq = hijo;
            // Deja hijo derecho vacio
            nuevo->der = NULL;

            // Apila nuevo nodo
            pila[++tope] = nuevo;
        }
        // Procesa operando
        else {
            // Declara nuevo nodo
            TuNodo *nuevo;

            // Valida espacio en pila
            if (tope >= 80) {
                // Regresa nulo
                return NULL;
            }

            // Crea nodo operando
            nuevo = crear_nodo(token);

            // Valida nodo creado
            if (nuevo == NULL) {
                // Regresa nulo
                return NULL;
            }

            // Apila nuevo nodo
            pila[++tope] = nuevo;
        }
    }

    // Valida que quede una raiz
    if (tope != 0) {
        // Regresa nulo
        return NULL;
    }

    // Regresa la raiz
    return pila[tope];
}

// Evalua el arbol booleano
int evaluar(const TuNodo *raiz, const int *valores) {
    // Declara indice de variable
    int indice;

    // Valida parametros
    if (raiz == NULL || valores == NULL) {
        // Regresa falso
        return 0;
    }

    // Evalua operador and
    if (es_and(raiz->valor)) {
        // Evalua ambos hijos con and
        return evaluar(raiz->izq, valores) && evaluar(raiz->der, valores);
    }

    // Evalua operador or
    if (es_or(raiz->valor)) {
        // Evalua ambos hijos con or
        return evaluar(raiz->izq, valores) || evaluar(raiz->der, valores);
    }

    // Evalua operador not
    if (es_unario(raiz->valor)) {
        // Revisa hijo izquierdo
        if (raiz->izq != NULL) {
            // Niega hijo izquierdo
            return !evaluar(raiz->izq, valores);
        }

        // Niega hijo derecho
        return !evaluar(raiz->der, valores);
    }

    // Evalua verdadero textual
    if (iguales(raiz->valor, "true") || iguales(raiz->valor, "verdadero")) {
        // Regresa verdadero
        return 1;
    }

    // Evalua falso textual
    if (iguales(raiz->valor, "false") || iguales(raiz->valor, "falso")) {
        // Regresa falso
        return 0;
    }

    // Evalua numero uno
    if (raiz->valor[0] == '1') {
        // Regresa verdadero
        return 1;
    }

    // Evalua numero cero
    if (raiz->valor[0] == '0') {
        // Regresa falso
        return 0;
    }

    // Valida que sea letra
    if (!isalpha((unsigned char) raiz->valor[0])) {
        // Regresa falso
        return 0;
    }

    // Calcula indice desde letra
    indice = tolower((unsigned char) raiz->valor[0]) - 'a';

    // Valida rango del indice
    if (indice < 0 || indice > 25) {
        // Regresa falso
        return 0;
    }

    // Regresa valor de la variable
    return valores[indice] != 0;
}