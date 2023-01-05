#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>

#endif
#define LED_PIN 6     //light strip connect to Pin D6
#define BUTTON_PIN 5  // Button connected to Pin D5
#define NUMPIXELS 17  //number of Led on the light strip

#define NUMSETTINGS 7

#define NUM_CIRCLE 12


Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
//create a new light strip object to define the data pattern

#define DELAYVAL 500  //time (in milliseconds) to pause between pixels

int light_setting = 3;

void setup() {
  Serial.begin(9600);
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  pixels.begin();  //intialize NeoPixel strip object
  pixels.setBrightness(20);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

/**
 * Increment light setting and perform mod.
 */
void update_light_setting() {
  light_setting = (light_setting + 1) % NUMSETTINGS;
  Serial.println(light_setting);
}

/**
 * Return true if button was clicked, and wait a delay
 * for debounce.
 */
bool button_was_clicked() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    while (digitalRead(BUTTON_PIN) == LOW) {
      delay(10);
    }
    return true;
  }
  return false;
}

/**
 * Wait for n miliseconds, returning true if the button was clicked
 */
bool button_wait(int n, int t_step = 1) {
  for (int i = 0; i < n; i++) {
    if (button_was_clicked()) {
      return true;
    }
    delay(t_step);
  }
  return false;
}

void rainbow(int wait) {
  while (true) {
    for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256) {
      if (button_was_clicked()) {
        update_light_setting();
        return;
      }

      for (int i = 0; i < pixels.numPixels(); i++) {
        int pixelHue = firstPixelHue + (i * 65536L / pixels.numPixels());
        pixels.setPixelColor(i, pixels.gamma32(pixels.ColorHSV(pixelHue)));
      }
      pixels.show();
      delay(wait);
    }
  }
}

int transformSmallerRange(int min_out, int max_out, int min_in, int max_in, int inputVal) {
  int range_out = max_out - min_out;
  int midpt = (max_in + min_in) / 2;

  if (inputVal < midpt) {
    return min_out + (inputVal / midpt ) * range_out;
  }
  else {
    return min_out + ( 1 - ((inputVal - midpt) / midpt) ) * range_out;
  }
}


bool fade_on(int pix, int r, int g, int b, int start_brightness, int end_brightness) {
  for (float br = start_brightness; br <= end_brightness; br++) {
    // float brightness = start_brightness + (end_brightness - start_brightness) * b / 1000;
    // pixels.setPixelColor(pix, pixels.Color(r / 256 * br, g / 256 * br, b / 256 * br));
    pixels.setPixelColor(pix, pixels.Color(r * (br/256), g * (br/256), b * (br/256)));
    pixels.show();
    if (button_wait(1)) {
      return false;
    }
  }
  return true;
}

bool fade_off(int pix, int r, int g, int b, int start_brightness, int end_brightness, int off_brightness) {
  for (float br = start_brightness; br >= end_brightness; br--) {
    // float brightness = start_brightness + (end_brightness - start_brightness) * b / 1000;
    // pixels.setPixelColor(pix, pixels.Color(r / 256 * br, g / 256 * br, b / 256 * br));
    pixels.setPixelColor(pix, pixels.Color(r * (br/256), g * (br/256), b * (br/256)));
    pixels.show();
    if (button_wait(1)) {
      return false;
    }
  }
  float br = float(off_brightness);
  pixels.setPixelColor(pix, pixels.Color(r * (br/256), g * (br/256), b * (br/256)));
  pixels.show();
  return true;
}

void gold_random(int wait) {
  int max_brightness = 150;
  int base_r = 255;
  int base_g = 70;
  int base_b = 0;
  pixels.fill((0, 0, 0));
  pixels.show();

  uint32_t base_color = pixels.Color(255, 70, 0);
  int min_br = 50;
  uint32_t off = pixels.Color(base_r * (min_br/256), base_g * (min_br/256), base_b * (min_br/256));

  bool end = false;

  while (not end) {
    int pix = random(NUMPIXELS);
    uint32_t current_color = pixels.getPixelColor(pix);
    if (current_color == off) {
      if (not fade_on(pix, base_r, base_g, base_b, 100, 255)) {
        break;
      };
    } else {
      if (not fade_off(pix, base_r, base_g, base_b, 255, 100, min_br)) {
        break;
      }
    }

    pixels.show();
    if (button_wait(wait + random(wait / 2, wait / 2))) {
        end = true;
        break;
      }
  }
  update_light_setting();
}

