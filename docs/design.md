# corex — Design

`corex` is a **foundation layer** for game engines and graphics systems. It is
the mathematical and spatial *kernel* that renderers, physics, and gameplay
systems build on top of.

What corex **is**:

- a minimal spatial + math + transform + collision + camera + utility kernel

What corex **is not**:

- not a game engine
- not a renderer
- not a physics engine

## Design goals

- **STB-style single header.** One file, drop it in, define
  `COREX_IMPLEMENTATION` in exactly one translation unit.
- **Zero dependencies.** Only the C standard library (`<math.h>`, `<assert.h>`,
  `<stdlib.h>`, `<stdbool.h>`).
- **C99 portable, usable from C++.** All public symbols are wrapped in
  `extern "C"` when compiled as C++.
- **Minimal but complete.** Enough to express transforms, cameras, and
  collision without bloat.
- **Deterministic math.** Fixed evaluation order, no hidden global state.
- **Engine-grade naming.** `vec2_add`, `mat3_mul`, `transform2d_matrix`, etc.
- **Expandable to 3D later** (`vec3`, `vec4`, `mat4` types are already present).

## Configuration

Define these *before* including the header (before the implementation):

| Macro | Default | Purpose |
| --- | --- | --- |
| `COREX_USE_DOUBLE` | `0` | Use `double` instead of `float` for `corex_real`. |
| `COREX_STATIC` | _unset_ | Give all functions `static` linkage. |
| `COREX_ASSERT(x)` | `assert(x)` | Override the assertion hook. |
| `COREX_MALLOC(sz)` | `malloc` | Allocation hook (unused by the core today). |
| `COREX_FREE(ptr)` | `free` | Free hook (unused by the core today). |

### The `corex_real` switch

```c
#if COREX_USE_DOUBLE
typedef double corex_real;
#else
typedef float corex_real;
#endif
```

All math routes through precision-aware primitives (`COREX__SQRT`, `COREX__SIN`,
`COREX__COS`, `COREX__FABS`) selected by the same switch.

## Conventions

### Matrices

`mat3` is the core 2D transform type. It is stored **row-major**
(`m[row][col]`) and points are treated as **column vectors** `(x, y, 1)`:

```
| m00 m01 m02 |   | x |
| m10 m11 m12 | * | y |
| m20 m21 m22 |   | 1 |
```

`mat3_transform_vec2` applies this product and returns the `(x, y)` part.

### Composition order

`transform2d_matrix` builds `T * R * S`. Applied to a point this means **scale
first, then rotate, then translate** — the order intuitively expected for an
object's local transform.

### Camera

`camera2d_view` is the inverse of the camera's world placement:

```
view = S(zoom) * R(-rotation) * T(-position)
```

`screen_to_world` is implemented as the exact analytical inverse rather than a
generic matrix inversion, so the round trip is numerically stable:

```
world = T(position) * R(rotation) * S(1/zoom)
```

The `examples/camera_demo.c` program verifies that
`screen_to_world(world_to_screen(p)) == p` to within floating-point error.

## Build philosophy (layers)

1. **Foundation** — `vec2`, `mat3`, `rect`, `circle`, utility math. Goal: math
   correctness.
2. **Engine core** — `transform2d`, `camera2d`, collision. Goal: spatial
   reasoning.
3. **Game-ready** — examples like pong, camera, collision. Goal: usable
   gameplay primitives.
4. **Future expansion** — `vec3`, `mat4`, 3D transforms, ray casting, ECS
   hooks, renderer binding. Goal: evolve into a full engine kernel.

The types for phase 4 (`vec3`, `vec4`, `mat4`) already exist so the public ABI
does not churn as those layers land.
