#pragma once
#include "../data_structures/Lista.h"
#include "../data_structures/Cola.h"
#include "../data_structures/Pila.h"
#include "Cancion.h"
#include <string>

// Estados de reproducción
enum class EstadoReproduccion { DETENIDO, REPRODUCIENDO, EN_PAUSA };

// Modos de repetición
enum class ModoRepeticion { DESACTIVADO, REPETIR_UNA, REPETIR_TODAS };

// Reproductor encapsula toda la lógica del reproductor de música.
// Usa:
//   Lista<Cancion*> catalogo     — todas las canciones disponibles
//   Cola<Cancion*>  pendientes   — canciones por reproducir (FIFO)
//   Pila<Cancion*>  historial    — canciones ya reproducidas (LIFO)
class Reproductor {
private:
    Lista<Cancion*>  catalogo;
    Cola<Cancion*>   pendientes;
    Pila<Cancion*>   historial;

    Cancion*          cancionActual;
    EstadoReproduccion estado;
    ModoRepeticion     modoRepeticion;
    bool               modoAleatorio;

    // Mezcla las canciones del catálogo en la cola pendientes
    void llenarColaAleatoria(Cancion* excluir = nullptr);

    // Construye el string de indicadores "(S-R1)" etc.
    std::string indicadores() const;

public:
    Reproductor();
    ~Reproductor();

    // ── Catálogo ────────────────────────────────────────────────
    void agregarCancion(Cancion* c);
    bool eliminarCancion(const std::string& id);
    bool catalogoVacio() const;
    int  totalCanciones() const;
    Lista<Cancion*>& getCatalogo();

    // ── Control de reproducción ─────────────────────────────────
    void reproducirPausar();
    void siguiente();
    void anterior();
    void saltarA(int posicionEnCola);      // 1-based, desde la cola pendientes
    void reproducirDirecta(Cancion* c);   // reproduce una canción en específico

    // ── Modos ────────────────────────────────────────────────────
    void toggleAleatorio();
    void toggleRepeticion();

    // ── Cola pendientes ──────────────────────────────────────────
    void agregarAlFinalDeCola(Cancion* c);
    Cola<Cancion*>& getPendientes();

    // ── Estado ───────────────────────────────────────────────────
    Cancion*           getCancionActual()   const;
    EstadoReproduccion getEstado()          const;
    ModoRepeticion     getModoRepeticion()  const;
    bool               getModoAleatorio()   const;

    // ── Mostrar ──────────────────────────────────────────────────
    void mostrarEncabezado() const;
};
