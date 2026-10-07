#include <Servo.h>

// ---------------------------------------------------------------------------
// Braço robótico - 4 servos controlados pela serial (protocolo "id:angulo\n")
//   0 = Base (D3)   1 = Ombro (D5)   2 = Cotovelo (D6)   3 = Garra (D9)
//
// ALIMENTAÇÃO: os fios vermelhos dos servos devem ir em uma fonte externa
// (4 pilhas AA ou fonte 5 V / 2 A), NÃO no pino 5V do Arduino.
// O negativo da fonte externa precisa estar ligado ao GND do Arduino.
// ---------------------------------------------------------------------------

const byte NUM_SERVOS = 4;
const byte GARRA = 3;

Servo servos[NUM_SERVOS];
const byte servoPins[NUM_SERVOS] = {3, 5, 6, 9};

// Limites de segurança de cada junta.
// Se mudar o limite da garra, mude também GARRA_FECHADA no index.html.
const int minAngles[NUM_SERVOS] = {0, 30, 30, 0};
const int maxAngles[NUM_SERVOS] = {180, 150, 150, 60};

// Posição em que cada servo está e posição para onde deve ir
int currentAngles[NUM_SERVOS] = {90, 90, 90, 10};
int targetAngles[NUM_SERVOS]  = {90, 90, 90, 10};

bool isAttached[NUM_SERVOS] = {false, false, false, false};

// Velocidade: 1 grau a cada STEP_MS. Aumente para deixar mais lento/suave.
const unsigned long STEP_MS = 15;
// Pausa depois de ligar um servo, para o pico de corrente passar
const unsigned long ATTACH_MS = 300;

unsigned long lastStep = 0;
unsigned long waitMs = STEP_MS;

// Buffer da linha recebida pela serial
char lineBuf[16];
byte lineLen = 0;
bool lineOverflow = false;


void ativarServo(byte id) {
    servos[id].attach(servoPins[id]);
    servos[id].write(currentAngles[id]);
    isAttached[id] = true;
    lastStep = millis();
    waitMs = ATTACH_MS;
}


// Anda 1 grau por vez, e apenas um servo de cada vez, para que dois motores
// nunca puxem corrente de partida ao mesmo tempo. Não bloqueia o loop().
void atualizarMovimento() {
    if (millis() - lastStep < waitMs) {
        return;
    }

    for (byte id = 0; id < NUM_SERVOS; id++) {
        if (currentAngles[id] == targetAngles[id]) {
            continue;
        }

        // Ombro e cotovelo só são ligados no primeiro comando
        if (!isAttached[id]) {
            ativarServo(id);
            return;
        }

        currentAngles[id] += (targetAngles[id] > currentAngles[id]) ? 1 : -1;
        servos[id].write(currentAngles[id]);

        lastStep = millis();
        waitMs = STEP_MS;
        return;
    }
}


// Interpreta uma linha "id:angulo". Linhas inválidas são ignoradas.
void processarLinha(char *line) {
    char *sep = strchr(line, ':');
    if (sep == NULL || sep == line || *(sep + 1) == '\0') {
        Serial.println(F("ERRO formato"));
        return;
    }

    char *end;
    long id = strtol(line, &end, 10);
    if (end != sep) {
        Serial.println(F("ERRO id"));
        return;
    }

    long angle = strtol(sep + 1, &end, 10);
    if (*end != '\0') {
        Serial.println(F("ERRO angulo"));
        return;
    }

    if (id < 0 || id >= NUM_SERVOS) {
        Serial.println(F("ERRO id"));
        return;
    }

    targetAngles[id] = constrain(angle, minAngles[id], maxAngles[id]);

    Serial.print(F("OK "));
    Serial.print(id);
    Serial.print(':');
    Serial.println(targetAngles[id]);
}


void lerSerial() {
    while (Serial.available() > 0) {
        char c = Serial.read();

        if (c == '\n' || c == '\r') {
            if (lineLen > 0 && !lineOverflow) {
                lineBuf[lineLen] = '\0';
                processarLinha(lineBuf);
            }
            lineLen = 0;
            lineOverflow = false;
        } else if (c == ' ') {
            // ignora espaços
        } else if (lineLen < sizeof(lineBuf) - 1) {
            lineBuf[lineLen++] = c;
        } else {
            lineOverflow = true;  // linha grande demais: descarta inteira
        }
    }
}


void setup() {
    Serial.begin(115200);

    // Só Base e Garra são ligadas aqui; Ombro e Cotovelo ligam sob demanda
    ativarServo(0);
    delay(ATTACH_MS);
    ativarServo(GARRA);
    delay(ATTACH_MS);

    Serial.println(F("PRONTO"));
}


void loop() {
    lerSerial();
    atualizarMovimento();
}
