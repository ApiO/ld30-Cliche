#pragma once

#include <engine/pge.h>
#include <runtime/memory.h>
#include <runtime/hash.h>
#include <game_types.h>
#include "enemy_controller.h"

// internals
namespace game
{
  struct EnemyParam
  {
    EnemyParam(char *n, glm::vec3 p, f32 s, i32 _hp);
    const char *name;
    glm::vec3   position;
    f32         speed;
    i32         hp;
  };

  inline EnemyParam::EnemyParam(char *n, glm::vec3 p, f32 s, i32 _hp) : name(n), position(p), speed(s), hp(_hp){}

  const EnemyParam ENEMIES[] = {
    EnemyParam("units/monsters/monster_1", glm::vec3(1200, -140, 0), 600, 3),
    EnemyParam("units/monsters/monster_2", glm::vec3(1200, -405, 0), 400, 4),
    EnemyParam("units/monsters/monster_3", glm::vec3(1200, -370, 0), 800, 2)
  };

  const f32 SPAWN_CD = .6f;
  const f32 SPAWN_CD_VARIANCE = .1f;
  const i32 SPAWN_NUMBER = 4;


  static void hit_igloo(const Array<ContactPoint> &contacts)
  {
    for (u32 i = 0; i < array::size(contacts); i++)
      enemy_controller.hit_enemy(contacts[i].actor);
  }
}


// definitions

namespace game
{
  EnemyController enemy_controller;

  void EnemyController::init(u64 w, Allocator &a)
  {
    world = w;
    enemies = MAKE_NEW(a, Hash<Enemy>, a);
    spawn_accumulator = 0;

    /*
    // gets enemy params
    i32 type = randomize(0, 2);
    const EnemyParam &param = ENEMIES[type];
    Enemy e;
    e.position = param.position;
    e.type = type;

    // spawns
    u64 id = world::spawn_unit(world, param.name, e.position, IDENTITY_ROTATION);

    // register instance
    hash::set(*enemies, id, e);
    */
  }

  void EnemyController::shutdown(void)
  {
    Hash<Enemy>::Entry *entry, *end = hash::end(*enemies);
    for (entry = hash::begin(*enemies); entry < end; entry++)
      world::despawn_unit(world, entry->key);

    MAKE_DELETE((*enemies->_data._allocator), Hash<Enemy>, enemies);
  }

  void EnemyController::update(f64 dt)
  {
    // handles spawn
    spawn_accumulator += dt;

    //*
    f32 spawn_variance = rand_variance(SPAWN_CD_VARIANCE);
    if (spawn_accumulator + spawn_variance >= SPAWN_CD)
    {
      i32 num_spawn = randomize(0, SPAWN_NUMBER);
      for (i32 i = 0; i < num_spawn; i++)
      {
        // gets enemy params
        i32 type = randomize(0, 2);
        const EnemyParam &param = ENEMIES[type];
        Enemy e;
        e.position = param.position;
        e.type = type;

        // spawns
        u64 id = world::spawn_unit(world, param.name, e.position, IDENTITY_ROTATION);
        //actor::set_touched_callback(world, unit::actor(world, id, 0), hit_igloo);

        // register instance
        hash::set(*enemies, id, e);
      }
      spawn_accumulator = 0.f;
    }
    //*/

    // update enemies positions
    Hash<Enemy>::Entry *entry, *end = hash::end(*enemies);
    for (entry = hash::begin(*enemies); entry < end; entry++){
      Enemy &enemy = entry->value;
      const EnemyParam &param = ENEMIES[enemy.type];
      enemy.position.x -= (f32)(param.speed * dt);
      unit::set_local_position(world, entry->key, 0, enemy.position);
    }
  }

  void EnemyController::hit_enemy(u64 actor)
  {
    u32 unit = actor::unit(world, actor);

    //ASSERT(hash::has(*enemies, unit));

    Enemy *enemy = hash::get(*enemies, unit);
    enemy->hp -= 1;

    if (enemy->hp) return;

    world::despawn_unit(world, unit);
    hash::remove(*enemies, unit);
  }

}