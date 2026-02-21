#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# HawkPro Bootstrap Installer
# v1.1.0 - Cocoon
# -----------------------------------------------------------------------------

set -e

# ---------------- Config ----------------

REPO_URL="https://github.com/3rr0r-505/HawkPro.git"
TMP_DIR="$(mktemp -d)"

INSTALL_BIN="/usr/local/bin"
MAN_DIR="/usr/local/share/man/man1"
STATE_FILE="/usr/local/share/hawkpro_install_state"

BINARY="$INSTALL_BIN/hawkpro"
CTL="$INSTALL_BIN/hawkproctl"
MANPAGE="$MAN_DIR/hawkpro.1"

ALIAS_BLOCK_START="# >>> HawkPro alias start >>>"
ALIAS_BLOCK_END="# <<< HawkPro alias end <<<"

# ---------------- Shell Detection ----------------

detect_shell_rc() {
    case "$SHELL" in
        */zsh) echo "$HOME/.zshrc" ;;
        */fish) echo "$HOME/.config/fish/config.fish" ;;
        *) echo "$HOME/.bashrc" ;;
    esac
}

BASHRC="$(detect_shell_rc)"

# ---------------- Shell Refresh ----------------

refresh_shell() {
    if [[ "$BASHRC" == *fish/config.fish ]]; then
        fish -c "source $BASHRC" 2>/dev/null || true
    else
        source "$BASHRC" 2>/dev/null || true
    fi
}

# ---------------- Transaction System ----------------

start_transaction() { echo "IN_PROGRESS" > "$STATE_FILE"; }

commit_transaction() { echo "INSTALLED" > "$STATE_FILE"; }

rollback_install() {

    echo "[!] Rolling back..."

    rm -f "$BINARY"
    rm -f "$CTL"
    rm -f "$MANPAGE"

    remove_alias

    rm -f "$STATE_FILE"

    mandb >/dev/null 2>&1 || true

    echo "[!] Rollback completed."
}

trap 'rollback_install' ERR

# ---------------- Safety Helpers ----------------

require_root() {
    [[ $EUID -ne 0 ]] && { echo "Run with sudo"; exit 1; }
}

require_cmd() {
    command -v "$1" >/dev/null 2>&1 || {
        echo "[ERROR] Missing dependency: $1"
        exit 1
    }
}

check_write_perm() {
    [[ -w "$1" ]] || { echo "[ERROR] No write permission: $1"; exit 1; }
}

# ---------------- Package Manager ----------------

detect_package_manager() {
    command -v apt >/dev/null 2>&1 && echo "apt" && return
    command -v dnf >/dev/null 2>&1 && echo "dnf" && return
    command -v pacman >/dev/null 2>&1 && echo "pacman" && return
    echo "unknown"
}

install_dependencies() {

    PM=$(detect_package_manager)

    echo "[*] Installing dependencies..."

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
            echo "Unsupported package manager"
            exit 1
            ;;
    esac
}

# ---------------- Alias Management ----------------

add_alias() {

    if [[ "$BASHRC" == *fish/config.fish ]]; then
        grep -q "hawkpro" "$BASHRC" || \
        echo "alias hawkpro='hawkproctl'" >> "$BASHRC"
    else
        grep -q "$ALIAS_BLOCK_START" "$BASHRC" || \
        cat >> "$BASHRC" <<EOF

$ALIAS_BLOCK_START
alias hawkpro='hawkproctl'
$ALIAS_BLOCK_END
EOF
    fi

    refresh_shell
    echo "[*] Alias configured"
}

remove_alias() {

    if [[ "$BASHRC" == *fish/config.fish ]]; then
        sed -i "/hawkpro/d" "$BASHRC"
    else
        sed -i "/$ALIAS_BLOCK_START/,/$ALIAS_BLOCK_END/d" "$BASHRC"
    fi

    refresh_shell
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
    [[ $EUID -ne 0 ]] && { echo "Run with sudo"; exit 1; }
}

remove_alias() {
    BASHRC="$HOME/.bashrc"
    sed -i "/$ALIAS_BLOCK_START/,/$ALIAS_BLOCK_END/d" "$BASHRC" 2>/dev/null || true
}

uninstall_hawkpro() {

    require_root

    echo "[*] Uninstalling HawkPro..."

    rm -f "$BINARY"
    rm -f "$CTL"
    rm -f "$MANPAGE"

    remove_alias

    mandb >/dev/null 2>&1 || true

    rm -f /usr/local/share/hawkpro_install_state

    echo "HawkPro fully uninstalled."
}

install_check() {
    [[ -f "$BINARY" ]] && echo "HawkPro already installed" \
        || echo "HawkPro not installed"
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
    start_transaction

    [[ -f "$BINARY" ]] && { echo "Already installed"; exit 0; }

    require_cmd git
    require_cmd cmake
    require_cmd make

    check_write_perm "$INSTALL_BIN"

    install_dependencies

    echo "[*] Cloning repo..."
    git clone --depth 1 "$REPO_URL" "$TMP_DIR"

    trap 'rm -rf "$TMP_DIR"' EXIT

    echo "[*] Building..."
    cd "$TMP_DIR"

    mkdir -p build
    cd build

    cmake .
    make -j"$(nproc)"

    echo "[*] Installing binary..."
    cp hawkpro "$BINARY"
    chmod +x "$BINARY"

    echo "[*] Installing man page..."
    mkdir -p "$MAN_DIR"

    if [[ -f ../docs/hawkpro.1 ]]; then
        cp ../docs/hawkpro.1 "$MANPAGE"
        mandb >/dev/null 2>&1 || true
    fi

    echo "[*] Installing lifecycle manager..."
    create_ctl

    echo "[*] Configuring alias..."
    add_alias

    commit_transaction

    echo ""
    echo "HawkPro installed successfully"
    echo "Run: hawkpro"
}

install_hawkpro
