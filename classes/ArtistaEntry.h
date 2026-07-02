#pragma once
#include "Cancion.h"
#include "../data_structures/ArbolAVL.h"
#include "../data_structures/Lista.h"
#include <string>

// Representa un artista dentro del indice de la biblioteca.
// Guarda sus canciones en un arbol AVL ordenado alfabeticamente por
// nombre de cancion (tal como sugiere el enunciado para el listado de
// canciones por artista) y acumula el total de reproducciones del artista
// (suma de las reproducciones de todas sus canciones) para el TOP 10.
class ArtistaEntry {
private:
    std::string nombreArtista;
    ArbolAVL<Cancion*> canciones;
    int reproduccionesTotales;

public:
    explicit ArtistaEntry(const std::string& nombreArtista);

    std::string getNombreArtista() const;

    // Inserta la cancion en el AVL del artista y suma sus reproducciones
    // ya registradas al total acumulado.
    void agregarCancion(Cancion* c);

    // Lista de canciones del artista ordenadas alfabeticamente (in-order del AVL)
    Lista<Cancion*> getCancionesOrdenadas() const;

    int getTotalCanciones() const;
    int getReproduccionesTotales() const;

    // Incrementa en 1 el total de reproducciones del artista
    void sumarReproduccion();
};

// Comparador alfabetico por nombre de artista (usado en el indice AVL de artistas)
int compararArtistaPorNombre(ArtistaEntry* const& a, ArtistaEntry* const& b);
