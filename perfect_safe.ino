/*
 * Perfect safe.
 *
 * Code for littleBits Arduino.
 *
 * Author: 2014 Michal Valoušek, michal@plovarna.cz
 */

#define PASSWORD_LENGTH     3
#define PASSWORD_PIN        A1
#define BEEP_PIN            1
#define SERVO_PIN           9
#define LIGHT_PIN           5
#define CLOSED_PIN          0
#define VALUE_BUFFER_LENGTH 8
#define MAX_SAME_VALUES     30

// Password! If you want to change it, set macro PASSWORD_LENGTH 
// and PASSWORD constant.
//
// For example to set passcode with 3 numbers:
//
//      const int PASSWORD[PASSWORD_LENGTH] = {2, 9, 5};
//      #define PASSWORD_LENGTH 3
//
// To set passcode with 5 numbers:
//
//      const int PASSWORD[PASSWORD_LENGTH] = {1, 2, 3, 4, 5};
//      #define PASSWORD_LENGTH 5
//
// Beware! It is forbidden to set two same numbers in sequence.
// This passcode is invalid (there are two "9" in sequence):
//
//      const int PASSWORD[PASSWORD_LENGTH] = {2, 9, 9, 3};
//      #define PASSWORD_LENGTH 4
//
const int PASSWORD[PASSWORD_LENGTH] = {2, 9, 5};


int password_buffer[PASSWORD_LENGTH];
int password_buffer_idx = -1;

int value_buffer[VALUE_BUFFER_LENGTH];
int value_buffer_idx = -1;

int same_value_counter = 0;

boolean ready_to_close = false;

int beep_counter = 0;

int door_counter = -1;


void store_value() {
  int value = map(analogRead(PASSWORD_PIN), 0, 1023, 0, 10);

  if (value_buffer_idx < (VALUE_BUFFER_LENGTH-1)) {
    value_buffer_idx++;
  }
  else {
    value_buffer_idx = 0;
  }
  value_buffer[value_buffer_idx] = value;
}

void clear_value_buffer() {
  int old_value = value_buffer[value_buffer_idx];
  for (int i=0; i<VALUE_BUFFER_LENGTH; i++) {
    value_buffer[i] = -1;
  }
  value_buffer[value_buffer_idx] = old_value;
}

boolean value_is_stable() {
  int value = value_buffer[value_buffer_idx];
  if (value == -1) {
    return false;
  }

  int diff;
  for (int i=0; i<VALUE_BUFFER_LENGTH; i++) {
    diff = abs(value_buffer[i] - value);
    if (diff > 0) {
      return false;
    }
  }
  return true;
}


boolean store_pw_value() {
  int value = value_buffer[value_buffer_idx];

  if ((password_buffer_idx != -1) && (value == password_buffer[password_buffer_idx])) {
    same_value_counter++;
    if (same_value_counter > MAX_SAME_VALUES) {
      clear_value_buffer();
      clear_pw_buffer();
      same_value_counter = 0;
    }
    return false;
  }
  same_value_counter = 0;

  if (password_buffer_idx < (PASSWORD_LENGTH-1)) {
    password_buffer_idx++;
  }
  else {
    password_buffer_idx = 0;
  }
  password_buffer[password_buffer_idx] = value;
  return true;
}

boolean check_pw() {
  for (int i=0; i<PASSWORD_LENGTH; i++) {
    boolean found = true;
    for (int y=0; y<PASSWORD_LENGTH; y++) {
      if (abs(password_buffer[(i+y) % PASSWORD_LENGTH]-PASSWORD[y]) > 0) {
        found = false;
        break;
      }
    }
    if (found) {
      return true;
    }
  }
  return false;
}

void clear_pw_buffer() {
  int old_value = password_buffer[password_buffer_idx];
  for (int i=0; i<PASSWORD_LENGTH; i++) {
    password_buffer[i] = -1;
  }
  password_buffer[password_buffer_idx] = old_value;
  digitalWrite(BEEP_PIN, LOW);
}


void start_beep() {
  beep_counter = 1;
  digitalWrite(BEEP_PIN, HIGH);
}

void stop_beep() {
  digitalWrite(BEEP_PIN, LOW);
}


void open_door() {
  digitalWrite(SERVO_PIN, HIGH);  
  digitalWrite(LIGHT_PIN, HIGH);
  door_counter = 15;
}

void close_door() {
  digitalWrite(SERVO_PIN, LOW);
  digitalWrite(LIGHT_PIN, LOW);
  door_counter = -1;
  clear_value_buffer();
  clear_pw_buffer();
}


ISR(TIMER1_COMPA_vect) {
  if (beep_counter > 0) {
    beep_counter--;
  }
  else if (beep_counter == 0) {
    stop_beep();
  }

  if (door_counter > 0) {
    door_counter--;
  }
  else if (door_counter == 0) {
    ready_to_close = true;
    return;
  }

  store_value();
  if (!value_is_stable()) {
    return;
  }

  if (!store_pw_value()) {
    return;
  }

  start_beep();

  if (check_pw()) {
    open_door();
  }
}


void setup() {
  for (int i=0; i<PASSWORD_LENGTH; i++) {
    password_buffer[i] = -1;
  }

  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A = 3125;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS12) | (1 << CS10);
  TIMSK1 |= (1 << OCIE1A);
  sei();

  pinMode(BEEP_PIN, OUTPUT);
  digitalWrite(BEEP_PIN, LOW);

  pinMode(SERVO_PIN, OUTPUT);
  digitalWrite(SERVO_PIN, LOW);

  pinMode(LIGHT_PIN, OUTPUT);
  digitalWrite(LIGHT_PIN, LOW);
}


void loop() {
  if (ready_to_close) {
    if (digitalRead(CLOSED_PIN) == HIGH) {
      close_door();
      ready_to_close = false;
    }
  }
  delay(300);
}
