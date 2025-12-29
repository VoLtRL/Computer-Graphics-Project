# Computer Graphics Project
## Project Goal
Develop a roguelike game using C++ and OpenGL.
Core Features :
- Implementation of a moving character and map exploration.
- Stretch Goals: Add a combat system with various monsters and an inventory system to enhance player statistics.

C'est une excellente idée. Un bon README est essentiel pour que n'importe qui (y compris tes profs) puisse lancer ton projet sans douleur.

Voici un bloc complet en **anglais**, formaté en Markdown, que tu peux copier-coller directement dans ton fichier `README.md`.

J'ai inclus une section sur les **prérequis Linux** (car même si tu as GLFW dans ton dossier, GLFW a besoin des librairies graphiques de Linux pour se compiler).

---
## Contribution Guidelines

* **No direct pushes to `main`**: All changes must be submitted via a Pull Request.
* **Conventions**: Pull Request titles must follow the [Conventional Commits](https://www.conventionalcommits.org/) format (e.g., `feat:`, `fix:`, `docs:`), otherwise the merge will be automatically blocked.

---
## Build and Run

This project uses **CMake** to handle the build process. It includes all necessary libraries (GLFW, GLEW, GLM) in the `external/` folder, but you need to ensure your system has the base development tools and OpenGL drivers.

### 1. Prerequisites (Linux)

Before building, ensure you have a C++ compiler, CMake, and the necessary system libraries for OpenGL and window management:

```bash
sudo apt-get update
sudo apt-get install build-essential cmake libgl1-mesa-dev libx11-dev libxi-dev libxrandr-dev libxinerama-dev libxcursor-dev

```

### 2. Build Instructions

Run the following commands from the root directory of the project:

```bash
# 1. Create a build directory to keep the project clean
mkdir build
cd build

# 2. Generate the Makefiles using CMake
cmake ..

# 3. Compile the project
make

```

### 3. Running the Game

Once the compilation is finished, you can run the executable from the `build` directory:

```bash
./opengl_program

```

---
