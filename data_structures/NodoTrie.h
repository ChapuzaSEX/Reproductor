#pragma once
#include "Lista.h"

// Nodo de un Trie. Los hijos se guardan como una lista enlazada
// de pares (caracter, nodo) en vez de un arreglo fijo o un mapa STL.
template <typename T>
struct NodoTrie {
    char caracter;
    NodoTrie<T>* siguienteHermano; // encadena los hijos del mismo padre
    NodoTrie<T>* primerHijo;

    // Elementos cuyo sufijo/prefijo pasa por este nodo (acumulado en la insercion).
    // Permite responder una busqueda por prefijo en O(1) una vez encontrado el nodo.
    Lista<T> elementos;

    explicit NodoTrie(char c)
        : caracter(c), siguienteHermano(nullptr), primerHijo(nullptr) {}
};
