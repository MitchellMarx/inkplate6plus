#pragma once

#ifdef USE_HOST
#include "../sdl_esphome.h"
#include "esphome/components/touchscreen/touchscreen.h"

namespace esphome::sdl {

class SdlTouchscreen : public touchscreen::Touchscreen, public Parented<Sdl> {
 public:
  void setup() override {
    this->x_raw_max_ = this->display_->get_width();
    this->y_raw_max_ = this->display_->get_height();
  }

  void update_touches() override {
    // LOCAL PATCH: consume one queued button transition per poll instead of sampling
    // the current level. A short click therefore becomes press on this poll and
    // release on the next, and back-to-back clicks stay separate. When no transition
    // is queued the current level is used, so a held button keeps reporting a touch.
    // Each state is held for a full poll interval, which must stay longer than LVGL's
    // input read period (30 ms by default) for LVGL to observe it; see sdl.yaml.
    bool down = this->parent_->mouse_down;
    int x = this->parent_->mouse_x;
    int y = this->parent_->mouse_y;
    auto &edges = this->parent_->button_edges;
    if (!edges.empty()) {
      down = edges.front().down;
      x = edges.front().x;
      y = edges.front().y;
      edges.pop_front();
    }
    if (down) {
      add_raw_touch_position_(0, x, y);
    }
  }
};

}  // namespace esphome::sdl
#endif
