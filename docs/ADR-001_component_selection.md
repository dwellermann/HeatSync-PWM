# ADR-001: Architektur für den PWM Signal Translator

**Status:** Akzeptiert

**Datum:** 2025-12-20

## Kontext

Das Projekt wurde initiiert, um eine spezifische Signal-Inkompatibilität zwischen einer **Viessmann Solarsteuerung EM-SM1** und einer modernen **OEG Umwälzpumpe** zu beheben.

**Das Problem:** Die EM-SM1 Steuerung gibt ein PWM-Signal mit invertierter Logik aus (z.B. 90% Tastgrad für minimale Leistung). Die OEG Pumpe erwartet jedoch ein Standard-PWM-Signal (90% Tastgrad für maximale Leistung). Zudem kann nicht von einer simplen linearen Invertierung ausgegangen werden, da die Pumpe eventuell Mindest- und Höchstgrenzen für den Tastgrad hat. Eine direkte Verbindung führt zu einem fehlerhaften und ineffizienten Betrieb des Solarsystems.

**Ziel:** Entwicklung eines kompakten, sicheren und zuverlässigen Adapters, der als intelligenter "Übersetzer" zwischen den beiden Geräten fungiert.

## Entscheidung

1.  **Kernlogik:** Ein **Arduino Nano** wird als programmierbarer Signal-Übersetzer eingesetzt. Er misst den Tastgrad des EM-SM1-Signals und bildet diesen Wert über eine im Code definierte **Mapping-Funktion** auf einen neuen, für die OEG-Pumpe korrekten Ausgangs-Tastgrad ab.
2.  **Flexible Abbildung (Mapping):** Die Software wird explizit so gestaltet, dass sie an die Kennlinien der OEG Pumpe angepasst werden kann. Dies erlaubt die Definition von Grenzwerten (z.B. Mindestdrehzahl ab 10%, Maximaldrehzahl bei 95%), um die Pumpe optimal und sicher zu betreiben.
3.  **Eingangssicherheit:** Das Signal der Viessmann EM-SM1 wird zwingend über einen **ILD213T Optokoppler** eingelesen. Dies stellt eine galvanische Trennung sicher und schützt die empfindliche Elektronik der Solarsteuerung.
4.  **Ausgangspräzision:** Das übersetzte PWM-Signal für die OEG-Pumpe wird mit der **`TimerOne` Bibliothek** erzeugt, um eine stabile 500-Hz-Frequenz zu garantieren.
5.  **Stromversorgung:** Das Modul wird über ein externes **5V-USB-Netzteil** versorgt, um die Sicherheit zu maximieren und die Komplexität zu minimieren.

## Konsequenzen

### Positiv:
*   **Gezielte Lösung:** Bietet eine validierte Lösung für die bekannte Inkompatibilität zwischen der Viessmann EM-SM1 und OEG Pumpen.
*   **Schutz der Komponenten:** Die galvanische Trennung schützt die teure Solarsteuerung vor potenziellen Störungen oder Fehlern seitens der Pumpe oder des Adapters.
*   **Optimaler Pumpenbetrieb:** Die flexible Mapping-Logik ermöglicht eine Feinabstimmung, die die Pumpe effizient und innerhalb ihrer Spezifikationen betreibt.
*   **Anpassbarkeit:** Obwohl für eine spezifische Kombination entwickelt, kann die Codebasis leicht für andere Pumpen oder Steuerungen mit ähnlichen Problemen angepasst werden.

### Negativ:
*   **Feinabstimmung potenziell nötig:** Die im Code vordefinierten Mapping-Werte sind ein gut funktionierender Startpunkt. Für eine absolute Optimierung kann eine Anpassung an die spezifischen hydraulischen Gegebenheiten des Systems erforderlich sein.