/*
    pong.c - a headless Pong simulation built on corex.h
    -----------------------------------------------------
    Demonstrates: vec2 math, rect/circle geometry, collision, and a
    simple deterministic update loop. There is no rendering dependency;
    the game state is printed to the console each frame so you can see
    the math driving real gameplay.

    Build (any C99 compiler), e.g.:
        gcc -std=c99 -I.. pong.c -o pong -lm
        cl /I.. pong.c
*/

#include <stdio.h>

#define COREX_IMPLEMENTATION
#include "../corex.h"

#define FIELD_W  80.0f
#define FIELD_H  24.0f
#define PADDLE_W  1.5f
#define PADDLE_H  5.0f
#define BALL_R    0.6f

typedef struct {
    rect left_paddle;
    rect right_paddle;
    circle ball;
    vec2 ball_vel;
    int score_left;
    int score_right;
} pong_state;

static void pong_reset_ball(pong_state *g, float dir)
{
    g->ball = circle_new(vec2_new(FIELD_W * 0.5f, FIELD_H * 0.5f), BALL_R);
    g->ball_vel = vec2_new(dir * 28.0f, 12.0f);
}

static void pong_init(pong_state *g)
{
    g->left_paddle  = rect_new(2.0f, FIELD_H * 0.5f - PADDLE_H * 0.5f, PADDLE_W, PADDLE_H);
    g->right_paddle = rect_new(FIELD_W - 2.0f - PADDLE_W,
                               FIELD_H * 0.5f - PADDLE_H * 0.5f, PADDLE_W, PADDLE_H);
    g->score_left = 0;
    g->score_right = 0;
    pong_reset_ball(g, 1.0f);
}

/* Trivial AI: move a paddle toward the ball's y, clamped to the field. */
static void pong_track(rect *paddle, float target_y, float dt)
{
    float center = paddle->y + paddle->h * 0.5f;
    float speed = 20.0f;
    float step = clamp(target_y - center, -speed * dt, speed * dt);
    paddle->y = clamp(paddle->y + step, 0.0f, FIELD_H - paddle->h);
}

static void pong_update(pong_state *g, float dt)
{
    /* Integrate the ball. */
    g->ball.center = vec2_add(g->ball.center, vec2_mul(g->ball_vel, dt));

    /* Bounce off top and bottom walls. */
    if (g->ball.center.y - g->ball.radius < 0.0f) {
        g->ball.center.y = g->ball.radius;
        g->ball_vel.y = -g->ball_vel.y;
    } else if (g->ball.center.y + g->ball.radius > FIELD_H) {
        g->ball.center.y = FIELD_H - g->ball.radius;
        g->ball_vel.y = -g->ball_vel.y;
    }

    /* Paddle AI follows the ball. */
    pong_track(&g->left_paddle,  g->ball.center.y, dt);
    pong_track(&g->right_paddle, g->ball.center.y, dt);

    /* Paddle collisions reflect horizontal velocity. */
    if (g->ball_vel.x < 0.0f && rect_circle_intersects(g->left_paddle, g->ball)) {
        g->ball.center.x = g->left_paddle.x + g->left_paddle.w + g->ball.radius;
        g->ball_vel.x = -g->ball_vel.x;
    }
    if (g->ball_vel.x > 0.0f && rect_circle_intersects(g->right_paddle, g->ball)) {
        g->ball.center.x = g->right_paddle.x - g->ball.radius;
        g->ball_vel.x = -g->ball_vel.x;
    }

    /* Scoring. */
    if (g->ball.center.x < 0.0f) {
        g->score_right++;
        pong_reset_ball(g, 1.0f);
    } else if (g->ball.center.x > FIELD_W) {
        g->score_left++;
        pong_reset_ball(g, -1.0f);
    }
}

int main(void)
{
    pong_state g;
    int frame;
    const float dt = 1.0f / 30.0f; /* fixed timestep, deterministic */

    pong_init(&g);

    printf("corex pong (headless) - simulating 150 frames\n");
    printf("field: %gx%g, ball radius: %g\n\n", FIELD_W, FIELD_H, BALL_R);

    for (frame = 0; frame < 150; ++frame) {
        pong_update(&g, dt);
        if (frame % 15 == 0) {
            printf("f%3d  ball=(%6.2f, %6.2f)  L.y=%5.2f  R.y=%5.2f  score %d:%d\n",
                   frame, g.ball.center.x, g.ball.center.y,
                   g.left_paddle.y, g.right_paddle.y,
                   g.score_left, g.score_right);
        }
    }

    printf("\nfinal score  %d : %d\n", g.score_left, g.score_right);
    return 0;
}
