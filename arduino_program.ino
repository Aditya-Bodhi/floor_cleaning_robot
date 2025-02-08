// Motor control pins
#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5

// Ultrasonic sensor pins
#define TRIG_PIN 10
#define ECHO_PIN 11

// Relay control pin
#define RELAY_PIN 12

// Mode control variables
bool bluetoothMode = true; // Start in Bluetooth mode

// Timer variables for relay control
unsigned long previousMillis = 0; // Stores last time the relay was updated
const long relayOnTime = 2000; // Relay on for 2 seconds
const long relayOffTime = 30000; // Relay off for 30 seconds
bool pumpState = false; // Track the state of the pump (on/off)

void setup() {
  // Motor driver pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Ultrasonic sensor pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Relay pin
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Ensure pump is off at start

  // Bluetooth communication
  Serial.begin(9600);
}

void loop() {
  // Automated pump control
  unsigned long currentMillis = millis(); // Get current time
 
  // Check if it's time to change pump state
  if (pumpState) {
    if (currentMillis - previousMillis >= relayOnTime) {
      digitalWrite(RELAY_PIN, LOW); // Turn pump off
      pumpState = false;
      previousMillis = currentMillis; // Reset timer
    }
  } else {
    if (currentMillis - previousMillis >= relayOffTime) {
      digitalWrite(RELAY_PIN, HIGH); // Turn pump on
      pumpState = true;
      previousMillis = currentMillis; // Reset timer
    }
  }

  if (bluetoothMode) {
    // Bluetooth control mode
    if (Serial.available()) {
      char command = Serial.read();
      executeBluetoothCommand(command);
    }
  } else {
    // Autonomous mode
    autonomousMode();
  }
}

// Function to execute Bluetooth commands
void executeBluetoothCommand(char command) {
  stopMotors(); // Stop motors before processing command
 
  switch (command) {
    case 'F': // Move forward
      moveForward();
      break;
    case 'B': // Move backward
      moveBackward();
      break;
    case 'L': // Turn left
      turnLeft();
      break;
    case 'R': // Turn right
      turnRight();
      break;
    case 'S': // Switch to autonomous mode
      bluetoothMode = false;
      break;
    case 'C': // Switch back to Bluetooth mode
      bluetoothMode = true;
      stopMotors();
      break;
    default:
      stopMotors();
      break;
  }
}

// Function for autonomous obstacle avoidance
void autonomousMode() {
  while (!bluetoothMode) { // Stay in autonomous mode until 'C' is received
    long distance = measureDistance();
    if (distance < 30) {
      stopMotors();
      delay(500);
      turnLeft();
      delay(1000); // Adjust delay for turning
      stopMotors();
      delay(500);
    } else {
      moveForward();
    }

    // Check if 'C' is received to exit autonomous mode
    if (Serial.available() && Serial.read() == 'C') {
      bluetoothMode = true;
      stopMotors();
      break;
    }
  }
}

// Function to measure distance with ultrasonic sensor
long measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  long distance = duration * 0.034 / 2; // Convert to cm
  return distance;
}

// Motor control functions
void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void moveBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}


