const int ledPins[] = {4, 0, 2, 14};  // D2, D3, D4, D5
const int irPin = 5;                 // D1
const int resetButtonPin = 12;      // D6

int sujoodCount = 0;
bool previousIRState = false;
bool previousButtonState = HIGH;

void setup() {
  Serial.begin(115200);

  // إعداد الليدات
  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  pinMode(irPin, INPUT);
  pinMode(resetButtonPin, INPUT_PULLUP);  // مقاومة داخلية

  Serial.println("ESP8266 Ready. Press reset button or send 'r' to reset.");
}

void loop() {
  // قراءة حساس IR
  bool currentIRState = digitalRead(irPin) == LOW;

  if (currentIRState && !previousIRState) {
    sujoodCount++;
    Serial.print("Sujood Count: ");
    Serial.println(sujoodCount);

    updateLEDs(sujoodCount / 2);
  }
  previousIRState = currentIRState;

  // قراءة زر التصفير
  bool currentButtonState = digitalRead(resetButtonPin);

  if (currentButtonState == LOW && previousButtonState == HIGH) {
    sujoodCount = 0;
    updateLEDs(0);
    Serial.println("Counter Reset via Button");
  }
  previousButtonState = currentButtonState;

  // تصفير عبر Serial
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'r') {
      sujoodCount = 0;
      updateLEDs(0);
      Serial.println("Counter Reset via Serial (r)");
    }
  }

  delay(50);
  yield();  // مهم جداً لتجنّب WDT reset
}

void updateLEDs(int numOn) {
  Serial.print("LED Status: ");
  for (int i = 0; i < 4; i++) {
    if (i < numOn) {
      digitalWrite(ledPins[i], HIGH);
      Serial.print("1 ");
    } else {
      digitalWrite(ledPins[i], LOW);
      Serial.print("0 ");
    }
  }
  Serial.println();
  Serial.flush();
}

