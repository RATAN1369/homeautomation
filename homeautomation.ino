#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#define Password_Length 8
char Data[Password_Length];
char Master[Password_Length] = "1A1A1A1";

int lockOutput = 13;
byte data_count = 0;
char customKey;

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define FAN_PIN 12
#define BUZZER_PIN A1

void setup() {
  lcd.backlight();
  lcd.init();
  pinMode(lockOutput, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.begin(9600); // Set the baud rate to your Bluetooth module
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");

  customKey = customKeypad.getKey();
  if (customKey) {
    Data[data_count] = customKey;
    lcd.setCursor(data_count, 1);
    lcd.print(Data[data_count]);
    data_count++;
  }

  fan(); // Control fan based on serial input

  if (data_count == Password_Length - 1) {
    lcd.clear();

    if (!strcmp(Data, Master)) {
      lcd.print("Correct");
      delay(1000);
      lcd.clear();
      lcd.print("Door is open");
      digitalWrite(BUZZER_PIN, HIGH); // Turn on buzzer
      delay(2000); // Sound buzzer for 2 seconds
      digitalWrite(BUZZER_PIN, LOW); // Turn off buzzer
      lcd.clear();
      lcd.print("Welcome back");
      delay(1000);

      // Counter program starts here
      counterProgram();
      return; // Exit loop after counter program ends
    }
    else {
      lcd.print("Incorrect");
      delay(1000);
      lcd.clear();
      lcd.print("Invader ?");
      delay(1000);
    }

    lcd.clear();
    clearData();
  }

  delay(100);
}

void clearData() {
  while (data_count != 0) {
    Data[data_count--] = 0;
  }
}

void counterProgram() {
  int sensorPin1 = 10;
  int sensorPin2 = 11;
  int relay = 13;

  int sensorState1 = 0;
  int sensorState2 = 0;
  int count = 0;

  pinMode(sensorPin1, INPUT_PULLUP);
  pinMode(sensorPin2, INPUT_PULLUP);
  pinMode(relay, OUTPUT);

  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("COUNTER");
  lcd.setCursor(0, 1);
  lcd.print("No Visitors     ");
  delay(200);

  while (true) {
    sensorState1 = digitalRead(sensorPin1);
    sensorState2 = digitalRead(sensorPin2);

    // Check if sensor 1 becomes HIGH before sensor 2
    if (sensorState1 == HIGH && sensorState2 == LOW ) {
      count++; // Increment count when someone enters
      delay(500);
    }

    // Check if sensor 2 becomes HIGH before sensor 1
    if (sensorState2 == HIGH && sensorState1 == LOW ) {
      count--; // Decrement count when someone exits
      delay(500);
    }

    // Update LCD display based on count value
    if (count <= 0) {
      digitalWrite(relay, LOW);
      lcd.setCursor(0, 1);
      lcd.print("No visitors    ");
      count=0;
    } else if (count > 0 ) {
      digitalWrite(relay, HIGH);
      lcd.setCursor(0, 1);
      lcd.print("Visitors:   ");
      lcd.setCursor(12, 1);
      lcd.print(count);
      lcd.setCursor(13, 1);
      lcd.print("  ");
    } else {
      digitalWrite(relay, HIGH);
      lcd.setCursor(0, 1);
      lcd.print("Visitors:   ");
      lcd.setCursor(12, 1);
      lcd.print(count);
      delay(5000);
      return;
    }
  }
}

void fan() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == '1') {
      digitalWrite(FAN_PIN, HIGH); // Turn on fan
      Serial.println("FAN ON");
    } else if (command == '0') {
      digitalWrite(FAN_PIN, LOW); // Turn off fan
      Serial.println("FAN OFF");
    }
  }
}
