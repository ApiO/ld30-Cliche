#include <engine/pge.h>
#include <runtime/array.h>
#include <game_types.h>
#include "portal.h"

namespace game
{

  const f32 ROTATION_SPEED = PI / 30.f;

  const f32 PART_1_SPEED = ROTATION_SPEED * 20.f;
  const f32 PART_2_SPEED = ROTATION_SPEED * -4.f;
  const f32 PART_3_SPEED = ROTATION_SPEED * -20.f;

  const glm::vec3 PORTAL_ORIENTATION(PI*.25f, 0.f, 0.f);

  static u64 temp_world;


  static void recieve_bullet(const Array<ContactPoint> &contacts, const void * user_data) //!\ todo update cb params into API : add world ID
  {
    for (u32 i = 0; i < array::size(contacts); i++) 
      world::despawn_unit(temp_world, actor::unit(temp_world, contacts[i].actor));
  }

  void Portal::load(u64 w, glm::vec3 &position)
  {
    temp_world = world = w;

    parts[2].id = world::spawn_sprite(w, "sprites/portal/portal_3", position, IDENTITY_ROTATION);
    parts[1].id = world::spawn_sprite(w, "sprites/portal/portal_2", position, IDENTITY_ROTATION);
    parts[0].id = world::spawn_sprite(w, "sprites/portal/portal_1", position, IDENTITY_ROTATION);

    parts[0].euler = parts[1].euler = parts[2].euler = glm::vec3(0);

    wall = world::spawn_unit(world, "units/ground/border", glm::vec3(1700, 0, 0), IDENTITY_ROTATION, glm::vec3(100, 1000, 1));
    actor::set_touched_callback(world, unit::actor(world, wall, 0), recieve_bullet, NULL);
  }

  void Portal::unload(void)
  {
    world::despawn_sprite(world, parts[0].id);
    world::despawn_sprite(world, parts[1].id);
    world::despawn_sprite(world, parts[2].id);
  }

  inline void update_part(u64 world, Part &part, f32 v)
  {
    part.euler.z += v;
    glm::quat q(PORTAL_ORIENTATION + part.euler);
    sprite::set_local_rotation(world, part.id, q);
  }

  void Portal::update(f64 dt)
  {
    update_part(world, parts[0], (f32)(PART_1_SPEED * dt));
    update_part(world, parts[1], (f32)(PART_2_SPEED * dt));
    update_part(world, parts[2], (f32)(PART_3_SPEED * dt));
  }

}