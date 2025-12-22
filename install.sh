#!/usr/bin/env bash
set -Eeuo pipefail
# Check if a header exists
have_header() {
    local HEADER="$1"
    echo "#include <$HEADER>" | cc -c -xc - -o /dev/null >/dev/null 2>&1
}
# install pkgs depending on the package manager
install_pkgs() {
    case "$PM" in
        apt)     sudo apt update && sudo apt install -y "$@" ;;
        dnf)     sudo dnf install -y "$@" ;;
        pacman) sudo pacman -S --needed --noconfirm "$@" ;;
        *) echo "Unsupported package manager: $PM"; exit 1 ;;
    esac
}

# Detect package manager
PM=""
for mgr in apt dnf pacman; do
    command -v "$mgr" &>/dev/null && PM="$mgr" && break
done
[[ -z "$PM" ]] && { echo "No supported package manager found"; exit 1; }

# Ensure all required tools are there (eg. cc, sudo, make, pkg-config)
for cmd in cc make; do
    command -v "$cmd" &>/dev/null || {
        echo "Required tool missing: $cmd"
        exit 1
    }
done

# Ensure the C compiler works
temp=$(mktemp)
if ! echo 'int main(void){return 0;}' | cc -x c - -o "$temp" >/dev/null 2>&1; then
    echo "C compiler is present but not functional"
    rm -f "$temp"
    exit 1
fi
rm -f "$temp"

if [ "$XDG_SESSION_TYPE" = "wayland" ]; then
  WAYLAND=1
else
  WAYLAND=0
fi

# Header / pkg checks
missing=()
# Check which headers exists
have_header "GL/gl.h" || missing+=("OpenGL headers")
have_header "X11/Xlib.h"                 || missing+=("X11 headers")
have_header "X11/extensions/Xrandr.h"    || missing+=("Xrandr headers")

# if wayland then check for headers
if (( WAYLAND )); then
  have_header "wayland-client.h"   || missing+=("Wayland headers")
  have_header "xkbcommon/xkbcommon.h" || missing+=("xkbcommon headers")
fi

for m in "${missing[@]}"; do
  echo "$m"
done

# if the headers are missing then prompt to install and if yes install them on the proper linux distro
if (( ${#missing[@]} > 0 )); then
    echo "Missing development components:"
    printf "  - %s\n" "${missing[@]}"

    read -rp "Install required dependencies? [Y/n] " ans
    [[ "$ans" =~ ^[Nn]$ ]] && exit 1
    command -v sudo &>/dev/null || { echo "sudo is required to install packages"; exit 1; }

    . /etc/os-release

    # Debian / Ubuntu / derivatives
    if [[ "${ID:-}" =~ ^(debian|ubuntu)$ ]] || [[ "${ID_LIKE:-}" =~ (debian|ubuntu) ]]; then
        pkgs=(
            build-essential
            cmake
            libgl-dev
            libgl1-mesa-dev
            xorg-dev
        )

        if (( WAYLAND )); then
            pkgs+=(
                libwayland-dev
                libxkbcommon-dev
                libegl1-mesa-dev
            )
        fi

        install_pkgs "${pkgs[@]}"

    # Fedora / RHEL / derivatives
    elif [[ "${ID:-}" =~ ^(fedora|rhel|centos)$ ]] || [[ "${ID_LIKE:-}" =~ (fedora|rhel) ]]; then
        pkgs=(
            gcc gcc-c++
            cmake
            mesa-libGL-devel
            libX11-devel
            libXrandr-devel
            libXcursor-devel
            libXi-devel
            libXinerama-devel
        )

        if (( WAYLAND )); then
            pkgs+=(
                wayland-devel
                libxkbcommon-devel
                mesa-libEGL-devel
            )
        fi

        install_pkgs "${pkgs[@]}"

    # Arch / Manjaro
    elif [[ "${ID:-}" =~ ^(arch|manjaro)$ ]] || [[ "${ID_LIKE:-}" =~ arch ]]; then
        pkgs=(
            base-devel
            cmake
            libglvnd
            libx11
            libxrandr
            libxcursor
        )

        if (( WAYLAND )); then
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

BUILD_DIR="build"
PREFIX="${1:-/usr/local}"
# Build the project
cmake -S . -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$PREFIX"
cmake --build "$BUILD_DIR" --parallel
# If the prefix is /usr/* then sudo is required to install
if [[ "$PREFIX" == /usr/* ]]; then
    sudo cmake --install "$BUILD_DIR"
else
    cmake --install "$BUILD_DIR"
fi

if [[ -d "$BUILD_DIR" ]]; then
  if [[ "$PREFIX" == /usr/* ]]; then
    sudo rm -rf "$BUILD_DIR"
  else
    rm -rf "$BUILD_DIR"
  fi
fi

# Done :)
echo "Installation complete."