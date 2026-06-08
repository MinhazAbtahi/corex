/*
    collision_demo.c - exercising the corex.h collision system
    ----------------------------------------------------------
    Demonstrates: point/rect, point/circle, rect/rect, circle/circle,
    and rect/circle intersection tests, with expected results asserted.

    Build:
        gcc -std=c99 -I.. collision_demo.c -o collision_demo -lm
        cl /I.. collision_demo.c
*/

#include <stdio.h>

#define COREX_IMPLEMENTATION
#include "../corex.h"

static int g_pass = 0;
static int g_fail = 0;

static void check(const char *name, bool got, bool expected)
{
    bool ok = (got == expected);
    if (ok) g_pass++; else g_fail++;
    printf("[%s] %-32s got=%d expected=%d\n",
           ok ? "PASS" : "FAIL", name, (int)got, (int)expected);
}

int main(void)
{
    printf("corex collision demo\n\n");

    rect   r  = rect_new(0.0f, 0.0f, 10.0f, 10.0f);
    circle c1 = circle_new(vec2_new(5.0f, 5.0f), 3.0f);
    circle c2 = circle_new(vec2_new(20.0f, 5.0f), 3.0f);
    circle c3 = circle_new(vec2_new(12.0f, 5.0f), 3.0f); /* reaches into r's edge */
    circle c4 = circle_new(vec2_new(9.0f, 5.0f), 3.0f);  /* overlaps c1 (dist 4 < 6) */

    /* point in rect */
    check("point_in_rect inside",  point_in_rect(vec2_new(5.0f, 5.0f), r),   true);
    check("point_in_rect on edge", point_in_rect(vec2_new(0.0f, 0.0f), r),   true);
    check("point_in_rect outside", point_in_rect(vec2_new(-1.0f, 5.0f), r),  false);

    /* point in circle */
    check("point_in_circle center", point_in_circle(vec2_new(5.0f, 5.0f), c1), true);
    check("point_in_circle inside", point_in_circle(vec2_new(6.0f, 6.0f), c1), true);
    check("point_in_circle outside",point_in_circle(vec2_new(9.0f, 9.0f), c1), false);

    /* rect vs rect */
    check("rect_intersects overlap",
          rect_intersects(r, rect_new(5.0f, 5.0f, 10.0f, 10.0f)), true);
    check("rect_intersects disjoint",
          rect_intersects(r, rect_new(20.0f, 20.0f, 5.0f, 5.0f)), false);

    /* circle vs circle */
    check("circle_intersects far",   circle_intersects(c1, c2), false);
    check("circle_intersects close", circle_intersects(c1, c4), true);

    /* rect vs circle */
    check("rect_circle inside",  rect_circle_intersects(r, c1), true);
    check("rect_circle edge",    rect_circle_intersects(r, c3), true);
    check("rect_circle outside", rect_circle_intersects(r, c2), false);

    printf("\nresults: %d passed, %d failed\n", g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}
