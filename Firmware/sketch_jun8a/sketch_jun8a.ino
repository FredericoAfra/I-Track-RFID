// Definição dos pinos de controle do motor M1
const int pinoM1_A = 5;
const int pinoM1_B = 6;

void setup() {
  // Configura os pinos do motor como saídas
  pinMode(pinoM1_A, OUTPUT);
  pinMode(pinoM1_B, OUTPUT);
}

void loop() {
  // --- Girar para frente (Velocidade máxima: 255) ---
  analogWrite(pinoM1_A, 255);
  analogWrite(pinoM1_B, 0);
}