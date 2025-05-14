#include <Servo.h>

// Pin definitions
const int pirPin = 2;
const int buzzerPin = 4;
const int greenLED = 7;
const int yellowLED = 8;
const int servoPin = 9;

const int wlPot = A0;
const int hlzPot = A1;

Servo lockServo;

// Timing control
unsigned long lastSensorRead = 0;
unsigned long lastReportPrint = 0;
const unsigned long sensorInterval = 5000;
const unsigned long reportInterval = 60000;
unsigned long buzzerStartTime = 0;
unsigned long hazardUnlockTime = 0;
bool isBuzzerOn = false;

// State tracking
int lastWLStatus = -1;
int lastHLZStatus = -1;
bool motionPreviouslyDetected = false;
bool hazardUnlock = false;
bool isDoorLocked = false;

void setup() {
  pinMode(pirPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);

  Serial.begin(9600);
  lockServo.attach(servoPin);
  lockServo.write(90);
  isDoorLocked = false;
  Serial.println("System Initialized. Home is Secure.");
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastSensorRead >= sensorInterval) {
    lastSensorRead = currentTime;

    int motion = digitalRead(pirPin);
    int wlVal = analogRead(wlPot);
    int hlzVal = analogRead(hlzPot);

    int wlPercent = map(wlVal, 0, 1023, 0, 100);
    int hlzPercent = map(hlzVal, 0, 1023, 0, 100);

    bool dangerDetected = false;
    int wlStatus, hlzStatus;

    // --- Hazard Level  ---
    if (hlzPercent <= 10) hlzStatus = 0;
    else if (hlzPercent <= 30) hlzStatus = 1;
    else if (hlzPercent <= 60) hlzStatus = 2;
    else if (hlzPercent <= 85) hlzStatus = 3;
    else hlzStatus = 4;

    if (hlzStatus != lastHLZStatus) {
      Serial.print("🌡️ Hazard Level Changed: ");
      switch (hlzStatus) {
        case 0: Serial.println("Safe"); break;
        case 1: Serial.println("Low Risk"); break;
        case 2: Serial.println("Moderate Risk - Monitoring..."); dangerDetected = true; break;
        case 3: Serial.println("High Risk - Immediate Attention Required!"); hazardUnlock = true; dangerDetected = true; break;
        case 4: Serial.println("❗ DANGER! Life-threatening Hazard DETECTED!"); hazardUnlock = true; dangerDetected = true; break;
      }
      lastHLZStatus = hlzStatus;
    }

    // --- Unlock for Emergency ---
    if (hlzStatus >= 3) {
      if (isDoorLocked) {
        lockServo.write(90); 
        isDoorLocked = false;
        Serial.println("🔓 Servo Status: Door UNLOCKED for Emergency!");
      }
      hazardUnlock = true;
      hazardUnlockTime = currentTime;
    } else if (hazardUnlock && currentTime - hazardUnlockTime > 500 && hlzStatus <= 1) {
      if (!isDoorLocked) {
        lockServo.write(0); 
        isDoorLocked = true;
        Serial.println("🔒 Door re-locked after emergency passed.");
      }
      hazardUnlock = false;
    }

    // --- Window Lock Status ---
    if (wlPercent <= 5) wlStatus = 0;
    else if (wlPercent <= 15) wlStatus = 1;
    else if (wlPercent <= 30) wlStatus = 2;
    else if (wlPercent <= 70) wlStatus = 3;
    else if (wlPercent <= 95) wlStatus = 4;
    else wlStatus = 5;

    if (wlStatus != lastWLStatus) {
      Serial.print("🔒 Window Status Changed: ");
      switch (wlStatus) {
        case 0: Serial.println("Fully Closed"); break;
        case 1: Serial.println("Mostly Closed"); break;
        case 2: Serial.println("Slightly Open"); break;
        case 3: Serial.println("Partially Open - Caution"); dangerDetected = true; break;
        case 4: Serial.println("Mostly Open - Risk!"); dangerDetected = true; break;
        case 5: Serial.println("❗ Fully Open - Security Breach!"); dangerDetected = true; break;
      }
      lastWLStatus = wlStatus;
    }

    // --- Motion Detection  ---
    if (motion == HIGH) {
      if (!motionPreviouslyDetected) {
        Serial.println("🚨 ALERT: Motion detected near your home!");
        motionPreviouslyDetected = true;

        if (!hazardUnlock && hlzStatus < 3) {
          lockServo.write(0); 
          isDoorLocked = true;
          Serial.println("🔒 Servo Status: Door LOCKED due to motion.");
        } else {
          Serial.println("⚠️ Motion detected, but door remains UNLOCKED due to hazard risk.");
        }

        dangerDetected = true;
      }
    } else {
      if (motionPreviouslyDetected) {
        Serial.println("✅ Motion stopped.");
        motionPreviouslyDetected = false;
      }
    }

    // --- Buzzer & LEDs ---

    digitalWrite(buzzerPin, LOW);
    isBuzzerOn = false;
    // Buzzer Logic
    if (motion == HIGH || wlStatus == 5 || hlzStatus >= 3) {
      digitalWrite(buzzerPin, HIGH);
      buzzerStartTime = currentTime;
      isBuzzerOn = true;
      dangerDetected = true;
    }
    // LED logic 
    if (hlzStatus >= 2 || wlStatus >= 3 || motion == HIGH) {
      digitalWrite(yellowLED, HIGH);
      digitalWrite(greenLED, LOW);
      dangerDetected = true;
    } else {
      digitalWrite(yellowLED, LOW);
      digitalWrite(greenLED, HIGH);
    }

    if (dangerDetected) {
      digitalWrite(yellowLED, HIGH);
      digitalWrite(greenLED, LOW);
    } else {
      digitalWrite(yellowLED, LOW);
      digitalWrite(greenLED, HIGH);
    }
  }

  // Turn off buzzer after 2 seconds in moderate risk
  if (isBuzzerOn && millis() - buzzerStartTime >= 2000) {
    digitalWrite(buzzerPin, LOW);
    isBuzzerOn = false;
  }

  // Print report every 60 seconds
  if (millis() - lastReportPrint >= reportInterval) {
    lastReportPrint = millis();

    int wlVal = analogRead(wlPot);
    int hlzVal = analogRead(hlzPot);
    int wlPercent = map(wlVal, 0, 1023, 0, 100);
    int hlzPercent = map(hlzVal, 0, 1023, 0, 100);

    Serial.println("========= 🏠 SECURITY STATUS REPORT 🕒 =========");
    Serial.print("🔒 Window Lock: "); Serial.print(wlPercent); Serial.println("%");
    Serial.print("🌡️ Hazard Level: "); Serial.print(hlzPercent); Serial.println("%");
    Serial.print("🚪 Door Servo: "); Serial.println(isDoorLocked ? "Locked" : "Unlocked for Emergency");
    Serial.println("===============================================");
  }
}
