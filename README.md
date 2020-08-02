# ESPHome LED Matrix based on strips

This is a custom component using led strips organized in a matrix to be used as a display in [ESPHome](https://esphome.io/).

This work is based on the work of [rnauber](https://github.com/rnauber/ESPHomeMatrixLED).

## API

You can use the same API as any other [DisplayBuffer](https://esphome.io/api/classesphome_1_1display_1_1_display_buffer.html) with this additional functionality:

```c
// select one of this strip modes and rotate as needed with set_rotation
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

static auto display = new MatrixLedDisplay(light::AddressableLight* lights, StripMode strip_mode, int width, int height);
//static auto display = new MatrixLedDisplay(light::AddressableLight* lights, int width, int height);

// set foreground color
display->set_fg_color(light::ESPColor& color);

// set background color
display->set_bg_color(light::ESPColor& color);

// set if background should be printed
display->draw_bg(bool draw);
```

Actually this is not a buffer but prints directly to the led strips.

Please see [example.yaml](https://github.com/escoand/matrix_led_display/blob/master/example.yaml) for a complex configuration.
