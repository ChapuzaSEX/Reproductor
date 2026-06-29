#pragma once
#include "Nodo.h"
#include <stdexcept>

// Cola (FIFO) implementada con nodos enlazados.
// Se usa para la lista de reproducción pendiente:
// encola la siguiente canción al frente, desencola al reproducir.
template <typename T>
class Cola {
private:
    Nodo<T>* frente;
    Nodo<T>* fin;
    int tamano;

public:
    Cola() : frente(nullptr), fin(nullptr), tamano(0) {}

    ~Cola() {
        limpiar();
    }

    // Encola al final — O(1)
    void encolar(T dato) {
        Nodo<T>* nuevo = new Nodo<T>(dato);
        if (fin == nullptr) {
            frente = fin = nuevo;
        } else {
            fin->siguiente = nuevo;
            fin = nuevo;
        }
        tamano++;
    }

    // Desencola del frente — O(1)
    T desencolar() {
        if (estaVacia())
            throw std::underflow_error("Cola vacia");
        T dato = frente->dato;
        Nodo<T>* aBorrar = frente;
        frente = frente->siguiente;
        if (frente == nullptr) fin = nullptr;
        delete aBorrar;
        tamano--;
        return dato;
    }

    // Consulta el frente sin eliminarlo — O(1)
    T verFrente() const {
        if (estaVacia())
            throw std::underflow_error("Cola vacia");
        return frente->dato;
    }

    void limpiar() {
        while (!estaVacia()) desencolar();
    }

    bool estaVacia() const { return frente == nullptr; }
    int getTamano() const { return tamano; }
    Nodo<T>* getFrente() const { return frente; }
};
