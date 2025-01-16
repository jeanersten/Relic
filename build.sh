#!/bin/bash

# DIRECTORIES
SOURCE_DIR="src"
BINARY_DEBUG_DIR="build/debug/bin"
BINARY_RELEASE_DIR="build/release/bin"
OBJECT_DEBUG_DIR="build/debug/obj"
OBJECT_RELEASE_DIR="build/release/obj"
SFML_DIR="vendor/SFML"

# FILES
SOURCE_FILES=$(find "$SOURCE_DIR" -name "*.cpp")
OBJECT_FILES_DEBUG=($(for file in "${SOURCE_FILES[@]}"; do echo "$OBJECT_DEBUG_DIR/$(basename "${file%.*}.o")"; done))
OBJECT_FILES_RELEASE=($(for file in "${SOURCE_FILES[@]}"; do echo "$OBJECT_RELEASE_DIR/$(basename "${file%.*}.o")"; done))

# DEFINE COMMAND
CXX="g++"

# COMPILER FLAGS
INCLUDE_FLAGS="-Ivendor/SFML/include -Isrc"
LIBRARY_FLAGS="-Lvendor/SFML/lib"
LIBRARY_FLAGS_DEBUG="$LIBRARY_FLAGS -lstdc++exp \
                                    -lsfml-graphics-d -lsfml-window-d -lsfml-system-d -lsfml-audio-d -lsfml-network-d"

LIBRARY_FLAGS_RELEASE="$LIBRARY_FLAGS -lstdc++exp \
                                      -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network"
CXX_FLAGS="-m64 -pedantic-errors -std=c++23 -Wall -Wextra -Wsign-conversion $INCLUDE_FLAGS"
CXX_FLAGS_DEBUG="-ggdb -DDEBUG_BUILD"
CXX_FLAGS_RELEASE="-O2 -DNDEBUG -DRELEASE_BUILD"

# EXECUTABLE TARGET
TARGET="Relic"
TARGET_DEBUG="$BINARY_DEBUG_DIR/$TARGET"
TARGET_RELEASE="$BINARY_RELEASE_DIR/$TARGET"

# TEXT COLOR
COLOR_BLUE="\033[1;36m"
COLOR_GREEN="\033[1;32m"
COLOR_ORANGE="\033[1;38;5;214m"
COLOR_PURPLE="\033[1;35m"
COLOR_RESET="\033[0m"

# FOR LOGGING
function log() {
    echo -e "$1"
}

# CLEAN BUILD FILES
function clean() {
    log "${COLOR_BLUE}Cleaning all build and binary files...${COLOR_RESET}"

    for obj in "$OBJECT_DEBUG_DIR"/*; do
        if [ -f "$obj" ]; then
            log "Removing object file: $obj"
            rm -f "$obj"
        fi
    done
    for obj in "$OBJECT_RELEASE_DIR"/*; do
        if [ -f "$obj" ]; then
            log "Removing object file: $obj"
            rm -f "$obj"
        fi
    done

    for bin in "$BINARY_DEBUG_DIR"/*; do
        if [ -f "$bin" ]; then
            log "Removing binary file: $bin"
            rm -f "$bin"
        fi
    done
    for bin in "$BINARY_RELEASE_DIR"/*; do
        if [ -f "$bin" ]; then
            log "Removing binary file: $bin"
            rm -f "$bin"
        fi
    done

    log "${COLOR_PURPLE}All build and binary files have been cleaned.${COLOR_RESET}"
}

# BUILD FOR DEBUG
function build_debug() {
    log "${COLOR_BLUE}Building for debug...${COLOR_RESET}"
    mkdir -p "$OBJECT_DEBUG_DIR" "$BINARY_DEBUG_DIR"

    for src in "${SOURCE_FILES[@]}"; do
        obj="$OBJECT_DEBUG_DIR/$(basename "${src%.*}.o")"
        log "Compiling $src to $obj..."
        $CXX $CXX_FLAGS $CXX_FLAGS_DEBUG -c "$src" -o "$obj" || { log "${COLOR_ORANGE}Debug build failed.${COLOR_RESET}"; exit 1; }
    done

    cp $SFML_DIR/bin/openal32.dll $BINARY_DEBUG_DIR/
    cp $SFML_DIR/bin/sfml-audio-d-2.dll $BINARY_DEBUG_DIR/
    cp $SFML_DIR/bin/sfml-graphics-d-2.dll $BINARY_DEBUG_DIR/
    cp $SFML_DIR/bin/sfml-network-d-2.dll $BINARY_DEBUG_DIR/
    cp $SFML_DIR/bin/sfml-system-d-2.dll $BINARY_DEBUG_DIR/
    cp $SFML_DIR/bin/sfml-window-d-2.dll $BINARY_DEBUG_DIR/

    log "Linking all debug files to executable..."
    $CXX $CXX_FLAGS $CXX_FLAGS_DEBUG -o "$TARGET_DEBUG" "${OBJECT_FILES_DEBUG[@]}" $LIBRARY_FLAGS_DEBUG || { log "${COLOR_ORANGE}Debug build failed.${COLOR_RESET}"; exit 1; }
    log "${COLOR_GREEN}Debug build succeeded. Executable created at $BINARY_DEBUG_DIR.${COLOR_RESET}"
}

# BUILD FOR RELEASE
function build_release() {
    log "${COLOR_BLUE}Building for release...${COLOR_RESET}"
    mkdir -p "$OBJECT_RELEASE_DIR" "$BINARY_RELEASE_DIR"

    for src in "${SOURCE_FILES[@]}"; do
        obj="$OBJECT_RELEASE_DIR/$(basename "${src%.*}.o")"
        log "Compiling $src to $obj..."
        $CXX $CXX_FLAGS $CXX_FLAGS_RELEASE -c "$src" -o "$obj" || { log "${COLOR_ORANGE}Release build failed.${COLOR_RESET}"; exit 1; }
    done

    cp $SFML_DIR/bin/openal32.dll $BINARY_RELEASE_DIR/
    cp $SFML_DIR/bin/sfml-audio-2.dll $BINARY_RELEASE_DIR/
    cp $SFML_DIR/bin/sfml-graphics-2.dll $BINARY_RELEASE_DIR/
    cp $SFML_DIR/bin/sfml-network-2.dll $BINARY_RELEASE_DIR/
    cp $SFML_DIR/bin/sfml-system-2.dll $BINARY_RELEASE_DIR/
    cp $SFML_DIR/bin/sfml-window-2.dll $BINARY_RELEASE_DIR/

    log "Linking all release files to executable..."
    $CXX $CXX_FLAGS $CXX_FLAGS_RELEASE -o "$TARGET_RELEASE" "${OBJECT_FILES_RELEASE[@]}" $LIBRARY_FLAGS_RELEASE || { log "${COLOR_ORANGE}Release build failed.${COLOR_RESET}"; exit 1; }
    log "${COLOR_GREEN}Release build succeeded. Executable created at $BINARY_RELEASE_DIR.${COLOR_RESET}"
}

# ENABLE ARGUMENT
case "$1" in
    debug)
        build_debug
        ;;
    release)
        build_release
        ;;
    clean)
        clean
        ;;
    *)
        echo "Usage: $0 {debug|release|clean}"
        ;;
esac
