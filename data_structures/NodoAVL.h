#pragma once

// Nodo para arbol AVL generico.
template <typename T>
struct NodoAVL {
    T dato;
    NodoAVL<T>* izquierda;
    NodoAVL<T>* derecha;
    int altura;

    explicit NodoAVL(T dato)
        : dato(dato), izquierda(nullptr), derecha(nullptr), altura(1) {}
};
