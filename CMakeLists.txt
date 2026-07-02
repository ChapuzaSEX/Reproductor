cmake_minimum_required(VERSION 3.14)
project(Reproductor CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(reproductor
    main.cpp
    classes/Cancion.cpp
    classes/Reproductor.cpp
    core/FileManager.cpp
    core/MenuManager.cpp
        classes/ArtistaEntry.cpp
        classes/ArtistaEntry.h
        data_structures/ArbolAVL.h
        data_structures/NodoAVL.h
        data_structures/NodoTrie.h
        data_structures/Trie.h
        data_structures/Heap.h
)

target_include_directories(reproductor PRIVATE .)
