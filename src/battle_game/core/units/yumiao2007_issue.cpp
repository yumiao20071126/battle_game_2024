#include "yumiao2007_issue.h"
#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
namespace battle_game::unit {

int energy=10;

namespace {
uint32_t tank_body_model_index = 0xffffffffu;
uint32_t tank_turret_model_index = 0xffffffffu;
uint32_t life_bar_model_index = 0xffffffffu;
}  // namespace

const int CHARGE=0, READY=1, BOOST=2;

yumiao2007_issue::yumiao2007_issue(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Unit(game_core, id, player_id) {
  if (!~tank_body_model_index) {
    auto mgr = AssetsManager::GetInstance();
    {
      /* Tank Body */
      tank_body_model_index = mgr->RegisterModel(
          {
        // {{-0.8f, 0.8f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},  // 蓝色
        //   {{-0.8f, -1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}, // 蓝色
        //   {{0.8f, 0.8f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},   // 蓝色
        //   {{0.8f, -1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},  // 蓝色
        //   // distinguish front and back
        //   {{0.6f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},   // 蓝色
        //   {{-0.6f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},  // 蓝色
        //   },
          {{-0.6f, 0.8f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},  // 蓝色
          {{-0.8f, -1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}, // 蓝色
          {{0.6f, 0.8f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},   // 蓝色
          {{0.8f, -1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},  // 蓝色
          // distinguish front and back
          {{0.6f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},   // 蓝色
          {{-0.6f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},  // 蓝色
          },
          {0, 1, 2, 1, 2, 3, 0, 2, 5, 2, 4, 5});
    }

    {
      /* Tank Turret */
      backround_engbar_color_={1.0f, 0.0f, 0.0f, 0.9f};
      charging_front_engbar_color_={0.0f, 0.0f, 1.0f, 0.9f};
      charged_front_engbar_color_={1.0f, 1.0f, 0.0f, 0.9f};
      boosted_engbar_color_={1.0f, 0.5f, 0.0f, 0.9f};
      std::vector<ObjectVertex> turret_vertices;
      std::vector<uint32_t> turret_indices;
      const int precision = 60;
      const float inv_precision = 1.0f / float(precision);
      for (int i = 0; i < precision; i++) {
        auto theta = (float(i) + 0.5f) * inv_precision;
        theta *= glm::pi<float>() * 2.0f;
        auto sin_theta = std::sin(theta);
        auto cos_theta = std::cos(theta);
        turret_vertices.push_back({{sin_theta * 0.5f, cos_theta * 0.5f},
                                   {0.0f, 0.0f},
                                   {0.7f, 0.7f, 0.7f, 1.0f}});
        turret_indices.push_back(i);
        turret_indices.push_back((i + 1) % precision);
        turret_indices.push_back(precision);
      }
      turret_vertices.push_back(
          {{0.0f, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{-0.1f, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{0.1f, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{-0.1f, 1.2f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{0.1f, 1.2f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_indices.push_back(precision + 1 + 0);
      turret_indices.push_back(precision + 1 + 1);
      turret_indices.push_back(precision + 1 + 2);
      turret_indices.push_back(precision + 1 + 1);
      turret_indices.push_back(precision + 1 + 2);
      turret_indices.push_back(precision + 1 + 3);
      tank_turret_model_index =
          mgr->RegisterModel(turret_vertices, turret_indices);
    }
    
  }
   if (!~life_bar_model_index)
  {
    auto mgr = AssetsManager::GetInstance();
    {
      life_bar_model_index = mgr->RegisterModel(
          {{{-0.5f, 0.08f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{-0.5f, -0.08f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{0.5f, 0.08f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{0.5f, -0.08f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}},
          {0, 1, 2, 1, 2, 3});
    }
  }
}

void yumiao2007_issue::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture(0);
  battle_game::SetColor(glm::vec4{1.5f, 2.0f, 1.0f, 0.5f});
  battle_game::DrawModel(tank_body_model_index);
  battle_game::SetRotation(turret_rotation_);
  battle_game::DrawModel(tank_turret_model_index);

}

void yumiao2007_issue::RenderHelper() {
  auto parent_unit = game_core_->GetUnit(id_);
  auto pos = parent_unit->GetPosition() + lifebar_offset_;
  pos += glm::vec2{0.0f, 0.2f};

  float energy_percentage = static_cast<float>(energy) / 10.0f;
  glm::vec2 shift_eng = {(float)lifebar_length_ * (1 - energy_percentage) / 2, 0.0f};

  SetTransformation(pos, 0.0f, {lifebar_length_, 1.0f});
  SetColor(backround_engbar_color_);
  SetTexture(0);
  DrawModel(life_bar_model_index);

  SetTransformation(pos - shift_eng, 0.0f, {lifebar_length_ * energy_percentage, 1.0f});
  SetColor(glm::vec4{0.5f, 0.0f, 0.5f, 1.0f});  // 绿色
  DrawModel(life_bar_model_index);
}

void yumiao2007_issue::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}

void yumiao2007_issue::TankMove(float move_speed, float rotate_angular_speed) {
if (move_count_down_==0){
    auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    glm::vec2 offset{0.0f};
    if (input_data.key_down[GLFW_KEY_W]) {
      offset.y += 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_S]) {
      offset.y -= 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_O]) {
      offset.x -= 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_P]) {
      offset.x += 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_E]) {
      // 按下 E 键时，坦克向炮管方向后退
      glm::vec2 backward = Rotate(glm::vec2{0.0f, 15.0f}, turret_rotation_+ glm::radians(90.0f));
      offset += backward;
    move_count_down_ = kTickPerSecond;
    }
    if (input_data.key_down[GLFW_KEY_F]) {
      // 按下 E 键时，坦克向炮管方向后退
      glm::vec2 backward = Rotate(glm::vec2{0.0f, 15.0f}, turret_rotation_+ glm::radians(90.0f));
      offset += backward;
    move_count_down_ = kTickPerSecond; 
    }
    if (input_data.key_down[GLFW_KEY_G]) {
      // 按下 E 键时，坦克向炮管方向后退
      glm::vec2 backward = Rotate(glm::vec2{0.0f, 30.0f}, turret_rotation_+ glm::radians(90.0f));
      offset += backward;
    move_count_down_ = kTickPerSecond; 
    }
    float speed = move_speed * GetSpeedScale();
    offset *= kSecondPerTick * speed;
    auto new_position =
        position_ + glm::vec2{glm::rotate(glm::mat4{1.0f}, rotation_,
                                          glm::vec3{0.0f, 0.0f, 1.0f}) *
                              glm::vec4{offset, 0.0f, 0.0f}};
    if (!game_core_->IsBlockedByObstacles(new_position)) {
      game_core_->PushEventMoveUnit(id_, new_position);
    }
    float rotation_offset = 0.0f;
    if (input_data.key_down[GLFW_KEY_A]) {
      rotation_offset += 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_D]) {
      rotation_offset -= 1.0f;
    }
    
    rotation_offset *= kSecondPerTick * rotate_angular_speed * GetSpeedScale();
    game_core_->PushEventRotateUnit(id_, rotation_ + rotation_offset);
  }

} 
if (move_count_down_) {
    move_count_down_--;
  }
  }

void yumiao2007_issue::TurretRotate() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    auto diff = input_data.mouse_cursor_position - position_;
    if (glm::length(diff) < 1e-4) {
      turret_rotation_ = rotation_;
    } else {
      turret_rotation_ = std::atan2(diff.y, diff.x) - glm::radians(90.0f);
    }
  }
}

void yumiao2007_issue::Fire() {
  if (fire_count_down_ == 0) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();


      glm::vec2 fire_position = position_ + Rotate({0.0f, 1.2f}, turret_rotation_);
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 15.0f}, turret_rotation_);

        // 发射第一颗子弹
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, 1.0f * GetDamageScale(), velocity);

        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.

      }

      if (input_data.key_down[GLFW_KEY_E]&&energy>0) {
        auto velocity1 = Rotate(glm::vec2{0.0f, 15.0f}, turret_rotation_ - glm::radians(10.0f));
        auto velocity2 = Rotate(glm::vec2{0.0f, 15.0f}, turret_rotation_ + glm::radians(10.0f));
        auto velocity3 = Rotate(glm::vec2{0.0f, 15.0f}, turret_rotation_);

        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_ - glm::radians(30.0f)),
            turret_rotation_ - glm::radians(30.0f), 1.0f * GetDamageScale(), velocity1);

        // 发射第二颗子弹
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_ + glm::radians(30.0f)),
            turret_rotation_ + glm::radians(30.0f), 1.0f * GetDamageScale(), velocity2);

        // 发射第三颗子弹
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, 1.0f * GetDamageScale(), velocity3);
        energy--;
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.

      }

      
      if (input_data.key_down[GLFW_KEY_F]&&energy>0) {
        auto velocity3 = Rotate(glm::vec2{0.0f, 30.0f}, turret_rotation_);
        
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 2.8f}, turret_rotation_),
            turret_rotation_ , 1.0f * GetDamageScale(), velocity3);

        // 发射第二颗子弹
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 2.0f}, turret_rotation_),
            turret_rotation_, 1.0f * GetDamageScale(), velocity3);

        // 发射第三颗子弹
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, 1.0f * GetDamageScale(), velocity3);
        energy--;
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.

      }
    if (input_data.key_down[GLFW_KEY_G]&&energy>0) {
        auto velocity1 = Rotate(glm::vec2{0.0f, 10.0f}, turret_rotation_ - glm::radians(10.0f));
        auto velocity2 = Rotate(glm::vec2{0.0f, 10.0f}, turret_rotation_ + glm::radians(10.0f));
        auto velocity3 = Rotate(glm::vec2{0.0f, 10.0f}, turret_rotation_);
                auto velocity4 = Rotate(glm::vec2{0.0f, 10.0f}, turret_rotation_ - glm::radians(20.0f));
        auto velocity5 = Rotate(glm::vec2{0.0f, 10.0f}, turret_rotation_ + glm::radians(20.0f));
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_ , 1.0f * GetDamageScale(), velocity1);

        // 发射第二颗子弹
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_ ),
            turret_rotation_ , 1.0f * GetDamageScale(), velocity2);

        // 发射第三颗子弹
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, 1.0f * GetDamageScale(), velocity3);
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, 1.0f * GetDamageScale(), velocity4);
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, 1.0f * GetDamageScale(), velocity5);
        
        energy--;
        energy--;
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second.

      }
      
    }
  }
  if (fire_count_down_) {
    fire_count_down_--;
  }
}

bool yumiao2007_issue::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);

  return position.x > -0.8f && position.x < 0.8f && position.y > -1.0f &&
         position.y < 1.0f && position.x + position.y < 1.6f &&
         position.y - position.x < 1.6f;
}

const char *yumiao2007_issue::UnitName() const {
  return "yumiao2007_issue";
}

const char *yumiao2007_issue::Author() const {
  return "s-yu24";
}
}  // namespace battle_game::unit
