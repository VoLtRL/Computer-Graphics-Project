# Computer Graphics Project

## Project Goal

Develop a roguelike game using C++ and OpenGL.
Core Features :

- Implementation of a moving character and map exploration.
- Stretch Goals: Add a combat system with various monsters and an inventory system to enhance player statistics.

---

## Contribution Guidelines

- **No direct pushes to `main`**: All changes must be submitted via a Pull Request.
- **Conventions**: Pull Request titles must follow the [Conventional Commits](https://www.conventionalcommits.org/) format (e.g., `feat:`, `fix:`, `docs:`), otherwise the merge will be automatically blocked.

---

## Build and Run

This project uses **CMake** to handle the build process. It includes all necessary libraries (GLFW, GLEW, GLM) in the `external/` folder, but you need to ensure your system has the base development tools and OpenGL drivers.

### 1. Prerequisites (Linux)

Before building, ensure you have a C++ compiler, CMake, and the necessary system libraries for OpenGL and window management:

```bash
sudo apt-get update
sudo apt-get install build-essential cmake libgl1-mesa-dev libx11-dev libxi-dev libxrandr-dev libxinerama-dev libxcursor-dev

```

### 2. Clone the Repository

Clone the repository to your local machine:

```bash
git clone --recurse-submodules <repository_url>
cd Computer-Graphics-Project
```

### 3. Build Instructions

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

### 4. Running the Game

Once the compilation is finished, you can run the executable from the `build` directory:

```bash
./opengl_program

```

### 5. Controls

- **ZQSD**: Move the player character.
- **Mouse**: Look around.
- **Left Click**: Shoot a projectile.
- **R**: Reset the game after death.
- **ESC**: Quit the game.
- **F**: Toggle fullscreen mode.
- **V**: Open/close the stats menu.

---

## Project Features

### 1. Graphics & Rendering Engine

The rendering engine utilizes modern OpenGL (Core Profile) with custom shaders to handle 3D and 2D rendering.

- **Lighting System:**
    - **Blinn-Phong Reflection Model:** Implements Ambient, Diffuse (Lambertian), and Specular (Blinn-Phong) lighting components.
    - **Dynamic Lighting:** Supports a Directional Light (Sun) and up to **100 Dynamic Point Lights** simultaneously. Point lights are attached to projectiles, creating dynamic illumination as they travel through the scene.
    - **Attenuation:** Point lights feature quadratic distance attenuation for realistic falloff.
    - **Emissive Materials:** Support for emissive objects (like projectiles) that ignore lighting calculations to appear bright.

- **Advanced Shader Effects:**
    - **Shadow Mapping:** Implements shadows using a shadow map texture with **Percentage-Closer Filtering (PCF)** to soften shadow edges and reduce aliasing.
    - **Volumetric Fog:** Distance-based linear fog calculation. The fog color dynamically changes based on game progression (purification level).

- **UI & 2D Rendering:**
    - **Sprite Rendering:** A dedicated system for rendering 2D textured quads (Health bars, XP bars, Game Over/Victory screens, Crosshair).
    - **Text Rendering:** Support for TrueType fonts (JetBrains Mono) to display real-time stats like kills, level, and timer.

### 2. Physics & Collision System

A custom-built physics engine handles object interactions without relying on external physics libraries.

- **Collision Detection:**
    - **Shape-Based:** Supports various collision primitives including **Spheres, Boxes, and Capsules**.
    - **Broad Phase:** The `HandlePhysics` class manages the update loop, checking interactions between dynamic physical objects.
    - **Collision Resolution:** Handles elastic and inelastic collisions using restitution, friction, and damping.

- **Collision Filtering:**
    - The system uses **Collision Groups** and **Collision Masks** (e.g., `CG_PLAYER`, `CG_ENEMY`, `CG_PROJECTILE`) to efficiently filter interactions.

- **Object Dynamics:**
    - **Kinematic Objects:** Immovable objects (like Spawners) that affect others but are not moved by forces.
    - **Dynamic Objects:** Objects (Player, Projectiles) affected by mass, velocity, and acceleration.

### 3. Camera System

A custom Third-Person Orbit Camera tightly integrated with the physics engine.

- **Orbit Controls:** Locks onto and orbits the player, calculating position using spherical coordinates (Yaw/Pitch) while maintaining a fixed distance.
- **Input Handling:** Supports mouse look for rotation (with pitch constraints) and dynamic FOV zooming.

### 4. Game Mechanics

- **Player Controller:**
    - **Movement:** WASD movement with acceleration physics, jumping mechanics, and air control.
    - **Procedural Animation:** A custom hierarchical animation system procedurally rotates the player model's limbs (Torso, Arms, Legs) based on movement speed, state (jumping/idle), and combat actions.
    - **RPG Stats:** System for Leveling, Experience (XP), Health, and Attack Speed. Leveling up heals the player and scales difficulty.

- **Combat System:**
    - **Projectiles:** Physics-based projectiles with properties for speed, damage, range, and **piercing** capabilities.
    - **Enemy AI:** Mobs track the player's position. They feature a "Fear" mechanic where they flee if the player picks up a specific item.
    - **Loot System:** RNG-based drop system. Enemies drop power-ups (Damage Boost, Speed Boost, Health Packs, Fear) based on a calculated probability table.

- **Game Loop:**
    - **Win/Loss:** Features a survival timer and a "Purification" percentage. Killing enemies changes the environment (fog/sky color) from dark to bright blue, symbolizing victory.

- **Game Reset:**
    - On player death pressing 'R' resets the game state, clearing enemies, projectiles, and resetting player stats and map state.

### 5. Assets & Tools

- **Asset Management:** Custom `EntityLoader` and `ResourceManager` to handle loading and cloning of assets.
- **Blender Integration:** Imports `.glb` models including the Player (Knight), Enemies (Ghosts T1-T4), and map segments (Visuals/Collisions).
- **Build System:** Uses CMake to manage the build process, handling dependencies (GLFW, GLEW, GLM) and compilation.

---

## Project Libraries implemented

- **Assimp:** 3D model loading.
- **freetype**: Font rendering.
- **GLAD:** OpenGL function loading.
- **GLFW:** Window and input management.
- **GLM:** Mathematics library for vectors and matrices.
- **stb_image:** Image loading for textures.
