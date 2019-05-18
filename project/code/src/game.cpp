#include <engine/pge.h>
#include <engine/pge_types.h>

#include <stdio.h>
#include <runtime/memory.h>
#include <runtime/hash.h>
#include <runtime/temp_allocator.h>
#include <camera.h>
#include <time.h>

#include "behaviors/behaviors.h"
#include "player/player.h"
#include "game_camera.h"
#include "game_types.h"
#include "environement/portal.h"
#include "enemies/enemy_controller.h"

namespace
{
  using namespace pge;
  using namespace game;
  

  struct Game
  {
    u64         world;
    GameCamera  camera;
    u64         bg_world;
    Camera      bg_camera;
    u64         hud_world;
    Camera      hud_camera;
    u64         viewport;
    u64         level;
    Player      player;
    u64         sun;
    i32         screen_width;
    i32         screen_height;
    u64         flake_effect;
    u64         package;
    //i32         pad_index;
    Portal      portal;
    Array<u64> *fx;
    bool show_debug;
    bool show_aabb;
  };

  char _str_buf[256];

  const char *FONT_NAME = "fonts/consolas.24/consolas.24";
  const u32 PAD_INDEX = 0;
  const f32 CAMERA_DAMPING = 4.0f;
  const glm::vec2 virtual_resolution(1920, 1080);


  void raycast_to_ground_cb(const Array<ContactPoint> &hits) {
    ground_found = array::size(hits) > 0u;

    if (!ground_found) return;

    ground_dir = glm::normalize(glm::vec2(hits[0].normal));
    f32 tmp = ground_dir.x;
    ground_dir.x = ground_dir.y;
    ground_dir.y = tmp * -1;
    ground_dist = hits[0].distance;
  }
}

namespace game
{
  using namespace pge;

  // EXTERNALS -------------
  bool      ground_found;
  u64       raycast_to_ground;
  glm::vec2 ground_dir;
  f32       ground_dist;
  //-----------------------

  Game game;

  void init()
  {
    Allocator &a = memory_globals::default_allocator();
    window::get_resolution(game.screen_width, game.screen_height);

    game.world = application::create_world();
    game.hud_world = application::create_world();
    game.bg_world = application::create_world();

    game.fx = MAKE_NEW(a, Array<u64>, a);

    {
      f32 target_aspect = virtual_resolution.x / virtual_resolution.y;
      i32 width = game.screen_width;
      i32 height = (i32)(width / target_aspect + 0.5f);

      if (height > game.screen_height) {
        height = game.screen_height;
        width = (int)(height * target_aspect + 0.5f);
      }
      game.viewport = application::create_viewport((game.screen_width / 2) - (width / 2), (game.screen_height / 2) - (height / 2), width, height);
    }

    game.package = application::resource_package("packages/default");
    resource_package::load(game.package);
    resource_package::flush(game.package);

    game_camera::init(game.camera, game.world, virtual_resolution, a);

    game.hud_camera.init(game.hud_world, CAMERA_MODE_OTRHO, (f32)virtual_resolution.x, (f32)virtual_resolution.y);
    game.hud_camera.set_orthographic_projection(f32(-virtual_resolution.x*.5f), f32(virtual_resolution.x*.5f), f32(-virtual_resolution.y*.5f), f32(virtual_resolution.y*.5f));
    game.hud_camera.set_near_range(-1.0f);
    game.hud_camera.set_far_range(1.0f);

    game.bg_camera.init(game.bg_world, CAMERA_MODE_OTRHO, (f32)virtual_resolution.x, (f32)virtual_resolution.y);
    game.bg_camera.set_orthographic_projection(f32(-virtual_resolution.x*.5f), f32(virtual_resolution.x*.5f), f32(-virtual_resolution.y*.5f), f32(virtual_resolution.y*.5f));
    game.bg_camera.set_near_range(-1.0f);
    game.bg_camera.set_far_range(1.0f);


    /*
    // finds first pad slot
    for (u32 i = 0; i < MAX_NUM_PADS; i++)
      if (pad::active(i) && pad::num_buttons(i) < 15){
      game.pad_index = i;
      break;
      }
    */

    game.level = world::load_level(game.world, "levels/default", IDENTITY_TRANSLATION, IDENTITY_ROTATION);

    glm::vec3 position;

    position = glm::vec3(-game.screen_width * .5f, game.screen_height * .5f, 0.f);
    game.sun = world::spawn_sprite(game.bg_world, "sprites/sun", position, IDENTITY_ROTATION);
    
    position = glm::vec3(1200, -250, 0);
    game.portal.load(game.world, position);
    
    position = glm::vec3(-1200, 0, 0);
    game.player.load(game.world, position);

    enemy_controller.init(game.world, a);

    position = glm::vec3(-400, 1000, 0);
    game.flake_effect = world::spawn_particles(game.world, "particles/flake_blur", position, IDENTITY_ROTATION);

    raycast_to_ground = physics::create_raycast(game.world, raycast_to_ground_cb, "ground_cast", true, false);

    physics::show_debug(game.show_debug);
    srand((u32)time(NULL));
  }

  void update(f64 delta_time)
  {
    if (pad::active(0) && pad::pressed(0, PAD_KEY_7)){
      application::quit();
      return;
    }

    if (keyboard::any_pressed()) {
      if (keyboard::button(KEYBOARD_KEY_ESCAPE)){
        application::quit();
        return;
      }

      if (keyboard::pressed(KEYBOARD_KEY_P)) {
        game.show_debug = !game.show_debug;
        physics::show_debug(game.show_debug);
      }
      if (keyboard::pressed(KEYBOARD_KEY_B)) {
        game.show_aabb = !game.show_aabb;
        application::show_culling_debug(game.show_aabb);
      }
    }

    world::update(game.world, delta_time);

    // sync camera
    glm::vec3 p;
    game.player.get_position(p);
    p.y += 400.0f;

    game_camera::set_target(game.camera, (glm::vec2)p);
    game_camera::update(game.camera, delta_time);

    game.hud_camera.update();
    game.bg_camera.update();

    for (u32 i = 0; i < array::size(*game.fx); i++) {
      while (i < array::size(*game.fx) && !unit::is_playing_animation(game.world, (*game.fx)[i])) {
        world::despawn_unit(game.world, (*game.fx)[i]);
        (*game.fx)[i] = array::pop_back(*game.fx);
      }
    }

    game.player.update(delta_time);
    game.portal.update(delta_time);

    enemy_controller.update(delta_time);

    world::update(game.bg_world, delta_time);
    world::update(game.world, 0.0f);
    world::update(game.hud_world, delta_time);
  }

  void render()
  {
    application::render_world(game.bg_world, game.bg_camera.get_id(), game.viewport);
    application::render_world(game.world, game.camera.id, game.viewport);
    application::render_world(game.hud_world, game.hud_camera.get_id(), game.viewport);
  }

  void shutdown()
  {
    for (u32 i = 0; i < array::size(*game.fx); i++)
      world::despawn_unit(game.world, (*game.fx)[i]);

    MAKE_DELETE(memory_globals::default_allocator(), Array<u64>, game.fx);

    game.player.unload();

    enemy_controller.shutdown();

    physics::destroy_raycast(game.world, raycast_to_ground);

    //world::despawn_particles(game.hud_world, game.flake_effect);

    world::unload_level(game.world, game.level);

    application::destroy_viewport(game.viewport);

    game_camera::destroy(game.camera);

    application::destroy_world(game.world);
    application::destroy_world(game.hud_world);
    application::destroy_world(game.bg_world);

    resource_package::unload(game.package);
    application::release_resource_package(game.package);
  }
}