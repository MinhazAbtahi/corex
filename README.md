# corex

[![CI](https://github.com/MinhazAbtahi/corex/actions/workflows/ci.yml/badge.svg)](https://github.com/MinhazAbtahi/corex/actions/workflows/ci.yml)
![C99](https://img.shields.io/badge/C-99-blue.svg)
![Single header](https://img.shields.io/badge/single--header-yes-brightgreen.svg)
![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)

> A minimal spatial + math + transform + collision + camera kernel for 2D/3D game engines — in a single C99 header.

`corex` is the **mathematical foundation layer** that game engines, renderers,
and physics systems build on top of. It is the kind of core that Unity, Godot,
and Unreal keep internally for transforms and 2D/3D math — but small, readable,
dependency-free, and yours.

`corex` **is**: vectors, matrices, geometry, collision, transforms, cameras, and utility math.

`corex` is **not**: a game engine, a renderer, or a physics engine.

## Highlights

- **Single header**, STB-style — drop in `corex.h`, define `COREX_IMPLEMENTATION` once.
- **Zero dependencies** beyond the C standard library.
- **C99 portable**, and usable from C++ (wrapped in `extern "C"`).
- **`float` or `double`** precision via a single `#define`.
- **Deterministic math** with documented conventions (row-major `mat3`, `T*R*S` composition).
- **Minimal but complete** — enough to express real gameplay (see `examples/`).

## Quick start

```bash
git clone https://github.com/MinhazAbtahi/corex.git
```

Or just copy the single `corex.h` into your project. In **one** translation unit:

```c
#define COREX_IMPLEMENTATION
#include "corex.h"
```

Everywhere else, just `#include "corex.h"`.

```c
#define COREX_IMPLEMENTATION
#include "corex.h"
#include <stdio.h>

int main(void) {
    transform2d t = transform2d_identity();
    t.position = vec2_new(10.0f, 5.0f);
    t.rotation = deg_to_rad(90.0f);

    mat3 m = transform2d_matrix(t);
    vec2 world = mat3_transform_vec2(m, vec2_new(1.0f, 0.0f));

    printf("world = (%.3f, %.3f)\n", world.x, world.y); /* ~ (10.000, 6.000) */
    return 0;
}
```

## Modules

| Module | What it provides |
| --- | --- |
| **Types** | `vec2`, `vec3`, `vec4`, `mat3`, `mat4`, `rect`, `circle`, `transform2d`, `camera2d`, `color` |
| **Vector math** | add/sub/scale, dot/cross, length, normalize, lerp, distance, perp, rotate |
| **Matrix system** | identity, translation, rotation, scale, multiply, transform a point |
| **Geometry** | rect & circle constructors, rect center |
| **Collision** | point↔rect, point↔circle, rect↔rect, circle↔circle, rect↔circle |
| **Transform** | `transform2d_matrix` builds `T * R * S` |
| **Camera** | `camera2d_view`, `world_to_screen`, `screen_to_world` (exact inverse) |
| **Utility** | `clamp`, `lerp`, `inverse_lerp`, `smoothstep`, `deg_to_rad`, `rad_to_deg` |

See [Documentation](#documentation) below.

## Configuration

Define these before including the implementation:

| Macro | Default | Purpose |
| --- | --- | --- |
| `COREX_USE_DOUBLE` | `0` | Use `double` instead of `float`. |
| `COREX_STATIC` | _unset_ | Give all functions `static` linkage. |
| `COREX_ASSERT(x)` | `assert(x)` | Override the assertion hook. |
| `COREX_MALLOC` / `COREX_FREE` | `malloc`/`free` | Allocation hooks (reserved for higher layers). |

## Documentation

| Document | Purpose |
| --- | --- |
| [`docs/building_on_corex.md`](docs/building_on_corex.md) | **Integration guide** — scene graphs, renderer binding, physics, recipes |
| [`docs/api_reference.md`](docs/api_reference.md) | Complete function and type reference |
| [`docs/design.md`](docs/design.md) | Design goals, matrix conventions, configuration |

## Examples

Located in [`examples/`](examples/):

- **`pong.c`** — a headless Pong simulation (vectors, geometry, collision, fixed-timestep loop).
- **`camera_demo.c`** — world↔screen transforms and a verified zero-error round trip.
- **`collision_demo.c`** — an asserted test suite for every collision predicate.

### Building the examples

**Windows (MSYS2 / MinGW), one command:**

```bat
build.bat run
```

**With Make (Linux, macOS, MSYS2):**

```bash
make run      # build and run all examples
make          # just build into build/
make clean
```

**Manually with any C99 compiler:**

```bash
gcc -std=c99 -Wall -Wextra -I. examples/collision_demo.c -o collision_demo -lm
./collision_demo
```

```bat
cl /I. examples\collision_demo.c
```

## Roadmap

- **Phase 1 — Foundation** ✅ `vec2`, `mat3`, `rect`, `circle`, utils.
- **Phase 2 — Engine core** ✅ `transform2d`, `camera2d`, collision.
- **Phase 3 — Game-ready** ✅ pong / camera / collision examples.
- **Phase 4 — Future** ⏳ `vec3`/`mat4` 3D transforms, ray casting, ECS hooks, renderer binding.

## Contributing

Contributions are welcome — see [CONTRIBUTING.md](CONTRIBUTING.md).

## License

Licensed under the [MIT License](LICENSE).
