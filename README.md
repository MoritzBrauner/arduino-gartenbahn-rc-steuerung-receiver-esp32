## ESP32 Gartenbahn RC-Receiver

Projekt basiert auf einem **ESP32 NodeMCU Dev Board** als Empfänger für Gartenbahn-RC-Steuerung.

---

## ESP32 Pinout Referenz: 
https://lastminuteengineers.com/esp32-pinout-reference/

## 🧩 ESP32 Dev Board Pinout (Übersicht)

![ESP32 Dev Board Pinout](./d8007507-7d27-4124-92f5-79e3ae0863d9.png)

> ℹ️ Stelle sicher, dass sich das Bild im Repository befindet (z. B. im Root-Verzeichnis oder unter `/docs/images/`) und passe ggf. den Pfad an.

---

## ⚠️ GPIO-Pins – Nutzung & Einschränkungen

Die folgende Tabelle zeigt, welche GPIOs **sicher nutzbar** sind und bei welchen Pins besondere Vorsicht geboten ist.

| Label | GPIO | Safe to use? | Reason |
|------|------|--------------|--------|
| D0 | 0 | ⚠️ eingeschränkt | Must be **HIGH during boot**, **LOW for programming** |
| TX0 | 1 | ❌ | TX pin, used for flashing and debugging |
| D2 | 2 | ⚠️ eingeschränkt | Must be **LOW during boot**, connected to on-board LED |
| RX0 | 3 | ❌ | RX pin, used for flashing and debugging |
| D4 | 4 | ✅ | — |
| D5 | 5 | ⚠️ eingeschränkt | Must be **HIGH during boot** |
| D6 | 6 | ❌ | Connected to Flash memory |
| D7 | 7 | ❌ | Connected to Flash memory |
| D8 | 8 | ❌ | Connected to Flash memory |
| D9 | 9 | ❌ | Connected to Flash memory |
| D10 | 10 | ❌ | Connected to Flash memory |
| D11 | 11 | ❌ | Connected to Flash memory |
| D12 | 12 | ⚠️ eingeschränkt | Must be **LOW during boot** |
| D13 | 13 | ✅ | — |
| D14 | 14 | ✅ | — |
| D15 | 15 | ⚠️ eingeschränkt | Must be **HIGH during boot**, LOW disables startup log |
| RX2 | 16 | ✅ | — |
| TX2 | 17 | ✅ | — |
| D18 | 18 | ✅ | — |
| D19 | 19 | ✅ | — |
| D21 | 21 | ✅ | — |
| D22 | 22 | ✅ | — |
| D23 | 23 | ✅ | — |
| D25 | 25 | ✅ | — |
| D26 | 26 | ✅ | — |
| D27 | 27 | ✅ | — |
| D32 | 32 | ✅ | — |
| D33 | 33 | ✅ | — |
| D34 | 34 | ❌ | Input-only GPIO |
| D35 | 35 | ❌ | Input-only GPIO |
| VP | 36 | ❌ | Input-only GPIO |
| VN | 39 | ❌ | Input-only GPIO |
