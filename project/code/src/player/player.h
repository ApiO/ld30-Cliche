#pragma once

#include <glm/glm.hpp>
#include <game_types.h>

namespace game
{
  using namespace pge;

  class Player
  {
  public:
    void load(u64 world, glm::vec3 &position);
    void unload(void);

    void update(f64 delta_time);

    u64  get_id(void);
    u64  get_mover(void);
    void get_position(glm::vec3 &position);

  private:
    u64  world;
    u64  id;
    u64  mover;
    bool ascending;
    f32  fly_time;
    f32  fly_from;
    f64  attack_accumulator;
  };
}