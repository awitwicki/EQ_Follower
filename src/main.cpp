#include <TMCStepper.h>

#define DIR_PIN          3 // Direction
#define STEP_PIN         4 // Step
#define CS_PIN           5 // Chip select
#define SW_MOSI          11 // Software Master Out Slave In (MOSI)
#define SW_MISO          12 // Software Master In Slave Out (MISO)
#define SW_SCK           13 // Software Slave Clock (SCK)

#define R_SENSE 0.11f // Match to your driver
                      // SilentStepStick series use 0.11
                      // UltiMachine Einsy and Archim2 boards use 0.2
                      // Panucatt BSD2660 uses 0.1
                      // Watterott TMC5160 uses 0.075

// Select your stepper driver type

TMC5160Stepper driver = TMC5160Stepper(CS_PIN, R_SENSE, SW_MOSI, SW_MISO, SW_SCK);
bool direction = true;
bool isManualMode = true;

// Motor have 200 steps
// Each step is 8 microstep signals
// Motor shaft is coupled directly to a harmonic drive (CSF-mini 11) with 100:1 reduction
// Harmonic drive output is the RA axis, so it needs 1 turn per sidereal day (86164.0905s)
// Steps per sidereal day is 100 (harmonic reduction) * 200 (steps) * 8 (1/8 step per signal) = 160000
// Period is 86164.0905 / 160000 = 0.5385256s = 538526us
#define STEP_PERIOD_US 538526UL
#define STEP_PULSE_US  20
unsigned long lastStepTime = 0;

void setDirection(bool newDirection) {
    direction = newDirection;
    digitalWrite(DIR_PIN, direction);
}

void updateDirection(bool newDirection) {
  if (direction != newDirection) {
    setDirection(newDirection);
  }
}

void setup() {
  // Serial.begin(9600);
  // while(!Serial);
  // Serial.println("Start...");

  // Initiate pins and registeries
  driver.begin();

  // Set stepper current to 1600mA. The command is the same as command TMC2130.setCurrent(600, 0.11, 0.5);
  driver.rms_current(200);

  // Enable extremely quiet stepping
  driver.en_pwm_mode(1);
  driver.microsteps(8);

  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);

  setDirection(true);

  // Serial.print("DRV_STATUS=0b");
  // Serial.println(driver.DRV_STATUSs(), BIN);
}

void automaticMode () {
  if (isManualMode) {
    isManualMode = false;
    updateDirection(true);
    lastStepTime = micros();
  }

  // Schedule steps on micros() instead of blocking with delay(), so the joystick
  // keeps being read between steps and loop overhead doesn't stretch the period.
  // Unsigned arithmetic keeps this correct across micros() overflow.
  if (micros() - lastStepTime >= STEP_PERIOD_US) {
    lastStepTime += STEP_PERIOD_US;

    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(STEP_PULSE_US);
    digitalWrite(STEP_PIN, LOW);
  }
}

void manualMode(int speed) {
    isManualMode = true;

    // Direction is controlled by DIR pin
    updateDirection(speed > 0);

    int pulseTime = map(abs(speed), 0, 1000, 3000, 200);
    int waitTime = map(abs(speed), 0, 1000, 3000, 200);
    // Serial.println(pulseTime);

    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(pulseTime);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(waitTime);
}

void loop()
{
  // Get value from analog joystick on pin A0 and map it to -100 to 100 range
  long speedValue = map(analogRead(1), 0, 1023, -1000, 1000);

  if (abs(speedValue) > 80) {
   manualMode(speedValue);
  } else {
   automaticMode();
  }
}
