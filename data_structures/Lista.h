#pragma once
#include "Nodo.h"
#include <stdexcept>

// Lista enlazada simple genérica.
// Prohibido usar STL (vector, list, etc.) según enunciado.
template <typename T>
class Lista {
private:
    Nodo<T>* cabeza;
    Nodo<T>* cola;
    int tamano;

    void copiarDe(const Lista<T>& otra) {
        Nodo<T>* actual = otra.cabeza;
        while (actual != nullptr) {
            agregarFinal(actual->dato);
            actual = actual->siguiente;
        }
    }

public:
    Lista() : cabeza(nullptr), cola(nullptr), tamano(0) {}

    // Constructor de copia y operador de asignación: hacen una copia
    // profunda de los NODOS de la lista (no de aquello a lo que apunte T,
    // p. ej. si T = Cancion*, se copian los punteros, no las canciones).
    // Sin esto, al retornar una Lista por valor (como hacen los métodos de
    // búsqueda y ranking) el compilador haría una copia "shallow" que
    // comparte los mismos nodos entre el original y la copia; al destruirse
    // ambos objetos, cada uno intentaría liberar los mismos nodos, causando
    // un doble free.
    Lista(const Lista<T>& otra) : cabeza(nullptr), cola(nullptr), tamano(0) {
        copiarDe(otra);
    }

    Lista<T>& operator=(const Lista<T>& otra) {
        if (this == &otra) return *this;
        limpiar();
        copiarDe(otra);
        return *this;
    }

    ~Lista() {
        limpiar();
    }

    // Agrega al final — O(1) gracias al puntero cola
    void agregarFinal(T dato) {
        Nodo<T>* nuevo = new Nodo<T>(dato);
        if (cola == nullptr) {
            cabeza = cola = nuevo;
        } else {
            cola->siguiente = nuevo;
            cola = nuevo;
        }
        tamano++;
    }

    // Agrega al principio — O(1)
    void agregarInicio(T dato) {
        Nodo<T>* nuevo = new Nodo<T>(dato);
        nuevo->siguiente = cabeza;
        cabeza = nuevo;
        if (cola == nullptr) cola = cabeza;
        tamano++;
    }

    // Obtiene elemento por índice 0-based — O(n)
    T obtener(int indice) const {
        if (indice < 0 || indice >= tamano)
            throw std::out_of_range("Indice fuera de rango");
        Nodo<T>* actual = cabeza;
        for (int i = 0; i < indice; i++)
            actual = actual->siguiente;
        return actual->dato;
    }

    // Elimina por índice 0-based — O(n)
    void eliminar(int indice) {
        if (indice < 0 || indice >= tamano)
            throw std::out_of_range("Indice fuera de rango");

        if (indice == 0) {
            Nodo<T>* aBorrar = cabeza;
            cabeza = cabeza->siguiente;
            if (cabeza == nullptr) cola = nullptr;
            delete aBorrar;
        } else {
            Nodo<T>* anterior = cabeza;
            for (int i = 0; i < indice - 1; i++)
                anterior = anterior->siguiente;
            Nodo<T>* aBorrar = anterior->siguiente;
            anterior->siguiente = aBorrar->siguiente;
            if (aBorrar == cola) cola = anterior;
            delete aBorrar;
        }
        tamano--;
    }

    // Elimina el primer nodo que cumpla el predicado — O(n)
    template <typename Predicado>
    bool eliminarSi(Predicado pred) {
        Nodo<T>* actual = cabeza;
        Nodo<T>* anterior = nullptr;
        while (actual != nullptr) {
            if (pred(actual->dato)) {
                if (anterior == nullptr) cabeza = actual->siguiente;
                else anterior->siguiente = actual->siguiente;
                if (actual == cola) cola = anterior;
                delete actual;
                tamano--;
                return true;
            }
            anterior = actual;
            actual = actual->siguiente;
        }
        return false;
    }

    // Vacía la lista liberando memoria — O(n)
    void limpiar() {
        Nodo<T>* actual = cabeza;
        while (actual != nullptr) {
            Nodo<T>* siguiente = actual->siguiente;
            delete actual;
            actual = siguiente;
        }
        cabeza = cola = nullptr;
        tamano = 0;
    }

    bool estaVacia() const { return tamano == 0; }
    int getTamano() const { return tamano; }
    Nodo<T>* getCabeza() const { return cabeza; }
};
