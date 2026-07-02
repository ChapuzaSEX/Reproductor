#pragma once
#include "../data_structures/Lista.h"
#include "../data_structures/Cola.h"
#include "../data_structures/Pila.h"
#include "../data_structures/ArbolAVL.h"
#include "../data_structures/Trie.h"
#include "Cancion.h"
#include "ArtistaEntry.h"
#include <string>

// Estados de reproducción
enum class EstadoReproduccion { DETENIDO, REPRODUCIENDO, EN_PAUSA };

// Modos de repetición
enum class ModoRepeticion { DESACTIVADO, REPETIR_UNA, REPETIR_TODAS };

// Reproductor encapsula toda la lógica del reproductor de música.
// Usa:
//   Lista<Cancion*> catalogo       — todas las canciones disponibles
//   Cola<Cancion*>  pendientes     — canciones por reproducir (FIFO)
//   Pila<Cancion*>  historial      — canciones ya reproducidas (LIFO)
//   Trie<Cancion*>  trieBusqueda   — indice de subcadenas para la búsqueda (F)
//   ArbolAVL<ArtistaEntry*> indiceArtistas — artistas ordenados alfabéticamente,
//                                    cada uno con sus canciones en un AVL propio
class Reproductor {
private:
    Lista<Cancion*>  catalogo;
    Cola<Cancion*>   pendientes;
    Pila<Cancion*>   historial;

    Cancion*          cancionActual;
    EstadoReproduccion estado;
    ModoRepeticion     modoRepeticion;
    bool               modoAleatorio;

    // Índices de búsqueda / ranking. Se reconstruyen cada vez que el
    // catálogo cambia (agregarCancion / eliminarCancion), lo que mantiene
    // la búsqueda y los listados por artista siempre al día.
    Trie<Cancion*>          trieBusqueda;
    ArbolAVL<ArtistaEntry*> indiceArtistas;

    // Mezcla las canciones del catálogo en la cola pendientes
    void llenarColaAleatoria(Cancion* excluir = nullptr);

    // Construye el string de indicadores "(S-R1)" etc.
    std::string indicadores() const;

    // Reconstruye trieBusqueda e indiceArtistas a partir del catálogo actual
    void reconstruirIndices();

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

    // Fuerza la reconstrucción de los índices (Trie / AVL de artistas).
    // Se usa después de cargar el ranking de reproducciones desde disco,
    // para que los totales por artista queden correctos desde el inicio.
    void reindexar();

    // ── Búsqueda (Trie) ──────────────────────────────────────────
    // Retorna todas las canciones cuyo nombre o artista contiene "texto"
    // (no distingue mayúsculas/minúsculas). Lista vacía si no hay coincidencias.
    Lista<Cancion*> buscarCanciones(const std::string& texto) const;

    // ── Ranking / TOP 10 (Heap) ──────────────────────────────────
    // Registra que "c" comenzó a reproducirse (incrementa sus contadores)
    void registrarReproduccion(Cancion* c);

    // Top 10 canciones más escuchadas (o menos, si la biblioteca es más chica)
    Lista<Cancion*> top10Canciones() const;

    // Top 10 artistas más escuchados
    Lista<ArtistaEntry*> top10Artistas() const;

    // ── Listado por artista (AVL) ────────────────────────────────
    // Canciones del artista indicado, ordenadas alfabéticamente
    Lista<Cancion*> cancionesDeArtista(const std::string& nombreArtista) const;
    ArtistaEntry*   buscarArtista(const std::string& nombreArtista) const;
};
