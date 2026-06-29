#pragma once

template <typename T>
struct Nodo {
    T dato;
    Nodo<T>* siguiente;

    explicit Nodo(T dato) : dato(dato), siguiente(nullptr) {}
};
