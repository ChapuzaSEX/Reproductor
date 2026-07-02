#include "Cancion.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cctype>

Cancion::Cancion(const std::string& id,
                 const std::string& nombre,
                 const std::string& artista,
                 const std::string& album,
                 const std::string& anio,
                 int duracion,
                 const std::string& ruta,
                 int reproducciones)
    : id(id), nombre(nombre), artista(artista),
      album(album), anio(anio), duracion(duracion), ruta(ruta),
      reproducciones(reproducciones) {}

std::string Cancion::getId()       const { return id; }
std::string Cancion::getNombre()   const { return nombre; }
std::string Cancion::getArtista()  const { return artista; }
std::string Cancion::getAlbum()    const { return album; }
std::string Cancion::getAnio()     const { return anio; }
int         Cancion::getDuracion() const { return duracion; }
std::string Cancion::getRuta()     const { return ruta; }
int         Cancion::getReproducciones() const { return reproducciones; }

void Cancion::incrementarReproducciones() { reproducciones++; }
void Cancion::setReproducciones(int valor) { reproducciones = valor; }

std::string Cancion::getDuracionFormateada() const {
    int min = duracion / 60;
    int seg = duracion % 60;
    std::ostringstream oss;
    oss << min << ":" << std::setw(2) << std::setfill('0') << seg;
    return oss.str();
}

std::string Cancion::serializar(char delimitador) const {
    std::ostringstream oss;
    oss << id        << delimitador
        << nombre    << delimitador
        << artista   << delimitador
        << album     << delimitador
        << anio      << delimitador
        << duracion  << delimitador
        << ruta;
    return oss.str();
}

void Cancion::imprimir() const {
    std::cout << nombre << " - " << artista
              << " [" << album << ", " << anio << "] ("
              << getDuracionFormateada() << ")" << std::endl;
}

// ── Comparadores ────────────────────────────────────────────────────────────

static std::string aMinusculas(const std::string& s) {
    std::string r = s;
    for (char& c : r) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return r;
}

int compararCancionPorNombre(Cancion* const& a, Cancion* const& b) {
    std::string na = aMinusculas(a->getNombre());
    std::string nb = aMinusculas(b->getNombre());
    if (na < nb) return -1;
    if (na > nb) return 1;
    // Desempate por id para que dos canciones con el mismo nombre no se
    // consideren la misma clave dentro del AVL.
    if (a->getId() < b->getId()) return -1;
    if (a->getId() > b->getId()) return 1;
    return 0;
}

bool cancionesSonIguales(Cancion* const& a, Cancion* const& b) {
    return a->getId() == b->getId();
}
