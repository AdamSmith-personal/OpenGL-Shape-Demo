#!/usr/bin/env bash
set -Eeuo pipefail

have_header() {
    local HEADER="$1"
    echo "#include <$HEADER>" | cc -E - >/dev/null 2>&1
}

have_pkg() {
    pkg-config --exists "$1" >/dev/null 2>&1
}

install_pkgs() {
    case "$PM" in
        apt)     sudo apt update && sudo apt install -y "$@" ;;
        dnf)     sudo dnf install -y "$@" ;;
        pacman) sudo pacman -S --needed --noconfirm "$@" ;;
    esac
}

# Detect package manager
PM=""
for mgr in apt dnf pacman; do
    command -v "$mgr" &>/dev/null && PM="$mgr" && break
done

[[ -z "$PM" ]] && { echo "No supported package manager found"; exit 1; }

# Required tools
for cmd in cc sudo make pkg-config; do
    command -v "$cmd" &>/dev/null || {
        echo "Required tool missing: $cmd"
        exit 1
    }
done

# Verify C compiler actually works
if ! echo 'int main(void){return 0;}' | cc -x c - -o /tmp/cc-test >/dev/null 2>&1; then
    echo "C compiler is present but not functional"
    exit 1
fi
rm -f /tmp/cc-test

# Detect display backend
SESSION_TYPE="${XDG_SESSION_TYPE:-x11}"
echo "Detected session type: $SESSION_TYPE"

# Header / pkg checks
missing=()

have_header "GL/gl.h" || missing+=("OpenGL headers")
# Wayland support
if [[ "$SESSION_TYPE" == "wayland" ]]; then
    have_header "wayland-client.h"       || missing+=("Wayland headers")
    have_header "xkbcommon/xkbcommon.h"   || missing+=("xkbcommon headers")
fi

# X11 is still required even on Wayland (GLFW/XWayland)
have_header "X11/Xlib.h"                 || missing+=("X11 headers")
have_header "X11/extensions/Xrandr.h"    || missing+=("Xrandr headers")

for m in "${missing[@]}"; do
  echo "$m"
done

# Dependency install

if (( ${#missing[@]} > 0 )); then
    echo "Missing development components:"
    printf "  - %s\n" "${missing[@]}"

    read -rp "Install required dependencies? [Y/n] " ans
    [[ "$ans" =~ ^[Nn]$ ]] && exit 1

    # sudo only needed here
    command -v sudo &>/dev/null || {
        echo "sudo is required to install packages"
        exit 1
    }

    . /etc/os-release

    # Debian / Ubuntu / derivatives
    if [[ "$ID" =~ ^(debian|ubuntu)$ ]] || [[ "${ID_LIKE:-}" =~ (debian|ubuntu) ]]; then
        pkgs=(
            build-essential
            cmake
            pkg-config
            libgl-dev
            libgl1-mesa-dev
            xorg-dev
        )

        if [[ "$SESSION_TYPE" == "wayland" ]]; then
            pkgs+=(
                libwayland-dev
                libxkbcommon-dev
                libegl1-mesa-dev
            )
        fi

        install_pkgs "${pkgs[@]}"

    # Fedora / RHEL / derivatives
    elif [[ "$ID" =~ ^(fedora|rhel|centos)$ ]] || [[ "${ID_LIKE:-}" =~ (fedora|rhel) ]]; then
        pkgs=(
            gcc gcc-c++
            cmake
            pkgconf-pkg-config
            mesa-libGL-devel
            libX11-devel
            libXrandr-devel
            libXcursor-devel
            libXi-devel
            libXinerama-devel
        )

        if [[ "$SESSION_TYPE" == "wayland" ]]; then
            pkgs+=(
                wayland-devel
                libxkbcommon-devel
                mesa-libEGL-devel
            )
        fi

        install_pkgs "${pkgs[@]}"

    # Arch / Manjaro
    elif [[ "$ID" =~ ^(arch|manjaro)$ ]] || [[ "${ID_LIKE:-}" =~ arch ]]; then
        pkgs=(
            base-devel
            cmake
            pkgconf
            libglvnd
            libx11
            libxrandr
            libxcursor
        )

        if [[ "$SESSION_TYPE" == "wayland" ]]; then
            pkgs+=(
                wayland
                libxkbcommon
            )
        fi

        install_pkgs "${pkgs[@]}"

    else
        echo "Unsupported distro: $ID"
        exit 1
    fi
else
    echo "All required dependencies found."
fi

# Build & install
BUILD_DIR="build"
PREFIX="${1:-/usr/local}"

cmake -S . -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$PREFIX"

cmake --build "$BUILD_DIR" --parallel

if [[ "$PREFIX" == /usr/* ]]; then
    sudo cmake --install "$BUILD_DIR"
else
    cmake --install "$BUILD_DIR"
fi

echo "Installation complete."