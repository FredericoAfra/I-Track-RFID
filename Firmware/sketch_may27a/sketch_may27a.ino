#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <SPI.h>
#include <MFRC522.h>
#include "secrets.h"

// --- CONFIGURAÇÕES DO WI-FI ---
const char* WIFI_SSID = SECRET_SSID;
const char* WIFI_PASSWORD = SECRET_PASS;

// --- CONFIGURAÇÕES DO FIREBASE ---
const char* API_KEY = SECRET_API;
const char* DATABASE_URL = SECRET_URL;

// --- CONFIGURAÇÕES DE USUÁRIO DO FIREBASE ---
const char* USER_EMAIL = SECRET_EMAIL;
const char* USER_PASSWORD = SECRET_UPASS;

// --- PINOS RFID RC522 ---
#define SS_PIN  5
#define RST_PIN 21

// --- COMUNICAÇÃO SERIAL COM ARDUINO ---
#define RX2 16
#define TX2 17

const String PONTO_LEITURA = "armazem";

MFRC522 rfid(SS_PIN, RST_PIN);
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

String posicaoParaNome(int posicao) {
  if (posicao == 1) return "esteira 2";
  if (posicao == 2) return "esteira 1";
  if (posicao == 3) return "despache";
  return "desconhecido";
}

void setup() {
  Serial.begin(115200); // Serial Monitor do PC
  Serial2.begin(9600, SERIAL_8N1, RX2, TX2); // Serial para o Arduino Uno

  // Inicializa SPI e RFID
  SPI.begin();
  rfid.PCD_Init();

  // Conexão Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nConectado com sucesso!");

  // Configuração Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  
  // Autenticação de usuario
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.reconnectWiFi(true);
  Firebase.begin(&config, &auth);

  Serial.println("ESP32 Pronto.");
}

void loop() {
  // --- LEITURA E PROCESSAMENTO DO RFID ---
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String uidString = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      uidString += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
      uidString += String(rfid.uid.uidByte[i], HEX);
    }
    uidString.toUpperCase();
    
    Serial.println("\n----------------------------------");
    Serial.println("Tag RFID lida: " + uidString);

    // 2. DEFINIÇÃO DA POSIÇÃO PADRÃO
    // Se a tag não estiver cadastrada no Firebase, o motor adotará este valor:
    int posicaoServo = 3; // Altere para 1, 2 ou 3 para ser o seu padrão de fábrica
    
    // Monta o caminho dinâmico apontando direto para o valor da tag: /tags/VALOR_DA_TAG
    String caminhoTag = "/tags/" + uidString + "/servo";

    // VERIFICA SE O FIREBASE ESTÁ PRONTO E AUTENTICADO ANTES DE AGIR
    if (Firebase.ready()) {
      Serial.print("Enviando última tag lida para o Firebase... ");
      // 1. Envia a tag para o histórico
      if (Firebase.RTDB.setString(&fbdo, "/rfid/ultima_tag", uidString)) {
        Serial.println("OK");
      } else {
        Serial.println("Falha: " + fbdo.errorReason());
      }

      Serial.print("Buscando configuração no Firebase (" + caminhoTag + ")... ");

      // 3. Consulta o nó da tag correspondente
      if (Firebase.RTDB.getInt(&fbdo, caminhoTag)) {
        if (fbdo.dataType() == "int") {
          posicaoServo = fbdo.intData();
          Serial.println("Cadastrada! Posição encontrada: " + String(posicaoServo));
        } else {
          Serial.println("Nó encontrado, mas o dado não é um número. Mantendo padrão: " + String(posicaoServo));
        }
      } else {
        // Agora, se der erro, você saberá o motivo exato
        Serial.println("Não cadastrada ou erro de conexão. Motivo: " + fbdo.errorReason());
        Serial.println("Aplicando posição padrão: " + String(posicaoServo));
      }

      // Registro da leitura
      Serial.print("Enviando registro de leitura para o Firebase... ");
      FirebaseJson jsonLeitura;
      jsonLeitura.set("tag_id", uidString);
      jsonLeitura.set("ponto", PONTO_LEITURA);
      jsonLeitura.set("destino", posicaoParaNome(posicaoServo));
      jsonLeitura.set("timestamp/.sv", "timestamp");
      if (Firebase.RTDB.pushJSON(&fbdo, "/scans", &jsonLeitura)) {
        Serial.println("OK");
      } else {
        Serial.println("Falha: " + fbdo.errorReason());
      }

      // Atualização do nó do asset (tags)
      Serial.print("Atualizando localizacao do asset no Firebase... ");
      FirebaseJson jsonUpdate;
      jsonUpdate.set("posicao", PONTO_LEITURA);
      jsonUpdate.set("ultima_leitura/.sv", "timestamp");
      if (Firebase.RTDB.updateNode(&fbdo, "/tags/" + uidString, &jsonUpdate)) {
        Serial.println("OK");
      } else {
        Serial.println("Falha: " + fbdo.errorReason());
      }
    } else {
      Serial.println("ERRO: Firebase não está pronto ou conexão SSL ainda está sendo estabelecida.");
      Serial.println("Aplicando posição padrão: " + String(posicaoServo));
    }

    // 4. VALIDAÇÃO E ENVIO DO COMANDO PARA O ARDUINO UNO
    if (posicaoServo >= 1 && posicaoServo <= 3) {
      char comando = '0' + posicaoServo; // Converte int (1, 2 ou 3) para char ('1', '2' ou '3')
      Serial2.print(comando); 
      Serial.print("Comando enviado ao Arduino: ");
      Serial.println(comando);
    } else {
      Serial.println("Aviso: Posição inválida ignorada (deve ser entre 1 e 3)");
    }
    
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1(); // Corrigido para letras maiúsculas
  }

  delay(200); // Pequeno intervalo para estabilidade do loop
}