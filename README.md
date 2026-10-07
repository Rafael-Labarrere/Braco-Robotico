
Código alternatico:


ARDUINO:

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

Index.html:
<!DOCTYPE html>
<html lang="pt-BR">
<head>
  <meta charset="UTF-8">
  <title>Controle do Braço Robótico</title>
  <style>
    body { font-family: Arial, sans-serif; max-width: 500px; margin: 40px auto; padding: 20px; background: #f4f6f8; }
    h1 { text-align: center; color: #2c3e50; font-size: 22px; }
    .control { margin: 15px 0; background: #ffffff; padding: 15px; border-radius: 8px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }
    label { font-weight: bold; display: block; margin-bottom: 8px; color: #333; }
    input[type=range] { width: 100%; cursor: pointer; }
    button { display: block; width: 100%; padding: 12px; font-size: 16px; font-weight: bold; background: #28a745; color: white; border: none; border-radius: 6px; cursor: pointer; }
    button:hover { background: #218838; }
  </style>
</head>
<body>

  <h1>Controle do Braço Robótico</h1>
  <button id="btnConnect">Conectar ao Arduino</button>

  <div class="control">
    <label>Servo 0 (Pino 3): <span id="val0">90</span>°</label>
    <input type="range" id="servo0" min="0" max="180" value="90">
  </div>

  <div class="control">
    <label>Servo 1 (Pino 5): <span id="val1">90</span>°</label>
    <input type="range" id="servo1" min="0" max="180" value="90">
  </div>

  <div class="control">
    <label>Servo 2 (Pino 6): <span id="val2">90</span>°</label>
    <input type="range" id="servo2" min="0" max="180" value="90">
  </div>

  <div class="control">
    <label>Servo 3 (Pino 9): <span id="val3">90</span>°</label>
    <input type="range" id="servo3" min="0" max="180" value="90">
  </div>

  <script>
    let port;
    let writer;

    document.getElementById('btnConnect').addEventListener('click', async () => {
      try {
        port = await navigator.serial.requestPort();
        // Velocidade configurada para 115200 (igual ao Serial.begin do Arduino)
        await port.open({ baudRate: 115200 });
        const textEncoder = new TextEncoderStream();
        textEncoder.readable.pipeTo(port.writable);
        writer = textEncoder.writable.getWriter();
        alert('Arduino conectado com sucesso!');
      } catch (err) {
        alert('Erro ao conectar com o Arduino: ' + err);
      }
    });

    async function sendServoAngle(id, angle) {
      if (!writer) return;
      document.getElementById(`val${id}`).textContent = angle;
      // Envia no formato "ID:ÂNGULO\n" exigido pelo seu código Arduino
      await writer.write(`${id}:${angle}\n`);
    }

    [0, 1, 2, 3].forEach(id => {
      const slider = document.getElementById(`servo${id}`);
      slider.addEventListener('input', (e) => {
        sendServoAngle(id, e.target.value);
      });
    });
  </script>
</body>
</html>