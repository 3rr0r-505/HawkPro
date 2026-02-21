#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# HawkPro Bootstrap Installer
# -----------------------------------------------------------------------------

set -e

REPO_URL="https://github.com/3rr0r-505/HawkPro.git"
TMP_DIR="$(mktemp -d)"
INSTALL_BIN="/usr/local/bin"
MAN_DIR="/usr/local/share/man/man1"
BASHRC="$HOME/.bashrc"

BINARY="$INSTALL_BIN/hawkpro"
CTL="$INSTALL_BIN/hawkproctl"
MANPAGE="$MAN_DIR/hawkpro.1"

ALIAS_BLOCK_START="# >>> HawkPro alias start >>>"
ALIAS_BLOCK_END="# <<< HawkPro alias end <<<"

require_root() {
    if [[ $EUID -ne 0 ]]; then
        echo "Please run with sudo."
        exit 1
    fi
}

detect_package_manager() {
    if command -v apt >/dev/null 2>&1; then
        echo "apt"
    elif command -v dnf >/dev/null 2>&1; then
        echo "dnf"
    elif command -v pacman >/dev/null 2>&1; then
        echo "pacman"
    else
        echo "unknown"
    fi
}

install_dependencies() {
    PM=$(detect_package_manager)

    echo "[*] Installing required build dependencies..."

    case "$PM" in
        apt)
            apt update
            apt install -y build-essential cmake git libncurses-dev man-db
            ;;
        dnf)
            dnf install -y gcc-c++ make cmake git ncurses-devel man-db
            ;;
        pacman)
            pacman -Sy --noconfirm base-devel cmake git ncurses man-db
            ;;
        *)
            echo "Unsupported package manager."
            echo "Please install: gcc, make, cmake, git, ncurses, man-db manually."
            exit 1
            ;;
    esac
}

add_alias() {
    if ! grep -q "$ALIAS_BLOCK_START" "$BASHRC" 2>/dev/null; then
        cat >> "$BASHRC" <<EOF

$ALIAS_BLOCK_START
alias hawkpro='hawkproctl'
$ALIAS_BLOCK_END
EOF
        echo "[*] Alias added to $BASHRC"
    fi
}

remove_alias() {
    if grep -q "$ALIAS_BLOCK_START" "$BASHRC" 2>/dev/null; then
        sed -i "/$ALIAS_BLOCK_START/,/$ALIAS_BLOCK_END/d" "$BASHRC"
    fi
}

create_ctl() {
cat > "$CTL" << 'EOF'
#!/usr/bin/env bash

INSTALL_BIN="/usr/local/bin"
MAN_DIR="/usr/local/share/man/man1"
BASHRC="$HOME/.bashrc"

BINARY="$INSTALL_BIN/hawkpro"
CTL="$INSTALL_BIN/hawkproctl"
MANPAGE="$MAN_DIR/hawkpro.1"

ALIAS_BLOCK_START="# >>> HawkPro alias start >>>"
ALIAS_BLOCK_END="# <<< HawkPro alias end <<<"

require_root() {
    if [[ $EUID -ne 0 ]]; then
        echo "Please run with sudo."
        exit 1
    fi
}

remove_alias() {
    if grep -q "$ALIAS_BLOCK_START" "$BASHRC" 2>/dev/null; then
        sed -i "/$ALIAS_BLOCK_START/,/$ALIAS_BLOCK_END/d" "$BASHRC"
    fi
}

uninstall_hawkpro() {
    require_root

    if [[ ! -f "$BINARY" ]]; then
        echo "HawkPro is not installed."
        exit 0
    fi

    echo "[*] Removing binary..."
    rm -f "$BINARY"

    echo "[*] Removing man page..."
    rm -f "$MANPAGE"
    mandb >/dev/null 2>&1 || true

    echo "[*] Removing alias..."
    remove_alias

    echo "[*] Removing hawkproctl..."
    rm -f "$CTL"

    echo "HawkPro fully uninstalled."
}

install_check() {
    if [[ -f "$BINARY" ]]; then
        echo "HawkPro already installed."
    else
        echo "HawkPro not installed."
    fi
}

case "$1" in
    --uninstall)
        uninstall_hawkpro
        ;;
    --install)
        install_check
        ;;
    *)
        exec "$BINARY" "$@"
        ;;
esac
EOF

    chmod +x "$CTL"
}

install_hawkpro() {
    require_root

    if [[ -f "$BINARY" ]]; then
        echo "HawkPro already installed."
        exit 0
    fi

    install_dependencies

    echo "[*] Cloning repository..."
    git clone --depth 1 "$REPO_URL" "$TMP_DIR"

    echo "[*] Building HawkPro..."
    cd "$TMP_DIR"
    mkdir build
    cd build
    cmake ..
    make -j"$(nproc)"

    echo "[*] Installing binary..."
    cp hawkpro "$BINARY"
    chmod +x "$BINARY"

    echo "[*] Installing man page..."
    if [[ -f ../docs/hawkpro.1 ]]; then
        cp ../docs/hawkpro.1 "$MANPAGE"
        mandb >/dev/null 2>&1 || true
    else
        echo "Warning: man page not found."
    fi

    echo "[*] Installing lifecycle manager..."
    create_ctl

    echo "[*] Adding alias..."
    add_alias

    echo "[*] Cleaning temporary files..."
    rm -rf "$TMP_DIR"

    echo ""
    echo "HawkPro installed successfully."
    echo "Run: hawkpro"
    echo "For manual page: man hawkpro "
}

install_hawkpro
