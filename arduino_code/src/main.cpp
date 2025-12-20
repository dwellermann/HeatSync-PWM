#include <Arduino.h>

// Funktionsdeklarationen
void setPWMDutyCycle(int percentage);
void blinkInternalLED(int count);

// Pin-Definitionen
#define PWM_PIN 9        // PWM-Ausgang für Umwälzpumpe (OC1A)
#define INTERNAL_LED 13  // Interne LED für Phasenanzeige

// PWM-Parameter
#define PWM_FREQUENCY 500  // 500 Hz für Umwälzpumpe

// Zeitkonstanten (in Millisekunden)
#define PHASE_1_TIME 10000  // 10 Sekunden - Pumpe AUS (98%)
#define PHASE_2_TIME 10000  // 10 Sekunden - 70% PWM
#define PHASE_3_TIME 10000  // 10 Sekunden - 33% PWM
#define PHASE_4_TIME 10000  // 10 Sekunden - 5% PWM

// Globale Variablen
unsigned long startTime;
int currentPhase = 0;
bool testComplete = false;
int lastPWMValue = -1;
unsigned long lastBlinkTime = 0;
#define BLINK_INTERVAL 2000  // Alle 2 Sekunden blinken

void setup() {
  Serial.begin(115200);
  delay(1000); // Kurze Pause für Serial Monitor
  Serial.println("\n\n===========================================");
  Serial.println("Umwälzpumpen PWM-Tester v2.0");
  Serial.println("===========================================");
  Serial.println("Phasen-Anzeige über interne LED (Pin 13):");
  Serial.println("1x blinken = Phase 1 (98% = STOP)");
  Serial.println("2x blinken = Phase 2 (70% PWM)");
  Serial.println("3x blinken = Phase 3 (33% PWM)");
  Serial.println("4x blinken = Phase 4 (5% = MAX)");
  Serial.println("===========================================\n");

  // Pin-Konfiguration
  pinMode(PWM_PIN, OUTPUT);
  pinMode(INTERNAL_LED, OUTPUT);

  // Timer1 komplett zurücksetzen
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 0;
  
  // PWM-Frequenz auf 500Hz einstellen (Timer1, Fast PWM Mode 14)
  // Fast PWM Mode 14: WGM13=1, WGM12=1, WGM11=1, WGM10=0
  // COM1A1=1, COM1A0=0 für non-inverting mode auf OC1A (Pin 9)
  
  TCCR1A = _BV(COM1A1) | _BV(WGM11);              // COM1A1=1, WGM11=1
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS11);   // WGM13=1, WGM12=1, CS11=1 (prescaler 8)
  
  // ICR1 = (F_CPU / (Prescaler * Frequenz)) - 1
  // ICR1 = (16000000 / (8 * 500)) - 1 = 3999
  ICR1 = 3999;  // Exakter Wert für 500 Hz bei 16 MHz und Prescaler 8
  
  // Tatsächliche Frequenz berechnen
  float actualFreq = 16000000.0 / (8.0 * (ICR1 + 1));

  Serial.println("=== TIMER1 KONFIGURATION ===");
  Serial.print("TCCR1A: 0b");
  Serial.println(TCCR1A, BIN);
  Serial.print("TCCR1B: 0b");
  Serial.println(TCCR1B, BIN);
  Serial.print("ICR1 (TOP): ");
  Serial.println(ICR1);
  Serial.print("Soll-Frequenz: ");
  Serial.print(PWM_FREQUENCY);
  Serial.println(" Hz");
  Serial.print("Ist-Frequenz: ");
  Serial.print(actualFreq, 2);
  Serial.println(" Hz");
  Serial.print("Periode: ");
  Serial.print(1000.0 / actualFreq, 3);
  Serial.println(" ms\n");

  // Initial PWM auf 98% (Pumpe STOP)
  setPWMDutyCycle(98);
  Serial.println("WICHTIG: Pin 9 mit Oszilloskop/Multimeter prüfen!");
  Serial.println("Bei 98% PWM sollte ca. 4.9V DC messbar sein.\n");

  // Timer starten
  startTime = millis();
  lastBlinkTime = millis();
  currentPhase = 1;

  Serial.println("=== Phase 1 START ===");
  Serial.println("PWM: 98% (Pumpe STOP - Bereich 95-100%)");
  Serial.println("LED: ROT");
  Serial.println("Dauer: 10s\n");
  blinkInternalLED(1);
}

