#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# HawkPro Bootstrap Installer 
# v1.0.0
# -----------------------------------------------------------------------------

set -e

REPO_URL="https://github.com/3rr0r-505/HawkPro.git"
TMP_DIR="$(mktemp -d)"
INSTALL_BIN="/usr/local/bin"
MAN_DIR="/usr/local/share/man/man1"

BINARY="$INSTALL_BIN/hawkpro"
CTL="$INSTALL_BIN/hawkproctl"
MANPAGE="$MAN_DIR/hawkpro.1"

ALIAS_BLOCK_START="# >>> HawkPro alias start >>>"
ALIAS_BLOCK_END="# <<< HawkPro alias end <<<"

# ---------------- Universal Shell Detection ----------------

detect_shell_rc() {
    case "$SHELL" in
        */zsh) echo "$HOME/.zshrc" ;;
        */fish) echo "$HOME/.config/fish/config.fish" ;;
        *) echo "$HOME/.bashrc" ;;
    esac
}

BASHRC="$(detect_shell_rc)"

# ---------------- Safety Helpers ----------------

require_root() {
    if [[ $EUID -ne 0 ]]; then
        echo "Please run with sudo."
        exit 1
    fi
}

require_cmd() {
    command -v "$1" >/dev/null 2>&1 || {
        echo "[ERROR] Required command not found: $1"
        exit 1
    }
}

check_write_perm() {
    if [[ ! -w "$1" ]]; then
        echo "[ERROR] No write permission: $1"
        exit 1
    fi
}

# ---------------- Package Manager ----------------

detect_package_manager() {
    if command -v apt >/dev/null 2>&1; then echo "apt"
    elif command -v dnf >/dev/null 2>&1; then echo "dnf"
    elif command -v pacman >/dev/null 2>&1; then echo "pacman"
    else echo "unknown"
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
            exit 1
            ;;
    esac
}

# ---------------- Alias Management ----------------

add_alias() {
    if [[ "$BASHRC" == *fish/config.fish ]]; then
        if ! grep -q "hawkpro" "$BASHRC" 2>/dev/null; then
            echo "alias hawkpro='hawkproctl'" >> "$BASHRC"
        fi
    else
        if ! grep -q "$ALIAS_BLOCK_START" "$BASHRC" 2>/dev/null; then
            cat >> "$BASHRC" <<EOF

$ALIAS_BLOCK_START
alias hawkpro='hawkproctl'
$ALIAS_BLOCK_END
EOF
        fi
    fi

    echo "[*] Alias added"
}

remove_alias() {
    if [[ "$BASHRC" == *fish/config.fish ]]; then
        sed -i "/hawkpro/d" "$BASHRC"
    else
        sed -i "/$ALIAS_BLOCK_START/,/$ALIAS_BLOCK_END/d" "$BASHRC"
    fi
}

# ---------------- Lifecycle Manager ----------------

create_ctl() {
cat > "$CTL" << 'EOF'
#!/usr/bin/env bash

INSTALL_BIN="/usr/local/bin"
MAN_DIR="/usr/local/share/man/man1"

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
    BASHRC="$HOME/.bashrc"
    if grep -q "$ALIAS_BLOCK_START" "$BASHRC" 2>/dev/null; then
        sed -i "/$ALIAS_BLOCK_START/,/$ALIAS_BLOCK_END/d" "$BASHRC"
    fi
}

uninstall_hawkpro() {
    require_root

    echo "[*] Uninstalling HawkPro..."

    [[ -f "$BINARY" ]] && rm -f "$BINARY"
    [[ -f "$MANPAGE" ]] && rm -f "$MANPAGE"
    [[ -f "$CTL" ]] && rm -f "$CTL"

    mandb >/dev/null 2>&1 || true

    remove_alias

    echo "HawkPro fully uninstalled."
}

install_check() {
    [[ -f "$BINARY" ]] && echo "HawkPro already installed." \
        || echo "HawkPro not installed."
}

case "$1" in
    --uninstall) uninstall_hawkpro ;;
    --install) install_check ;;
    *) exec "$BINARY" "$@" ;;
esac
EOF

chmod +x "$CTL"
}

# ---------------- Installation ----------------

install_hawkpro() {
    require_root

    if [[ -f "$BINARY" ]]; then
        echo "HawkPro already installed."
        exit 0
    fi

    require_cmd git
    require_cmd cmake
    require_cmd make

    check_write_perm "$INSTALL_BIN"

    install_dependencies

    echo "[*] Cloning repository..."
    git clone --depth 1 "$REPO_URL" "$TMP_DIR" || {
        echo "Git clone failed"
        exit 1
    }

    trap 'rm -rf "$TMP_DIR"' EXIT

    echo "[*] Building HawkPro..."
    cd "$TMP_DIR"

    mkdir -p build
    cd build

    cmake .. || exit 1
    make -j"$(nproc)" || {
        echo "Build failed"
        exit 1
    }

    echo "[*] Installing binary..."
    cp hawkpro "$BINARY"
    chmod +x "$BINARY"

    echo "[*] Installing man page..."
    mkdir -p "$MAN_DIR"

    if [[ -f ../docs/hawkpro.1 ]]; then
        cp ../docs/hawkpro.1 "$MANPAGE"
        mandb >/dev/null 2>&1 || true
    else
        echo "Warning: Man page not found."
    fi

    echo "[*] Installing lifecycle manager..."
    create_ctl

    echo "[*] Adding alias..."
    add_alias

    echo ""
    echo "HawkPro installed successfully."
    echo "Run: hawkpro"
    echo "Man page: man hawkpro"
}

install_hawkpro
