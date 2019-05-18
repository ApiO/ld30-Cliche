#include "player.h"

#include <time.h>
#include <engine/pge.h>
#include <runtime/array.h>
#include <enemies/enemy_controller.h>

// internals
namespace game
{
  const f32 SPEED = 512.f;
  const f32 ASCENT_HEIGHT = 256.0f;
  const f32 ASCENT_DURATION = 0.2f;
  const f32 FALL_ACCELERATION_DURATION = 0.2f;
  const f32 FALL_SPEED = 2048.0f;
  const f32 ATTACK_CD = .07f;
  const f32 BULLET_SPEED = 20;
  const glm::vec3 STAFF_OFFSET(130, 140, 0);
  const f32 SHOOT_ANGLE_VARIANCE = .1f;

  static void bullet_hit(const Array<ContactPoint> &contacts)
  {
    for (u32 i = 0; i < array::size(contacts); i++)
      enemy_controller.hit_enemy(contacts[i].actor);
  }
}

// definitions
namespace game
{
  void Player::load(u64 w, glm::vec3 &p)
  {
    world = w;

    ascending = false;
    fly_time = 0;
    attack_accumulator = 0.f;

    id = world::spawn_unit(w, "units/player/player", p, IDENTITY_ROTATION);
    mover = unit::mover(w, id, 0);
  }


  void Player::unload(void)
  {
    world::despawn_unit(world, id);
    mover = id = (u64)-1;
  }

  void Player::update(f64 dt)
  {
    f32 vx, vy;
    glm::vec3 p;
    vx = 0;
    vy = 0;

    mover::get_position(world, mover, p);

    attack_accumulator += dt;

    if (keyboard::button(KEYBOARD_KEY_SPACE) && attack_accumulator >= ATTACK_CD){
      attack_accumulator = 0.f;

      // spawns bullet
      u64 bullet = world::spawn_unit(world, randomize(0, 99) < 94 ? "units/bullet/bullet" : "units/shoe/shoe", p + STAFF_OFFSET, IDENTITY_ROTATION);

      // setups physics
      u64 bullet_actor = unit::actor(world, bullet, 0);

      glm::vec3 v(1, rand_variance(SHOOT_ANGLE_VARIANCE), 0);
      v *= BULLET_SPEED;
      actor::set_velocity(world, bullet_actor, v);

      //actor::set_touched_callback(world, bullet_actor, bullet_hit);
    }

    if (mover::collides_down(world, mover)) {
      if (keyboard::button(KEYBOARD_KEY_UP) && !keyboard::button(KEYBOARD_KEY_DOWN)) {
        ascending = true;
        fly_from = p.y;
        fly_time = 0;
      }
    }

    if (ascending) {
      // ascending
      fly_time += (f32)dt;
      vy = f32((ease_out_sine((f32)fly_time, fly_from, ASCENT_HEIGHT, ASCENT_DURATION) - p.y) / dt);
      if (fly_time >= ASCENT_DURATION) {
        ascending = false;
        fly_time = 0;
        fly_from = p.y + vy;
      }
    }
    else {
      if (mover::collides_down(world, mover)) {
        // grounded
        if (keyboard::button(KEYBOARD_KEY_UP) && !keyboard::button(KEYBOARD_KEY_DOWN)) {
          ascending = true;
          fly_from = p.y;
          fly_time = 0;
        }
      }
      else {
        // falling
        fly_time += (f32)dt;
        vy = fly_time < FALL_ACCELERATION_DURATION
          ? ease_in_sine((f32)fly_time, 0, FALL_SPEED, FALL_ACCELERATION_DURATION)
          : FALL_SPEED;
        vy *= -1;
      }
    }

    if (keyboard::button(KEYBOARD_KEY_RIGHT) && !keyboard::button(KEYBOARD_KEY_LEFT))
      vx = SPEED;

    if (keyboard::button(KEYBOARD_KEY_LEFT) && !keyboard::button(KEYBOARD_KEY_RIGHT))
      vx = -SPEED;

    mover::move(world, mover, glm::vec3(vx, vy, 0) * (f32)dt);
  }


  u64 Player::get_id(void)
  {
    return id;
  }

  u64 Player::get_mover(void)
  {
    return mover;
  }

  void Player::get_position(glm::vec3 &p)
  {
    unit::get_world_position(world, id, 0, p);
  }
}