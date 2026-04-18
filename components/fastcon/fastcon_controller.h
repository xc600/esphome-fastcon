#pragma once

#include <queue>
#include <mutex>
#include <vector>
#include "esphome/core/component.h"
#include "esphome/components/esp32_ble_server/ble_server.h"

namespace esphome {
namespace light { class LightState; } 

namespace fastcon {

class FastconController : public Component {
 public:
  void setup() override;
  void loop() override;

  // YAML Setters (Fixes the main.cpp errors)
  void set_adv_interval(uint16_t val) { adv_interval_min_ = adv_interval_max_ = val; }
  void set_adv_interval_min(uint16_t val) { adv_interval_min_ = val; }
  void set_adv_interval_max(uint16_t val) { adv_interval_max_ = val; }
  void set_adv_duration(uint16_t val) { adv_duration_ = val; }
  void set_adv_gap(uint16_t val) { adv_gap_ = val; }
  void set_max_queue_size(size_t size) { max_queue_size_ = size; }

  // Protocol & Queueing
  void single_control(uint32_t light_id, const std::vector<uint8_t> &light_data);
  void queueCommand(uint32_t light_id, const std::vector<uint8_t> &data);
  std::vector<uint8_t> get_light_data(light::LightState *state);
  std::vector<uint8_t> get_white_light_data(light::LightState *state);

  void set_mesh_key(std::array<uint8_t, 4> key) { mesh_key_ = key; }

 protected:
  struct Command {
    std::vector<uint8_t> data;
    uint8_t retries{0};
    static constexpr uint8_t MAX_RETRIES = 20; 
  };

  std::queue<Command> queue_;
  mutable std::mutex queue_mutex_;
  size_t max_queue_size_{100};
  
  enum class AdvertiseState { IDLE, ADVERTISING, GAP };
  AdvertiseState adv_state_{AdvertiseState::IDLE};
  uint32_t state_start_time_{0};

  void start_advertising_(const std::vector<uint8_t> &data);
  void stop_advertising_();
  std::vector<uint8_t> generate_command(uint8_t n, uint32_t light_id, const std::vector<uint8_t> &data, bool forward);

  std::array<uint8_t, 4> mesh_key_{};
  uint16_t adv_interval_min_{0x20}; 
  uint16_t adv_interval_max_{0x20};
  uint16_t adv_duration_{40}; 
  uint16_t adv_gap_{5};
};

} // namespace fastcon
} // namespace esphome