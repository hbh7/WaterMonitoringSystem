/* Global variables */
// This is the input pin on the Arduino (Water in to tank)
int flowPinIn = 2;
// This is the input pin on the Arduino (Water out to house)
int flowPinOut = 3;
// These are values we intend to calculate (totals per second)
double flowRateIn;
double gallonsIn;
double gallonsOut;
double flowRateOut;
// These integers store interrupt data and need to be set as volatile to ensure they update correctly during the interrupt process.
volatile int countIn;
volatile int countOut;

/* Setup pins, interrupts, and serial connection */
void setup() {
  // Set the pin modes for the flow meters
  pinMode(flowPinIn, INPUT);
  pinMode(flowPinOut, INPUT);
  // Attach interrupts and interrupt functions to the flow meter pins
  attachInterrupt(digitalPinToInterrupt(flowPinIn), FlowIn, RISING);
  attachInterrupt(digitalPinToInterrupt(flowPinOut), FlowOut, RISING);
  // Start the serial connection
  Serial.begin(9600);
  // Set up the LED pin
  pinMode(LED_BUILTIN, OUTPUT);
}

/* Calculation loop */
void loop() {
  // Turn the LED off (gets turned back on if interrupt is received)
  digitalWrite(LED_BUILTIN, LOW);
  // Reset the counters to 0
  countIn = 0;
  countOut = 0;
  // Enables interrupts on the Arduino
  interrupts();

  // Wait 1 second to collect data via interrupts
  delay (1000);

  // Disable interrupts on the Arduino to perform calculations
  noInterrupts();
   
  // Calculate the serial output values
  // Take counted pulses in the last second and multiply by gallon amount of flow sensor (1 pulse = 1/1380 of a gallon)
  // Then convert seconds to minutes, giving you gallons per minute
  gallonsIn = (countIn * 0.0007246);
  flowRateIn = gallonsIn * 60;

  gallonsOut = (countOut * 0.0007246);
  flowRateOut = gallonsOut * 60;

  // Write data to serial with 7 digits after the decimal point
  Serial.print("gallonsIn:");
  Serial.print(gallonsIn, 7);
  Serial.print(",flowRateIn:");
  Serial.print(flowRateIn, 7);
  Serial.print(",gallonsOut:");
  Serial.print(gallonsOut, 7);
  Serial.print(",flowRateOut:");
  Serial.print(flowRateOut, 7);
  Serial.println();
}

/* Interrupt handler function for input flow */
void FlowIn() {
  // Every time this function is called, increment the respective counter by 1
  countIn++;
  // Turn the LED on to indicate data was received
  digitalWrite(LED_BUILTIN, HIGH);
}

/* Interrupt handler function for output flow */
void FlowOut() {
  // Every time this function is called, increment the respective counter by 1
  countOut++;
  // Turn the LED on to indicate data was received
  digitalWrite(LED_BUILTIN, HIGH);
}
