#include "ArtistaEntry.h"
#include <cctype>

ArtistaEntry::ArtistaEntry(const std::string& nombreArtista)
    : nombreArtista(nombreArtista),
      canciones(compararCancionPorNombre),
      reproduccionesTotales(0) {}

std::string ArtistaEntry::getNombreArtista() const { return nombreArtista; }

void ArtistaEntry::agregarCancion(Cancion* c) {
    canciones.insertar(c);
    reproduccionesTotales += c->getReproducciones();
}

Lista<Cancion*> ArtistaEntry::getCancionesOrdenadas() const {
    return canciones.enOrden();
}

int ArtistaEntry::getTotalCanciones() const { return canciones.getTamano(); }

int ArtistaEntry::getReproduccionesTotales() const { return reproduccionesTotales; }

void ArtistaEntry::sumarReproduccion() { reproduccionesTotales++; }

// ── Comparador ───────────────────────────────────────────────────────────────

static std::string aMinusculas(const std::string& s) {
    std::string r = s;
    for (char& c : r) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return r;
}

int compararArtistaPorNombre(ArtistaEntry* const& a, ArtistaEntry* const& b) {
    std::string na = aMinusculas(a->getNombreArtista());
    std::string nb = aMinusculas(b->getNombreArtista());
    if (na < nb) return -1;
    if (na > nb) return 1;
    return 0;
}
