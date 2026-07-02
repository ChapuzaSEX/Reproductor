#pragma once
#include "../classes/Reproductor.h"
#include <string>

// FileManager se encarga de toda la E/S de archivos.
// Separar esto del resto del programa hace que cambiar
// el formato de archivo no afecte la lógica del reproductor.
namespace FileManager {

    // Detecta el delimitador usado en el archivo CSV
    char detectarDelimitador(const std::string& linea);

    // Carga canciones desde music_source.txt al catálogo del reproductor.
    // Retorna true si se cargó al menos una canción.
    bool cargarCanciones(const std::string& ruta, Reproductor& rep);

    // Guarda el catálogo actualizado en music_source.txt
    bool guardarCanciones(const std::string& ruta, Reproductor& rep);

    // Carga el estado previo desde status.cfg
    bool cargarEstado(const std::string& ruta, Reproductor& rep);

    // Guarda el estado actual en status.cfg
    bool guardarEstado(const std::string& ruta, Reproductor& rep);

    // Carga el contador de reproducciones por canción desde song_ranking.txt
    // (formato: id,reproducciones por línea). Si el archivo no existe,
    // simplemente deja todos los contadores en 0 (biblioteca nueva).
    bool cargarRanking(const std::string& ruta, Reproductor& rep);

    // Guarda el contador de reproducciones de cada canción del catálogo
    bool guardarRanking(const std::string& ruta, Reproductor& rep);
}
