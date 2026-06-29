#include "MenuManager.h"
#include "FileManager.h"
#include <iostream>
#include <string>
#include <cctype>
#include <limits>

namespace MenuManager {

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

} // namespace MenuManager
