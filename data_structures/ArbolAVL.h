#pragma once
#include "NodoAVL.h"
#include "Lista.h"

// Arbol AVL generico y auto-balanceado.
// El orden se define mediante un comparador externo (puntero a funcion)
// que retorna: <0 si a va antes que b, 0 si son "iguales", >0 si a va despues que b.
// Se usa para mantener el listado de canciones de cada artista en orden
// alfabetico (ver ArtistaEntry) y como indice ordenado de artistas.
template <typename T>
class ArbolAVL {
public:
    typedef int (*Comparador)(const T&, const T&);

private:
    NodoAVL<T>* raiz;
    int tamano;
    Comparador comparar;

    int altura(NodoAVL<T>* nodo) const {
        return nodo == nullptr ? 0 : nodo->altura;
    }

    int factorBalance(NodoAVL<T>* nodo) const {
        return nodo == nullptr ? 0 : altura(nodo->izquierda) - altura(nodo->derecha);
    }

    void actualizarAltura(NodoAVL<T>* nodo) {
        int hIzq = altura(nodo->izquierda);
        int hDer = altura(nodo->derecha);
        nodo->altura = 1 + (hIzq > hDer ? hIzq : hDer);
    }

    NodoAVL<T>* rotarDerecha(NodoAVL<T>* y) {
        NodoAVL<T>* x = y->izquierda;
        NodoAVL<T>* t2 = x->derecha;
        x->derecha = y;
        y->izquierda = t2;
        actualizarAltura(y);
        actualizarAltura(x);
        return x;
    }

    NodoAVL<T>* rotarIzquierda(NodoAVL<T>* x) {
        NodoAVL<T>* y = x->derecha;
        NodoAVL<T>* t2 = y->izquierda;
        y->izquierda = x;
        x->derecha = t2;
        actualizarAltura(x);
        actualizarAltura(y);
        return y;
    }

    NodoAVL<T>* balancear(NodoAVL<T>* nodo) {
        actualizarAltura(nodo);
        int fb = factorBalance(nodo);

        // Izquierda pesada
        if (fb > 1) {
            if (factorBalance(nodo->izquierda) < 0)
                nodo->izquierda = rotarIzquierda(nodo->izquierda); // caso izq-der
            return rotarDerecha(nodo); // caso izq-izq
        }
        // Derecha pesada
        if (fb < -1) {
            if (factorBalance(nodo->derecha) > 0)
                nodo->derecha = rotarDerecha(nodo->derecha); // caso der-izq
            return rotarIzquierda(nodo); // caso der-der
        }
        return nodo;
    }

    NodoAVL<T>* insertarRec(NodoAVL<T>* nodo, T dato, bool& insertado) {
        if (nodo == nullptr) {
            insertado = true;
            tamano++;
            return new NodoAVL<T>(dato);
        }

        int cmp = comparar(dato, nodo->dato);
        if (cmp < 0)
            nodo->izquierda = insertarRec(nodo->izquierda, dato, insertado);
        else if (cmp > 0)
            nodo->derecha = insertarRec(nodo->derecha, dato, insertado);
        else
            return nodo; // ya existe (misma clave), no se duplica

        return balancear(nodo);
    }

    NodoAVL<T>* minimoNodo(NodoAVL<T>* nodo) const {
        NodoAVL<T>* actual = nodo;
        while (actual->izquierda != nullptr) actual = actual->izquierda;
        return actual;
    }

    NodoAVL<T>* eliminarRec(NodoAVL<T>* nodo, const T& clave, bool& eliminado) {
        if (nodo == nullptr) return nullptr;

        int cmp = comparar(clave, nodo->dato);
        if (cmp < 0) {
            nodo->izquierda = eliminarRec(nodo->izquierda, clave, eliminado);
        } else if (cmp > 0) {
            nodo->derecha = eliminarRec(nodo->derecha, clave, eliminado);
        } else {
            eliminado = true;
            if (nodo->izquierda == nullptr || nodo->derecha == nullptr) {
                NodoAVL<T>* hijo = (nodo->izquierda != nullptr) ? nodo->izquierda : nodo->derecha;
                delete nodo;
                tamano--;
                return hijo;
            } else {
                NodoAVL<T>* sucesor = minimoNodo(nodo->derecha);
                nodo->dato = sucesor->dato;
                bool dummy = false;
                nodo->derecha = eliminarRec(nodo->derecha, sucesor->dato, dummy);
            }
        }
        if (nodo == nullptr) return nullptr;
        return balancear(nodo);
    }

    NodoAVL<T>* buscarRec(NodoAVL<T>* nodo, const T& clave) const {
        if (nodo == nullptr) return nullptr;
        int cmp = comparar(clave, nodo->dato);
        if (cmp == 0) return nodo;
        if (cmp < 0) return buscarRec(nodo->izquierda, clave);
        return buscarRec(nodo->derecha, clave);
    }

    void inOrdenRec(NodoAVL<T>* nodo, Lista<T>& salida) const {
        if (nodo == nullptr) return;
        inOrdenRec(nodo->izquierda, salida);
        salida.agregarFinal(nodo->dato);
        inOrdenRec(nodo->derecha, salida);
    }

    void limpiarRec(NodoAVL<T>* nodo) {
        if (nodo == nullptr) return;
        limpiarRec(nodo->izquierda);
        limpiarRec(nodo->derecha);
        delete nodo;
    }

public:
    explicit ArbolAVL(Comparador comparador)
        : raiz(nullptr), tamano(0), comparar(comparador) {}

    ~ArbolAVL() {
        limpiarRec(raiz);
    }

    // Inserta un dato. Si ya existe una clave equivalente, no hace nada.
    bool insertar(T dato) {
        bool insertado = false;
        raiz = insertarRec(raiz, dato, insertado);
        return insertado;
    }

    // Elimina el nodo cuya clave equivale a "clave". Retorna true si elimino algo.
    bool eliminar(const T& clave) {
        bool eliminado = false;
        raiz = eliminarRec(raiz, clave, eliminado);
        return eliminado;
    }

    // Busca y retorna un puntero al dato almacenado (o nullptr si no existe).
    T* buscar(const T& clave) const {
        NodoAVL<T>* nodo = buscarRec(raiz, clave);
        return nodo == nullptr ? nullptr : &(nodo->dato);
    }

    // Retorna una lista con todos los elementos en orden (segun el comparador).
    Lista<T> enOrden() const {
        Lista<T> salida;
        inOrdenRec(raiz, salida);
        return salida;
    }

    bool estaVacio() const { return raiz == nullptr; }
    int getTamano() const { return tamano; }

    // Elimina todos los nodos del arbol (no libera los datos T si son punteros;
    // eso es responsabilidad de quien los posee).
    void limpiar() {
        limpiarRec(raiz);
        raiz = nullptr;
        tamano = 0;
    }
};
