#  HawkPro

![Language](https://img.shields.io/badge/Language-C%2B%2B-blue?logo=c%2B%2B)
![Standard](https://img.shields.io/badge/C%2B%2B-11%2B-blueviolet?logo=c%2B%2B)
![Build](https://img.shields.io/badge/Build-CMake-0068c7?logo=cmake)
![Compiler](https://img.shields.io/badge/Compiler-g%2B%2B-success?logo=gnu)
![Platform](https://img.shields.io/badge/Platform-Linux-black?logo=linux)
![Type](https://img.shields.io/badge/Type-System%20Monitor-red?logo=htop)
![Interface](https://img.shields.io/badge/UI-Terminal-darkgreen?logo=gnubash)
![Data Source](https://img.shields.io/badge/Data-/proc-critical?logo=linux)
![Docker](https://img.shields.io/badge/Docker-Supported-2496ED?logo=docker)

## ⚡ Overview

**HawkPro** is a lightweight, terminal-based Linux process monitoring tool built using **C++ and ncurses**. It provides real-time system insights with a clean keyboard-driven interface and minimal resource overhead.

---

## ✨ Features

✅ Real-time CPU and Memory monitoring  
✅ Sort processes dynamically  
✅ Navigation & Search  
✅ Built-in documentation  t  
✅ Container support via Docker  

## 🛠 Tech Stack

| Component | Technology |
|---|---|
| Language | C++ |
| Standard | C++17 |
| UI | ncurses |
| Data Source | Linux `/proc` filesystem |
| Build System | CMake |

---

## 🚀 Installation

### One-Line Bootstrap (Recommended ⭐)
```bash
wget https://github.com/3rr0r-505/HawkPro/raw/main/installer.sh
chmod +x installer.sh
sudo bash installer.sh
````

### 🐳 Docker Usage
```bash
docker build --no-cache -t hawkpro .
docker run -it --rm --pid=host --net=host -v /proc:/proc hawkpro
```
`⚠️ Docker Note:`
When running inside Docker, HawkPro may detect containerized system information instead of the physical host due to Linux namespace isolation.
For best accuracy and performance → run natively on Linux.

### 🛠 Build From Source
```bash
git clone https://github.com/3rr0r-505/HawkPro.git
cd HawkPro

mkdir build && cd build
cmake ..
make

./hawkpro
```

---

## 🎮 Controls
| Key        | Action         |
| ---------- | -------------- |
| C / c      | Sort by CPU    |
| M / m      | Sort by Memory |
| U / u      | Sort by User   |
| Ctrl + F   | Search process |
| Arrow Keys | Navigate       |
| Q / q      | Quit           |

---

## 📄 License
This project is licensed under **Apache License 2.0**.
Free to use, modify, and learn from.

## ❤️ Support
If you like this project, consider giving it a ⭐ on GitHub!
