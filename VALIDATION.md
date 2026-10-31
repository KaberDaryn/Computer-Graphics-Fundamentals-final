# Technical validation

Validation date: 31 October 2026. Status: conditionally ready; the TA machine, MSVC and Linux are not verified.

## Builds and automated tests

Clean GCC 16.2.0 / Ninja builds succeeded on Windows in Debug and Release. WowSequenceStateMachine and CameraControls passed in both configurations. Finale assertions remain enabled in Release. The application started successfully from a different working directory using an absolute executable path.

Visual Studio/MSVC was not installed in the inspected environment. WSL exposed only the Docker Desktop service distribution, so no Linux development build was performed.

## Performance

CPU: AMD Ryzen 7 4800H. Active GL renderer: AMD Radeon(TM) Graphics (not the separate NVIDIA GPU). OpenGL: 3.3 Core; driver string 24.3.1.240216. Framebuffer: 1280x720. Release build, VSync disabled, fixed camera and UI visible. Two seconds warmup followed by fifteen seconds sampling per mode. glFinish includes completed GPU work; CPU rendering, UI and swap are timed. CSV writing is excluded. Desktop background activity was not controlled; these are short local observations, not isolated laboratory measurements or a guarantee for the TA machine.

| Mode | Frames | Average FPS | Mean ms | P99 ms | Maximum ms |
| --- | ---: | ---: | ---: | ---: | ---: |
| Feed enabled | 7137 | 475.87 | 2.101 | 2.658 | 11.497 |
| Feed disabled | 7807 | 520.59 | 1.921 | 2.563 | 9.515 |
| Finale with feed | 6506 | 433.78 | 2.305 | 3.025 | 9.106 |

Reproduce with the README benchmark commands. Repeat on the target computer, with both feed settings and camera views relevant to the demonstration.

## Graphics and requirements

Real OpenGL rendering was inspected for the desk, monitors, keyboard, chair, plant, lamp, textured floor, window, secondary camera and dark RGB finale. No obvious artifacts appeared in inspected views; arbitrary camera positions were not exhaustively tested. The user independently confirmed controls and ImGui interactions. GPU memory leaks were not checked with a profiler.

| Requirement | Status | Evidence |
| --- | --- | --- |
| R1 | PASS | Mesh.cpp VAO/VBO/EBO, Shader.cpp and GLSL; real Core context |
| R2 | PASS | DeskFactory.cpp and RoomFactory.cpp; recognizable objects rendered |
| R3 | PASS | SceneNode.cpp hierarchy, Camera.cpp, basic.vert MVP and inverse-transpose normals |
| R4 | PASS | Lighting.cpp and basic.frag; multiple point lights and switchable lamp |
| R5 | PASS | Texture.cpp mipmaps, UVs and rgb.frag |
| R6 | PASS | Application.cpp and UiPanel.cpp; user control verification and camera test |
| R7 | PARTIAL | Local performance exceeds target; TA machine untested |
| R8 | PASS on GCC/Ninja | Clean Debug/Release builds; MSVC/Linux not tested |

Both stretch goals are implemented: a framebuffer with color/depth attachments supplies the live monitor, excluding that monitor from the producer pass; day/night controls affect window and lighting. The monitor target stays at 640x368 independently of window size, while the main viewport follows framebuffer dimensions.

## Changes and limitations

Asset discovery works beside the executable, in the working directory or source tree. Day/night keyboard switching is smooth. Texture loading restores unpack alignment and texture binding. A camera regression test was added; the finale test no longer silently loses assertions in Release. A separate environment-controlled benchmark records frame times. Normal interactive controls and VSync remain unchanged.

Before final submission, verify MSVC if required by the course, test on the TA machine, and inspect additional camera angles. No release tag or academic deliverables are certified by this validation.
