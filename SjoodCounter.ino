const int ledPins[] = {4, 0, 2, 14};  // D2, D3, D4, D5
const int irPin = 5;                 // D1
const int resetButtonPin = 12;      // D6

int sujoodCount = 0;
bool inSujood = false;
unsigned long sujoodStartTime = 0;

bool previousButtonState = HIGH;

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  pinMode(irPin, INPUT);
  pinMode(resetButtonPin, INPUT_PULLUP);

  Serial.println("ESP8266 Ready. Hold Sujood for 3 seconds to count.");
}

void loop() {
  bool irState = digitalRead(irPin) == LOW;  // LOW = يوجد جسم (سجود)
  unsigned long currentTime = millis();

  // بداية السجود
  if (irState && !inSujood) {
    sujoodStartTime = currentTime;
    inSujood = true;
  }

  // التحقق من استمرار السجود 3 ثوانٍ
  if (inSujood && irState && (currentTime - sujoodStartTime >= 3000)) {
    sujoodCount++;
    Serial.print("Sujood Count: ");
    Serial.println(sujoodCount);
    updateLEDs(sujoodCount / 2);

    // ننتظر حتى يرفع الشخص رأسه (ir = HIGH) قبل السماح بسجدة جديدة
    while (digitalRead(irPin) == LOW) {
      delay(50);
      yield();
    }
    inSujood = false;
  }

  // التصفير بالزر
  bool currentButtonState = digitalRead(resetButtonPin);
  if (currentButtonState == LOW && previousButtonState == HIGH) {
    sujoodCount = 0;
    updateLEDs(0);
    Serial.println("Counter Reset via Button");
  }
  previousButtonState = currentButtonState;

  // التصفير عبر Serial
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'r') {
      sujoodCount = 0;
      updateLEDs(0);
      Serial.println("Counter Reset via Serial");
    }
  }

  delay(50);
  yield();  // مهم لمنع WDT reset
}

void updateLEDs(int numOn) {
  Serial.print("LEDs: ");
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
}
