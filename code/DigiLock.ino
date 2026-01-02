#include <Keypad.h>
#include <Servo.h>

// === KEYPAD SETUP ===
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {A0, A1, A2, A3};
byte colPins[COLS] = {A4, A5, 12, 13};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// === PASSWORD ===
String password = "1234AB";
String input = "";
int wrongAttempts = 0;

// === SERVO ===
Servo myServo;
const int servoPin = 10;

// === BUZZER ===
const int buzzerPin = 11;

// === 7-SEGMENT DISPLAY (Common Anode) ===
int segPins[8] = {2, 3, 4, 5, 6, 7, 8, 9}; // last one is dp
const byte digits[16][8] = {
  {0, 0, 0, 0, 0, 0, 1, 1}, // 0
  {1, 0, 0, 1, 1, 1, 1, 1}, // 1
  {0, 0, 1, 0, 0, 1, 0, 1}, // 2
  {0, 0, 0, 0, 1, 1, 0, 1}, // 3
  {1, 0, 0, 1, 1, 0, 0, 1}, // 4
  {0, 1, 0, 0, 1, 0, 0, 1}, // 5
  {0, 1, 0, 0, 0, 0, 0, 1}, // 6
  {0, 0, 0, 1, 1, 1, 1, 1}, // 7
  {0, 0, 0, 0, 0, 0, 0, 1}, // 8
  {0, 0, 0, 0, 1, 0, 0, 1}, // 9
  {0, 0, 0, 1, 0, 0, 0, 1}, // A
  {1, 1, 0, 0, 0, 0, 0, 1}, // B
  {0, 1, 1, 0, 0, 0, 1, 1}, // C
  {1, 0, 0, 0, 0, 1, 0, 1}, // D
  {0, 1, 1, 0, 0, 0, 0, 1}, // E
  {0, 1, 1, 1, 0, 0, 0, 1}  // F
};

void setup() {
  Serial.begin(9600);
  myServo.attach(servoPin);
  myServo.write(0); // Locked

  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  for (int i = 0; i < 8; i++) {
    pinMode(segPins[i], OUTPUT);
    digitalWrite(segPins[i], HIGH); // Turn OFF (common anode)
  }
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    playKeyBeep(); // Beep on key press

    Serial.print(key); // Live print to MATLAB
    input += key;
    displayChar(key);

    if (input.length() == 6) {
      delay(300);
      if (input == password) {
        correctAction();
        wrongAttempts = 0;
      } else {
        wrongAttempts++;
        wrongAction();  // Beep 4 times
        clearDisplay(); // Turn OFF display after incorrect
      }
      input = ""; // Reset input
    }
  }

  if (wrongAttempts >= 6) {
    tone(buzzerPin, 1000); // Long warning beep
  }
}

// === BEEP FOR KEY ===
void playKeyBeep() {
  tone(buzzerPin, 2000); // High-pitch
  delay(100);
  noTone(buzzerPin);
}

// === WRONG ACTION ===
void wrongAction() {
  for (int i = 0; i < 4; i++) {
    tone(buzzerPin, 300); // Low-pitch
    delay(200);
    noTone(buzzerPin);
    delay(100);
  }
}

// === CORRECT ===
void correctAction() {
  myServo.write(90); // Unlock
  showDot();          // Show dot
  delay(4000);        // Wait
  myServo.write(0);   // Lock again
  clearDisplay();     // Clear after unlock
}

// === DISPLAY ===
void displayChar(char ch) {
  int index = -1;
  if (ch >= '0' && ch <= '9') index = ch - '0';
  else if (ch >= 'A' && ch <= 'F') index = ch - 'A' + 10;

  if (index != -1) {
    for (int i = 0; i < 8; i++) {
      digitalWrite(segPins[i], digits[index][i]);
    }
  }
}

void showDot() {
  for (int i = 0; i < 7; i++) digitalWrite(segPins[i], HIGH); // Turn off all segments
  digitalWrite(segPins[7], LOW); // Turn on dot
}

void clearDisplay() {
  for (int i = 0; i < 8; i++) digitalWrite(segPins[i], HIGH);
}
