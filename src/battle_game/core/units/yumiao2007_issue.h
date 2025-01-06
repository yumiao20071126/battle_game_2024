#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class yumiao2007_issue : public Unit {
 public:
  yumiao2007_issue(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void TankMove(float move_speed, float rotate_angular_speed);
  void TurretRotate();
  void Fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  uint32_t move_count_down_{0};
  uint32_t mine_count_down_{0};
  uint32_t fire_count{0};
  int status{0};
  float energy_{0.0f};
  glm::vec4 backround_engbar_color_{};
  glm::vec4 charging_front_engbar_color_{};
  glm::vec4 charged_front_engbar_color_{};
  glm::vec4 boosted_engbar_color_{};
  float fadeout_health_{1.0f};
};
}  // namespace battle_game::unit