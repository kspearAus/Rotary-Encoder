/* Gary's solution  https://garrysblog.com/2021/03/20/reliably-debouncing-rotary-encoders-with-arduino-and-esp32/#more-187
 *  Based on Oleg Mazurov's code for rotary encoder interrupt service routines for AVR micros
   here https://chome.nerpa.tech/mcu/reading-rotary-encoder-on-arduino/
   and using interrupts https://chome.nerpa.tech/mcu/rotary-encoder-interrupt-service-routine-for-avr-micros/
*/
// Define rotary encoder pins white 21, yellow 5, orange 13, vcc, gnd
#define ENC_A 25  // 21 white
#define ENC_B 35  // 5  yellow
volatile int counter = 0;

// button stuff
const int buttonPin = 34; //orange
int buttonState = 0;  

void setup() {

  // Set encoder pins and attach interrupts
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_A), read_encoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_B), read_encoder, CHANGE);
  
  pinMode(buttonPin, INPUT);
 
  // Start the serial monitor to show output
  Serial.begin(115200); // Change to 9600 for Nano, 115200 for ESP32
  delay(500);           // Wait for serial to start  
  Serial.println("Start");
}

void loop() {
  static int lastCounter = 0;
  if(counter != lastCounter){  // Show and save new value
    Serial.println(counter);
    lastCounter = counter;  }
    
    buttonState = digitalRead(buttonPin); // button operation
    if (buttonState == LOW) { 
     Serial.println("Switch on"); delay(1000); }
}

void read_encoder() {
  // Encoder interrupt routine for both pins. Updates counter
  // if they are valid and have rotated a full indent
 
  static uint8_t old_AB = 3;  // Lookup table index
  static int8_t encval = 0;   // Encoder value  
  static const int8_t enc_states[]  = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0}; // Lookup table
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  
  old_AB <<=2;  // Remember previous state
  
  if (digitalRead(ENC_A)) old_AB |= 0x02; // Add current state of pin A
  if (digitalRead(ENC_B)) old_AB |= 0x01; // Add current state of pin B
  
  encval += enc_states[( old_AB & 0x0f )];

  // Update counter if encoder has rotated a full indent, that is at least 4 steps
  if( encval > 3 ) {                                    // Four steps forward
    if (interruptTime - lastInterruptTime > 40) {       // Greater than 40 milliseconds
      counter ++;                                       // Increase by 1
    } else if (interruptTime - lastInterruptTime > 20){ // Greater than 20 milliseconds
      counter += 3;                                     // Increase by 3
    } else {                                            // Faster than 20 milliseconds
      counter += 10;                                    // Increase by 10
    }
    encval = 0;
    lastInterruptTime = millis();                       // Remember time
  }
  else if( encval < -3 ) {                              // Four steps backwards
    if (interruptTime - lastInterruptTime > 40) {       // Greater than 40 milliseconds
      counter --;                                       // Increase by 1
    } else if (interruptTime - lastInterruptTime > 20){ // Greater than 20 milliseconds
      counter -= 3;                                     // Increase by 3
    } else {                                            // Faster than 20 milliseconds
      counter -= 10;                                    // Increase by 10
    }
    encval = 0;
    lastInterruptTime = millis();                       // Remember time
  }
}
