#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>
#include <EEPROM.h>

SoftwareSerial mySerial(2, 3); // RX, TX
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

const int led1 = 7;
const int led2 = 8;
const int ledA = 9;
const int ledA2 = 10;
const int ledAM = 11;

const int fingerprintSlot = 1; // Slot de armazenamento para a digital

void setup()
{
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(ledA, OUTPUT);
  pinMode(ledA2, OUTPUT);
  pinMode(ledAM, OUTPUT);

  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(ledA, LOW);
  digitalWrite(ledA2, LOW);
  digitalWrite(ledAM, LOW);

  Serial.begin(9600);
  mySerial.begin(57600);

  if (finger.verifyPassword())
  {
    Serial.println("Sensor de biometria encontrado!");
  }
  else
  {
    Serial.println("Sensor de biometria não encontrado. Verifique a conexão!");
    while (1)
      ; // Aguarda indefinidamente
  }

  finger.getTemplateCount(); // Obtém o número total de digitais armazenadas no sensor
  Serial.print("Total de digitais registradas: ");
  Serial.println(finger.templateCount);

  finger.deleteModel(fingerprintSlot); // Apaga o modelo da digital no slot (caso exista)

  enrollFinger(); // Inicia o processo de registro de uma nova digital
}

void loop()
{
  // Não faz nada no loop principal
}

void enrollFinger()
{
  digitalWrite(led1, HIGH);

  Serial.println("Coloque o dedo no sensor...");

  // Aguarda o dedo ser colocado no sensor
  while (finger.getImage() != FINGERPRINT_OK)
    ;

  // Converte a imagem em características e a armazena no buffer 1
  if (finger.image2Tz(1) == FINGERPRINT_OK)
  {
    Serial.println("Imagem convertida com sucesso");
  }
  else
  {
    Serial.println("Erro ao capturar a imagem");
    return;
  }

  Serial.println("Remova o dedo do sensor...");

  // Aguarda o dedo ser removido do sensor
  while (finger.getImage() != FINGERPRINT_NOFINGER)
    ;

  Serial.println("Coloque o mesmo dedo novamente...");

  // Aguarda o dedo ser colocado novamente no sensor
  while (finger.getImage() != FINGERPRINT_OK)
    ;

  // Converte a imagem em características e a armazena no buffer 2
  if (finger.image2Tz(2) == FINGERPRINT_OK)
  {
    Serial.println("Imagem convertida com sucesso");
  }
  else
  {
    Serial.println("Erro ao capturar a imagem");
    return;
  }

  // Compara as características dos buffers 1 e 2 para verificar se são semelhantes
  if (finger.createModel() == FINGERPRINT_OK)
  {
    Serial.println("Modelo criado com sucesso");
  }
  else
  {
    Serial.println("As duas digitais não correspondem");
    return;
  }

  // Armazena o modelo da digital no sensor
  if (finger.storeModel(fingerprintSlot) == FINGERPRINT_OK)
  {
    Serial.println("Digital registrada com sucesso");

    // Salva as informações biométricas no armazenamento EEPROM do Arduino
    int address = 0;
    for (uint16_t i = 0; i < finger.templateCount; i++)
    {
      // Lê os dados do modelo da digital a partir do sensor
      uint8_t buffer[256];
      uint16_t length = finger.getModel();

      // Grava os dados no armazenamento EEPROM
      for (uint16_t j = 0; j < length; j++)
      {
        EEPROM.write(address, buffer[j]);
        address++;
      }
    }

    Serial.println("Informações biométricas salvas no armazenamento EEPROM");
  }
  else
  {
    Serial.println("Erro ao registrar a digital");
    return;
  }

  // Verifica se a digital registrada corresponde à digital atual
  Serial.println("Coloque o dedo para verificar...");
  while (finger.getImage() != FINGERPRINT_OK)
    ;

  if (finger.image2Tz(1) == FINGERPRINT_OK)
  {
    int result = finger.fingerFastSearch();
    if (result == FINGERPRINT_OK)
    {
      Serial.println("Digital reconhecida!");
      Serial.print("ID da digital: ");
      Serial.println(finger.fingerID);
    }
    else
    {
      Serial.println("Digital não reconhecida");
    }
  }
  else
  {
    Serial.println("Erro ao capturar a imagem");
    return;
  }
}