# 🖥️ BasedOS

<div align="center">

![OS](https://img.shields.io/badge/OS-BasedOS-blue?style=for-the-badge)
![Architecture](https://img.shields.io/badge/Architecture-x86--32-red?style=for-the-badge)
![Language](https://img.shields.io/badge/Language-C%2FASM-green?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-yellow?style=for-the-badge)

**A minimal 32-bit operating system for educational purposes**

[Features](#-features) • [Quick Start](#-quick-start) • [Building](#-building) • [Contributing](#-contributing)

</div>

---

## 📋 Overview

**BasedOS** (also referred to as **VitaliOS**) is a minimal 32-bit operating system written in C and assembly, designed to run on x86 hardware or emulators like QEMU. This project serves as an educational tool for learning OS development fundamentals.

> 🎯 **Goal**: Understand low-level system programming and operating system concepts through hands-on development.

## ✨ Features

<table>
<tr>
<td>

### 🚀 Core Components
- **Bootloader** - Loads kernel from 1.44MB floppy
- **Kernel** - Minimal kernel with interrupt support
- **Shell** - Interactive command-line interface
- **Memory Manager** - Basic heap allocator

</td>
<td>

### 🎮 User Interface
- **VGA Text Mode** - 80x25 color text display
- **Keyboard Input** - Interrupt-driven input handling
- **PC Speaker** - Basic sound output
- **Command History** - Navigate with arrow keys

</td>
</tr>
</table>

### 🛠️ Technical Details

| Component | Description |
|-----------|-------------|
| **Bootloader** | Loads kernel at `0x1000`, switches to 32-bit protected mode, sets up GDT and stack |
| **Kernel** | Initializes VGA text mode, starts shell, enters idle loop with interrupts |
| **Shell** | Commands: `help`, `clear`, `about`, `beep`, `history` with up-arrow navigation |
| **VGA Display** | Text output at `0xB8000` with cursor support |
| **Audio** | PC Speaker via Programmable Interval Timer (PIT) channel 2 |
| **Input** | IRQ1 keyboard interrupts with scancode-to-ASCII mapping |
| **Memory** | Linear heap allocator with `kmalloc` and `kfree` |

## 🚀 Quick Start

### Prerequisites

<details>
<summary>📦 Required Tools</summary>

```bash
# Ubuntu/Debian
sudo apt install nasm gcc binutils qemu-system-x86

# Arch Linux
sudo pacman -S nasm gcc binutils qemu

# macOS (with Homebrew)
brew install nasm gcc qemu
```

</details>

### 🏗️ Building

```bash
# Clone the repository
git clone https://github.com/yourusername/basedos.git
cd basedos

# Build the OS
make

# Run in QEMU
make run
```

### 🎮 Running

```bash
# Run with QEMU
qemu-system-i386 -fda basedos.img

# Run with debug output
qemu-system-i386 -fda basedos.img -serial stdio
```

## 📖 System Requirements

### Development Environment
- **OS**: Linux, WSL, or macOS
- **Architecture**: x86-64 (for cross-compilation)
- **RAM**: 512MB minimum
- **Storage**: 50MB free space

### Target Hardware
- **CPU**: x86 (32-bit)
- **RAM**: 1MB minimum
- **Storage**: 1.44MB floppy disk

## 🎯 Supported Commands

| Command | Description |
|---------|-------------|
| `help` | Display available commands |
| `clear` | Clear the screen |
| `about` | Show system information |
| `beep` | Play a sound through PC speaker |
| `history` | Show command history |

## 🔧 Development

### Project Structure
```
basedos/
├── boot/           # Bootloader code
├── kernel/         # Kernel source files
├── drivers/        # Device drivers
├── lib/           # Library functions
├── include/       # Header files
└── Makefile       # Build configuration
```

### Adding New Features
1. **New Commands**: Add to `shell.c`
2. **Drivers**: Create in `drivers/` directory
3. **System Calls**: Implement in `kernel/syscall.c`
4. **Memory Management**: Extend `kernel/memory.c`

## 🐛 Troubleshooting

<details>
<summary>Common Issues</summary>

### Build Errors
- **Missing tools**: Install required development tools
- **32-bit compilation**: Use `-m32` flag or cross-compiler
- **Linker errors**: Check memory layout in linker script

### Runtime Issues
- **Boot failure**: Verify bootloader is properly written to disk
- **Kernel panic**: Check memory alignment and stack setup
- **No input**: Ensure keyboard interrupts are enabled

</details>

## 📚 Learning Resources

- 📖 [OSDev Wiki](https://wiki.osdev.org/)
- 📘 [Operating Systems: Three Easy Pieces](https://pages.cs.wisc.edu/~remzi/OSTEP/)
- 🎥 [OS Development Tutorials](https://www.youtube.com/playlist?list=PLHh55M_Kq4OApWScZyPl5HhgsTJS9MZ6M)
- 📋 [x86 Assembly Guide](https://www.cs.virginia.edu/~evans/cs216/guides/x86.html)

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📜 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- OSDev community for excellent documentation
- QEMU team for the amazing emulator
- All contributors who help improve this project

---

<div align="center">

**⭐ Star this repo if you found it helpful!**

Made with ❤️ for learning OS development

</div>