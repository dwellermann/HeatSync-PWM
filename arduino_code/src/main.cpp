#include <Arduino.h>

// Funktionsdeklarationen
void setPWMDutyCycle(int percentage);
void setLEDColor(int red, int green, int blue);

// Pin-Definitionen
#define PWM_PIN 9        // PWM-Ausgang für Umwälzpumpe
#define LED_RED 3        // Roter LED Pin
#define LED_GREEN 5      // Grüner LED Pin
#define LED_BLUE 6       // Blauer LED Pin

// PWM-Parameter
#define PWM_FREQUENCY 500  // 500 Hz für Umwälzpumpe

// Zeitkonstanten (in Millisekunden)
#define PHASE_1_TIME 20000  // 20 Sekunden - Pumpe AUS (0%)
#define PHASE_2_TIME 20000  // 20 Sekunden - 33% PWM
#define PHASE_3_TIME 20000  // 20 Sekunden - 50% PWM
#define PHASE_4_TIME 7000   // 7 Sekunden - 100% PWM

// Globale Variablen
unsigned long startTime;
int currentPhase = 0;
bool testComplete = false;
int lastPWMValue = -1;  // Verhindert PWM-Spam im Serial Monitor

void setup() {
  Serial.begin(115200);
  Serial.println("Umwälzpumpen PWM-Tester gestartet");
  Serial.println("===================================");

  // Pin-Konfiguration
  pinMode(PWM_PIN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  // PWM-Frequenz auf 500Hz einstellen (Timer1)
  // Timer1 für Pin 9 und 10
  TCCR1A = _BV(COM1A1) | _BV(WGM11);        // Fast PWM, non-inverting mode
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS11); // Fast PWM, prescaler 8
  ICR1 = (16000000 / 8 / PWM_FREQUENCY) - 1;  // TOP-Wert berechnet aus PWM_FREQUENCY

  // LED-Test: Einzelfarben testen (gedimmt für Schutz ohne Widerstände)
  Serial.println("LED-Test startet...");
  setLEDColor(0, 0, 0);     delay(1000); // Alle AUS
  Serial.println("ROT");
  setLEDColor(77, 0, 0);    delay(1000); // Nur ROT (30% Helligkeit)
  Serial.println("GRÜN");
  setLEDColor(0, 77, 0);    delay(1000); // Nur GRÜN (30% Helligkeit)
  Serial.println("BLAU");
  setLEDColor(0, 0, 77);    delay(1000); // Nur BLAU (30% Helligkeit)
  Serial.println("WEISS");
  setLEDColor(77, 77, 77);  delay(1000); // WEISS (30% Helligkeit)
  Serial.println("AUS - Test beendet");
  setLEDColor(0, 0, 0);     delay(1000); // AUS

  // PWM auf 98% für Pumpe AUS setzen
  setPWMDutyCycle(98);

  // Timer starten
  startTime = millis();
  currentPhase = 1;

  Serial.println("Phase 1: Pumpe AUS (98% PWM) - LED ROT - 20s");
}

void loop() {
  if (testComplete) {
    return; // Test beendet, nichts mehr tun
  }

  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - startTime;

  switch (currentPhase) {
    case 1: // Phase 1: 20s AUS (98% PWM = Pumpe stoppt)
      setLEDColor(77, 0, 0);  // ROT (gedimmt)
      setPWMDutyCycle(98);    // 98% PWM für Pumpe AUS

      if (elapsedTime >= PHASE_1_TIME) {
        currentPhase = 2;
        startTime = millis();
        Serial.println("Phase 2: 33% PWM - LED GRÜN - 20s");
      }
      break;

    case 2: // Phase 2: 20s bei 33% (mittlere Drehzahl)
      setLEDColor(0, 77, 0);  // GRÜN (gedimmt)
      setPWMDutyCycle(33);    // 33% PWM

      if (elapsedTime >= PHASE_2_TIME) {
        currentPhase = 3;
        startTime = millis();
        Serial.println("Phase 3: 50% PWM - LED BLAU - 20s");
      }
      break;

    case 3: // Phase 3: 20s bei 50%
      setLEDColor(0, 0, 77);  // BLAU (gedimmt)
      setPWMDutyCycle(50);    // 50% PWM

      if (elapsedTime >= PHASE_3_TIME) {
        currentPhase = 4;
        startTime = millis();
        Serial.println("Phase 4: 5% PWM (höchste Drehzahl) - LED WEISS - 7s");
      }
      break;

    case 4: // Phase 4: 7s bei höchster Drehzahl (5% PWM)
      setLEDColor(77, 77, 77);     // WEISS (gedimmt)
      setPWMDutyCycle(5);          // 5% PWM = höchste Drehzahl

      if (elapsedTime >= PHASE_4_TIME) {
        // Test beenden
        testComplete = true;
        setPWMDutyCycle(98);     // 98% PWM = Pumpe AUS
        setLEDColor(0, 0, 0);    // LED ausschalten
        Serial.println("Test beendet! Reset-Knopf für Wiederholung drücken.");
      }
      break;
  }

  delay(100); // Kleine Pause für Stabilität
}

// Funktion zum Setzen der PWM-Duty-Cycle
void setPWMDutyCycle(int percentage) {
  if (percentage < 0) percentage = 0;
  if (percentage > 100) percentage = 100;

  // OCR1A berechnen für gewünschten Duty Cycle
  int ocrValue = (ICR1 * percentage) / 100;
  OCR1A = ocrValue;

  // Nur ausgeben wenn sich der Wert geändert hat (Anti-Spam)
  if (percentage != lastPWMValue) {
    Serial.print("PWM gesetzt auf: ");
    Serial.print(percentage);
    Serial.println("%");
    lastPWMValue = percentage;
  }
}

// Funktion zum Setzen der LED-Farbe (gemeinsame Kathode)
void setLEDColor(int red, int green, int blue) {
  analogWrite(LED_RED, red);
  analogWrite(LED_GREEN, green);
  analogWrite(LED_BLUE, blue);
}
