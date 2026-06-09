/*
    corex.h - a minimal spatial + math kernel for 2D/3D game engines
    ------------------------------------------------------------------
    Single-header, STB-style library. C99 + optional C++.

    Corex is a FOUNDATION LAYER. It is not a game engine, renderer, or
    physics engine. It provides the mathematical and spatial kernel that
    such systems build upon:

        vectors, matrices, geometry, collision, transforms, cameras, utils.

    ------------------------------------------------------------------
    USAGE
    ------------------------------------------------------------------
    This is a single-header library. In *one* C or C++ translation unit,
    define COREX_IMPLEMENTATION before including the header:

        #define COREX_IMPLEMENTATION
        #include "corex.h"

    In every other translation unit, just include it normally:

        #include "corex.h"

    ------------------------------------------------------------------
    CONFIGURATION (define before including the implementation)
    ------------------------------------------------------------------
        COREX_USE_DOUBLE   - set to 1 to use double precision (default 0)
        COREX_STATIC       - make all functions 'static'
        COREX_ASSERT(x)    - override the assert macro
        COREX_MALLOC(sz)   - override allocation (currently unused by core)
        COREX_FREE(ptr)    - override free       (currently unused by core)

    ------------------------------------------------------------------
    LICENSE: MIT. See the LICENSE file in the repository root.
*/

#ifndef COREX_H
#define COREX_H

#define COREX_VERSION_MAJOR 1
#define COREX_VERSION_MINOR 0
#define COREX_VERSION_PATCH 0

/* ================================================================== */
/*  Configuration                                                     */
/* ================================================================== */

#ifndef COREX_USE_DOUBLE
#define COREX_USE_DOUBLE 0
#endif

/* Linkage / definition specifier (STB style). */
#ifndef COREX_DEF
#ifdef COREX_STATIC
#define COREX_DEF static
#else
#define COREX_DEF extern
#endif
#endif

/* Assertion hook. Defaults to <assert.h>. */
#ifndef COREX_ASSERT
#include <assert.h>
#define COREX_ASSERT(x) assert(x)
#endif

/* Allocation hooks. The core kernel does not allocate, but these are
   provided so dependent layers can route through a single policy. */
#if !defined(COREX_MALLOC) || !defined(COREX_FREE)
#include <stdlib.h>
#ifndef COREX_MALLOC
#define COREX_MALLOC(size) malloc(size)
#endif
#ifndef COREX_FREE
#define COREX_FREE(ptr) free(ptr)
#endif
#endif

/* bool for the C path; C++ has it built in. */
#ifndef __cplusplus
#include <stdbool.h>
#endif

/* ================================================================== */
/*  Real number type                                                  */
/* ================================================================== */

#if COREX_USE_DOUBLE
typedef double corex_real;
#else
typedef float corex_real;
#endif

/* ================================================================== */
/*  Constants                                                         */
/* ================================================================== */

#define COREX_PI      ((corex_real)3.14159265358979323846)
#define COREX_TAU     ((corex_real)6.28318530717958647692)
#define COREX_EPSILON ((corex_real)0.00001)

