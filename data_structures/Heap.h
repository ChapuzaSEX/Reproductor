#pragma once
#include <stdexcept>

// Heap binario generico (arbol binario completo representado de forma
// implicita en un arreglo dinamico propio -- no se usa std::vector ni
// ninguna otra estructura de la STL).
//
// "prioridadMayor(a, b)" debe retornar true si "a" tiene mayor prioridad
// que "b" (debe ir primero). Para el ranking de canciones/artistas se usa
// un comparador que ordena primero por cantidad de reproducciones
// descendente y luego alfabeticamente en caso de empate, de modo que al
// extraer repetidamente el tope se obtiene el orden exacto del TOP 10.
template <typename T>
class Heap {
public:
    typedef bool (*Comparador)(const T&, const T&);

private:
    T* datos;
    int capacidad;
    int tamano;
    Comparador prioridadMayor;

    void redimensionar() {
        int nuevaCapacidad = capacidad * 2;
        T* nuevo = new T[nuevaCapacidad];
        for (int i = 0; i < tamano; i++) nuevo[i] = datos[i];
        delete[] datos;
        datos = nuevo;
        capacidad = nuevaCapacidad;
    }

    void flotar(int i) {
        while (i > 0) {
            int padre = (i - 1) / 2;
            if (prioridadMayor(datos[i], datos[padre])) {
                T tmp = datos[i]; datos[i] = datos[padre]; datos[padre] = tmp;
                i = padre;
            } else break;
        }
    }

    void hundir(int i) {
        while (true) {
            int izq = 2 * i + 1;
            int der = 2 * i + 2;
            int mayor = i;
            if (izq < tamano && prioridadMayor(datos[izq], datos[mayor])) mayor = izq;
            if (der < tamano && prioridadMayor(datos[der], datos[mayor])) mayor = der;
            if (mayor == i) break;
            T tmp = datos[i]; datos[i] = datos[mayor]; datos[mayor] = tmp;
            i = mayor;
        }
    }

public:
    explicit Heap(Comparador comparador, int capacidadInicial = 16)
        : capacidad(capacidadInicial < 4 ? 4 : capacidadInicial), tamano(0), prioridadMayor(comparador) {
        datos = new T[capacidad];
    }

    ~Heap() {
        delete[] datos;
    }

    void insertar(T valor) {
        if (tamano == capacidad) redimensionar();
        datos[tamano] = valor;
        flotar(tamano);
        tamano++;
    }

    T extraerTope() {
        if (estaVacio())
            throw std::underflow_error("Heap vacio");
        T tope = datos[0];
        tamano--;
        datos[0] = datos[tamano];
        hundir(0);
        return tope;
    }

    bool estaVacio() const { return tamano == 0; }
    int getTamano() const { return tamano; }
};
