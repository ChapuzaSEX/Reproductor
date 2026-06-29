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
}