#ifdef __cplusplus
extern "C" {
#endif

/* ================================================================== */
/*  Module: Types (base layer)                                        */
/* ================================================================== */

typedef struct vec2 { corex_real x, y; } vec2;
typedef struct vec3 { corex_real x, y, z; } vec3;
typedef struct vec4 { corex_real x, y, z, w; } vec4;

/* mat3 is the core 2D transform matrix. Stored row-major:
   element m[row][col]. Points are treated as column vectors (x, y, 1). */
typedef struct mat3 { corex_real m[3][3]; } mat3;

/* mat4 is future-ready for 3D. Only identity is provided for now. */
typedef struct mat4 { corex_real m[4][4]; } mat4;

typedef struct rect   { corex_real x, y, w, h; } rect;
typedef struct circle { vec2 center; corex_real radius; } circle;

typedef struct transform2d {
    vec2       position;
    corex_real rotation; /* radians */
    vec2       scale;
} transform2d;

typedef struct camera2d {
    vec2       position;
    corex_real rotation; /* radians */
    corex_real zoom;     /* > 1 zooms in, < 1 zooms out */
} camera2d;

typedef struct color { corex_real r, g, b, a; } color;

/* ================================================================== */
/*  Module: Utility math                                              */
/* ================================================================== */

COREX_DEF corex_real clamp(corex_real x, corex_real min, corex_real max);
COREX_DEF corex_real lerp(corex_real a, corex_real b, corex_real t);
COREX_DEF corex_real inverse_lerp(corex_real a, corex_real b, corex_real v);
COREX_DEF corex_real smoothstep(corex_real a, corex_real b, corex_real t);
COREX_DEF corex_real deg_to_rad(corex_real d);
COREX_DEF corex_real rad_to_deg(corex_real r);

/* ================================================================== */
/*  Module: Vector math (vec2)                                        */
/* ================================================================== */

COREX_DEF vec2 vec2_new(corex_real x, corex_real y);
COREX_DEF vec2 vec2_zero(void);
COREX_DEF vec2 vec2_one(void);

COREX_DEF vec2 vec2_add(vec2 a, vec2 b);
COREX_DEF vec2 vec2_sub(vec2 a, vec2 b);
COREX_DEF vec2 vec2_mul(vec2 v, corex_real s);
COREX_DEF vec2 vec2_div(vec2 v, corex_real s);
COREX_DEF vec2 vec2_neg(vec2 v);

COREX_DEF corex_real vec2_dot(vec2 a, vec2 b);
COREX_DEF corex_real vec2_cross(vec2 a, vec2 b); /* 2D scalar cross */

COREX_DEF corex_real vec2_len(vec2 v);
COREX_DEF corex_real vec2_len_sq(vec2 v);

COREX_DEF vec2 vec2_normalize(vec2 v);
COREX_DEF vec2 vec2_lerp(vec2 a, vec2 b, corex_real t);

COREX_DEF corex_real vec2_distance(vec2 a, vec2 b);
COREX_DEF corex_real vec2_distance_sq(vec2 a, vec2 b);
COREX_DEF vec2 vec2_perp(vec2 v);   /* 90 degree CCW rotation */
COREX_DEF vec2 vec2_rotate(vec2 v, corex_real radians);

/* ================================================================== */
/*  Module: Matrix system (mat3)                                      */
/* ================================================================== */

COREX_DEF mat3 mat3_identity(void);
COREX_DEF mat3 mat3_translation(vec2 p);
COREX_DEF mat3 mat3_rotation(corex_real radians);
COREX_DEF mat3 mat3_scale(vec2 s);
COREX_DEF mat3 mat3_mul(mat3 a, mat3 b);
COREX_DEF vec2 mat3_transform_vec2(mat3 m, vec2 v);

COREX_DEF mat4 mat4_identity(void); /* future-ready 3D */

/* ================================================================== */
/*  Module: Geometry                                                  */
/* ================================================================== */

COREX_DEF rect   rect_new(corex_real x, corex_real y, corex_real w, corex_real h);
COREX_DEF circle circle_new(vec2 center, corex_real radius);

COREX_DEF vec2 rect_center(rect r);
COREX_DEF color color_new(corex_real r, corex_real g, corex_real b, corex_real a);

/* ================================================================== */
/*  Module: Collision                                                 */
/* ================================================================== */

COREX_DEF bool point_in_rect(vec2 p, rect r);
COREX_DEF bool point_in_circle(vec2 p, circle c);

COREX_DEF bool rect_intersects(rect a, rect b);
COREX_DEF bool circle_intersects(circle a, circle b);
COREX_DEF bool rect_circle_intersects(rect r, circle c);

/* ================================================================== */
/*  Module: Transform                                                 */
/* ================================================================== */

COREX_DEF transform2d transform2d_identity(void);
COREX_DEF mat3 transform2d_matrix(transform2d t);

/* ================================================================== */
/*  Module: Camera                                                    */
/* ================================================================== */

COREX_DEF camera2d camera2d_identity(void);
COREX_DEF mat3 camera2d_view(camera2d c);
COREX_DEF vec2 camera2d_world_to_screen(camera2d c, vec2 p);
COREX_DEF vec2 camera2d_screen_to_world(camera2d c, vec2 p);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* COREX_H */

/* ================================================================== */
/* ================================================================== */
/*                          IMPLEMENTATION                            */
/* ================================================================== */
/* ================================================================== */

#ifdef COREX_IMPLEMENTATION

#ifndef COREX_IMPLEMENTATION_INCLUDED
#define COREX_IMPLEMENTATION_INCLUDED

#include <math.h>

/* Precision-aware math primitives. */
#if COREX_USE_DOUBLE
#define COREX__SQRT sqrt
#define COREX__SIN  sin
#define COREX__COS  cos
#define COREX__FABS fabs
#else
#define COREX__SQRT sqrtf
#define COREX__SIN  sinf
#define COREX__COS  cosf
#define COREX__FABS fabsf
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/*  Utility math                                                      */
/* ------------------------------------------------------------------ */

corex_real clamp(corex_real x, corex_real min, corex_real max)
{
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

corex_real lerp(corex_real a, corex_real b, corex_real t)
{
    return a + (b - a) * t;
}

corex_real inverse_lerp(corex_real a, corex_real b, corex_real v)
{
    corex_real d = b - a;
    if (COREX__FABS(d) < COREX_EPSILON) return (corex_real)0;
    return (v - a) / d;
}

corex_real smoothstep(corex_real a, corex_real b, corex_real t)
{
    /* Hermite interpolation between a and b. t is remapped into [0,1]
       using the edges and smoothed with 3t^2 - 2t^3. */
    corex_real x = clamp(inverse_lerp(a, b, t), (corex_real)0, (corex_real)1);
    return x * x * ((corex_real)3 - (corex_real)2 * x);
}

corex_real deg_to_rad(corex_real d)
{
    return d * (COREX_PI / (corex_real)180);
}

corex_real rad_to_deg(corex_real r)
{
    return r * ((corex_real)180 / COREX_PI);
}

/* ------------------------------------------------------------------ */
/*  Vector math (vec2)                                                */
/* ------------------------------------------------------------------ */

vec2 vec2_new(corex_real x, corex_real y)
{
    vec2 v; v.x = x; v.y = y; return v;
}

vec2 vec2_zero(void) { return vec2_new((corex_real)0, (corex_real)0); }
vec2 vec2_one(void)  { return vec2_new((corex_real)1, (corex_real)1); }

vec2 vec2_add(vec2 a, vec2 b) { return vec2_new(a.x + b.x, a.y + b.y); }
vec2 vec2_sub(vec2 a, vec2 b) { return vec2_new(a.x - b.x, a.y - b.y); }
vec2 vec2_mul(vec2 v, corex_real s) { return vec2_new(v.x * s, v.y * s); }
vec2 vec2_neg(vec2 v) { return vec2_new(-v.x, -v.y); }

vec2 vec2_div(vec2 v, corex_real s)
{
    /* Division by zero is a programmer error; guard in debug builds. */
    COREX_ASSERT(COREX__FABS(s) > (corex_real)0);
    return vec2_new(v.x / s, v.y / s);
}

corex_real vec2_dot(vec2 a, vec2 b)   { return a.x * b.x + a.y * b.y; }
corex_real vec2_cross(vec2 a, vec2 b) { return a.x * b.y - a.y * b.x; }

corex_real vec2_len_sq(vec2 v) { return v.x * v.x + v.y * v.y; }
corex_real vec2_len(vec2 v)    { return COREX__SQRT(vec2_len_sq(v)); }

vec2 vec2_normalize(vec2 v)
{
    corex_real len = vec2_len(v);
    if (len < COREX_EPSILON) return vec2_zero();
    return vec2_mul(v, (corex_real)1 / len);
}

vec2 vec2_lerp(vec2 a, vec2 b, corex_real t)
{
    return vec2_new(lerp(a.x, b.x, t), lerp(a.y, b.y, t));
}

corex_real vec2_distance_sq(vec2 a, vec2 b) { return vec2_len_sq(vec2_sub(a, b)); }
corex_real vec2_distance(vec2 a, vec2 b)    { return vec2_len(vec2_sub(a, b)); }

vec2 vec2_perp(vec2 v) { return vec2_new(-v.y, v.x); }

vec2 vec2_rotate(vec2 v, corex_real radians)
{
    corex_real c = COREX__COS(radians);
    corex_real s = COREX__SIN(radians);
    return vec2_new(v.x * c - v.y * s, v.x * s + v.y * c);
}

/* ------------------------------------------------------------------ */
/*  Matrix system (mat3)                                              */
/*  Row-major storage; points are columns (x, y, 1).                  */
/* ------------------------------------------------------------------ */

mat3 mat3_identity(void)
{
    mat3 r;
    r.m[0][0] = 1; r.m[0][1] = 0; r.m[0][2] = 0;
    r.m[1][0] = 0; r.m[1][1] = 1; r.m[1][2] = 0;
    r.m[2][0] = 0; r.m[2][1] = 0; r.m[2][2] = 1;
    return r;
}

mat3 mat3_translation(vec2 p)
{
    mat3 r = mat3_identity();
    r.m[0][2] = p.x;
    r.m[1][2] = p.y;
    return r;
}

mat3 mat3_rotation(corex_real radians)
{
    corex_real c = COREX__COS(radians);
    corex_real s = COREX__SIN(radians);
    mat3 r = mat3_identity();
    r.m[0][0] =  c; r.m[0][1] = -s;
    r.m[1][0] =  s; r.m[1][1] =  c;
    return r;
}

mat3 mat3_scale(vec2 s)
{
    mat3 r = mat3_identity();
    r.m[0][0] = s.x;
    r.m[1][1] = s.y;
    return r;
}

mat3 mat3_mul(mat3 a, mat3 b)
{
    mat3 r;
    int i, j;
    for (i = 0; i < 3; ++i) {
        for (j = 0; j < 3; ++j) {
            r.m[i][j] = a.m[i][0] * b.m[0][j]
                      + a.m[i][1] * b.m[1][j]
                      + a.m[i][2] * b.m[2][j];
        }
    }
    return r;
}

vec2 mat3_transform_vec2(mat3 m, vec2 v)
{
    /* Treat v as (x, y, 1). */
    return vec2_new(
        m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2],
        m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2]);
}

