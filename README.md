# ECEngine_2026

![C++](https://img.shields.io/badge/C%2B%2B-20-blue.svg)
![Platform](https://img.shields.io/badge/platform-Windows%20x64-lightgrey)
![Build](https://img.shields.io/badge/build-MSVC-success)
![OpenGL](https://img.shields.io/badge/OpenGL-4.5-5586A4)

## Overview

**ECEngine_2026** is a small 3D game engine written in C++20 during my studies at **Humber Polytechnic**.
It uses **SDL3**, **OpenGL 4.5**, and **GLEW**, and is built around an **Entity-Component** design: every object in the scene is an `Actor` made of components such as Transform, Mesh, Shader, Material, Physics, and Collision.

The demo scene is a chessboard with 32 textured pieces, lit by moving point lights. You can generate colliders, show their wireframes, and launch pieces into each other from an in-engine ImGui panel.

---

## Engine Architecture

* **Actors and components.** Actors add, get, and remove components through type-safe templates. Actors can have parents, so the chess pieces follow the board when it moves.
* **Data-driven assets.** An `AssetManager` singleton reads `data/scene1.xml` (with tinyxml2). It loads each mesh, shader, and texture once, and all pieces of the same type share them.
* **Lighting.** A GLSL Phong shader supports up to 8 point lights, each with its own color, intensity, and distance falloff.
* **Firework lights.** Five lights launch from random spots, rise, flicker, burst, and fade, then respawn with new colors and timing.
* **Collision system.** A `CollisionSystem` checks sphere–sphere and AABB–AABB overlaps and bounces pieces with an elastic, mass-based response.
* **Memory safety.** Scenes own actors through `shared_ptr` (`Ref`), and back-references use `weak_ptr` (`WeakRef`). Debug builds print reference counts after setup and before teardown to catch leaks.

---

## Language Standard

This project **must be compiled using C++20**.

> Earlier C++ standards are not supported.

* **MSVC**: `/std:c++20`

---

## Math Library Acknowledgement

This project includes and relies on a **custom math library** provided by **Humber Polytechnic**, developed by:

* **Scott Fielder** (https://scottfielder.com/ · https://github.com/ScottFielder/MathLibrary)
* **Umer Noor**

The library provides:

* Vector and matrix math
* Quaternions and dual quaternions
* Planes and geometric algebra utilities

It is used throughout the project for:

* Transforms and parent-child model matrices
* Camera view and projection
* Collision detection and response
* Light and firework movement

Only the headers this project uses are included, in `vendor/MathLibrary/include/`.

---

## Setup

All third-party libraries are bundled in the `vendor/` folder, so no extra install or PATH setup is needed.

1. Open `ECEngine_2026.sln` in **Visual Studio 2022** or **JetBrains Rider**.
2. Select the **`x64`** platform (Debug or Release).
3. Build and run.

The build copies the SDL3 and GLEW DLLs next to the `.exe` automatically.

> Requires the MSVC **v143** toolset (Visual Studio 2022 C++ workload).

---

## Scene Overview

### Scene 1 — Chess Collision Sandbox

![Scene 1](images/Scene1.png)

**Overview**

A full chessboard built from XML data, lit by one static light and five firework lights that rise and burst over the board.
The ImGui panel lets you switch between four cameras, generate sphere or AABB colliders for every piece, show their wireframes (which change color on contact), and launch a chosen piece across the board to watch it collide with the others.

**Controls**

* `W` `A` `S` `D` Move the free camera forward, left, back, and right
* `SPACE` / `LEFT SHIFT` Move the free camera up / down
* `ESC` or `Q` Exit the application

**ImGui Panel**

| Section | What it does |
| ------- | ------------ |
| Camera View | Switch between the `Free`, `Top`, `Left`, and `Right` cameras |
| Generate Collision Bounds | Add `Sphere` or `AABB` colliders to every piece, or clear them |
| Visualization | Show or hide collider wireframes |
| Status | Show the active collider mode and the piece count |
| Launch Piece | Pick a piece type and index, then launch it across the board |

---

## Dependencies

All bundled in `vendor/`, except for the libraries compiled with the project.

| Library | Version | Location |
| ------- | ------- | -------- |
| SDL3 | 3.2.28 | `vendor/SDL3` |
| SDL3_image | 3.2.4 | `vendor/SDL3` |
| GLEW | 2.1.0 | `vendor/GLEW` |
| Humber Math Library | course-provided | `vendor/MathLibrary` |
| Dear ImGui | 1.91.0 | `ECEngine_2026/imgui` |
| tinyxml2 | 9.0.0 | `ECEngine_2026/tinyxml` |
| tinyobjloader | 2.0.0 | `ECEngine_2026/tiny_obj_loader.h` |

* **OpenGL 4.5** capable GPU and driver
* **MSVC (Windows, x64)**

---

## Academic Context

This project represents **my personal work completed at Humber Polytechnic** as part of the Game Programming curriculum.

It reflects:

* Component-based engine architecture
* Data-driven scene setup
* Real-time lighting and collision physics
* Careful ownership and memory management with smart pointers

---

## AI Usage

Generative AI was used to help prepare this repository for GitHub. It assisted with the following commits:

* [`1b6fe88`](https://github.com/Vakarian-S/2026_ECEngine/commit/1b6fe88b2d94eca339426e2c3ebcca98e1f9c986) — Set up the `.gitignore`, untracked build output, and tracked the required assets
* [`cd8b0d7`](https://github.com/Vakarian-S/2026_ECEngine/commit/cd8b0d7de46a81e0c6638af8ef5421e8de0d92e3) — Moved the third-party libraries into `vendor/` and updated the project paths
* [`536906b`](https://github.com/Vakarian-S/2026_ECEngine/commit/536906b0a487a826b3a7b92e40ca768d76ae1cf8) — Renamed and moved the solution and project files
* [`36fe080`](https://github.com/Vakarian-S/2026_ECEngine/commit/36fe080c0ada01c48ea63a1a9a69bc5a4bf9cf9b) — Drafted this README

---

## License

This repository is intended for **educational and portfolio purposes**.
Third-party libraries and provided academic materials remain the property of their respective authors.

---
