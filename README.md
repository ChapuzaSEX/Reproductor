# Reproductor de Música — Taller 2 (Estructuras de Datos)

## Integrantes
- Dylan Cortes
- Joaquin Cabrera

## Sobre el Proyecto
Reproductor de música por consola, desarrollado en C++ sin librerías STL de
contenedores. Sobre la base del Taller 1 (listas, colas y pilas propias) se
agregaron tres funcionalidades nuevas usando árboles implementados a mano:
búsqueda de canciones por subcadena (Trie), listado de canciones por artista
en orden alfabético (AVL) y ranking TOP 10 de canciones/artistas más
escuchados (Heap). El contador de reproducciones se persiste en un archivo
independiente (`song_ranking.txt`), separado de la biblioteca
(`music_source.txt`).

## Diagrama de clases 



## Compilación y ejecución



###  g++ directo
```bash
g++ -std=c++17 -Wall -o reproductor main.cpp classes/*.cpp core/*.cpp -I.
./reproductor
```

El ejecutable debe correrse desde la carpeta donde estén (o se generarán)
`music_source.txt`, `status.cfg` y `song_ranking.txt`.

## Funcionamiento

Al iniciar, el programa carga la biblioteca desde `music_source.txt`, el
estado previo desde `status.cfg` y el contador de reproducciones desde
`song_ranking.txt` (si no existen, arranca vacío/en 0 sin error).

Menú principal:
```
W - Reproducir/Pausar        Q - Pista anterior       E - Pista siguiente
S - Modo aleatorio            R - Modo repetición       A - Lista de reproducción
L - Listado de canciones      F - Buscar canciones      T - TOP 10 artistas y canciones
X - Salir
```

- **F (Búsqueda):** ingresa un texto y muestra todas las canciones cuyo
  nombre o artista lo contiene en cualquier posición (usa un Trie de
  sufijos). Se puede reproducir, agregar a la cola, repetir la búsqueda con
  otro texto o volver.
- **T (TOP 10):** muestra el ranking de canciones más escuchadas (Heap) y
  el de artistas más escuchados (Heap, sumando las reproducciones de sus
  canciones). Desde el TOP 10 de artistas se puede ver el listado alfabético
  de canciones de un artista (AVL).
- Agregar o eliminar canciones desde el listado (`L`) actualiza
  automáticamente los índices de búsqueda y de artistas.

## Estructura del proyecto
```
data_structures/   Lista, Cola, Pila, ArbolAVL, Trie, Heap y sus nodos (genéricos, sin STL)
classes/           Cancion, ArtistaEntry, Reproductor (lógica de dominio)
core/              FileManager (persistencia), MenuManager (interfaz de consola)
main.cpp           Punto de entrada
```

## Archivos de datos
- `music_source.txt` — biblioteca de canciones (CSV: id,nombre,artista,album,anio,duracion,ruta)
- `status.cfg` — estado de reproducción (canción actual, modos, cola pendiente)
- `song_ranking.txt` — contador de reproducciones por canción (CSV: id,reproducciones)
