#include "esphome.h"
using namespace esphome;

/*
 * MODE:    - 0 -   - 1 -   - 2 -   - 3 -
 *          0 1 2   0 3 6   0 1 2   0 5 6
 *          3 4 5   1 4 7   5 4 3   1 4 7
 *          6 7 8   2 5 8   6 7 8   2 3 8
 */
enum StripMode {
  STRIP_MODE_ROW_BY_ROW = 0, // default
  STRIP_MODE_COL_BY_COL = 1,
  STRIP_MODE_ZIG_ZAG_ROWS = 2,
  STRIP_MODE_ZIG_ZAG_COLS = 3,
};

class MatrixLedDisplay : public display::DisplayBuffer, public Component {
 public:
  MatrixLedDisplay(light::AddressableLight* lights,
                   const uint8_t width_internal,
                   const uint8_t height_internal) {
    this->lights_ = lights;
    this->width_ = width_internal;
    this->height_ = height_internal;
  }

  MatrixLedDisplay(light::AddressableLight* lights, const StripMode strip_mode,
                   const uint8_t width_internal,
                   const uint8_t height_internal) {
    this->lights_ = lights;
    this->strip_mode_ = strip_mode;
    this->width_ = width_internal;
    this->height_ = height_internal;
  }

  void set_fg_color(const light::ESPColor& fg_color) {
    this->fg_color_ = fg_color;
  }

  void set_bg_color(const light::ESPColor& bg_color) {
    this->bg_color_ = bg_color;
  }

  void draw_bg(const bool draw_bg) {
    this->draw_bg_ = draw_bg;
  }

  void clear() {
    auto before = this->draw_bg_;
    this->draw_bg_ = true;
    DisplayBuffer::clear();
    this->draw_bg_ = before;
  }

  uint16_t get_absolute_pixel(int x, int y) {
    switch (this->rotation_) {
      case DISPLAY_ROTATION_0_DEGREES:
        break;
      case DISPLAY_ROTATION_90_DEGREES:
        std::swap(x, y);
        x = this->get_width_internal() - x - 1;
        break;
      case DISPLAY_ROTATION_180_DEGREES:
        x = this->get_width_internal() - x - 1;
        y = this->get_height_internal() - y - 1;
        break;
      case DISPLAY_ROTATION_270_DEGREES:
        std::swap(x, y);
        y = this->get_height_internal() - y - 1;
        break;
    }

    return this->get_absolute_pixel_internal(x, y);
  }

  bool is_on(int x, int y) {
    uint16_t pos = this->get_absolute_pixel(x, y);
    if(pos < 0 || pos >= this->lights_->size())
      return false;
    return this->lights_->get(pos).get().is_on();
  }

  bool is_on(int x, int y, Image* image) {
    for (int i = 0; i < image->get_width(); i++)
      for (int j = 0; j < image->get_height(); j++)
        if (image->get_pixel(i, j) & this->is_on(x + i, y + j)) return true;
    return false;
  }

 protected:
  uint16_t get_absolute_pixel_internal(int x, int y) {
    uint16_t pos = 0;

    switch (this->strip_mode_) {
      case STRIP_MODE_ROW_BY_ROW:
        pos = (y * this->width_) + x;
        break;
      case STRIP_MODE_COL_BY_COL:
        pos = (x * this->height_) + y;
        break;
      case STRIP_MODE_ZIG_ZAG_ROWS:
        pos = (y * this->width_) + (y % 2 == 0 ? x : this->width_ - x - 1);
        break;
      case STRIP_MODE_ZIG_ZAG_COLS:
        pos = (x * this->height_) + (x % 2 == 0 ? y : this->height_ - y - 1);
        break;
    }

    return pos;
  }

  void draw_absolute_pixel_internal(int x, int y, int color) override {
    if (x >= this->width_ || x < 0 || y >= this->height_ || y < 0) return;

    uint16_t pos = this->get_absolute_pixel_internal(x, y);

    if (pos < 0 || pos >= this->lights_->size()) {
      ESP_LOGD("MatrixLedDisplay",
               "Size mismatch of AddressableLight and Matrix, could not get "
               "x=%i y=%i pos=%i",
               x, y, pos);
      return;
    }

    if (color) {
      (*(this->lights_))[pos] = this->fg_color_;
    } else if (this->draw_bg_) {
      (*(this->lights_))[pos] = this->bg_color_;
    }
  }

  int get_height_internal() override { return this->height_; }
  int get_width_internal() override { return this->width_; }

  light::AddressableLight* lights_;
  StripMode strip_mode_{STRIP_MODE_ROW_BY_ROW};
  uint8_t width_{8};
  uint8_t height_{8};
  light::ESPColor fg_color_{light::ESPColor(0xff, 0xff, 0xff)};
  light::ESPColor bg_color_{light::ESPColor(0x00, 0x00, 0x00)};
  bool draw_bg_{true};
};
