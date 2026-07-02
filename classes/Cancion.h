#pragma once
#include <string>

// Representa una pista de audio con sus metadatos.
// Es el tipo de dato almacenado en todas las estructuras.
class Cancion {
private:
    std::string id;
    std::string nombre;
    std::string artista;
    std::string album;
    std::string anio;
    int duracion;       // en segundos
    std::string ruta;
    int reproducciones; // contador de veces reproducida (para el ranking TOP 10)

public:
    Cancion(const std::string& id,
            const std::string& nombre,
            const std::string& artista,
            const std::string& album,
            const std::string& anio,
            int duracion,
            const std::string& ruta,
            int reproducciones = 0);

    // Getters
    std::string getId()      const;
    std::string getNombre()  const;
    std::string getArtista() const;
    std::string getAlbum()   const;
    std::string getAnio()    const;
    int         getDuracion()const;
    std::string getRuta()    const;
    int         getReproducciones() const;

    // Incrementa el contador de reproducciones en 1
    void incrementarReproducciones();
    void setReproducciones(int valor);

    // Devuelve "MM:SS" a partir de duracion en segundos
    std::string getDuracionFormateada() const;

    // Serializa la canción al formato CSV para guardar en archivo
    std::string serializar(char delimitador = ',') const;

    // Imprime info resumida por consola
    void imprimir() const;
};

// ── Comparadores y utilidades para usar Cancion* en las estructuras genericas ──

// Compara alfabeticamente por nombre de cancion (usado en el AVL por artista)
int compararCancionPorNombre(Cancion* const& a, Cancion* const& b);

// Igualdad por id (usada por el Trie para no duplicar resultados)
bool cancionesSonIguales(Cancion* const& a, Cancion* const& b);
