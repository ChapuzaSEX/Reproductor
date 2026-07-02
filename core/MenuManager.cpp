#include "MenuManager.h"
#include "FileManager.h"
#include <iostream>
#include <string>
#include <cctype>
#include <limits>

namespace MenuManager {

// Archivo independiente donde se persiste el contador de reproducciones
// por canción (ver FileManager::guardarRanking / cargarRanking).
static const std::string RUTA_RANKING = "song_ranking.txt";

// Guarda en conjunto el estado de reproducción y el ranking de reproducciones.
// Se usa cada vez que una acción puede haber disparado una nueva reproducción.
static void guardarProgreso(Reproductor& rep, const std::string& rutaEstado) {
    FileManager::guardarEstado(rutaEstado, rep);
    FileManager::guardarRanking(RUTA_RANKING, rep);
}

// Submenús del ranking TOP 10 (internos, se llaman entre sí)
static void menuTop10Canciones(Reproductor& rep, const std::string& rutaEstado);
static void menuTop10Artistas(Reproductor& rep, const std::string& rutaEstado);
static bool menuCancionesPorArtista(Reproductor& rep, const std::string& rutaEstado, ArtistaEntry* artista);

void limpiarConsola() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

char mostrarMenuPrincipal(Reproductor& rep) {
    limpiarConsola();
    rep.mostrarEncabezado();

    std::cout << "\nOpciones:" << std::endl;
    std::cout << " W - Reproducir/Pausar"   << std::endl;
    if (!rep.catalogoVacio()) {
        std::cout << " Q - Pista Anterior"      << std::endl;
        std::cout << " E - Pista Siguiente"      << std::endl;
    }
    std::cout << " S - Activar/Desactivar modo aleatorio" << std::endl;
    std::cout << " R - Repeticion (Desactivado/Repetir una/Repetir todas)" << std::endl;
    std::cout << " A - Ver lista de reproduccion actual"  << std::endl;
    std::cout << " L - Listado de canciones"              << std::endl;
    std::cout << " F - Buscar canciones"                  << std::endl;
    std::cout << " T - TOP 10 Artistas y Canciones"       << std::endl;
    std::cout << " X - Salir"                             << std::endl;
    std::cout << "\nIngrese Opcion: ";

    char op;
    std::cin >> op;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return static_cast<char>(std::toupper(op));
}

void menuListaReproduccion(Reproductor& rep, const std::string& rutaEstado) {
    while (true) {
        limpiarConsola();
        Cancion* actual = rep.getCancionActual();

        // Encabezado
        if (actual != nullptr)
            std::cout << "Actual: " << actual->getNombre()
                      << " - " << actual->getArtista() << std::endl;
        else
            std::cout << "Actual: (ninguna)" << std::endl;

        std::cout << "\nLista de reproduccion actual:" << std::endl;

        Cola<Cancion*>& cola = rep.getPendientes();
        if (cola.estaVacia()) {
            std::cout << "  Vacia" << std::endl;
            std::cout << "\nOpciones:\n V - Volver al menu principal\n\nIngrese Opcion: ";
            char op;
            std::cin >> op;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (std::toupper(op) == 'V') return;
            continue;
        }

        // Listar pendientes
        Nodo<Cancion*>* nodo = cola.getFrente();
        int i = 1;
        while (nodo != nullptr) {
            std::cout << "  " << i << ". "
                      << nodo->dato->getNombre()
                      << " - " << nodo->dato->getArtista() << std::endl;
            nodo = nodo->siguiente;
            i++;
        }

        std::cout << "\nOpciones:"      << std::endl;
        std::cout << " S<num> - Saltar a la cancion seleccionada" << std::endl;
        std::cout << " V      - Volver al menu principal"         << std::endl;
        std::cout << "\nIngrese Opcion: ";

        std::string entrada;
        std::getline(std::cin, entrada);
        if (entrada.empty()) continue;

        char primera = static_cast<char>(std::toupper(entrada[0]));

        if (primera == 'V') return;

        if (primera == 'S' && entrada.size() > 1) {
            try {
                int pos = std::stoi(entrada.substr(1));
                rep.saltarA(pos);
                FileManager::guardarEstado(rutaEstado, rep);
                return; // volver al menú principal
            } catch (...) {
                std::cout << "Posicion invalida." << std::endl;
            }
        } else {
            std::cout << "Opcion no reconocida." << std::endl;
        }
    }
}

void agregarCancionNueva(Reproductor& rep, const std::string& rutaCanciones) {
    limpiarConsola();
    std::cout << "=== AGREGAR NUEVA CANCION ===" << std::endl;

    // Generar nuevo ID basado en el total actual
    std::string id = std::to_string(rep.totalCanciones() + 1);

    std::string nombre, artista, album, anio, durStr, ruta;

    std::cout << "Nombre de la cancion: ";  std::getline(std::cin, nombre);
    std::cout << "Artista: ";               std::getline(std::cin, artista);
    std::cout << "Album: ";                 std::getline(std::cin, album);
    std::cout << "Anio: ";                  std::getline(std::cin, anio);
    std::cout << "Duracion (segundos): ";   std::getline(std::cin, durStr);
    std::cout << "Ruta del archivo: ";      std::getline(std::cin, ruta);

    int duracion = 0;
    try { duracion = std::stoi(durStr); } catch (...) { duracion = 0; }

    Cancion* nueva = new Cancion(id, nombre, artista, album, anio, duracion, ruta);
    rep.agregarCancion(nueva);
    FileManager::guardarCanciones(rutaCanciones, rep);

    std::cout << "Cancion agregada correctamente." << std::endl;
    std::cout << "Presione Enter para continuar...";
    std::cin.get();
}

void menuListadoCanciones(Reproductor& rep,
                          const std::string& rutaCanciones,
                          const std::string& rutaEstado) {
    while (true) {
        limpiarConsola();
        Cancion* actual = rep.getCancionActual();
        if (actual != nullptr)
            std::cout << "Actual: " << actual->getNombre()
                      << " - " << actual->getArtista() << std::endl;

        std::cout << "\nCanciones registradas:" << std::endl;
        Lista<Cancion*>& cat = rep.getCatalogo();
        Nodo<Cancion*>* nodo = cat.getCabeza();
        int i = 1;
        while (nodo != nullptr) {
            std::cout << "  " << i << ". ";
            nodo->dato->imprimir();
            nodo = nodo->siguiente;
            i++;
        }

        std::cout << "\nOpciones:"                                                   << std::endl;
        std::cout << " R<num> - Reproducir cancion seleccionada"                     << std::endl;
        std::cout << " A<num> - Agregar cancion al final de la lista de reproduccion"<< std::endl;
        std::cout << " N      - Agregar cancion al registro"                         << std::endl;
        std::cout << " D<num> - Eliminar cancion seleccionada"                       << std::endl;
        std::cout << " V      - Volver al menu principal"                            << std::endl;
        std::cout << "\nIngrese Opcion: ";

        std::string entrada;
        std::getline(std::cin, entrada);
        if (entrada.empty()) continue;

        char primera = static_cast<char>(std::toupper(entrada[0]));

        if (primera == 'V') return;

        if (primera == 'N') {
            agregarCancionNueva(rep, rutaCanciones);
            continue;
        }

        if (entrada.size() > 1) {
            int num = -1;
            try { num = std::stoi(entrada.substr(1)); } catch (...) {}

            if (num < 1 || num > cat.getTamano()) {
                std::cout << "Numero invalido." << std::endl;
                std::cout << "Presione Enter..."; std::cin.get();
                continue;
            }

            Cancion* elegida = cat.obtener(num - 1); // 0-based

            if (primera == 'R') {
                rep.reproducirDirecta(elegida);
                FileManager::guardarEstado(rutaEstado, rep);
                return;
            } else if (primera == 'A') {
                rep.agregarAlFinalDeCola(elegida);
                FileManager::guardarEstado(rutaEstado, rep);
                std::cout << "Cancion agregada a la cola." << std::endl;
                std::cout << "Presione Enter..."; std::cin.get();
            } else if (primera == 'D') {
                std::string idEliminar = elegida->getId();
                rep.eliminarCancion(idEliminar);
                FileManager::guardarCanciones(rutaCanciones, rep);
                FileManager::guardarEstado(rutaEstado, rep);
                std::cout << "Cancion eliminada." << std::endl;
                std::cout << "Presione Enter..."; std::cin.get();
            } else {
                std::cout << "Opcion no reconocida." << std::endl;
                std::cout << "Presione Enter..."; std::cin.get();
            }
        } else {
            std::cout << "Opcion no reconocida." << std::endl;
            std::cout << "Presione Enter..."; std::cin.get();
        }
    }
}

// ── Submenú F: Búsqueda de canciones (Trie) ──────────────────────────────────

void menuBusqueda(Reproductor& rep, const std::string& rutaEstado) {
    while (true) {
        limpiarConsola();
        std::cout << "Busqueda de canciones" << std::endl;
        std::cout << "\nBuscar canciones que contengan: ";

        std::string texto;
        std::getline(std::cin, texto);
        if (texto.empty()) return; // sin texto -> volver al menu principal

        Lista<Cancion*> resultados = rep.buscarCanciones(texto);

        while (true) {
            limpiarConsola();
            std::cout << "Busqueda de canciones" << std::endl;
            std::cout << "\nCanciones que contienen \"" << texto << "\":" << std::endl;

            if (resultados.estaVacia()) {
                std::cout << "  No se encontraron coincidencias." << std::endl;
            } else {
                Nodo<Cancion*>* nodo = resultados.getCabeza();
                int i = 1;
                while (nodo != nullptr) {
                    std::cout << "  " << i << ". " << nodo->dato->getNombre()
                              << " - " << nodo->dato->getArtista() << std::endl;
                    nodo = nodo->siguiente;
                    i++;
                }
            }

            std::cout << "\nOpciones:" << std::endl;
            std::cout << " R<num> - Reproducir cancion seleccionada" << std::endl;
            std::cout << " A<num> - Agregar cancion seleccionada al final de la lista de reproduccion actual" << std::endl;
            std::cout << " F      - Repetir busqueda con un texto diferente" << std::endl;
            std::cout << " V      - Volver al menu principal" << std::endl;
            std::cout << "\nIngrese Opcion: ";

            std::string entrada;
            std::getline(std::cin, entrada);
            if (entrada.empty()) continue;

            char primera = static_cast<char>(std::toupper(entrada[0]));

            if (primera == 'V') return;
            if (primera == 'F' && entrada.size() == 1) break; // volver a pedir texto

            if (entrada.size() > 1 && (primera == 'R' || primera == 'A')) {
                int num = -1;
                try { num = std::stoi(entrada.substr(1)); } catch (...) {}

                if (num < 1 || num > resultados.getTamano()) {
                    std::cout << "Numero invalido." << std::endl;
                    std::cout << "Presione Enter..."; std::cin.get();
                    continue;
                }

                Cancion* elegida = resultados.obtener(num - 1);

                if (primera == 'R') {
                    rep.reproducirDirecta(elegida);
                    guardarProgreso(rep, rutaEstado);
                    return;
                } else {
                    rep.agregarAlFinalDeCola(elegida);
                    guardarProgreso(rep, rutaEstado);
                    std::cout << "Cancion agregada a la cola." << std::endl;
                    std::cout << "Presione Enter..."; std::cin.get();
                }
            } else {
                std::cout << "Opcion no reconocida." << std::endl;
                std::cout << "Presione Enter..."; std::cin.get();
            }
        }
        // "F" presionado: vuelve al inicio del ciclo externo a pedir nuevo texto
    }
}

// ── Submenú T: Ranking TOP 10 (Heap) ─────────────────────────────────────────

void menuRankingTop(Reproductor& rep, const std::string& rutaEstado) {
    while (true) {
        limpiarConsola();
        std::cout << "Ranking TOP" << std::endl;
        std::cout << "\nOpciones:" << std::endl;
        std::cout << " C - Top 10 canciones mas escuchadas" << std::endl;
        std::cout << " A - Top 10 artistas mas escuchados"  << std::endl;
        std::cout << " X - Salir" << std::endl;
        std::cout << "\nIngrese Opcion: ";

        char op;
        std::cin >> op;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        op = static_cast<char>(std::toupper(op));

        if (op == 'X') return;
        if (op == 'C') { menuTop10Canciones(rep, rutaEstado); return; }
        if (op == 'A') { menuTop10Artistas(rep, rutaEstado); return; }
        // opcion invalida: se vuelve a mostrar el menu
    }
}

static void menuTop10Canciones(Reproductor& rep, const std::string& rutaEstado) {
    while (true) {
        limpiarConsola();
        Lista<Cancion*> top = rep.top10Canciones();

        std::cout << "Ranking TOP 10 Canciones mas escuchadas:" << std::endl;
        if (top.estaVacia()) {
            std::cout << "  (sin canciones registradas)" << std::endl;
        } else {
            Nodo<Cancion*>* nodo = top.getCabeza();
            int i = 1;
            while (nodo != nullptr) {
                std::cout << "  " << i << ". [" << nodo->dato->getReproducciones() << "] "
                          << nodo->dato->getNombre() << " - " << nodo->dato->getArtista() << std::endl;
                nodo = nodo->siguiente;
                i++;
            }
        }

        std::cout << "\nOpciones:" << std::endl;
        std::cout << " R<num> - Reproducir cancion seleccionada" << std::endl;
        std::cout << " A<num> - Agregar cancion seleccionada al final de la lista de reproduccion actual" << std::endl;
        std::cout << " A      - Top 10 artistas mas escuchados" << std::endl;
        std::cout << " V      - Volver al menu principal" << std::endl;
        std::cout << "\nIngrese Opcion: ";

        std::string entrada;
        std::getline(std::cin, entrada);
        if (entrada.empty()) continue;

        char primera = static_cast<char>(std::toupper(entrada[0]));

        if (primera == 'V') return;
        if (primera == 'A' && entrada.size() == 1) { menuTop10Artistas(rep, rutaEstado); return; }

        if (entrada.size() > 1 && (primera == 'R' || primera == 'A')) {
            int num = -1;
            try { num = std::stoi(entrada.substr(1)); } catch (...) {}

            if (num < 1 || num > top.getTamano()) {
                std::cout << "Numero invalido." << std::endl;
                std::cout << "Presione Enter..."; std::cin.get();
                continue;
            }

            Cancion* elegida = top.obtener(num - 1);

            if (primera == 'R') {
                rep.reproducirDirecta(elegida);
                guardarProgreso(rep, rutaEstado);
                return;
            } else {
                rep.agregarAlFinalDeCola(elegida);
                guardarProgreso(rep, rutaEstado);
                std::cout << "Cancion agregada a la cola." << std::endl;
                std::cout << "Presione Enter..."; std::cin.get();
            }
        } else {
            std::cout << "Opcion no reconocida." << std::endl;
            std::cout << "Presione Enter..."; std::cin.get();
        }
    }
}

static void menuTop10Artistas(Reproductor& rep, const std::string& rutaEstado) {
    while (true) {
        limpiarConsola();
        Lista<ArtistaEntry*> top = rep.top10Artistas();

        std::cout << "Ranking TOP 10 Artistas mas escuchados:" << std::endl;
        if (top.estaVacia()) {
            std::cout << "  (sin artistas registrados)" << std::endl;
        } else {
            Nodo<ArtistaEntry*>* nodo = top.getCabeza();
            int i = 1;
            while (nodo != nullptr) {
                std::cout << "  " << i << ". [" << nodo->dato->getReproduccionesTotales() << "] "
                          << nodo->dato->getNombreArtista() << std::endl;
                nodo = nodo->siguiente;
                i++;
            }
        }

        std::cout << "\nOpciones:" << std::endl;
        std::cout << " S<num> - Mostrar canciones del artista" << std::endl;
        std::cout << " C      - Top 10 canciones mas escuchadas" << std::endl;
        std::cout << " V      - Volver al menu principal" << std::endl;
        std::cout << "\nIngrese Opcion: ";

        std::string entrada;
        std::getline(std::cin, entrada);
        if (entrada.empty()) continue;

        char primera = static_cast<char>(std::toupper(entrada[0]));

        if (primera == 'V') return;
        if (primera == 'C' && entrada.size() == 1) { menuTop10Canciones(rep, rutaEstado); return; }

        if (primera == 'S' && entrada.size() > 1) {
            int num = -1;
            try { num = std::stoi(entrada.substr(1)); } catch (...) {}

            if (num < 1 || num > top.getTamano()) {
                std::cout << "Numero invalido." << std::endl;
                std::cout << "Presione Enter..."; std::cin.get();
                continue;
            }

            ArtistaEntry* artista = top.obtener(num - 1);
            bool irAlMenuPrincipal = menuCancionesPorArtista(rep, rutaEstado, artista);
            if (irAlMenuPrincipal) return;
            // si no, se vuelve a mostrar el TOP 10 de artistas (continua el ciclo)
        } else {
            std::cout << "Opcion no reconocida." << std::endl;
            std::cout << "Presione Enter..."; std::cin.get();
        }
    }
}

// Retorna true si hay que volver directo al menu principal (X),
// false si hay que volver al listado de TOP 10 artistas (V).
static bool menuCancionesPorArtista(Reproductor& rep, const std::string& rutaEstado, ArtistaEntry* artista) {
    while (true) {
        limpiarConsola();
        Lista<Cancion*> canciones = artista->getCancionesOrdenadas();

        std::cout << "Ranking TOP 10 Artistas mas escuchados:" << std::endl;
        std::cout << "\nArtista: " << artista->getNombreArtista() << std::endl;

        if (canciones.estaVacia()) {
            std::cout << "  (sin canciones registradas)" << std::endl;
        } else {
            Nodo<Cancion*>* nodo = canciones.getCabeza();
            int i = 1;
            while (nodo != nullptr) {
                std::cout << "  " << i << ". " << nodo->dato->getNombre() << std::endl;
                nodo = nodo->siguiente;
                i++;
            }
        }

        std::cout << "\nOpciones:" << std::endl;
        std::cout << " R<num> - Reproducir cancion seleccionada" << std::endl;
        std::cout << " A<num> - Agregar cancion seleccionada al final de la lista de reproduccion actual" << std::endl;
        std::cout << " V      - Volver al listado de TOP 10 artistas" << std::endl;
        std::cout << " X      - Volver al menu principal" << std::endl;
        std::cout << "\nIngrese Opcion: ";

        std::string entrada;
        std::getline(std::cin, entrada);
        if (entrada.empty()) continue;

        char primera = static_cast<char>(std::toupper(entrada[0]));

        if (primera == 'V') return false;
        if (primera == 'X') return true;

        if (entrada.size() > 1 && (primera == 'R' || primera == 'A')) {
            int num = -1;
            try { num = std::stoi(entrada.substr(1)); } catch (...) {}

            if (num < 1 || num > canciones.getTamano()) {
                std::cout << "Numero invalido." << std::endl;
                std::cout << "Presione Enter..."; std::cin.get();
                continue;
            }

            Cancion* elegida = canciones.obtener(num - 1);

            if (primera == 'R') {
                rep.reproducirDirecta(elegida);
                guardarProgreso(rep, rutaEstado);
                return true; // al reproducir, se muestra la pantalla principal
            } else {
                rep.agregarAlFinalDeCola(elegida);
                guardarProgreso(rep, rutaEstado);
                std::cout << "Cancion agregada a la cola." << std::endl;
                std::cout << "Presione Enter..."; std::cin.get();
            }
        } else {
            std::cout << "Opcion no reconocida." << std::endl;
            std::cout << "Presione Enter..."; std::cin.get();
        }
    }
}

} // namespace MenuManager
