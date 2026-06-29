#include "Cancion.h"
#include <iostream>
#include <sstream>
#include <iomanip>

Cancion::Cancion(const std::string& id,
                 const std::string& nombre,
                 const std::string& artista,
                 const std::string& album,
                 const std::string& anio,
                 int duracion,
                 const std::string& ruta)
    : id(id), nombre(nombre), artista(artista),
      album(album), anio(anio), duracion(duracion), ruta(ruta) {}

std::string Cancion::getId()       const { return id; }
std::string Cancion::getNombre()   const { return nombre; }
std::string Cancion::getArtista()  const { return artista; }
std::string Cancion::getAlbum()    const { return album; }
std::string Cancion::getAnio()     const { return anio; }
int         Cancion::getDuracion() const { return duracion; }
std::string Cancion::getRuta()     const { return ruta; }

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
