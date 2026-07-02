#include <iostream>
#include "classes/Reproductor.h"
#include "core/FileManager.h"
#include "core/MenuManager.h"

const std::string RUTA_CANCIONES = "music_source.txt";
const std::string RUTA_ESTADO    = "status.cfg";
const std::string RUTA_RANKING   = "song_ranking.txt";

int main() {
    Reproductor rep;

    // 1. Cargar catálogo de canciones
    bool hayCanciones = FileManager::cargarCanciones(RUTA_CANCIONES, rep);

    if (!hayCanciones) {
        std::cout << "Advertencia: no se encontro " << RUTA_CANCIONES
                  << ". El reproductor iniciara sin canciones." << std::endl;
    }

    // 2. Restaurar estado previo y ranking de reproducciones (si existen)
    if (hayCanciones) {
        FileManager::cargarEstado(RUTA_ESTADO, rep);
        FileManager::cargarRanking(RUTA_RANKING, rep);
        rep.reindexar(); // recalcula totales por artista con el ranking recien cargado
    }

    // 3. Bucle principal
    char opcion = ' ';
    do {
        opcion = MenuManager::mostrarMenuPrincipal(rep);

        switch (opcion) {
            case 'W':
                rep.reproducirPausar();
                FileManager::guardarEstado(RUTA_ESTADO, rep);
                FileManager::guardarRanking(RUTA_RANKING, rep);
                break;

            case 'Q':
                rep.anterior();
                FileManager::guardarEstado(RUTA_ESTADO, rep);
                FileManager::guardarRanking(RUTA_RANKING, rep);
                break;

            case 'E':
                rep.siguiente();
                FileManager::guardarEstado(RUTA_ESTADO, rep);
                FileManager::guardarRanking(RUTA_RANKING, rep);
                break;

            case 'S':
                rep.toggleAleatorio();
                FileManager::guardarEstado(RUTA_ESTADO, rep);
                break;

            case 'R':
                rep.toggleRepeticion();
                FileManager::guardarEstado(RUTA_ESTADO, rep);
                break;

            case 'A':
                MenuManager::menuListaReproduccion(rep, RUTA_ESTADO);
                break;

            case 'L':
                MenuManager::menuListadoCanciones(rep, RUTA_CANCIONES, RUTA_ESTADO);
                break;

            case 'F':
                MenuManager::menuBusqueda(rep, RUTA_ESTADO);
                break;

            case 'T':
                MenuManager::menuRankingTop(rep, RUTA_ESTADO);
                break;

            case 'X':
                FileManager::guardarEstado(RUTA_ESTADO, rep);
                FileManager::guardarRanking(RUTA_RANKING, rep);
                std::cout << "Saliendo..." << std::endl;
                break;

            default:
                // Opción inválida: el menú se refresca igual
                break;
        }

    } while (opcion != 'X');

    return 0;
}
