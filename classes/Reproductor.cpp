#include "Reproductor.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <stdexcept>

Reproductor::Reproductor()
    : cancionActual(nullptr),
      estado(EstadoReproduccion::DETENIDO),
      modoRepeticion(ModoRepeticion::DESACTIVADO),
      modoAleatorio(false) {
    srand(static_cast<unsigned int>(time(nullptr)));
}

Reproductor::~Reproductor() {
    // El catálogo es dueño de los punteros: los libera
    Nodo<Cancion*>* actual = catalogo.getCabeza();
    while (actual != nullptr) {
        delete actual->dato;
        actual = actual->siguiente;
    }
}

// ── Catálogo ─────────────────────────────────────────────────────────────────

void Reproductor::agregarCancion(Cancion* c) {
    catalogo.agregarFinal(c);
}

bool Reproductor::eliminarCancion(const std::string& id) {
    // Si era la canción actual, limpiar
    if (cancionActual != nullptr && cancionActual->getId() == id) {
        cancionActual = nullptr;
        estado = EstadoReproduccion::DETENIDO;
    }

    // Buscar y eliminar del catálogo liberando memoria
    Cancion* encontrada = nullptr;
    catalogo.eliminarSi([&](Cancion* c) {
        if (c->getId() == id) { encontrada = c; return true; }
        return false;
    });

    if (encontrada != nullptr) {
        delete encontrada;
        return true;
    }
    return false;
}

bool Reproductor::catalogoVacio() const { return catalogo.estaVacia(); }
int  Reproductor::totalCanciones() const { return catalogo.getTamano(); }
Lista<Cancion*>& Reproductor::getCatalogo() { return catalogo; }

// ── Helpers privados ─────────────────────────────────────────────────────────

void Reproductor::llenarColaAleatoria(Cancion* excluir) {
    pendientes.limpiar();
    int n = catalogo.getTamano();
    if (n == 0) return;

    // Copiar punteros a arreglo temporal
    Cancion** arr = new Cancion*[n];
    Nodo<Cancion*>* nodo = catalogo.getCabeza();
    for (int i = 0; i < n; i++) {
        arr[i] = nodo->dato;
        nodo = nodo->siguiente;
    }

    // Fisher-Yates
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Cancion* tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
    }

    // Encolar excluyendo la canción actual si se indica
    for (int i = 0; i < n; i++) {
        if (arr[i] != excluir)
            pendientes.encolar(arr[i]);
    }

    delete[] arr;
}

std::string Reproductor::indicadores() const {
    std::string s = "";
    bool aleatorio = modoAleatorio;
    bool r1  = (modoRepeticion == ModoRepeticion::REPETIR_UNA);
    bool rAll= (modoRepeticion == ModoRepeticion::REPETIR_TODAS);

    if (aleatorio || r1 || rAll) {
        s += " (";
        if (aleatorio) s += "S";
        if (r1)        s += aleatorio ? "-R1" : "R1";
        if (rAll)      s += aleatorio ? "-RA" : "RA";
        s += ")";
    }
    return s;
}

// ── Control de reproducción ──────────────────────────────────────────────────

void Reproductor::reproducirPausar() {
    if (catalogoVacio()) return;

    if (cancionActual == nullptr) {
        // Primera vez: tomar la primera del catálogo
        cancionActual = catalogo.obtener(0);
        llenarColaAleatoria(cancionActual);
        estado = EstadoReproduccion::REPRODUCIENDO;
        return;
    }

    if (estado == EstadoReproduccion::REPRODUCIENDO)
        estado = EstadoReproduccion::EN_PAUSA;
    else
        estado = EstadoReproduccion::REPRODUCIENDO;
}

void Reproductor::siguiente() {
    if (catalogoVacio()) return;

    // Repetir una: no avanza
    if (modoRepeticion == ModoRepeticion::REPETIR_UNA) return;

    // Guardar actual en historial
    if (cancionActual != nullptr)
        historial.push(cancionActual);

    // Tomar siguiente de la cola
    if (!pendientes.estaVacia()) {
        cancionActual = pendientes.desencolar();
        estado = EstadoReproduccion::REPRODUCIENDO;
        return;
    }

    // Cola vacía
    if (modoRepeticion == ModoRepeticion::REPETIR_TODAS) {
        llenarColaAleatoria();
        if (!pendientes.estaVacia()) {
            cancionActual = pendientes.desencolar();
            estado = EstadoReproduccion::REPRODUCIENDO;
        }
    } else {
        // Fin de lista sin repetición
        cancionActual = nullptr;
        estado = EstadoReproduccion::DETENIDO;
    }
}