void lightning(int wait) {
  int max_brightness = 150;
  // int base_r = 134;
  // int base_g = 59;
  // int base_b = 255;
  int base_r = 145;
  int base_g = 205;
  int base_b = 255;
  pixels.fill((0, 0, 0));
  pixels.show();

  uint32_t base_color = pixels.Color(base_r, base_g, base_b);
  int min_br = 50;
  uint32_t off = pixels.Color(base_r * (min_br/256), base_g * (min_br/256), base_b * (min_br/256));

  bool end = false;

  int min_wait = wait;
  int max_wait = 5 * wait;
  int active_wait = max_wait;
  int random_multiplier = 100;
  while (not end) {
    int pix = random(NUMPIXELS);
    uint32_t current_color = pixels.getPixelColor(pix);
    if (current_color == off) {
      pixels.setPixelColor(pix, base_color);
      pixels.show();
    } else {
      pixels.setPixelColor(pix, off);
    }

    pixels.show();
    if (button_wait(active_wait + random(-active_wait /2, active_wait/2))) {
        end = true;
        break;
      }

    if (random(random_multiplier) == 1) {
      if (active_wait == max_wait) {
        active_wait = min_wait;
        random_multiplier = 200;
      } else {
        active_wait = max_wait;
        // pixels.fill(off);
        // pixels.show();
        random_multiplier = 40;
      }
    }
  }
  update_light_setting();
}

void solid_color(int r, int g, int b) {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  pixels.show();

  while (true) {
    if (button_wait(1000)) {
      break;
    }
  }

  update_light_setting();
}

void some_solid_color(int r, int g, int b, int idx_list[], int arr_len) {
  pixels.fill((0, 0, 0));

  // for (byte i=0; i < (sizeof(idx_list) / sizeof(idx_list[0])); i++) {
  for (byte i = 0; i < arr_len; i++) {
    int idx = idx_list[i] % NUMPIXELS;
    pixels.setPixelColor(idx, pixels.Color(r, g, b));
  }

  pixels.show();

  while (true) {
    if (button_wait(1000)) {
      break;
    }
  }

  update_light_setting();
}

void const_cyber(int r, int g, int b) {
  const int status_start = 14;
  const int circle_start = 1;
  const int circle_length = 12;
  for (int i=status_start; i < status_start + 3; i++) {
    pixels.setPixelColor(i, pixels.Color(100, 0, 0));
  }

  int back_idxs [2] = {0, 13};
  for (int i: back_idxs) {
    pixels.setPixelColor(i, pixels.Color(100, 0, 0));
  }

  pixels.show();

  int timestep = 100;

  const uint32_t circle_colors [12] = {
    pixels.Color(200, 100, 0),
    pixels.Color(100, 0, 0),
    pixels.Color(100, 0, 0),
    pixels.Color(100, 0, 0),
    pixels.Color(100, 0, 0),
    pixels.Color(100, 0, 0),
    pixels.Color(200, 100, 0),
    pixels.Color(100, 0, 0),
    pixels.Color(100, 0, 0),
    pixels.Color(100, 0, 0),
    pixels.Color(100, 0, 0),
    pixels.Color(100, 0, 0),
  };

  int start_idx = 0;

  int line_cnt = 0;

  const int line_on [5] = {
    0, 1, 2, 1
  };
  const uint32_t line_color [3] = {
    pixels.Color(100, 100, 0),
    pixels.Color(100, 100, 0),
    pixels.Color(100, 100, 0),
  };
  const int line_period = 3;
  while (not button_wait(timestep)) {
    for (int i=0; i<NUM_CIRCLE; i++) {
      pixels.setPixelColor(circle_start + ((start_idx + i) % NUM_CIRCLE), circle_colors[i]);
    }

    if (line_cnt % line_period == 0) {
      for (int i=status_start; i < status_start + 3; i++) {
        pixels.setPixelColor(i, pixels.Color(100, 0, 0));
      }
      const int on_idx = line_on[int(floor(line_cnt / line_period))];
      pixels.setPixelColor(status_start + on_idx, line_color[on_idx]);
    }

    if (line_cnt == 4 * line_period) {
      line_cnt = 0;
    }

    pixels.show();
    start_idx = (start_idx + 1) % NUM_CIRCLE;
    line_cnt++;
  }

  update_light_setting();
}


void cyber(const uint32_t *circle_colors, const uint32_t back_color, const uint32_t *line_colors,
  const uint32_t *line_background_colors) {
  // https://www.programiz.com/cpp-programming/pointers-arrays
  const int status_start = 14;
  const int circle_start = 1;

  const int back_idxs [2] = {0, 13};
  for (int i: back_idxs) {
    pixels.setPixelColor(i, back_color);
  }

  // pixels.show();

  const int timestep = 100;

  int start_idx = 0;

  int line_cnt = 0;

  const int line_on [5] = {
    0, 1, 2, 1
  };

  const int line_period = 3;
  while (not button_wait(timestep)) {
    // Set circle colors
    for (int i=0; i<NUM_CIRCLE; i++) {
      pixels.setPixelColor(circle_start + ((start_idx + i) % NUM_CIRCLE), *(circle_colors + i));
    }

    // Set line colors
    if (line_cnt % line_period == 0) {
      for (int i=status_start; i < status_start + 3; i++) {
        pixels.setPixelColor(i, *(line_background_colors + i - status_start));
      }

      // Allows period of line to multiple of period of circle
      const int on_idx = line_on[int(floor(line_cnt / line_period))];
      pixels.setPixelColor(status_start + on_idx, *(line_colors + on_idx));
    }

    if (line_cnt == 4 * line_period) {
      line_cnt = 0;
    }

    pixels.show();
    start_idx = (start_idx + 1) % NUM_CIRCLE;
    line_cnt++;
  }

  update_light_setting();
}


