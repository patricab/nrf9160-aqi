#include <Arduino.h>

uint8_t rx = 0;
const char *str;

void setup()
{
  // Set to 8N1: 8 data bits, 1 stop bit, no parity
  Serial.begin(9600);

  pinMode(3, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(9, OUTPUT);
}

void loop()
{
    Serial.write('\r');
    Serial.println(Serial.readStringUntil('\n'));
  // Check if serial is available
  // if (Serial.available() > 0)
  // {
    // rx = 's';
    // delay(100);
    // rx = Serial.read();

    // switch (rx)
    // {
    //   case '\r': // 0x0D - Single measurment
    //     // str = "1112\n";
    //     // Serial.write(str);
    //     digitalWrite(9, HIGH);
    //     break;
    // case 's': // Standby low power mode
    //   str = "Standby\n";
    //   Serial.write(str);
    //   digitalWrite(7, HIGH);
    //   break;
    // case 'Z': // Zero user calibration
    //   str = "Setting zero...done\n";
    //   Serial.write(str);
    //   digitalWrite(3, HIGH);
    //   break;
    // case 'c':
    //   digitalWrite(3, LOW);
    //   digitalWrite(7, LOW);
    //   digitalWrite(9, LOW);
    //   break;
    // }
  // }
}