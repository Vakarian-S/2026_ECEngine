# Vendored libraries

Only the files this project uses, so it builds and runs without a `C:\GameDev` install.

| Library | Version | License |
|---|---|---|
| SDL3 | 3.2.28 | zlib (see header comments) |
| SDL3_image | 3.2.4 | zlib (see header comments) |
| GLEW | 2.1.0 | Modified BSD / MIT (see `glew.h`) |
| MathLibrary | course-provided | Used with the instructor's permission |

Layout:

- `include/` headers
- `lib/` x64 import libraries
- `dll/` x64 runtime DLLs (copied next to the `.exe` after each build)
