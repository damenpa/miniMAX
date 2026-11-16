#!/usr/bin/env bash
# Descarga raylib precompilada y prepara los symlinks de enlace.
# Uso: ./scripts/setup.sh
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"

VERSION="6.2"

case "$(uname -m)" in
    x86_64)  ASSET="raylib-${VERSION}_linux_amd64.tar.gz" ;;
    aarch64) ASSET="raylib-${VERSION}_linux_arm64.tar.gz" ;;
    i386|i686) ASSET="raylib-${VERSION}_linux_i386.tar.gz" ;;
    *) echo "Arquitectura no soportada: $(uname -m)" >&2; exit 1 ;;
esac

if [ -f "vendor/raylib/include/raylib.h" ]; then
    echo "[=] raylib ya esta en vendor/raylib"
else
    echo "[+] Descargando raylib ${VERSION} (${ASSET})..."
    mkdir -p vendor
    tmp="$(mktemp -d)"
    curl -fL --progress-bar -o "$tmp/raylib.tar.gz" \
        "https://github.com/raysan5/raylib/releases/download/${VERSION}/${ASSET}"
    tar xzf "$tmp/raylib.tar.gz" -C "$tmp"
    mv "$tmp/${ASSET%.tar.gz}" "vendor/raylib"
    rm -rf "$tmp"
    echo "[=] raylib instalada en vendor/raylib"
fi

# Algunas distros solo traen libGL.so.1 / libX11.so.6 (runtime) sin los
# enlaces .so que necesita el linker. Creamos stubs locales en vendor/lib.
echo "[+] Preparando enlaces de libGL / X11..."
mkdir -p vendor/lib
for name in GL X11 Xrandr Xinerama Xcursor Xi; do
    target="vendor/lib/lib${name}.so"
    [ -e "$target" ] && continue

    src="$(ls /usr/lib/*/lib${name}.so.? 2>/dev/null | head -1 || true)"
    if [ -n "$src" ]; then
        ln -sf "$src" "$target"
        echo "    lib${name}.so -> $src"
    else
        echo "    [!] no encontre lib${name}.so.? ; instala el paquete -dev correspondiente" >&2
    fi
done

echo
echo "[ok] Listo. Ahora:"
echo "     make run     # compilar y jugar"
echo "     make test    # verificar el motor de IA"
