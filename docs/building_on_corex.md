# Building on corex

This document is the **integration guide** for using corex as the mathematical
and spatial foundation beneath your own engine, renderer, physics, or gameplay
systems.

If you only need function signatures, see [`api_reference.md`](api_reference.md).
For design rationale and conventions, see [`design.md`](design.md). This guide
focuses on **how to compose higher-level systems** from corex primitives.

---

## Table of contents

1. [Where corex sits in your stack](#1-where-corex-sits-in-your-stack)
2. [Embedding corex in a project](#2-embedding-corex-in-a-project)
3. [Coordinate spaces and conventions](#3-coordinate-spaces-and-conventions)
4. [The transform pipeline](#4-the-transform-pipeline)
5. [Building a scene graph](#5-building-a-scene-graph)
6. [Binding a renderer](#6-binding-a-renderer)
7. [Input, picking, and UI](#7-input-picking-and-ui)
8. [Collision workflows](#8-collision-workflows)
9. [Simple physics on top of corex](#9-simple-physics-on-top-of-corex)
10. [Game loop patterns](#10-game-loop-patterns)
11. [Recipes (copy-paste patterns)](#11-recipes-copy-paste-patterns)
12. [What corex does not provide](#12-what-corex-does-not-provide)
13. [Extending toward 3D (Phase 4)](#13-extending-toward-3d-phase-4)
14. [Common mistakes](#14-common-mistakes)
15. [Suggested project layout](#15-suggested-project-layout)

---

## 1. Where corex sits in your stack

corex is the **kernel layer**. Everything that deals with *where things are*,
*how they move*, or *whether they overlap* should ultimately express itself in
corex types.

```
┌─────────────────────────────────────────────┐
│  Your game / application                    │
│  (pong, platformer, editor, tool, sim)      │
├─────────────────────────────────────────────┤
│  Gameplay systems                           │
│  AI, scoring, inventory, animation logic    │
├─────────────────────────────────────────────┤
│  Engine services (YOU BUILD THESE)          │
│  scene graph, ECS, renderer, audio, input   │
├─────────────────────────────────────────────┤
│  corex  ← vectors, matrices, collision,     │
│           transforms, cameras, utils        │
├─────────────────────────────────────────────┤
│  Platform / OS / GPU API                    │
│  SDL, GLFW, Win32, OpenGL, Vulkan, etc.     │
└─────────────────────────────────────────────┘
```

**Rule of thumb:** if a function needs to know about pixels, textures, entities,
or file formats, it belongs *above* corex. If it only needs numbers and
spatial relationships, it probably belongs *in or on* corex.

### What you get for free

| Capability | corex provides |
| --- | --- |
| 2D points and directions | `vec2`, vector math |
| Object placement | `transform2d`, `mat3` |
| World ↔ view conversion | `camera2d` |
| Broad collision tests | `rect`, `circle`, intersection predicates |
| Interpolation / angles | `lerp`, `smoothstep`, `deg_to_rad` |

### What you build on top

| System | Built from |
| --- | --- |
| Scene graph | `transform2d` + parent/child matrix chain |
| Sprite rendering | `camera2d_world_to_screen` + your draw calls |
| Mouse picking | `camera2d_screen_to_world` + collision tests |
| Rigid-body motion | `vec2` integration + collision response |
| Tile maps | `rect` grid + `point_in_rect` |
| UI layout | `rect` + nested coordinate offsets |

---

## 2. Embedding corex in a project

### Single-header integration (recommended)

corex follows the STB model:

```c
/* corex_impl.c  — exactly ONE file in your project */
#define COREX_IMPLEMENTATION
#include "corex.h"
```

Every other file includes the header without the define:

```c
/* player.c */
#include "corex.h"
```

Link `corex_impl.c` with the rest of your project. On Unix you need `-lm`
because corex uses `<math.h>`.

### Multi-module C++ project

corex is C99 but C++-safe (`extern "C"`). Typical layout:

```cpp
// corex_impl.cpp
#define COREX_IMPLEMENTATION
#include "corex.h"

// game_object.hpp
#pragma once
#include "corex.h"

struct GameObject {
    transform2d local;
    mat3 world_matrix;
};
```

Use `corex_real` in your public engine API if you want precision switching
without rewriting types.

### Static / header-only in one translation unit

For tiny tools or jam games, you can define implementation in `main.c`:

```c
#define COREX_IMPLEMENTATION
#define COREX_STATIC   /* all symbols become static — no linker exports */
#include "corex.h"
```

### Configuration you should decide early

| Decision | Recommendation |
| --- | --- |
| Float vs double | **Float** (`default`) for games/rendering. **Double** for tools, editors, or simulations needing extra precision. Pick once per project — do not mix. |
| Assertion policy | Override `COREX_ASSERT` to integrate with your engine's debug break / logging. |
| Allocation | corex does not allocate today. Use `COREX_MALLOC` / `COREX_FREE` in *your* layers for a unified allocator story. |

---

## 3. Coordinate spaces and conventions

Understanding spaces is the most important prerequisite for building on corex.

### 3.1 World space

The global, authoritative coordinate system of your simulation.

- Positions are `vec2` in world units (meters, tiles, abstract units — your choice).
- +X is right, +Y is **up** (mathematical convention used throughout corex).
- Rotation is **counter-clockwise**, in **radians**.

> **Note:** many 2D APIs (SDL, some UI libs) use +Y down. You can either flip
> Y when converting to screen pixels, or treat world space as Y-up and invert
> only at the renderer boundary. Pick one convention and document it in your
> engine.

### 3.2 Local space

Coordinates relative to an object's origin before it is placed in the world.

- A sprite's vertices might be defined around `(0, 0)` or its center.
- `transform2d` maps **local → world** via `transform2d_matrix`.

### 3.3 View / camera space

Coordinates after the camera transform — what you draw relative to the camera.

- `camera2d_world_to_screen(cam, world_point)` produces view-space coordinates.
- The camera's own position maps to `(0, 0)` in view space.
- Zoom scales distances: `zoom = 2` makes everything appear twice as large.

### 3.4 Screen / pixel space

Physical pixels on the display. corex does **not** include viewport offset or
aspect ratio — you add that at the renderer layer (see [Section 6](#6-binding-a-renderer)).

### 3.5 Matrix convention (critical)

corex `mat3` is **row-major** storage with **column vectors**:

```
| m00 m01 m02 |   | x |
| m10 m11 m12 | * | y |
| m20 m21 m22 |   | 1 |
```

- `mat3_mul(A, B)` computes `A * B`.
- Applying transform `T` then `R` to a point: `mat3_mul(R, T)` (rightmost applied first).
- `transform2d_matrix` returns `T * R * S` — scale first, then rotate, then translate.

### 3.6 Geometry conventions

**`rect`** uses top-left corner + size:

```c
rect r = rect_new(x, y, width, height);
/* spans [x, x+w] × [y, y+h] inclusive on edges */
```

**`circle`** uses center + radius:

```c
circle c = circle_new(center, radius);
```

Collision tests use **inclusive** boundaries (`<=`, `>=`). Two shapes touching
edge-to-edge count as intersecting.

---

## 4. The transform pipeline

Every frame, spatial data flows through a predictable pipeline:

```
LOCAL POINT
    │  transform2d_matrix(local_transform)
    ▼
WORLD POINT
    │  camera2d_world_to_screen(camera)
    ▼
VIEW POINT
    │  your viewport transform (offset, Y-flip, pixel scale)
    ▼
SCREEN PIXELS
```

### Single object, no hierarchy

```c
transform2d obj = transform2d_identity();
obj.position = vec2_new(100.0f, 50.0f);
obj.rotation = deg_to_rad(45.0f);
obj.scale    = vec2_new(2.0f, 2.0f);

mat3 world = transform2d_matrix(obj);
vec2 world_corner = mat3_transform_vec2(world, vec2_new(1.0f, 0.0f));
```

### With camera

```c
camera2d cam = camera2d_identity();
cam.position = vec2_new(100.0f, 50.0f);
cam.zoom = 1.5f;

vec2 screen = camera2d_world_to_screen(cam, world_corner);
```

### Inverse (picking)

```c
vec2 mouse_view = /* from camera transform */;
vec2 mouse_world = camera2d_screen_to_world(cam, mouse_view);
```

The round-trip is exact (verified in `examples/camera_demo.c`):

```c
vec2 back = camera2d_screen_to_world(cam, camera2d_world_to_screen(cam, p));
/* vec2_distance(p, back) == 0 */
```

---

## 5. Building a scene graph

corex provides **local transforms** but not a scene graph. You add parent/child
relationships in your engine layer.

### Minimal node

```c
typedef struct scene_node {
    transform2d local;
    mat3        world;          /* cached each frame */
    struct scene_node *parent;
    struct scene_node **children;
    int child_count;
    int child_capacity;
} scene_node;
```

### Computing world matrices

A child's world matrix is the parent's world matrix multiplied by the child's
local matrix:

```c
void scene_node_update(scene_node *node, mat3 parent_world)
{
    mat3 local = transform2d_matrix(node->local);
    node->world = mat3_mul(parent_world, local);

    for (int i = 0; i < node->child_count; ++i)
        scene_node_update(node->children[i], node->world);
}

void scene_update(scene_node *root)
{
    root->world = transform2d_matrix(root->local);
    for (int i = 0; i < root->child_count; ++i)
        scene_node_update(root->children[i], root->world);
}
```

**Order matters:** `world = parent_world * local_matrix`. The local transform is
applied first (rightmost), then accumulated up the hierarchy.

### Transforming points through the hierarchy

```c
vec2 local_point = vec2_new(10.0f, 0.0f);
vec2 world_point = mat3_transform_vec2(node->world, local_point);
```

### Propagating direction vectors

For velocities, normals, or offsets that should **not** pick up translation,
use only the rotation/scale part — or transform two points and subtract:

```c
vec2 world_velocity = vec2_sub(
    mat3_transform_vec2(node->world, local_velocity),
    mat3_transform_vec2(node->world, vec2_zero())
);
```

For uniform scale and rotation only, `vec2_rotate(local_velocity, total_rotation)`
plus scale multiplication is cheaper.

---

## 6. Binding a renderer

corex does not draw anything. Your renderer adapter converts corex spatial types
into draw calls.

### 6.1 Viewport struct (you define this)

```c
typedef struct viewport {
    float x, y;       /* top-left in screen pixels */
    float width, height;
    int flip_y;       /* 1 if your GPU/API uses +Y down */
} viewport;
```

### 6.2 World → pixel conversion

```c
vec2 world_to_pixel(camera2d cam, viewport vp, vec2 world)
{
    vec2 view = camera2d_world_to_screen(cam, world);
    vec2 pixel = vec2_new(
        vp.x + vp.width  * 0.5f + view.x,
        vp.y + vp.height * 0.5f + (vp.flip_y ? -view.y : view.y)
    );
    return pixel;
}
```

This assumes:
- Camera view space origin is the **center** of the viewport.
- World units map 1:1 to pixels at `zoom = 1` (adjust with an extra scale if needed).

### 6.3 Drawing an axis-aligned sprite

```c
void draw_sprite(camera2d cam, viewport vp, transform2d obj,
                 float sprite_w, float sprite_h)
{
    mat3 world = transform2d_matrix(obj);

    /* Four corners in local space (centered sprite) */
    vec2 corners[4] = {
        vec2_new(-sprite_w * 0.5f, -sprite_h * 0.5f),
        vec2_new( sprite_w * 0.5f, -sprite_h * 0.5f),
        vec2_new( sprite_w * 0.5f,  sprite_h * 0.5f),
        vec2_new(-sprite_w * 0.5f,  sprite_h * 0.5f),
    };

    for (int i = 0; i < 4; ++i) {
        vec2 world_pt = mat3_transform_vec2(world, corners[i]);
        corners[i] = world_to_pixel(cam, vp, world_pt);
    }

    /* your_gpu_draw_quad(corners[0..3], texture, color); */
}
```

### 6.4 Batch-friendly path

If you transform many points per frame, cache the camera view matrix:

```c
mat3 view = camera2d_view(cam);
/* For each point: mat3_transform_vec2(view, world_point) then viewport step */
```

---

## 7. Input, picking, and UI

### 7.1 Mouse world position

```c
vec2 pixel_to_view(viewport vp, float mx, float my)
{
    return vec2_new(
        mx - (vp.x + vp.width  * 0.5f),
        (vp.flip_y ? -(my - (vp.y + vp.height * 0.5f))
                   :  (my - (vp.y + vp.height * 0.5f)))
    );
}

vec2 mouse_world(camera2d cam, viewport vp, float mx, float my)
{
    vec2 view = pixel_to_view(vp, mx, my);
    return camera2d_screen_to_world(cam, view);
}
```

### 7.2 Hit testing entities

```c
bool hit_test_circle(vec2 world_mouse, vec2 entity_pos, float radius)
{
    circle c = circle_new(entity_pos, radius);
    return point_in_circle(world_mouse, c);
}

bool hit_test_aabb(vec2 world_mouse, rect bounds)
{
    return point_in_rect(world_mouse, bounds);
}
```

For rotated sprites, transform the mouse into **local space** and test against
an axis-aligned shape there:

```c
bool hit_test_transformed(vec2 world_mouse, transform2d obj, rect local_bounds)
{
    mat3 world = transform2d_matrix(obj);
    /* Invert: translate by -position, rotate by -rotation, scale by 1/scale.
       For production engines, add mat3_inverse to your math layer. */
    vec2 local = /* your inverse transform of world_mouse */;
    return point_in_rect(local, local_bounds);
}
```

### 7.3 UI panels

UI is just nested rectangles in screen space:

```c
rect panel = rect_new(20.0f, 20.0f, 300.0f, 200.0f);
if (point_in_rect(mouse_pixel, panel)) {
    /* handle UI */
}
```

Keep UI in **screen space** and gameplay in **world space** to avoid coupling.

---

## 8. Collision workflows

corex provides **boolean intersection tests**, not collision resolution,
manifolds, or broadphase structures. Build those in your engine layer.

### 8.1 Collision shapes as components

```c
typedef enum {
    COLSHAPE_RECT,
    COLSHAPE_CIRCLE
} collider_kind;

typedef struct collider {
    collider_kind kind;
    union {
        rect   rect;    /* in local space, relative to entity origin */
        circle circle;
    } shape;
} collider;
```

### 8.2 World-space collider from transform

```c
circle collider_world_circle(transform2d obj, collider col)
{
    vec2 center = mat3_transform_vec2(transform2d_matrix(obj), col.shape.circle.center);
    corex_real max_scale = obj.scale.x > obj.scale.y ? obj.scale.x : obj.scale.y;
    return circle_new(center, col.shape.circle.radius * max_scale);
}

rect collider_world_rect(transform2d obj, collider col)
{
    /* For axis-aligned rects attached to rotated objects, either:
       (a) forbid rotation on rect colliders, or
       (b) use a circle approximation, or
       (c) add OBB support in your engine layer. */
    vec2 pos = mat3_transform_vec2(transform2d_matrix(obj), vec2_new(col.shape.rect.x, col.shape.rect.y));
    return rect_new(pos.x, pos.y, col.shape.rect.w * obj.scale.x, col.shape.rect.h * obj.scale.y);
}
```

### 8.3 Pair test dispatch

```c
bool colliders_overlap(circle a, circle b) { return circle_intersects(a, b); }
bool collider_rect_circle(rect r, circle c) { return rect_circle_intersects(r, c); }
```

### 8.4 Layer masks (you add this)

corex has no physics layers. A typical pattern:

```c
typedef struct body {
    transform2d transform;
    collider    shape;
    uint32_t    layer;       /* what I am */
    uint32_t    mask;        /* what I collide with */
    vec2        velocity;
} body;

bool layers_can_collide(body *a, body *b)
{
    return (a->layer & b->mask) && (b->layer & a->mask);
}
```

### 8.5 Resolution pattern (from `examples/pong.c`)

The pong example shows the simplest resolution workflow:

1. **Detect** overlap with `rect_circle_intersects`.
2. **Separate** by pushing the object out along the penetration axis.
3. **Reflect** velocity on the collision normal.

```c
/* Wall bounce: flip velocity component */
ball_vel.y = -ball_vel.y;

/* Paddle hit: push ball out, flip horizontal velocity */
ball.center.x = paddle.x + paddle.w + ball.radius;
ball_vel.x = -ball_vel.x;
```

For a general engine, you will want contact normals (`vec2_normalize`),
impulse-based response, and optional friction — all built from `vec2` math.

---

## 9. Simple physics on top of corex

corex is not a physics engine, but you can implement lightweight motion easily.

### 9.1 Kinematic integration (Euler)

```c
void body_integrate(body *b, corex_real dt)
{
    b->transform.position = vec2_add(b->transform.position, vec2_mul(b->velocity, dt));
}
```

### 9.2 Acceleration

```c
void body_apply_force(body *b, vec2 force, corex_real inv_mass, corex_real dt)
{
    b->velocity = vec2_add(b->velocity, vec2_mul(force, inv_mass * dt));
}
```

### 9.3 Gravity

```c
b->velocity = vec2_add(b->velocity, vec2_mul(gravity, dt));
```

### 9.4 Drag

```c
b->velocity = vec2_mul(b->velocity, (corex_real)1 - drag * dt);
```

### 9.5 Deterministic fixed timestep

From `examples/pong.c`:

```c
const corex_real dt = (corex_real)1 / (corex_real)60;
while (running) {
    simulate(world, dt);   /* always the same dt — reproducible */
    render(world);
}
```

Use `lerp` / `smoothstep` for visual interpolation between simulation steps
if you decouple sim rate from frame rate.

---

## 10. Game loop patterns

### 10.1 Minimal loop

```c
typedef struct {
    camera2d    camera;
    scene_node *root;
    body       *bodies;
    int         body_count;
} world;

void world_update(world *w, corex_real dt)
{
    scene_update(w->root);
    for (int i = 0; i < w->body_count; ++i)
        body_integrate(&w->bodies[i], dt);
    /* collision detect + resolve */
}

void world_draw(world *w, viewport vp)
{
    /* traverse scene graph, world_to_pixel, draw */
}
```

### 10.2 Update vs render separation

| Phase | Uses corex for |
| --- | --- |
| **Update** | integration, collision, AI movement, transform caching |
| **Render** | `camera2d_world_to_screen`, viewport mapping, culling via `rect_intersects` |

Culling example — skip objects outside the view:

```c
rect view_bounds = /* compute world-space AABB of viewport corners */;
if (!rect_intersects(object_bounds, view_bounds))
    return; /* skip draw */
```

### 10.3 System dependency order

Recommended update order:

1. Read input → world-space mouse/aim vectors
2. Apply player / AI forces
3. Integrate velocities → positions
4. Update scene graph world matrices
5. Run collision detection + resolution
6. Run gameplay logic (damage, scoring, triggers)
7. Render

---

## 11. Recipes (copy-paste patterns)

### 11.1 Direction from angle

```c
#include <math.h>

vec2 direction_from_angle(corex_real radians)
{
    return vec2_new(cosf((float)radians), sinf((float)radians));
    /* With COREX_USE_DOUBLE=1, use cos() / sin() instead of cosf() / sinf(). */
}
```

### 11.2 Angle between two vectors

```c
corex_real angle_between(vec2 a, vec2 b)
{
    corex_real dot = vec2_dot(vec2_normalize(a), vec2_normalize(b));
    dot = clamp(dot, (corex_real)-1, (corex_real)1);
    return acosf(dot); /* add to your math layer if needed */
}
```

### 11.3 Move toward target at constant speed

```c
vec2 move_toward(vec2 from, vec2 to, corex_real max_delta)
{
    vec2 diff = vec2_sub(to, from);
    corex_real dist = vec2_len(diff);
    if (dist <= max_delta || dist < COREX_EPSILON)
        return to;
    return vec2_add(from, vec2_mul(diff, max_delta / dist));
}
```

Used in pong for paddle tracking (`pong_track` uses the same idea with `clamp`).

### 11.4 Rect from center + size

```c
rect rect_from_center(vec2 center, corex_real w, corex_real h)
{
    return rect_new(center.x - w * 0.5f, center.y - h * 0.5f, w, h);
}
```

### 11.5 Expand rect by margin (fatten hitbox)

```c
rect rect_expand(rect r, corex_real margin)
{
    return rect_new(r.x - margin, r.y - margin, r.w + 2.0f * margin, r.h + 2.0f * margin);
}
```

### 11.6 Circle from two points (diameter)

```c
circle circle_from_diameter(vec2 a, vec2 b)
{
    vec2 center = vec2_lerp(a, b, (corex_real)0.5);
    return circle_new(center, vec2_distance(a, b) * (corex_real)0.5);
}
```

### 11.7 Follow camera (smooth)

```c
#include <math.h>

void camera_follow(camera2d *cam, vec2 target, corex_real smooth, corex_real dt)
{
    cam->position = vec2_lerp(cam->position, target, (corex_real)1 - expf((float)(-smooth * dt)));
}
```

### 11.8 Zoom toward mouse cursor

```c
void camera_zoom_at_cursor(camera2d *cam, vec2 mouse_world,
                           corex_real new_zoom, corex_real old_zoom)
{
    /* Keep the world point under the cursor fixed while zoom changes. */
    vec2 offset = vec2_sub(mouse_world, cam->position);
    corex_real ratio = old_zoom / new_zoom;
    cam->position = vec2_add(cam->position, vec2_mul(offset, (corex_real)1 - ratio));
    cam->zoom = new_zoom;
}
```

---

## 12. What corex does not provide

Plan to implement these in your engine layer (or wait for Phase 4 in corex):

| Missing today | Typical home |
| --- | --- |
| `vec3` / 3D math ops | Your engine or future corex Phase 4 |
| `mat3_inverse`, `mat4` transforms | Your engine math extensions |
| OBB, polygon, capsule collision | Physics / collision module |
| Ray casting | Rendering / picking module |
| Spatial partitioning (grid, BVH) | Broadphase in physics |
| Quaternions | 3D rotation module |
| Entity IDs, ECS, scene graph | Your engine core |
| Serialization | Your save/load system |
| Time, events, resources | Platform / engine services |

**Do not patch these into `corex.h` unless they belong in the foundation layer
for everyone.** Prefer a separate `engine_math.h` or `mygame_physics.h` that
includes corex and adds project-specific utilities.

---

## 13. Extending toward 3D (Phase 4)

corex already declares `vec3`, `vec4`, and `mat4` so your engine types can
stabilize before the 3D API lands:

```c
typedef struct entity3d {
    vec3 position;      /* future */
    /* quaternion rotation — add in your layer for now */
    vec3 scale;
} entity3d;
```

When Phase 4 arrives, the expected pattern mirrors 2D:

```
local  ──mat4──►  world  ──view_proj──►  clip/screen
```

Your 2D engine code should continue working unchanged — 3D is additive.

---

## 14. Common mistakes

### Mixing degrees and radians

`transform2d.rotation` and `camera2d.rotation` are **always radians**. Convert
at the UI boundary:

```c
obj.rotation = deg_to_rad(editor_degrees);
```

### Assuming +Y down

If your renderer uses +Y down, **flip at the renderer boundary**, not inside
simulation code, or your physics will feel inverted.

### Using `rect` on rotated objects without care

`rect_intersects` is **axis-aligned**. A rotated sprite needs an OBB test or
a circle approximation in your engine layer.

### Composing transforms in the wrong order

Remember: `mat3_mul(A, B)` means **apply B first, then A**. Parent world matrix
is the left operand; local is the right.

### Comparing floats with `==`

Use `COREX_EPSILON` or squared distance (`vec2_distance_sq`) for proximity tests.

### Defining `COREX_IMPLEMENTATION` in multiple files

You will get duplicate symbol linker errors. Exactly **one** `.c` / `.cpp` file.

### Forgetting `-lm` on Linux/macOS

Link with `-lm` when compiling the implementation unit.

---

## 15. Suggested project layout

Once you outgrow a single-file jam game, a clean layout that keeps corex as the
kernel:

```
myengine/
├── third_party/
│   └── corex.h              # vendored copy or submodule
├── src/
│   ├── corex_impl.c         # #define COREX_IMPLEMENTATION
│   ├── math_ext.c           # mat3_inverse, angle_from_vec, etc.
│   ├── scene.c              # scene graph
│   ├── physics.c            # bodies, integration, resolution
│   ├── render_adapter.c     # world_to_pixel, draw submission
│   └── main.c
├── include/
│   ├── scene.h
│   ├── physics.h
│   └── render_adapter.h
└── examples/
    └── my_game.c
```

### Submodule setup (optional)

```bash
git submodule add https://github.com/MinhazAbtahi/corex.git third_party/corex
```

Then `#include "third_party/corex/corex.h"`.

---

## Quick reference: which corex type for which job

| Job | Use |
| --- | --- |
| Position, velocity, direction | `vec2` |
| Object placement | `transform2d` |
| Cached world matrix | `mat3` via `transform2d_matrix` |
| Camera | `camera2d` |
| Axis-aligned bounds | `rect` |
| Round hitbox | `circle` |
| Hit test | `point_in_*`, `*_intersects` |
| Color for renderer | `color` |
| Smooth animation | `lerp`, `smoothstep` |
| Angle conversion | `deg_to_rad`, `rad_to_deg` |

---

## Next steps

1. Read through `examples/pong.c` — a complete sim loop using collision and integration.
2. Read through `examples/camera_demo.c` — camera round-trip guarantees.
3. Run `examples/collision_demo.c` — baseline for your own test suite.
4. Start your engine with a `world` struct, a `scene_node`, and a `viewport`.
5. Add `math_ext.c` only when you need inverses, angles, or 3D — not before.

For API details: [`api_reference.md`](api_reference.md).
For conventions and config: [`design.md`](design.md).
