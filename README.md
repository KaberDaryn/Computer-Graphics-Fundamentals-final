# Dream Developer Desk

Computer Graphics Fundamentals final project, topic 28: an interactive 3D developer workspace built with C++17 and OpenGL 3.3 Core.

The scene contains a desk, two monitors, an RGB keyboard and LED strip, mouse, chair, plant, lamp, shelves and a window. It includes an orbit camera, monitor layouts, a switchable lamp, adjustable RGB, day/night mode and an RGB finale. A second camera renders a live view onto the right monitor. The renderer uses indexed VAO/VBO/EBO meshes, a parent-child scene graph, separate GLSL shaders, Blinn-Phong lighting and mipmapped textures.

## Build

Requires CMake 3.21+, a C++17 compiler and an OpenGL 3.3-capable graphics driver. Third-party source libraries are included in `external/`.

Windows with Visual Studio 2022 and the Desktop development with C++ workload, from the repository root:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
cd build/Release
./DreamDeveloperDesk.exe
```

Windows with MSYS2 UCRT64, GCC, CMake and Ninja in PATH:

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
cd build
./DreamDeveloperDesk.exe
```

Linux requires a C++ toolchain, CMake, OpenGL and X11 development headers. On Debian/Ubuntu install `build-essential cmake libgl1-mesa-dev libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libxkbcommon-dev`, then:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
cd build && ./DreamDeveloperDesk
```

CMake copies assets beside the executable. Run from that directory so relative asset paths resolve correctly.

## Controls

| Input | Action |
| --- | --- |
| Left mouse drag / W A S D | Orbit camera |
| Mouse wheel / Q E | Zoom |
| L | Toggle desk lamp |
| M | Change monitor arrangement |
| R | Cycle RGB mode |
| [ / ] | Adjust RGB speed |
| N | Toggle day/night |
| P | Toggle live monitor feed |
| F | Start or restore RGB finale |
| H | Show/hide controls |
| Esc | Exit |

The panel also adjusts RGB colour, speed and daylight. FPS is displayed in the panel and window title.

## Testing

`ctest --test-dir build -C Release --output-on-failure` runs the finale state-machine test. The Windows UCRT64 Release build and CPU test were run successfully; the application and second-camera view were visually inspected. The window displayed approximately 144 FPS on the development machine with VSync enabled. This is not a benchmark on the TA machine. Linux and Visual Studio builds still need verification in those environments.

`src/` contains application, scene, rendering and UI modules; `assets/` contains textures and shaders; `tests/` contains the CPU test; `external/` contains third-party libraries and licenses. See [CREDITS.md](CREDITS.md).

## Team and AI use

Team members: Kaber Daryn, Arthur Kopzhasarov, Ernar Sadenov and Erkebulan Sovetov. The team should add each member's actual responsibilities before submission.

OpenAI ChatGPT assisted with the initial architecture, C++/GLSL code, generated textures, tests and documentation; Codex assisted with dependency integration, build verification and repository cleanup. Team members should review and understand the implementation and accurately disclose their own changes and AI use in the report.

The course also requires a report, presentation, demo video, genuine contribution history and a `v1.0` release on the submission date. These require team input, recordings and verification and are not represented as completed here.
