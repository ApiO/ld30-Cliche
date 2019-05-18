#pragma once

#include <glm/glm.hpp>
#include <runtime/types.h>

namespace game
{
  using namespace pge;
  struct Part
  {
    u64 id;
    glm::vec3 euler;
  };

  class Portal
  {
  public:
    void load(u64 world, glm::vec3 &position);
    void unload(void);
    void update(f64 delta_time);
  private:
    u64       world;
    Part      parts[3];
    u64       wall;
  };
}