void Reproductor::anterior() {
    if (catalogoVacio()) return;

    // Repetir una: no retrocede
    if (modoRepeticion == ModoRepeticion::REPETIR_UNA) return;

    if (historial.estaVacia()) return;

    // Volver a encolar la actual al frente (para poder avanzar de vuelta)
    // Usamos agregarFinal pero necesitamos al frente:
    // reencolar cancionActual al inicio de pendientes reutilizando Cola
    // Solución: vaciar pendientes a un arreglo, insertar cancionActual, reencolar
    if (cancionActual != nullptr) {
        int n = pendientes.getTamano();
        Cancion** arr = new Cancion*[n];
        for (int i = 0; i < n; i++) arr[i] = pendientes.desencolar();
        pendientes.encolar(cancionActual);
        for (int i = 0; i < n; i++) pendientes.encolar(arr[i]);
        delete[] arr;
    }

    cancionActual = historial.pop();
    estado = EstadoReproduccion::REPRODUCIENDO;
}

void Reproductor::saltarA(int posicionEnCola) {
    // posicion 1-based en la cola de pendientes
    int n = pendientes.getTamano();
    if (posicionEnCola < 1 || posicionEnCola > n) return;

    if (cancionActual != nullptr)
        historial.push(cancionActual);

    // Desencolar descartando las anteriores a la posición
    for (int i = 1; i < posicionEnCola; i++)
        pendientes.desencolar();   // descartadas (se pierden del historial)

    cancionActual = pendientes.desencolar();
    estado = EstadoReproduccion::REPRODUCIENDO;
}

void Reproductor::reproducirDirecta(Cancion* c) {
    if (c == nullptr) return;
    if (cancionActual != nullptr)
        historial.push(cancionActual);
    cancionActual = c;
    llenarColaAleatoria(c);
    estado = EstadoReproduccion::REPRODUCIENDO;
}

// ── Modos ─────────────────────────────────────────────────────────────────────

void Reproductor::toggleAleatorio() {
    if (catalogoVacio()) return;
    modoAleatorio = !modoAleatorio;
    if (modoAleatorio) {
        // Mezcla las pendientes una vez al activar
        int n = pendientes.getTamano();
        if (n > 1) {
            Cancion** arr = new Cancion*[n];
            for (int i = 0; i < n; i++) arr[i] = pendientes.desencolar();
            for (int i = n - 1; i > 0; i--) {
                int j = rand() % (i + 1);
                Cancion* tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
            }
            for (int i = 0; i < n; i++) pendientes.encolar(arr[i]);
            delete[] arr;
        }
    }
    // Desactivar no reordena (según enunciado)
}

void Reproductor::toggleRepeticion() {
    if (catalogoVacio()) return;
    switch (modoRepeticion) {
        case ModoRepeticion::DESACTIVADO:
            modoRepeticion = ModoRepeticion::REPETIR_UNA;   break;
        case ModoRepeticion::REPETIR_UNA:
            modoRepeticion = ModoRepeticion::REPETIR_TODAS; break;
        case ModoRepeticion::REPETIR_TODAS:
            modoRepeticion = ModoRepeticion::DESACTIVADO;   break;
    }
}

// ── Cola pendientes ───────────────────────────────────────────────────────────

void Reproductor::agregarAlFinalDeCola(Cancion* c) {
    pendientes.encolar(c);
}

Cola<Cancion*>& Reproductor::getPendientes() { return pendientes; }

// ── Estado ────────────────────────────────────────────────────────────────────

Cancion*           Reproductor::getCancionActual()  const { return cancionActual; }
EstadoReproduccion Reproductor::getEstado()         const { return estado; }
ModoRepeticion     Reproductor::getModoRepeticion() const { return modoRepeticion; }
bool               Reproductor::getModoAleatorio()  const { return modoAleatorio; }

// ── Mostrar ───────────────────────────────────────────────────────────────────

void Reproductor::mostrarEncabezado() const {
    if (cancionActual == nullptr) {
        std::cout << "Reproduccion Detenida" << std::endl;
        return;
    }

    std::string estadoStr;
    switch (estado) {
        case EstadoReproduccion::REPRODUCIENDO: estadoStr = "Reproduciendo"; break;
        case EstadoReproduccion::EN_PAUSA:      estadoStr = "En pausa";      break;
        default:                                estadoStr = "Detenido";      break;
    }

    std::cout << estadoStr << indicadores() << ": "
              << cancionActual->getNombre() << std::endl;
    std::cout << "Artista: " << cancionActual->getArtista() << std::endl;
    std::cout << "Album: "   << cancionActual->getAlbum()
              << " [" << cancionActual->getAnio() << "]" << std::endl;
}