/**
 * Script to pick colors using serial console.
*/
void color_picker(const int default_r, const int default_g, const int default_b) {
  const int circle_start = 1;
  for (int i=circle_start; i<circle_start + NUM_CIRCLE; i++) {
    pixels.setPixelColor(i, pixels.Color(default_r, default_g, default_b));
  }
  pixels.show();

  int current_r = default_r;
  int current_g = default_g;
  int current_b = default_b;

  // 1 100 100 100

  // message: 100 100 100 (11)

  // https://www.programmingelectronics.com/serial-read/

  const int message_len = 12;
  char user_color [message_len];
  static unsigned int current_pos = 0;

  while (!button_wait(100)) {
    while (Serial.available() > 0) {

      char in_byte = Serial.read();

      if (in_byte != '\n' && current_pos < message_len - 1) {
        user_color[current_pos] = in_byte;
        current_pos++;
      } else {
        user_color[current_pos] = '\0';
        Serial.println(user_color);

        int* colors = parse_color(&user_color[0]);
        Serial.println(*colors);

        current_r = min(*(colors + 0), 255);
        current_g = min(*(colors + 1), 255);
        current_b = min(*(colors + 2), 255);

        for (int i=circle_start; i<circle_start + NUM_CIRCLE; i++) {
          pixels.setPixelColor(i, pixels.Color(current_r, current_g, current_b));
        }
        pixels.show();

        Serial.println("r: " + String(current_r) + " g: " + String(current_g) + " b: " + String(current_b));

        current_pos = 0;

        break;
      }
    }
  }
}

/**
 * Given user_color as char array with first characters
 * "123 456 789"  where each represents their respective rgb value,
 * Return pointer to int array of values.
 */
int* parse_color(char* user_color) {
  const int message_len = 11;
  int values [3] = {0, 0, 0};
  for (int sec=0; sec<3; sec++) {
    int value = 0;
    for (int place=0; place<3; place++) {
      int digit = *(user_color + sec * 4 + place) - '0';
      // Convert to int and allow for rounding
      int digit_val = round(digit * pow(10, 2 - place));
      value += digit_val;
      // Serial.println(value);
    }
    values[sec] = value;
  }
  // Serial.println(values[0]);

  int* value_pointer = &values[0];
  Serial.println(*value_pointer); // Don't delete, code breaks without
  // Serial.println(*(value_pointer + 1)); // Don't delete, code breaks without
  // Serial.println(*(value_pointer + 2)); // Don't delete, code breaks without

  return value_pointer;
}


void off() {
  pixels.clear();
  pixels.show();

  while (not button_was_clicked()) {
    delay(2);
  }

  update_light_setting();
}

int solid_idxs[3] = { 0, 11 };

const uint32_t circle_colors[12] = {
  pixels.Color(100, 100, 100),
  pixels.Color(100, 0, 0),
  pixels.Color(100, 0, 0),
  pixels.Color(100, 0, 0),
  pixels.Color(100, 0, 0),
  pixels.Color(100, 0, 0),
  pixels.Color(100, 100, 100),
  pixels.Color(100, 0, 0),
  pixels.Color(100, 0, 0),
  pixels.Color(100, 0, 0),
  pixels.Color(100, 0, 0),
  pixels.Color(100, 0, 0),
};

const uint32_t line_colors [3] = {
  pixels.Color(0, 110, 80),
  pixels.Color(0, 110, 80),
  pixels.Color(0, 110, 80),
};

const uint32_t line_background_colors [3] = {
  pixels.Color(85, 110, 10),
  pixels.Color(85, 110, 10),
  pixels.Color(85, 110, 10),
};

// const uint32_t back_color = pixels.Color(100, 0, 0);
const uint32_t back_color = pixels.Color(0, 110, 80),

void loop() {
  switch (light_setting) {
    case 0:
      rainbow(10);
    case 1:
      // cyber(10, 10, 10);
      cyber(&circle_colors[0], back_color, &line_colors[0], &line_background_colors[0]);
    case 2:
      solid_color(69, 9, 59);  // Pink
    case 3:
      color_picker(100, 0, 0);
      // some_solid_color(80, 20, 20, solid_idxs, (sizeof(solid_idxs) / sizeof(int)));
      // solid_color(8, 8, 15);
      // night_mode();
    case 4:
      gold_random(50);
    case 5:
      lightning(50);
    case 6:
      off();
  }
  Serial.write(light_setting);
}