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

FadeLed red_fade;
FadeLed green_fade;
FadeLed orange_fade;
FadeLed blue_fade;

void setup(){
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
  uint32_t now = millis();
  uint8_t red_fade_value = red_fade.get_value(now);
  uint8_t green_fade_value = green_fade.get_value(now);
  uint8_t orange_fade_value = orange_fade.get_value(now);
  uint8_t blue_fade_value = blue_fade.get_value(now);
  switch (map(analogRead(A0), 0, 1024, 0, 6))
  {
  case 0:
    analogWrite(6, red_fade_value);
    analogWrite(9, green_fade_value);
    analogWrite(10, orange_fade_value);
    analogWrite(11, blue_fade_value);
    break;
  case 1:
    digitalWrite(6, HIGH);
    digitalWrite(9, HIGH);
    digitalWrite(10, HIGH);
    digitalWrite(11, HIGH);
    break;
  case 2:
    digitalWrite(6, HIGH);
    digitalWrite(9, LOW);
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
    break;
  case 3:
    digitalWrite(6, LOW);
    digitalWrite(9, HIGH);
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
    break;
  case 4:
    digitalWrite(6, LOW);
    digitalWrite(9, LOW);
    digitalWrite(10, HIGH);
    digitalWrite(11, LOW);
    break;
  case 5:
    digitalWrite(6, LOW);
    digitalWrite(9, LOW);
    digitalWrite(10, LOW);
    digitalWrite(11, HIGH);
    break;
  }
}