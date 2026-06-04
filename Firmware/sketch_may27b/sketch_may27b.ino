#include <Servo.h>

// --- PINOS DOS SERVOS ---
const int pinoServo1 = 9;
const int pinoServo2 = 10;

Servo servo1;
Servo servo2;

// --- AJUSTE DE POSIÇÕES (SERVO 1) ---
// Modifique os ângulos (0 a 180) de acordo com sua necessidade
const int anguloPosicao1 = 10;
const int anguloPosicao2 = 90;
const int anguloPosicao3 = 170;

// --- CONFIGURAÇÃO DE MOVIMENTO (SERVO 2) ---
const int anguloMinServo2 = 30;   // Posição A do movimento repetitivo
const int anguloMaxServo2 = 150;  // Posição B do movimento repetitivo
int anguloAtualServo2 = anguloMinServo2;
int direcaoServo2 = 1;            // 1 para somar, -1 para subtrair
unsigned long ultimoTempoServo2 = 0;
const int velocidadeServo2 = 15;  // Tempo em milissegundos para cada passo (menor = mais rápido)

void setup() {
  // Inicializa a comunicação serial nas portas padrões do Uno (Pinos 0 RX e 1 TX)
  // Lembre-se: Desconecte o ESP32 do pino 0 quando for descarregar o código no Uno!
  Serial.begin(9600);

  servo1.attach(pinoServo1);
  servo2.attach(pinoServo2);

  // Inicializa os servos nas posições iniciais
  servo1.write(anguloPosicao1);
  servo2.write(anguloAtualServo2);
}

void loop() {
  // --- TAREFA 1: LER MENSAGEM DO ESP32 (SERVO 1) ---
  if (Serial.available() > 0) {
    char comando = Serial.read();

    switch (comando) {
      case '1':
        servo1.write(anguloPosicao1);
        break;
      case '2':
        servo1.write(anguloPosicao2);
        break;
      case '3':
        servo1.write(anguloPosicao3);
        break;
      default:
        // Ignora qualquer outro caractere residual (\n, \r, etc)
        break;
    }
  }

  // --- TAREFA 2: MOVIMENTO CONSTANTE E REPETITIVO (SERVO 2) ---
  // Usa millis() para o Servo 2 se mover sem travar a leitura do Serial do Servo 1
  if (millis() - ultimoTempoServo2 >= velocidadeServo2) {
    ultimoTempoServo2 = millis();

    anguloAtualServo2 += direcaoServo2;
    servo2.write(anguloAtualServo2);

    // Inverte o sentido ao atingir os limites estabelecidos
    if (anguloAtualServo2 <= anguloMinServo2 || anguloAtualServo2 >= anguloMaxServo2) {
      direcaoServo2 = -direcaoServo2; 
    }
  }
}