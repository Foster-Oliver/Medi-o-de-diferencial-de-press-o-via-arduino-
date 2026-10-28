#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_ADS1X15.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS 0x3C
#define ADS_ADDRESS 0x48

Adafruit_SSD1306 display(
  SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1
);

Adafruit_ADS1115 ads;

const float PRESSAO_MAX_KPA = 700.0;
const float TENSAO_MIN = 0.2;
const float TENSAO_MAX = 4.7;

const int NUM_AMOSTRAS = 32;

float lerTensaoMedia(int quantidade) {
  float soma = 0.0;

  for (int i = 0; i < quantidade; i++) {
    int16_t leitura = ads.readADC_SingleEnded(0);
    soma += ads.computeVolts(leitura);
    delay(2);
  }

  return soma / quantidade;
}

float converterPressao(float tensao) {
  float pressao =
    (tensao - TENSAO_MIN) *
    PRESSAO_MAX_KPA /
    (TENSAO_MAX - TENSAO_MIN);

  if (pressao < 0.0) {
    pressao = 0.0;
  }

  if (pressao > PRESSAO_MAX_KPA) {
    pressao = PRESSAO_MAX_KPA;
  }

  return pressao;
}

void setup() {
  Serial.begin(9600);
  Wire.begin();

  if (!display.begin(
        SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("Erro ao iniciar OLED");
    while (true) {}
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Iniciando...");
  display.display();

  if (!ads.begin(ADS_ADDRESS)) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Erro ADS1115");
    display.display();

    Serial.println("Erro ao iniciar ADS1115");
    while (true) {}
  }

  ads.setGain(GAIN_TWOTHIRDS);

  delay(1000);
}

void loop() {
  float tensao = lerTensaoMedia(NUM_AMOSTRAS);
  float pressaoKPa = converterPressao(tensao);

  // Converte kPa para Pa
  float pressaoPa = pressaoKPa * 1000.0;

  // Atualiza o display
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.setCursor(0, 0);
  display.println("TCC UNIP 2026");

  display.setCursor(0, 20);
  display.print("Tensao: ");
  display.print(tensao, 4);
  display.println(" V");

  display.setCursor(0, 40);
  display.print("Pressao: ");
  display.print(pressaoPa, 0);
  display.println(" Pa");

  display.display();

  // Monitor Serial
  Serial.print("Tensao: ");
  Serial.print(tensao, 4);
  Serial.print(" V | Pressao: ");
  Serial.print(pressaoPa, 0);
  Serial.println(" Pa");

  delay(100);
}