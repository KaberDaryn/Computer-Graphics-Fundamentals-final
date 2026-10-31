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

CMake copies assets beside the executable. The application finds assets beside the executable first, then in the working directory and original source tree; launching with an absolute executable path from another directory is supported.

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

```powershell
ctest --test-dir build -C Release --output-on-failure
```

Tests cover the finale state machine and orbit-camera controls. Clean Windows GCC/Ninja Debug and Release builds passed, with 2/2 tests passing in each. The application was also launched from another working directory. See [VALIDATION.md](VALIDATION.md) for measured performance, evidence and platform limitations. Linux and Visual Studio builds still require verification. All bundled dependencies build from local sources without downloads.

`src/` contains application, scene, rendering and UI modules; `assets/` contains textures and shaders; `tests/` contains the CPU test; `external/` contains third-party libraries and licenses. See [CREDITS.md](CREDITS.md).

## Team and AI use

Team members and assigned responsibilities are listed below.

OpenAI ChatGPT assisted with the initial architecture, C++/GLSL code, generated textures, tests and documentation; Codex assisted with dependency integration, build verification and repository cleanup. Team members should review and understand the implementation and accurately disclose their own changes and AI use in the report.

The course also requires a report, presentation, demo video, genuine contribution history and a `v1.0` release on the submission date. These require team input, recordings and verification and are not represented as completed here.

## Assigned code responsibilities

- Kaber Daryn: Application, Camera, Shader, Mesh, Scene, WowSequence, GitHub integration and submission coordination.
- Arthur Kopzhasarov: CMake, dependencies, Texture, Visual Studio builds and build troubleshooting.
- Ernar Sadenov: RoomFactory, DeskFactory, furniture geometry, RGB shader, Lighting and visual improvements.
- Erkebulan Sovetov: Framebuffer, monitor shader, sky shader, day/night, ImGui and demonstration responsibilities.

## Reproducible performance check

Use a Release build. In PowerShell, run these commands from the repository root:

```powershell
$env:DDD_BENCHMARK = 'feed' # alternatively 'off' or 'finale'
./build/DreamDeveloperDesk.exe
Remove-Item Env:DDD_BENCHMARK
```

Each run disables VSync, uses the default 1280x720 window, warms up for two seconds and measures for fifteen seconds. The finale starts after warmup. The program prints the actual framebuffer dimensions and GL renderer, waits for GPU completion with glFinish each frame, and writes benchmark_MODE.csv beside the executable. Frame times include CPU rendering, UI, completed GPU work and swap; they exclude CSV writing. Compare modes sequentially with other heavy work stopped. These are fixed-camera tests, not an exhaustive benchmark of all views or the TA machine. Ordinary interactive runs retain VSync.
