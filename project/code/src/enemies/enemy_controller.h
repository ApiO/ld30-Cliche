#pragma once

#include <runtime/types.h>

namespace game
{
  using namespace pge;
  
  class EnemyController
  {
  public:
    EnemyController();
    void init(u64 world, Allocator &a);
    void shutdown(void);
    void update(f64 delta_time);
    void hit_enemy(u64 actor);
  private:
    struct Enemy
    {
      glm::vec3 position;
      i32 type;
      i32 hp;
    };
    u64 world;
    f64 spawn_accumulator;
    Hash<Enemy> *enemies;
  };

  inline EnemyController::EnemyController() : enemies(NULL){}

  extern EnemyController enemy_controller;
}