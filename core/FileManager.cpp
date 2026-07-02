#include "FileManager.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace FileManager {

// Detecta el delimitador probando cuál de los candidatos aparece en la línea
char detectarDelimitador(const std::string& linea) {
    const char candidatos[] = {',', ';', ':', '.', '-'};
    for (char c : candidatos) {
        // Contar ocurrencias: una línea válida tiene exactamente 6 campos → 5 delimitadores
        int count = 0;
        for (char ch : linea) if (ch == c) count++;
        if (count >= 5) return c;
    }
    return ','; // fallback
}

bool cargarCanciones(const std::string& ruta, Reproductor& rep) {
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) return false;

    std::string linea;
    char delim = ',';
    bool primeraLinea = true;

    while (std::getline(archivo, linea)) {
        if (linea.empty()) continue;

        // Detectar delimitador en la primera línea válida
        if (primeraLinea) {
            delim = detectarDelimitador(linea);
            primeraLinea = false;
        }

        std::istringstream ss(linea);
        std::string id, nombre, artista, album, anio, durStr, ruta_arch;

        std::getline(ss, id,        delim);
        std::getline(ss, nombre,    delim);
        std::getline(ss, artista,   delim);
        std::getline(ss, album,     delim);
        std::getline(ss, anio,      delim);
        std::getline(ss, durStr,    delim);
        std::getline(ss, ruta_arch, delim);

        int duracion = 0;
        try { duracion = std::stoi(durStr); } catch (...) { duracion = 0; }

        rep.agregarCancion(new Cancion(id, nombre, artista, album, anio, duracion, ruta_arch));
    }

    archivo.close();
    return !rep.catalogoVacio();
}

bool guardarCanciones(const std::string& ruta, Reproductor& rep) {
    std::ofstream archivo(ruta);
    if (!archivo.is_open()) return false;

    Nodo<Cancion*>* nodo = rep.getCatalogo().getCabeza();
    while (nodo != nullptr) {
        archivo << nodo->dato->serializar(',') << "\n";
        nodo = nodo->siguiente;
    }

    archivo.close();
    return true;
}

bool cargarEstado(const std::string& ruta, Reproductor& rep) {
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) return false;

    std::string linea;
    std::string idActual;
    bool reproduciendo = false;
    bool aleatorio = false;
    int repeticion = 0;
    std::string pendientesIds;  // IDs separados por "|"

    while (std::getline(archivo, linea)) {
        if (linea.empty()) continue;
        auto pos = linea.find('=');
        if (pos == std::string::npos) continue;

        std::string clave = linea.substr(0, pos);
        std::string valor = linea.substr(pos + 1);

        if      (clave == "id_actual")     idActual = valor;
        else if (clave == "reproduciendo") reproduciendo = (valor == "1");
        else if (clave == "aleatorio")     aleatorio = (valor == "1");
        else if (clave == "repeticion")    try { repeticion = std::stoi(valor); } catch (...) {}
        else if (clave == "pendientes")    pendientesIds = valor;
    }
    archivo.close();

    // Restaurar canción actual
    if (!idActual.empty()) {
        Nodo<Cancion*>* nodo = rep.getCatalogo().getCabeza();
        while (nodo != nullptr) {
            if (nodo->dato->getId() == idActual) {
                rep.reproducirDirecta(nodo->dato);
                break;
            }
            nodo = nodo->siguiente;
        }
    }

    // Restaurar modos (toggles desde estado DESACTIVADO)
    if (aleatorio) rep.toggleAleatorio();
    for (int i = 0; i < repeticion; i++) rep.toggleRepeticion();

    // Restaurar cola de pendientes en orden guardado
    if (!pendientesIds.empty()) {
        rep.getPendientes().limpiar();
        std::istringstream ss(pendientesIds);
        std::string pid;
        while (std::getline(ss, pid, '|')) {
            if (pid.empty()) continue;
            Nodo<Cancion*>* nodo = rep.getCatalogo().getCabeza();
            while (nodo != nullptr) {
                if (nodo->dato->getId() == pid) {
                    rep.agregarAlFinalDeCola(nodo->dato);
                    break;
                }
                nodo = nodo->siguiente;
            }
        }
    }

    if (!reproduciendo && rep.getCancionActual() != nullptr)
        rep.reproducirPausar(); // deja en pausa

    return true;
}

bool guardarEstado(const std::string& ruta, Reproductor& rep) {
    std::ofstream archivo(ruta);
    if (!archivo.is_open()) return false;

    Cancion* actual = rep.getCancionActual();
    archivo << "id_actual="     << (actual ? actual->getId() : "")    << "\n";
    archivo << "reproduciendo=" << (rep.getEstado() == EstadoReproduccion::REPRODUCIENDO ? "1" : "0") << "\n";
    archivo << "aleatorio="     << (rep.getModoAleatorio() ? "1" : "0") << "\n";

    int repVal = 0;
    if      (rep.getModoRepeticion() == ModoRepeticion::REPETIR_UNA)   repVal = 1;
    else if (rep.getModoRepeticion() == ModoRepeticion::REPETIR_TODAS) repVal = 2;
    archivo << "repeticion=" << repVal << "\n";

    // Guardar IDs de canciones pendientes separados por "|"
    archivo << "pendientes=";
    Nodo<Cancion*>* nodo = rep.getPendientes().getFrente();
    bool primero = true;
    while (nodo != nullptr) {
        if (!primero) archivo << "|";
        archivo << nodo->dato->getId();
        nodo = nodo->siguiente;
        primero = false;
    }
    archivo << "\n";

    archivo.close();
    return true;
}

bool cargarRanking(const std::string& ruta, Reproductor& rep) {
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) return false; // biblioteca nueva: contadores en 0, no es un error

    std::string linea;
    while (std::getline(archivo, linea)) {
        if (linea.empty()) continue;
        auto pos = linea.find(',');
        if (pos == std::string::npos) continue;

        std::string id = linea.substr(0, pos);
        std::string valorStr = linea.substr(pos + 1);
        int reproducciones = 0;
        try { reproducciones = std::stoi(valorStr); } catch (...) { continue; }

        Nodo<Cancion*>* nodo = rep.getCatalogo().getCabeza();
        while (nodo != nullptr) {
            if (nodo->dato->getId() == id) {
                nodo->dato->setReproducciones(reproducciones);
                break;
            }
            nodo = nodo->siguiente;
        }
    }
    archivo.close();
    return true;
}

bool guardarRanking(const std::string& ruta, Reproductor& rep) {
    std::ofstream archivo(ruta);
    if (!archivo.is_open()) return false;

    Nodo<Cancion*>* nodo = rep.getCatalogo().getCabeza();
    while (nodo != nullptr) {
        archivo << nodo->dato->getId() << "," << nodo->dato->getReproducciones() << "\n";
        nodo = nodo->siguiente;
    }
    archivo.close();
    return true;
}

} // namespace FileManager
