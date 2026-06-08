# corex — API Reference

Version 1.0.0. All functions use `corex_real`, which is `float` by default or
`double` when `COREX_USE_DOUBLE` is set to `1`.

```c
#define COREX_IMPLEMENTATION   /* in exactly one .c/.cpp file */
#include "corex.h"
```

## Constants

| Name | Value |
| --- | --- |
| `COREX_PI` | 3.14159265358979323846 |
| `COREX_TAU` | 6.28318530717958647692 |
| `COREX_EPSILON` | 0.00001 |
| `COREX_VERSION_MAJOR` / `MINOR` / `PATCH` | `1` / `0` / `0` |

## Types

```c
typedef struct vec2 { corex_real x, y; } vec2;
typedef struct vec3 { corex_real x, y, z; } vec3;
typedef struct vec4 { corex_real x, y, z, w; } vec4;

typedef struct mat3 { corex_real m[3][3]; } mat3;  /* row-major */
typedef struct mat4 { corex_real m[4][4]; } mat4;  /* future-ready */

typedef struct rect   { corex_real x, y, w, h; } rect;
typedef struct circle { vec2 center; corex_real radius; } circle;

typedef struct transform2d { vec2 position; corex_real rotation; vec2 scale; } transform2d;
typedef struct camera2d    { vec2 position; corex_real rotation; corex_real zoom; } camera2d;

typedef struct color { corex_real r, g, b, a; } color;
```

## Utility math

| Function | Description |
| --- | --- |
| `corex_real clamp(x, min, max)` | Clamp `x` into `[min, max]`. |
| `corex_real lerp(a, b, t)` | Linear interpolation `a + (b-a)*t`. |
| `corex_real inverse_lerp(a, b, v)` | Inverse of `lerp`; `0` if `a == b`. |
| `corex_real smoothstep(a, b, t)` | Smooth Hermite interpolation, clamped to `[0,1]`. |
| `corex_real deg_to_rad(d)` | Degrees → radians. |
| `corex_real rad_to_deg(r)` | Radians → degrees. |

## Vector math (vec2)

| Function | Description |
| --- | --- |
| `vec2 vec2_new(x, y)` | Construct. |
| `vec2 vec2_zero(void)` | `(0, 0)`. |
| `vec2 vec2_one(void)` | `(1, 1)`. |
| `vec2 vec2_add(a, b)` | Component-wise add. |
| `vec2 vec2_sub(a, b)` | Component-wise subtract. |
| `vec2 vec2_mul(v, s)` | Scale by scalar. |
| `vec2 vec2_div(v, s)` | Divide by scalar (asserts `s != 0`). |
| `vec2 vec2_neg(v)` | Negate. |
| `corex_real vec2_dot(a, b)` | Dot product. |
| `corex_real vec2_cross(a, b)` | 2D scalar cross `ax*by - ay*bx`. |
| `corex_real vec2_len(v)` | Magnitude. |
| `corex_real vec2_len_sq(v)` | Squared magnitude. |
| `vec2 vec2_normalize(v)` | Unit vector (`zero` if length < epsilon). |
| `vec2 vec2_lerp(a, b, t)` | Component-wise lerp. |
| `corex_real vec2_distance(a, b)` | Distance between points. |
| `corex_real vec2_distance_sq(a, b)` | Squared distance. |
| `vec2 vec2_perp(v)` | 90° CCW perpendicular `(-y, x)`. |
| `vec2 vec2_rotate(v, radians)` | Rotate around origin. |

## Matrix system (mat3)

| Function | Description |
| --- | --- |
| `mat3 mat3_identity(void)` | Identity matrix. |
| `mat3 mat3_translation(p)` | Translation by `p`. |
| `mat3 mat3_rotation(radians)` | Rotation. |
| `mat3 mat3_scale(s)` | Scale by `s.x`, `s.y`. |
| `mat3 mat3_mul(a, b)` | Matrix product `a * b`. |
| `vec2 mat3_transform_vec2(m, v)` | Apply `m` to point `(v.x, v.y, 1)`. |
| `mat4 mat4_identity(void)` | 4×4 identity (future 3D use). |

## Geometry

| Function | Description |
| --- | --- |
| `rect rect_new(x, y, w, h)` | Construct axis-aligned rect. |
| `circle circle_new(center, radius)` | Construct circle. |
| `vec2 rect_center(r)` | Center point of a rect. |
| `color color_new(r, g, b, a)` | Construct color. |

## Collision

| Function | Description |
| --- | --- |
| `bool point_in_rect(p, r)` | Point inside/on rect. |
| `bool point_in_circle(p, c)` | Point inside/on circle. |
| `bool rect_intersects(a, b)` | AABB overlap test. |
| `bool circle_intersects(a, b)` | Circle overlap test. |
| `bool rect_circle_intersects(r, c)` | Rect vs circle (closest-point test). |

## Transform

| Function | Description |
| --- | --- |
| `transform2d transform2d_identity(void)` | Zero position, zero rotation, unit scale. |
| `mat3 transform2d_matrix(t)` | Build `T * R * S` from a transform. |

## Camera

| Function | Description |
| --- | --- |
| `camera2d camera2d_identity(void)` | Origin, no rotation, zoom `1`. |
| `mat3 camera2d_view(c)` | View matrix `S(zoom) * R(-rot) * T(-pos)`. |
| `vec2 camera2d_world_to_screen(c, p)` | World point → screen/view space. |
| `vec2 camera2d_screen_to_world(c, p)` | Screen/view point → world space. |

## Minimal example

```c
#define COREX_IMPLEMENTATION
#include "corex.h"
#include <stdio.h>

int main(void) {
    transform2d t = transform2d_identity();
    t.position = vec2_new(10.0f, 5.0f);
    t.rotation = deg_to_rad(90.0f);

    mat3 m = transform2d_matrix(t);
    vec2 local = vec2_new(1.0f, 0.0f);
    vec2 world = mat3_transform_vec2(m, local);

    printf("world = (%.3f, %.3f)\n", world.x, world.y); /* ~ (10.000, 6.000) */
    return 0;
}
```
