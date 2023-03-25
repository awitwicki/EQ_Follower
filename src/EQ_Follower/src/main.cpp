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

bool dir = true;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Start...");
  driver.begin(); 			// Initiate pins and registeries
  driver.rms_current(1.600); 	// Set stepper current to 600mA. The command is the same as command TMC2130.setCurrent(600, 0.11, 0.5);
  driver.en_pwm_mode(1);  	// Enable extremely quiet stepping
  driver.microsteps(8);


  pinMode(STEP_PIN, OUTPUT);

  Serial.print("DRV_STATUS=0b");
  Serial.println(driver.DRV_STATUS(), BIN);
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
    // Each step is 16 signals
    // Planetary reductor 16:1
    // This goes to gear 36 teeth : gear 48 teeth -> 3:4
    // 6 turns of second gear means 8 turns of first gear
    // 8 turns of second gear * 16 (planetary reduction) * 200 (steps) * 16 (1/16 step per signal) / 3600
    // 1 / (8 * 16 * 200 * 16 / 3600) = 11.71875ms
    // Period is 11.71875ms
    // TODO Needs to recalculate for new driver and motor reductors

    digitalWrite(STEP_PIN, HIGH);
    delay(11);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(718);
  }
}
