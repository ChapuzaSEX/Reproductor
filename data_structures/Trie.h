#pragma once
#include "NodoTrie.h"
#include "Lista.h"
#include <string>
#include <cctype>

// Trie generico de caracteres.
//
// Para soportar busqueda por SUBCADENA (no solo por prefijo) se insertan
// todos los sufijos de cada texto indexado. Ejemplo: al indexar "Kiss" se
// insertan los sufijos "kiss", "iss", "ss", "s". Buscar "is" navega el trie
// letra por letra y, si el camino existe, el nodo final ya contiene
// (acumulados durante la insercion) todos los elementos cuyo texto original
// contiene "is" como subcadena. Esto entrega busquedas eficientes en tiempo
// proporcional al largo del texto buscado, no al tamano de la biblioteca.
template <typename T>
class Trie {
public:
    typedef bool (*Igualador)(const T&, const T&);

private:
    NodoTrie<T>* raiz;
    Igualador sonIguales;

    static char normalizar(char c) {
        return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }

    NodoTrie<T>* obtenerOCrearHijo(NodoTrie<T>* nodo, char c) {
        NodoTrie<T>* hijo = nodo->primerHijo;
        NodoTrie<T>* anterior = nullptr;
        while (hijo != nullptr) {
            if (hijo->caracter == c) return hijo;
            anterior = hijo;
            hijo = hijo->siguienteHermano;
        }
        NodoTrie<T>* nuevo = new NodoTrie<T>(c);
        if (anterior == nullptr) nodo->primerHijo = nuevo;
        else anterior->siguienteHermano = nuevo;
        return nuevo;
    }

    NodoTrie<T>* buscarHijo(NodoTrie<T>* nodo, char c) const {
        NodoTrie<T>* hijo = nodo->primerHijo;
        while (hijo != nullptr) {
            if (hijo->caracter == c) return hijo;
            hijo = hijo->siguienteHermano;
        }
        return nullptr;
    }

    bool contiene(Lista<T>& lista, const T& valor) const {
        Nodo<T>* actual = lista.getCabeza();
        while (actual != nullptr) {
            if (sonIguales(actual->dato, valor)) return true;
            actual = actual->siguiente;
        }
        return false;
    }

    void limpiarRec(NodoTrie<T>* nodo) {
        if (nodo == nullptr) return;
        limpiarRec(nodo->primerHijo);
        limpiarRec(nodo->siguienteHermano);
        delete nodo;
    }

public:
    explicit Trie(Igualador igualador) : sonIguales(igualador) {
        raiz = new NodoTrie<T>('\0');
    }

    ~Trie() {
        limpiarRec(raiz);
    }

    // Inserta un sufijo (o cualquier cadena) asociandolo al elemento "valor".
    // Cada nodo recorrido acumula el elemento (sin duplicar).
    void insertar(const std::string& texto, T valor) {
        NodoTrie<T>* actual = raiz;
        for (char c : texto) {
            actual = obtenerOCrearHijo(actual, normalizar(c));
            if (!contiene(actual->elementos, valor))
                actual->elementos.agregarFinal(valor);
        }
    }

    // Inserta todos los sufijos de "texto" (para permitir busqueda por subcadena).
    void insertarTodosLosSufijos(const std::string& texto, T valor) {
        for (size_t i = 0; i < texto.size(); i++) {
            insertar(texto.substr(i), valor);
        }
    }

    // Busca "consulta" navegando el trie caracter por caracter.
    // Retorna la lista de elementos acumulados en el nodo final, o una
    // lista vacia si la consulta no existe como subcadena indexada.
    Lista<T> buscarPrefijo(const std::string& consulta) const {
        NodoTrie<T>* actual = raiz;
        for (char c : consulta) {
            actual = buscarHijo(actual, normalizar(c));
            if (actual == nullptr) return Lista<T>();
        }
        return actual->elementos;
    }

    void limpiar() {
        limpiarRec(raiz);
        raiz = new NodoTrie<T>('\0');
    }
};
