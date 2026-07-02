#include "Reproductor.h"
#include "../data_structures/Heap.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <stdexcept>
#include <cctype>

// ── Comparadores de prioridad para los Heap de ranking ─────────────────────
// Retornan true si "a" debe ir antes que "b": mayor cantidad de
// reproducciones primero; en caso de empate, orden alfabético.
static std::string aMinusculasLocal(const std::string& s) {
    std::string r = s;
    for (char& c : r) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return r;
}

static bool prioridadCancion(Cancion* const& a, Cancion* const& b) {
    if (a->getReproducciones() != b->getReproducciones())
        return a->getReproducciones() > b->getReproducciones();
    return aMinusculasLocal(a->getNombre()) < aMinusculasLocal(b->getNombre());
}

static bool prioridadArtista(ArtistaEntry* const& a, ArtistaEntry* const& b) {
    if (a->getReproduccionesTotales() != b->getReproduccionesTotales())
        return a->getReproduccionesTotales() > b->getReproduccionesTotales();
    return aMinusculasLocal(a->getNombreArtista()) < aMinusculasLocal(b->getNombreArtista());
}

Reproductor::Reproductor()
    : cancionActual(nullptr),
      estado(EstadoReproduccion::DETENIDO),
      modoRepeticion(ModoRepeticion::DESACTIVADO),
      modoAleatorio(false),
      trieBusqueda(cancionesSonIguales),
      indiceArtistas(compararArtistaPorNombre) {
    srand(static_cast<unsigned int>(time(nullptr)));
}

Reproductor::~Reproductor() {
    // Libera las entradas del índice de artistas (los ArtistaEntry, no las canciones)
    Lista<ArtistaEntry*> artistas = indiceArtistas.enOrden();
    Nodo<ArtistaEntry*>* nodoArtista = artistas.getCabeza();
    while (nodoArtista != nullptr) {
        delete nodoArtista->dato;
        nodoArtista = nodoArtista->siguiente;
    }

    // El catálogo es dueño de los punteros: los libera
    Nodo<Cancion*>* actual = catalogo.getCabeza();
    while (actual != nullptr) {
        delete actual->dato;
        actual = actual->siguiente;
    }
}

// ── Índices de búsqueda / ranking ───────────────────────────────────────────

void Reproductor::reconstruirIndices() {
    // Limpiar índices previos
    trieBusqueda.limpiar();
    Lista<ArtistaEntry*> artistasViejos = indiceArtistas.enOrden();
    Nodo<ArtistaEntry*>* nodoViejo = artistasViejos.getCabeza();
    while (nodoViejo != nullptr) {
        delete nodoViejo->dato;
        nodoViejo = nodoViejo->siguiente;
    }
    indiceArtistas.limpiar();

    // Reconstruir a partir del catálogo actual
    Nodo<Cancion*>* nodo = catalogo.getCabeza();
    while (nodo != nullptr) {
        Cancion* c = nodo->dato;

        // Indexar en el trie: todos los sufijos del nombre y del artista
        trieBusqueda.insertarTodosLosSufijos(c->getNombre(), c);
        trieBusqueda.insertarTodosLosSufijos(c->getArtista(), c);

        // Indexar en el AVL de artistas
        ArtistaEntry claveTemp(c->getArtista());
        ArtistaEntry* clavePtr = &claveTemp;
        ArtistaEntry** encontrado = indiceArtistas.buscar(clavePtr);
        ArtistaEntry* entrada;
        if (encontrado == nullptr) {
            entrada = new ArtistaEntry(c->getArtista());
            indiceArtistas.insertar(entrada);
        } else {
            entrada = *encontrado;
        }
        entrada->agregarCancion(c);

        nodo = nodo->siguiente;
    }
}

// ── Catálogo ─────────────────────────────────────────────────────────────────

void Reproductor::agregarCancion(Cancion* c) {
    catalogo.agregarFinal(c);
    reconstruirIndices();
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
        reconstruirIndices();
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
        registrarReproduccion(cancionActual);
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
        registrarReproduccion(cancionActual);
        return;
    }

    // Cola vacía
    if (modoRepeticion == ModoRepeticion::REPETIR_TODAS) {
        llenarColaAleatoria();
        if (!pendientes.estaVacia()) {
            cancionActual = pendientes.desencolar();
            estado = EstadoReproduccion::REPRODUCIENDO;
            registrarReproduccion(cancionActual);
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
    registrarReproduccion(cancionActual);
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
    registrarReproduccion(cancionActual);
}

void Reproductor::reproducirDirecta(Cancion* c) {
    if (c == nullptr) return;
    if (cancionActual != nullptr)
        historial.push(cancionActual);
    cancionActual = c;
    llenarColaAleatoria(c);
    estado = EstadoReproduccion::REPRODUCIENDO;
    registrarReproduccion(cancionActual);
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

void Reproductor::reindexar() {
    reconstruirIndices();
}

// ── Búsqueda (Trie) ──────────────────────────────────────────────────────────

Lista<Cancion*> Reproductor::buscarCanciones(const std::string& texto) const {
    return trieBusqueda.buscarPrefijo(texto);
}

// ── Ranking / TOP 10 (Heap) ───────────────────────────────────────────────────

void Reproductor::registrarReproduccion(Cancion* c) {
    if (c == nullptr) return;
    c->incrementarReproducciones();

    ArtistaEntry claveTemp(c->getArtista());
    ArtistaEntry* clavePtr = &claveTemp;
    ArtistaEntry** encontrado = indiceArtistas.buscar(clavePtr);
    if (encontrado != nullptr) {
        (*encontrado)->sumarReproduccion();
    }
}

Lista<Cancion*> Reproductor::top10Canciones() const {
    Lista<Cancion*> resultado;
    Heap<Cancion*> heap(prioridadCancion);

    Nodo<Cancion*>* nodo = catalogo.getCabeza();
    while (nodo != nullptr) {
        heap.insertar(nodo->dato);
        nodo = nodo->siguiente;
    }

    int limite = heap.getTamano() < 10 ? heap.getTamano() : 10;
    for (int i = 0; i < limite; i++) {
        resultado.agregarFinal(heap.extraerTope());
    }
    return resultado;
}

Lista<ArtistaEntry*> Reproductor::top10Artistas() const {
    Lista<ArtistaEntry*> resultado;
    Heap<ArtistaEntry*> heap(prioridadArtista);

    Lista<ArtistaEntry*> artistas = indiceArtistas.enOrden();
    Nodo<ArtistaEntry*>* nodo = artistas.getCabeza();
    while (nodo != nullptr) {
        heap.insertar(nodo->dato);
        nodo = nodo->siguiente;
    }

    int limite = heap.getTamano() < 10 ? heap.getTamano() : 10;
    for (int i = 0; i < limite; i++) {
        resultado.agregarFinal(heap.extraerTope());
    }
    return resultado;
}

// ── Listado por artista (AVL) ─────────────────────────────────────────────────

ArtistaEntry* Reproductor::buscarArtista(const std::string& nombreArtista) const {
    ArtistaEntry claveTemp(nombreArtista);
    ArtistaEntry* clavePtr = &claveTemp;
    ArtistaEntry** encontrado = indiceArtistas.buscar(clavePtr);
    return encontrado == nullptr ? nullptr : *encontrado;
}

Lista<Cancion*> Reproductor::cancionesDeArtista(const std::string& nombreArtista) const {
    ArtistaEntry* entrada = buscarArtista(nombreArtista);
    if (entrada == nullptr) return Lista<Cancion*>();
    return entrada->getCancionesOrdenadas();
}
