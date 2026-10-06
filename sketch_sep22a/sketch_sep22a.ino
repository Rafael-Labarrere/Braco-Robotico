#include <Servo.h>

// Declaração do array de 4 servos e pinos
Servo servos[4];

const int servoPins[4] = {3, 5, 6, 9};

// Controle de ativação sob demanda
bool isAttached[4] = {false, false, false, false};

// Posições iniciais seguras
int currentAngles[4] = {90, 90, 90, 10};

// Limites de segurança (Min e Max) de cada junta
const int minAngles[4] = {0, 30, 30, 0};
const int maxAngles[4] = {180, 150, 150, 60};


void setup() {
    Serial.begin(115200);

    // Inicializa apenas a Base (0) e a Garra (3) no setup

    servos[0].attach(servoPins[0]);
    servos[0].write(currentAngles[0]);
    isAttached[0] = true;

    delay(200);

    servos[3].attach(servoPins[3]);
    servos[3].write(currentAngles[3]);
    isAttached[3] = true;

    delay(200);
}


void moveSuave(int id, int targetAngle) {

    if (id < 0 || id >= 4) {
        return;
    }

    targetAngle = constrain(
        targetAngle,
        minAngles[id],
        maxAngles[id]
    );

    // Ativa o motor (Ombro ou Cotovelo)
    // apenas na primeira vez que for acionado
    if (!isAttached[id]) {

        servos[id].attach(servoPins[id]);
        isAttached[id] = true;

        servos[id].write(targetAngle);
        currentAngles[id] = targetAngle;

        return;
    }

    // Movimento gradual para evitar picos de corrente
    int current = currentAngles[id];

    if (current < targetAngle) {

        for (int pos = current; pos <= targetAngle; pos++) {
            servos[id].write(pos);
            delay(12);
        }

    } else if (current > targetAngle) {

        for (int pos = current; pos >= targetAngle; pos--) {
            servos[id].write(pos);
            delay(12);
        }
    }

    currentAngles[id] = targetAngle;
}


void loop() {

    if (Serial.available() > 0) {

        String data = Serial.readStringUntil('\n');
        data.trim();

        int delimiterIdx = data.indexOf(':');

        if (delimiterIdx > 0) {

            int id = data.substring(0, delimiterIdx).toInt();
            int angle = data.substring(delimiterIdx + 1).toInt();

            if (id >= 0 && id < 4) {
                moveSuave(id, angle);
            }
        }
    }
}