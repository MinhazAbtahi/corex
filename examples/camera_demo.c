/*
    camera_demo.c - world <-> screen transforms with corex.h
    --------------------------------------------------------
    Demonstrates: camera2d, mat3 view matrices, and the round-trip
    guarantee that screen_to_world(world_to_screen(p)) == p.

    Build:
        gcc -std=c99 -I.. camera_demo.c -o camera_demo -lm
        cl /I.. camera_demo.c
*/

#include <stdio.h>

#define COREX_IMPLEMENTATION
#include "../corex.h"

static void show(const char *label, vec2 v)
{
    printf("%-22s (%8.3f, %8.3f)\n", label, v.x, v.y);
}

int main(void)
{
    camera2d cam = camera2d_identity();
    cam.position = vec2_new(100.0f, 50.0f);
    cam.rotation = deg_to_rad(30.0f);
    cam.zoom     = 2.0f;

    vec2 world_points[3];
    int i;
    world_points[0] = vec2_new(100.0f, 50.0f); /* exactly on the camera */
    world_points[1] = vec2_new(120.0f, 50.0f);
    world_points[2] = vec2_new(100.0f, 90.0f);

    printf("corex camera demo\n");
    printf("camera: pos=(%g,%g) rot=%g deg zoom=%g\n\n",
           cam.position.x, cam.position.y, rad_to_deg(cam.rotation), cam.zoom);

    for (i = 0; i < 3; ++i) {
        vec2 w = world_points[i];
        vec2 s = camera2d_world_to_screen(cam, w);
        vec2 back = camera2d_screen_to_world(cam, s);
        float err = vec2_distance(w, back);

        printf("point %d\n", i);
        show("  world", w);
        show("  -> screen", s);
        show("  -> back to world", back);
        printf("  round-trip error: %g\n\n", err);
    }

    /* The view matrix is also available directly for batching many points. */
    mat3 view = camera2d_view(cam);
    printf("view matrix (row-major):\n");
    printf("  | %7.3f %7.3f %7.3f |\n", view.m[0][0], view.m[0][1], view.m[0][2]);
    printf("  | %7.3f %7.3f %7.3f |\n", view.m[1][0], view.m[1][1], view.m[1][2]);
    printf("  | %7.3f %7.3f %7.3f |\n", view.m[2][0], view.m[2][1], view.m[2][2]);

    return 0;
}
