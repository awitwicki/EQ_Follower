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

void setup() {
  // Serial.begin(9600);
  // while(!Serial);
  // Serial.println("Start...");

  // Initiate pins and registeries
  driver.begin();

  // Set stepper current to 1600mA. The command is the same as command TMC2130.setCurrent(600, 0.11, 0.5);
  driver.rms_current(1.600);

  // Enable extremely quiet stepping
  driver.en_pwm_mode(1);
  driver.microsteps(8);


  pinMode(STEP_PIN, OUTPUT);

  // Serial.print("DRV_STATUS=0b");
  // Serial.println(driver.DRV_STATUS(), BIN);
}
void loop()
{
  int speedValue = analogRead(0) + 5;

  if (speedValue > 10) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(speedValue * 2);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(speedValue * 10);
  } else {
    // Motor have 200 steps
    // Each step is 8 microstep signals
    // Belt reductor (16T to 80T) is 5:1
    // 6 turns of second gear means 30 turns of motor per hour
    // 6 turns of second gear * 5 (belt reduction) * 200 (steps) * 8 (1/8 step per signal) / 3600
    // Period is 1 / (6 * 5 * 200 * 8 / 3600) = 0.075s
    // Period is 75ms

    digitalWrite(STEP_PIN, HIGH);
    delay(40);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(35);
  }
}
