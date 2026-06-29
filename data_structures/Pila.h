#pragma once
#include "Nodo.h"
#include <stdexcept>

// Pila (LIFO) implementada con nodos enlazados.
// Se usa para el historial de canciones reproducidas,
// permitiendo retroceder en O(1).
template <typename T>
class Pila {
private:
    Nodo<T>* tope;
    int tamano;

public:
    Pila() : tope(nullptr), tamano(0) {}

    ~Pila() {
        limpiar();
    }

    // Apila un elemento — O(1)
    void push(T dato) {
        Nodo<T>* nuevo = new Nodo<T>(dato);
        nuevo->siguiente = tope;
        tope = nuevo;
        tamano++;
    }

    // Desapila y retorna el tope — O(1)
    T pop() {
        if (estaVacia())
            throw std::underflow_error("Pila vacia");
        T dato = tope->dato;
        Nodo<T>* aBorrar = tope;
        tope = tope->siguiente;
        delete aBorrar;
        tamano--;
        return dato;
    }

    // Consulta el tope sin eliminarlo — O(1)
    T peek() const {
        if (estaVacia())
            throw std::underflow_error("Pila vacia");
        return tope->dato;
    }

    void limpiar() {
        while (!estaVacia()) pop();
    }

    bool estaVacia() const { return tope == nullptr; }
    int getTamano() const { return tamano; }
};
