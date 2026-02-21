# HawkPro
![Language](https://img.shields.io/badge/Language-C%2B%2B-blue?logo=c%2B%2B) 
![Standard](https://img.shields.io/badge/C%2B%2B-11%2B-blueviolet?logo=c%2B%2B)
![Build](https://img.shields.io/badge/Build-CMake-0068c7?logo=cmake&logoColor=0068c7)
![Compiler](https://img.shields.io/badge/Compiler-g%2B%2B-success?logo=gnu)
![OS](https://img.shields.io/badge/OS-Linux-black?logo=linux)
![Kernel](https://img.shields.io/badge/Kernel-Linux%20Userspace-yellow?logo=linux)
![Data Source](https://img.shields.io/badge/Data-%2Fproc-critical?logo=linux) 
![Interface](https://img.shields.io/badge/Interface-Terminal-darkgreen?logo=gnubash)
![Docker](https://img.shields.io/badge/Docker-Container-2496ED?logo=docker&logoColor=white)
![Monitoring](https://img.shields.io/badge/Type-Process%20Monitor-red?logo=htop)

**HawkPro** is a lightweight, terminal-based Linux system process monitor built with **C++ and ncurses**. It provides real-time system metrics and a sortable process table in a clean, keyboard-driven interface.

## ✨ Features

- ⚡ Real-time CPU & memory monitoring
- 📊 Sort processes by:
  - `C / c` → CPU usage
  - `M / m` → Memory usage
  - `U / u` → User
- 🔍 `Ctrl + F` → Search process by name
- ⬆️⬇️ Arrow navigation
- 📄 Horizontal scroll for long commands
- 🧭 Minimal, responsive ncurses UI
- 📖 Built-in man page
- 🐳 Docker support

---

## 🛠 Tech Stack

- **Language:** C++
- **Platform:** Linux
- **Data Source:** `/proc` filesystem
- **Build Tool:** g++ / make (simple compilation)
---

## 🚀 Installation (One-Line Bootstrap)

```bash
wget https://github.com/3rr0r-505/HawkPro/raw/main/installer.sh
sudo bash installer.sh
```


## 🐋 Docker Usage

Build image & Run:
```bash
docker build -t hawkpro .
docker run -it --pid=host --privileged hawkpro
```

## 🛠 Build From Source (Manual)
```bash
git clone https://github.com/3rr0r-505/HawkPro.git
cd HawkPro
mkdir build && cd build
cmake ..
make
./hawkpro
```

Dependencies:
- C++17 compiler
- CMake
- ncurses
---

## 📄 License
This project is licensed under **Apache License** for educational purposes. Free to use, modify, and learn from.
