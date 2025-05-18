# EsoxTorrent
A BitTorrent Client written in C++ based on ImGui

## General Build Instructions
### Windows
The preferred toolchain to build in Windows is Visual Studio 2022.

To Build EsoxTorrent in Windows, Follow the instructions:
- Install Premake
    - Download Premake from [here](https://github.com/premake/premake-core/releases/download/v5.0.0-beta6/premake-5.0.0-beta6-windows.zip)
    - Extract the files to some location add the location to `PATH` environmental variable 
- Install OpenSSL
    - Download OpenSSL from [here](https://slproweb.com/products/Win32OpenSSL.html)
    - Run the installer to install the application
    - Create an environmental variable `OPENSSL_PATH` with the installed location of OpenSSL
- Clone the project
    - Run `git clone --recursive https://github.com/shreyaspranav/EsoxTorrent.git`
- Generate Visual Studio project files by running `premake5 vs2022`
- Open generated `.sln` file in Visual Studio and click on "Run" button.
### Linux
The preferred toolchain to build in Linux is either GCC or Clang.
To Build EsoxTorrent in Linux, Follow the instructions:
- Install Premake
    - Download Premake from [here](https://github.com/premake/premake-core/releases/download/v5.0.0-beta6/premake-5.0.0-beta6-linux.tar.gz)
    - Extract the files to some location add the location to `PATH` environmental variable 
- Install OpenSSL
    - Ubuntu/Debian based systems: `sudo apt install libssl-dev`
    - Fedora/RHEL `sudo dnf install openssl-devel`
    - Arch `sudo pacman -S openssl`
- Install Zenity (Required for File Dialogs)
    - Ubuntu/Debian based systems: `sudo apt install zenity`
    - Fedora/RHEL `sudo dnf install zenity`
    - Arch `sudo pacman -S zenity`
- Clone the project
    - Run `git clone --recursive https://github.com/shreyaspranav/EsoxTorrent.git`
- Generate Makefiles by running `premake5 gmake --cc=clang` (or `--cc=gcc` if you prefer)
- Run `make config=<config_name>_<arch>` where `config_name` can be either of `debug`, `release` or `production` and `<arch>` can be `x64` or `x86`

