uint32_t get_time_delta(uint32_t since, uint32_t now) {
    uint32_t delta;
    if (since > now) {
      delta = UINT32_MAX - since + now;
    } else {
      delta = now - since;
    }
    return delta;
}

class Timer {
  public:
    uint32_t since, duration;

    Timer(){}
    Timer(uint32_t initial_since, uint32_t initial_duration) {
      since = initial_since;
      duration = initial_duration;
    }

    bool check(uint32_t now) {
      if (get_time_delta(since, now) >= duration) {
        return true;
      }
      return false;
    }

    void reset(uint32_t now) {
      since = now;
    }

    bool check_and_reset(uint32_t now) {
      if (check(now)) {
        reset(now);
        return true;
      }
      return false;
    }
};

class FadeLed {
  private:
    bool is_started = false;
    bool is_increase = true;
    bool is_edge = false;
    Timer start_delay_timer;
    Timer fade_delay_timer;
    Timer edge_delay_timer;
    uint8_t value = 0;
    void change_value(uint32_t now) {
      if (value == 0) {
        is_increase = true;
      };
      if (value == 255) {
        is_increase = false;
      };
      if (is_increase) {
        value++;
      } else {
        value--;
      };
      if (value == 0 or value == 255) {
        is_edge = true;
        edge_delay_timer.reset(now);
      } else {
        fade_delay_timer.reset(now);
      };
    }

  public:
    FadeLed(){};
    FadeLed(uint32_t now, uint32_t start_delay, uint32_t fade_delay, uint32_t edge_delay) {
      start_delay_timer = Timer(now, start_delay);
      fade_delay_timer = Timer(now, fade_delay);
      edge_delay_timer = Timer(now, edge_delay);
    }

    uint8_t get_value(uint32_t now) {
      if (!is_started) {
        if (start_delay_timer.check(now)) {
          is_started = true;
          change_value(now);
        };
        return 0;
      };
      if (is_edge) {
        if (edge_delay_timer.check(now)) {
          is_edge = false;
          change_value(now);
        }
        return value;
      }
      if (fade_delay_timer.check(now)) {
        change_value(now);
      }
      return value;
    }
};

class VaryLed {
    private:
        uint8_t active_leds[2] = {0, 1};
        uint8_t current_active_led = 0;
        Timer timer = Timer(millis(), 1500);

        uint8_t get_new_led_id(){
            uint8_t new_id;
            do {
                new_id = random(0, 4);
            } while (active_leds[0] == new_id || active_leds[1] == new_id);
            return new_id;
        }

        void change(){
            uint8_t current_led_id = active_leds[current_active_led];
            leds[current_led_id] = LOW;
            uint8_t new_led_id = get_new_led_id();
            leds[new_led_id] = HIGH;
            active_leds[current_active_led] = new_led_id;
            if (current_active_led == 0) {
                current_active_led = 1;
            } else {
                current_active_led =0;
            }
            is_changed = true;
        }
    public:
        bool is_changed = true;
        uint8_t leds[4] = {HIGH, HIGH, LOW, LOW};

        VaryLed(){};

        void tick(){
            if (!timer.check_and_reset(millis())){
                return;
            }
            change();
        }
};

FadeLed red_fade;
FadeLed green_fade;
FadeLed orange_fade;
FadeLed blue_fade;
VaryLed varyLed;

void setup(){
  randomSeed(analogRead(0));
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  uint32_t now = millis();
  red_fade = FadeLed(now, 0, 50, 1000);
  green_fade = FadeLed(now, 4000, 50, 1000);
  orange_fade = FadeLed(now, 8000, 50, 1000);
  blue_fade = FadeLed(now, 12000, 50, 1000);
}

void loop(){
  varyLed.tick();
  uint32_t now = millis();
  uint8_t red_fade_value = red_fade.get_value(now);
  uint8_t green_fade_value = green_fade.get_value(now);
  uint8_t orange_fade_value = orange_fade.get_value(now);
  uint8_t blue_fade_value = blue_fade.get_value(now);
  switch (map(analogRead(A0), 0, 1024, 0, 7))
  {
  case 0:
    analogWrite(6, red_fade_value);
    analogWrite(9, green_fade_value);
    analogWrite(10, orange_fade_value);
    analogWrite(11, blue_fade_value);
    break;
  case 1:
    if (varyLed.is_changed) {
        digitalWrite(6, varyLed.leds[0]);
        digitalWrite(9, varyLed.leds[1]);
        digitalWrite(10, varyLed.leds[2]);
        digitalWrite(11, varyLed.leds[3]);
        varyLed.is_changed = false;
    }
    break;
  case 2:
    digitalWrite(6, HIGH);
    digitalWrite(9, HIGH);
    digitalWrite(10, HIGH);
    digitalWrite(11, HIGH);
    break;
  case 3:
    digitalWrite(6, HIGH);
    digitalWrite(9, LOW);
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
    break;
  case 4:
    digitalWrite(6, LOW);
    digitalWrite(9, HIGH);
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
    break;
  case 5:
    digitalWrite(6, LOW);
    digitalWrite(9, LOW);
    digitalWrite(10, HIGH);
    digitalWrite(11, LOW);
    break;
  case 6:
    digitalWrite(6, LOW);
    digitalWrite(9, LOW);
    digitalWrite(10, LOW);
    digitalWrite(11, HIGH);
    break;
  }
}