mat4 mat4_identity(void)
{
    mat4 r;
    int i, j;
    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j)
            r.m[i][j] = (i == j) ? (corex_real)1 : (corex_real)0;
    return r;
}

/* ------------------------------------------------------------------ */
/*  Geometry                                                          */
/* ------------------------------------------------------------------ */

rect rect_new(corex_real x, corex_real y, corex_real w, corex_real h)
{
    rect r; r.x = x; r.y = y; r.w = w; r.h = h; return r;
}

circle circle_new(vec2 center, corex_real radius)
{
    circle c; c.center = center; c.radius = radius; return c;
}

vec2 rect_center(rect r)
{
    return vec2_new(r.x + r.w * (corex_real)0.5, r.y + r.h * (corex_real)0.5);
}

color color_new(corex_real r, corex_real g, corex_real b, corex_real a)
{
    color c; c.r = r; c.g = g; c.b = b; c.a = a; return c;
}

/* ------------------------------------------------------------------ */
/*  Collision                                                         */
/* ------------------------------------------------------------------ */

bool point_in_rect(vec2 p, rect r)
{
    return p.x >= r.x && p.x <= r.x + r.w &&
           p.y >= r.y && p.y <= r.y + r.h;
}

bool point_in_circle(vec2 p, circle c)
{
    return vec2_distance_sq(p, c.center) <= c.radius * c.radius;
}

