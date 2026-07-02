#pragma once
#include "../classes/Reproductor.h"
#include <string>

// MenuManager agrupa todos los submenús del reproductor.
// Separar menús en su propio módulo hace que main.cpp
// sea conciso y cada menú sea testeable por separado.
namespace MenuManager {

    // Limpia la consola (cross-platform)
    void limpiarConsola();

    // Muestra el encabezado + menú principal y retorna la opción elegida
    char mostrarMenuPrincipal(Reproductor& rep);

    // Submenú A: lista de reproducción actual
    void menuListaReproduccion(Reproductor& rep, const std::string& rutaEstado);

    // Submenú L: listado de canciones
    void menuListadoCanciones(Reproductor& rep,
                              const std::string& rutaCanciones,
                              const std::string& rutaEstado);

    // Solicita datos de una canción nueva por consola y la agrega
    void agregarCancionNueva(Reproductor& rep,
                             const std::string& rutaCanciones);

    // Submenú F: búsqueda de canciones por nombre y/o artista (Trie)
    void menuBusqueda(Reproductor& rep, const std::string& rutaEstado);

    // Submenú T: ranking TOP 10 (Heap) — pantalla de entrada C/A/X
    void menuRankingTop(Reproductor& rep, const std::string& rutaEstado);
}
