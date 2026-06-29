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

public:
    Cancion(const std::string& id,
            const std::string& nombre,
            const std::string& artista,
            const std::string& album,
            const std::string& anio,
            int duracion,
            const std::string& ruta);

    // Getters
    std::string getId()      const;
    std::string getNombre()  const;
    std::string getArtista() const;
    std::string getAlbum()   const;
    std::string getAnio()    const;
    int         getDuracion()const;
    std::string getRuta()    const;

    // Devuelve "MM:SS" a partir de duracion en segundos
    std::string getDuracionFormateada() const;

    // Serializa la canción al formato CSV para guardar en archivo
    std::string serializar(char delimitador = ',') const;

    // Imprime info resumida por consola
    void imprimir() const;
};