void loop() {
  if (testComplete) {
    return;
  }

  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - startTime;

  // Periodisches Blinken alle 2 Sekunden
  if (currentTime - lastBlinkTime >= BLINK_INTERVAL) {
    blinkInternalLED(currentPhase);
    lastBlinkTime = currentTime;
    
    // Debug-Info: verbleibende Zeit
    unsigned long remaining = 0;
    switch (currentPhase) {
      case 1: remaining = PHASE_1_TIME - elapsedTime; break;
      case 2: remaining = PHASE_2_TIME - elapsedTime; break;
      case 3: remaining = PHASE_3_TIME - elapsedTime; break;
      case 4: remaining = PHASE_4_TIME - elapsedTime; break;
    }
    Serial.print("Phase ");
    Serial.print(currentPhase);
    Serial.print(" - noch ");
    Serial.print(remaining / 1000);
    Serial.println("s");
  }

  switch (currentPhase) {
    case 1: // Phase 1: Pumpe AUS (98% PWM)

      setPWMDutyCycle(98);

      if (elapsedTime >= PHASE_1_TIME) {
        currentPhase = 2;
        startTime = millis();
        lastBlinkTime = millis();
        Serial.println("\n=== Phase 2 START ===");
        Serial.println("PWM: 70% (mittlere Drehzahl - Bereich 11-84%)");
        Serial.println("LED: GRÜN");
        Serial.println("Dauer: 10s\n");
        blinkInternalLED(2);
      }
      break;

    case 2: // Phase 2: 70% PWM

      setPWMDutyCycle(70);

      if (elapsedTime >= PHASE_2_TIME) {
        currentPhase = 3;
        startTime = millis();
        lastBlinkTime = millis();
        Serial.println("\n=== Phase 3 START ===");
        Serial.println("PWM: 33% (höhere Drehzahl - Bereich 11-84%)");
        Serial.println("LED: BLAU");
        Serial.println("Dauer: 10s\n");
        blinkInternalLED(3);
      }
      break;

    case 3: // Phase 3: 33% PWM

      setPWMDutyCycle(33);

      if (elapsedTime >= PHASE_3_TIME) {
        currentPhase = 4;
        startTime = millis();
        lastBlinkTime = millis();
        Serial.println("\n=== Phase 4 START ===");
        Serial.println("PWM: 5% (MAXIMALE Drehzahl - Bereich 1-10%)");
        Serial.println("LED: WEISS");
        Serial.println("Dauer: 10s\n");
        blinkInternalLED(4);
      }
      break;

    case 4: // Phase 4: 5% PWM (max Drehzahl)

      setPWMDutyCycle(5);

      if (elapsedTime >= PHASE_4_TIME) {
        testComplete = true;
        setPWMDutyCycle(98);
        digitalWrite(INTERNAL_LED, LOW);
        Serial.println("\n===========================================");
        Serial.println("=== TEST BEENDET ===");
        Serial.println("Pumpe gestoppt (98% PWM)");
        Serial.println("Reset-Knopf für Wiederholung drücken.");
        Serial.println("===========================================\n");
      }
      break;
  }

  delay(50);
}

// Funktion zum Blinken der internen LED
void blinkInternalLED(int count) {
  for (int i = 0; i < count; i++) {
    digitalWrite(INTERNAL_LED, HIGH);
    delay(150);
    digitalWrite(INTERNAL_LED, LOW);
    delay(150);
  }
}

// Funktion zum Setzen der PWM-Duty-Cycle
void setPWMDutyCycle(int percentage) {
  if (percentage < 0) percentage = 0;
  if (percentage > 100) percentage = 100;

  // OCR1A Berechnung
  unsigned long ocrValue = ((unsigned long)ICR1 * percentage) / 100;
  
  // Sicherheitscheck: OCR1A darf nicht > ICR1 sein für korrektes PWM
  if (ocrValue > ICR1) {
    ocrValue = ICR1;
  }
  
  OCR1A = ocrValue;

  if (percentage != lastPWMValue) {
    // Duty Cycle in Mikrosekunden berechnen
    float periodUs = 1000000.0 / PWM_FREQUENCY;  // Periode in µs
    float highTimeUs = (periodUs * percentage) / 100.0;
    float lowTimeUs = periodUs - highTimeUs;
    
    Serial.println("--- PWM UPDATE ---");
    Serial.print("Duty Cycle: ");
    Serial.print(percentage);
    Serial.println("%");
    Serial.print("OCR1A: ");
    Serial.print(ocrValue);
    Serial.print(" / ");
    Serial.print(ICR1);
    Serial.print(" (");
    Serial.print((ocrValue * 100.0) / ICR1, 2);
    Serial.println("%)");
    Serial.print("HIGH Zeit: ");
    Serial.print(highTimeUs, 1);
    Serial.print(" µs, LOW Zeit: ");
    Serial.print(lowTimeUs, 1);
    Serial.println(" µs");
    Serial.print("Erwartete DC-Spannung: ");
    Serial.print((5.0 * percentage) / 100.0, 2);
    Serial.println(" V");
    Serial.println("------------------\n");
    
    lastPWMValue = percentage;
  }
}