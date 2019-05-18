#pragma once

#include <engine/pge_types.h>

#define PI   3.14159265359f
#define PIX2 6.28318530718f
#define FRAME_DURATION .0166666666666667f

namespace game 
{
  using namespace pge;

  enum Command
  {
    CMD_JUMP,
    CMD_ATTACK,
    CMD_DASH,
    NUM_COMMANDS
  };

  enum State
  {
    STATE_GROUNDED = 0,
    STATE_ASCENDING,
    STATE_FALLING,
    STATE_SLIDING,
    STATE_FLYING,
    STATE_DASHING,
    STATE_ATP,
    STATE_ATTACK,
    STATE_STUNNED
  };

  enum DTapOrientation
  {
    DTAP_NONE,
    DTAP_UP,
    DTAP_DOWN,
    DTAP_LEFT,
    DTAP_RIGHT
  };

  inline int randomize(int min, int max)
  {
    return (rand() % (max - min + 1)) + min;
  }

  inline f32 rand_variance(f32 max)
  {
    i32 sign = rand() % 2 ? 1 : -1;
    return sign * static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / max));
  }

  inline f32 ease_out_sine(f32 t, f32 b, f32 c, f32 d) {
    return c * sin(t / d * (PI / 2)) + b;
  };

  inline f32 ease_in_sine(f32 t, f32 b, f32 c, f32 d) {
    return -c / 2 * (cos(PI*t / d) - 1) + b;
  };
}