bool rect_intersects(rect a, rect b)
{
    return a.x < b.x + b.w && a.x + a.w > b.x &&
           a.y < b.y + b.h && a.y + a.h > b.y;
}

bool circle_intersects(circle a, circle b)
{
    corex_real sum = a.radius + b.radius;
    return vec2_distance_sq(a.center, b.center) <= sum * sum;
}

bool rect_circle_intersects(rect r, circle c)
{
    /* Closest point on the rect to the circle center, then distance test. */
    corex_real cx = clamp(c.center.x, r.x, r.x + r.w);
    corex_real cy = clamp(c.center.y, r.y, r.y + r.h);
    vec2 closest = vec2_new(cx, cy);
    return vec2_distance_sq(closest, c.center) <= c.radius * c.radius;
}

/* ------------------------------------------------------------------ */
/*  Transform                                                         */
/* ------------------------------------------------------------------ */

transform2d transform2d_identity(void)
{
    transform2d t;
    t.position = vec2_zero();
    t.rotation = (corex_real)0;
    t.scale    = vec2_one();
    return t;
}

mat3 transform2d_matrix(transform2d t)
{
    /* Apply order to a point: scale, then rotate, then translate.
       As matrices that is T * R * S. */
    mat3 s = mat3_scale(t.scale);
    mat3 r = mat3_rotation(t.rotation);
    mat3 tr = mat3_translation(t.position);
    return mat3_mul(tr, mat3_mul(r, s));
}

/* ------------------------------------------------------------------ */
/*  Camera                                                            */
/* ------------------------------------------------------------------ */

camera2d camera2d_identity(void)
{
    camera2d c;
    c.position = vec2_zero();
    c.rotation = (corex_real)0;
    c.zoom     = (corex_real)1;
    return c;
}

mat3 camera2d_view(camera2d c)
{
    /* The view matrix maps world space into camera space. It is the
       inverse of the camera's world placement:
           view = S(zoom) * R(-rotation) * T(-position)              */
    vec2 inv_pos = vec2_neg(c.position);
    mat3 t = mat3_translation(inv_pos);
    mat3 r = mat3_rotation(-c.rotation);
    mat3 s = mat3_scale(vec2_new(c.zoom, c.zoom));
    return mat3_mul(s, mat3_mul(r, t));
}

vec2 camera2d_world_to_screen(camera2d c, vec2 p)
{
    return mat3_transform_vec2(camera2d_view(c), p);
}

vec2 camera2d_screen_to_world(camera2d c, vec2 p)
{
    /* Inverse of world_to_screen, built directly to stay exact:
           world = T(position) * R(rotation) * S(1/zoom)             */
    corex_real inv_zoom = (COREX__FABS(c.zoom) > COREX_EPSILON)
                        ? (corex_real)1 / c.zoom : (corex_real)0;
    vec2 q = vec2_mul(p, inv_zoom);
    q = vec2_rotate(q, c.rotation);
    q = vec2_add(q, c.position);
    return q;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* COREX_IMPLEMENTATION_INCLUDED */
#endif /* COREX_IMPLEMENTATION */
