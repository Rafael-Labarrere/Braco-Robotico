
Código alternatico:

#include <Servo.h>

Servo servos[4];

// Pinos digitais PWM para os servos
// Ajuste conforme suas conexões
const int servoPins[4] = {3, 5, 6, 9};

void setup() {
    Serial.begin(115200);

    for (int i = 0; i < 4; i++) {
        servos[i].attach(servoPins[i]);
        servos[i].write(90); // Posição neutra inicial
    }
}

void loop() {
    if (Serial.available() > 0) {
        // Lê linha terminada em '\n'
        String data = Serial.readStringUntil('\n');
        data.trim();

        int delimiterIdx = data.indexOf(':');

        if (delimiterIdx > 0) {
            int id = data.substring(0, delimiterIdx).toInt();
            int angle = data.substring(delimiterIdx + 1).toInt();

            if (id >= 0 && id < 4) {
                angle = constrain(angle, 0, 180);
                servos[id].write(angle);
            }
        }
    }
}