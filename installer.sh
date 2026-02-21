#!/usr/bin/env bash

# -----------------------------------------------------------------------------
# HawkPro Bootstrap Installer
# v1.0.1 - Cocoon
# -----------------------------------------------------------------------------

set -euo pipefail

# ---------------- Config ----------------

REPO_URL="https://github.com/3rr0r-505/HawkPro.git"
TMP_DIR="$(mktemp -d)"

INSTALL_BIN="/usr/local/bin"
MAN_DIR="/usr/local/share/man/man1"
INSTALL_MARK="/usr/local/share/hawkpro.installed"

BINARY="$INSTALL_BIN/hawkpro"
CTL="$INSTALL_BIN/hawkpro-uninstall"
MANPAGE="$MAN_DIR/hawkpro.1"

# ---------------- Safety Helpers ----------------
require_root() {
    if [[ "$EUID" -ne 0 ]]; then
        echo "Run installer with sudo"
        exec sudo "$0" "$@"
        exit 1
    fi
}
require_cmd() {
    command -v "$1" >/dev/null 2>&1 || {
        echo "[ERROR] Missing dependency: $1"
        exit 1
    }
}

# ---------------- Dependency Install ----------------

detect_package_manager() {
    command -v apt >/dev/null 2>&1 && { echo "apt"; return; }
    command -v dnf >/dev/null 2>&1 && { echo "dnf"; return; }
    command -v pacman >/dev/null 2>&1 && { echo "pacman"; return; }
    echo "unknown"
}

install_dependencies() {

    PM=$(detect_package_manager)

    echo "[*] Installing dependencies..."

    case "$PM" in
        apt)
            apt update -qq
            apt install -y build-essential cmake git libncurses-dev man-db &>/dev/null
            ;;
        dnf)
            dnf install -y gcc-c++ make cmake git ncurses-devel man-db &>/dev/null
            ;;
        pacman)
            pacman -Sy --noconfirm base-devel cmake git ncurses man-db &>/dev/null
            ;;
        *)
            echo "Unsupported package manager"
            exit 1
            ;;
    esac
}

# ---------------- Lifecycle Manager ----------------

create_ctl() {

cat > "$CTL" << 'EOF'
#!/usr/bin/env bash

BINARY="/usr/local/bin/hawkpro"
UNINSTALLER="/usr/local/bin/hawkpro-uninstall"
MANPAGE="/usr/local/share/man/man1/hawkpro.1"

require_root() {
    [[ "$(id -u)" -ne 0 ]] && { echo "Run with sudo"; exit 1; }
}

uninstall_hawkpro() {

    require_root

    echo "[*] Uninstalling HawkPro..."

    rm -f "$BINARY"
    rm -f "$MANPAGE"
    rm -f "$UNINSTALLER"
    rm -f /usr/local/share/hawkpro.installed

    mandb >/dev/null 2>&1 || true

    echo "[#] HawkPro successfully uninstalled."
}
uninstall_hawkpro
EOF

chmod +x "$CTL"
}

# ---------------- Installation ----------------

install_hawkpro() {

    require_root

    [[ -f "$INSTALL_MARK" ]] && {
        echo "HawkPro already installed"
        exit 0
    }

    require_cmd git
    require_cmd cmake
    require_cmd make

    install_dependencies

    echo ""
    echo "[*] Cloning repository..."
    git clone --depth 1 "$REPO_URL" "$TMP_DIR" &>/dev/null

    trap 'rm -rf "$TMP_DIR"' EXIT

    echo ""
    echo "[*] Building..."

    cd "$TMP_DIR"
    mkdir -p build
    cd build

    cmake .. &>/dev/null || {
        echo "[ERROR] CMake configuration failed"
        exit 1
    }

    make -j"$(nproc)" &>/dev/null || {
        echo "[ERROR] Build failed"
        exit 1
    }

    # ---- Binary detection safety ----
    BIN_SRC=$(find . -type f -name hawkpro -executable | head -n 1)

    [[ -z "$BIN_SRC" ]] && {
        echo "[ERROR] Binary not generated after build"
        exit 1
    }

    echo "[*] Installing binaries..."
    install -Dm755 "$BIN_SRC" "$BINARY"

    # ---- Man page install ----
    echo ""
    echo "[*] Installing man page..."

    mkdir -p "$MAN_DIR"

    if [[ -f ../docs/hawkpro.1 ]]; then
        install -Dm644 ../docs/hawkpro.1 "$MANPAGE"
        mandb >/dev/null 2>&1 || true
        echo "[✓] Man page installed"
    else
        echo "[!] Warning: Man page not found"
    fi

    echo ""
    echo "[*] Installing lifecycle manager..."
    create_ctl

    touch "$INSTALL_MARK"

    # ---- Post install verification ----
    if ! command -v hawkpro >/dev/null 2>&1; then
        echo ""
        echo "[!] Warning: HawkPro command may not be available in PATH yet"
    fi
    
    echo ""
    echo "---------------------------------"
    echo "[✓] HawkPro v1.0.1 Installed"
    echo "[#] Command: hawkpro"
    echo "[#] Uninstall: hawkpro-uninstall"
    echo "---------------------------------"
}

# ---------------- Entry ----------------

main() {
    install_hawkpro
}

main "$@"
