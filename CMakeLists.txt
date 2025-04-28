cmake_minimum_required(VERSION 3.16)        # 3.16 = VS 2019 & Ubuntu 20.04+

project(Morpion LANGUAGES C DESCRIPTION "Morpion 3x3 Deluxe")

# ──────────────────── Options / flags ────────────────────
# Active la génération compile_commands.json (utile à VS Code, clangd…)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Force le standard C99 partout, même si le compilo par défaut change
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Drapeaux communs
add_compile_options(
        $<$<C_COMPILER_ID:GNU,Clang>:
        -Wall -Wextra -Wpedantic -Wshadow -Wconversion
        >
        $<$<C_COMPILER_ID:MSVC>:
        /W4 /utf-8
        >
)

# ──────────────────── Sources ────────────────────────────
set(SRC
        morpion.c
)

add_executable(morpion ${SRC})

# ──────────────────── Definitions platform-specific ─────
# Permet de gérer la coloration ANSI (#ifdef _WIN32) dans le code
if (WIN32)
    target_compile_definitions(morpion PRIVATE PLATFORM_WINDOWS)
else()
    target_compile_definitions(morpion PRIVATE PLATFORM_POSIX)
endif()

# ──────────────────── Installation (optionnel) ───────────
install(TARGETS morpion RUNTIME DESTINATION bin)

