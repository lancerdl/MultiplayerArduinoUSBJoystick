/* USB HID Multiplayer Joystick */
/* Author: Matthew Nikkanen
 * Released into public domain.
 */

bool debug = false;

/*
 * Serial Synchronization
 */
uint8_t sync_seq[3] = { 0xFF, 0xFF, 0xFF };
uint64_t last_sync_timestamp;


#define PIN_FIRST           2
#define PINS_PER_JOYSTICK   12

typedef enum interface_id_e_ {
  IF_FIRST = 0,
  IF_0     = IF_FIRST,
  IF_1,
  IF_2,
  IF_3,
  IF_NUM,
} interface_id_e;

typedef enum joy_e_ {
  JOY_AXIS_X = 0,
  JOY_LEFT   = JOY_AXIS_X,  /* -ve */
  JOY_RIGHT,                /* +ve */
  JOY_AXIS_Y = 2,
  JOY_UP     = JOY_AXIS_Y,  /* -ve */
  JOY_DOWN,                 /* +ve */
  JOY_NUM,
} joy_e;

typedef enum axis_e_ {
  AXIS_FIRST = 0,
  AXIS_X     = AXIS_FIRST,
  AXIS_Y,
  AXIS_NUM,
} axis_e;

typedef enum button_e_ {
  BUTTON_FIRST  = 0,
  BUTTON_LAST   = 5,
  BUTTON_PLAYER = 6,
  BUTTON_SHIFT  = 7,
  BUTTON_NUM,
} button_e;

typedef struct joystick_state_t_ {
  int8_t axis[AXIS_NUM]; /* Array of joystick axes */
  uint8_t buttons;       /* Bit mask of the currently pressed buttons */
} joystick_state_t;

/*
 * State: recorded and previously sent.
 */
joystick_state_t joy_state[IF_NUM];
joystick_state_t prev_joy_state[IF_NUM];


/*
 * Setup.
 */
 
void joystick_setup(int if_ix)
{
  int pin_base = PIN_FIRST + if_ix*PINS_PER_JOYSTICK;
  int ix;

  /*
   * Configure pins.
   */
  for (ix = 0; ix < PINS_PER_JOYSTICK; ix++) {
    pinMode(pin_base + ix, INPUT);
    digitalWrite(pin_base + ix, HIGH);
  }

  /*
   * Initialize joystick state.
   */
  for (ix = AXIS_FIRST; ix < AXIS_NUM; ix++) {
    joy_state[if_ix].axis[ix] = 0;
  }
  joy_state[if_ix].buttons = 0;
}

void send_sync()
{
  if (!debug) {
    Serial.write(sync_seq, sizeof(sync_seq));
  } else {
    Serial.print(sync_seq[0]);
    Serial.print(sync_seq[1]);
    Serial.print(sync_seq[2]);
  }

  last_sync_timestamp = millis();
}

void setup()
{
  uint8_t if_ix, ix;
  
  Serial.begin(115200);
  delay(200);

  for (if_ix = IF_FIRST; if_ix < IF_NUM; if_ix++) {
    joystick_setup(if_ix);
  }

  /*
   * Reset serial.
   */
  send_sync();
}


/*
 * Loop activities.
 */

void button_state(joystick_state_t *joy_state,
                  uint8_t button,
                  bool is_pressed)
{
  if (is_pressed) {
    joy_state->buttons |= (1 << button);
  }
}

void update_joystick_state(int if_ix)
{
  int pin_base = PIN_FIRST + if_ix*PINS_PER_JOYSTICK;
  joystick_state_t *state, *prev_state;
  int ix, pin, val1, val2;
  bool is_shifted;

  state = &joy_state[if_ix];
  prev_state = &prev_joy_state[if_ix];

  /*
   * Get joystick inputs.
   */
  for (ix = AXIS_FIRST; ix < AXIS_NUM; ix++) {
    if (ix == AXIS_X) {
      pin = pin_base + JOY_AXIS_X;
    } else {
      pin = pin_base + JOY_AXIS_Y;
    }
    
    val1 = digitalRead(pin);
    val2 = digitalRead(pin + 1);
    if ((val1 == 0) && (val2 == 0)) { /* Both activated; cancel */
      state->axis[ix] = 0;
    } else if (val1 == 0) { /* Left/Up activated */
      state->axis[ix] = -127;
    } else if (val2 == 0) { /* Right/Down activated */
      state->axis[ix] = 127;
    } else { /* Centred */
      state->axis[ix] = 0;
    }
  }

  /*
   * Get button inputs.
   */
  state->buttons = 0;
  pin_base += JOY_NUM;

  for (ix = BUTTON_FIRST; ix <= BUTTON_NUM; ix++) {
    button_state(state, ix, (digitalRead(pin_base + ix) == 0));
  }
}

void send_joystick_state(int if_ix)
{
  if (!debug) {
    Serial.write((uint8_t *)&joy_state[if_ix], sizeof(joystick_state_t));
  } else {
    Serial.print("Joystick: ");
    Serial.println(if_ix);
    Serial.print("X: ");
    Serial.println(joy_state[if_ix].axis[AXIS_X]);
    Serial.print("Y: ");
    Serial.println(joy_state[if_ix].axis[AXIS_Y]);
    Serial.print("Buttons: ");
    Serial.println(joy_state[if_ix].buttons, HEX);
  }

  /*
   * Save reported state.
   */
  memcpy((uint8_t *)&prev_joy_state[if_ix],
         (uint8_t *)&joy_state[if_ix],
         sizeof(joystick_state_t));
}

void loop()
{
  bool force_send = false;
  int if_ix;

  /*
   * Synchronization.
   */
  if ((millis() - last_sync_timestamp) > 1000) {
    send_sync();
    force_send = true;
  }

  /*
   * Read joystick states.
   */
  for (if_ix = IF_FIRST; if_ix < IF_NUM; if_ix++) {
    update_joystick_state(if_ix);
  }

  /*
   * Send report.
   */
  if (force_send ||
      memcmp(&joy_state, &prev_joy_state,
             sizeof(joystick_state_t)*IF_NUM)) {
    for (if_ix = IF_FIRST; if_ix < IF_NUM; if_ix++) {
      send_joystick_state(if_ix);
    }
  }

  delay(10);
}
