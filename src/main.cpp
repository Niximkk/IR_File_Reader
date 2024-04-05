#if defined(STICK_C)
#include <M5StickC.h>
  #define SD_CLK_PIN 0
  #define SD_MISO_PIN 36
  #define SD_MOSI_PIN 26
  #define SD_CS_PIN 14
  #define DISP M5.Lcd
#endif

#if defined(STICK_C_PLUS)
  #include <M5StickCPlus.h>
  #define SD_CLK_PIN 0
  #define SD_MISO_PIN 36
  #define SD_MOSI_PIN 26
  #define SD_CS_PIN 14
  #define DISP M5.Lcd
#endif

#if defined(STICK_C_PLUS2)
  #include <M5StickCPlus2.h>
  #include <M5Unified.h>
  #define SD_CLK_PIN 0
  #define SD_MISO_PIN 36
  #define SD_MOSI_PIN 26
  #define SD_CS_PIN 14
  #define DISP M5.Lcd
#endif

#if defined(CARDPUTER)
  #include <M5Cardputer.h>
  #define SD_CLK_PIN 40
  #define SD_MISO_PIN 39
  #define SD_MOSI_PIN 14
  #define SD_CS_PIN 12
  #define DISP M5Cardputer.Display
#endif

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <vector>

struct IRSignal {
  String name;
  String type;
  int frequency;
  float dutyCycle;
  String data;
};

String fileName = "/arquivo.ir";

String logo = "\
                                           \n\
                  @    @                   \n\
                &@     ,@,                 IR File Reader\n\
               @@@      @@@                Made by ImNix\n\
              @@@        @@@               https://github.com/Niximkk\n\
             @@@@        @@@@              \n\
           &@@@@         .@@@@             \n\
          @@@@@@          @@@@@@           \n\
         @@@@@@(          @@@@@@@          \n\
          @@@@@@@@@    @@@@@@@@@           \n\
             @@@   @@@@@@@@@@              \n\
               @@@@@@@@@@                  \n\
            @@@@@@@@@@  @@@@@@             \n\
        @@@@@@@@@@     *@@@@@@@@@&         \n\
            @@@@@@@@&@@@@@@@@@             \n\
                @@@@@@@@@@                 \n\
                   @@@@                    \n\
                                           \n";

SPIClass SPI2_SD;
File file;
std::vector<IRSignal> irSignals;

int codesFound = 0;

void readFile() {
  if (!SD.begin(SD_CS_PIN, SPI2_SD)) {
    Serial.println("Failed to start SD card");
    return;
  }

  file = SD.open(fileName);
  if (!file) {
    Serial.println("Failed to open .ir file");
    return;
  }

  IRSignal signal;

  while (file.available()) {
    String line = file.readStringUntil('\n');

    if (line.startsWith("#")) {
      continue;
    }

    if (line.startsWith("name:")) {
      if (!signal.name.isEmpty()) {
        irSignals.push_back(signal);
        signal = IRSignal();
        codesFound++;
      }
      signal.name = line.substring(6);
    } else if (line.startsWith("type:")) {
      signal.type = line.substring(6);
    } else if (line.startsWith("frequency:")) {
      signal.frequency = line.substring(10).toInt();
    } else if (line.startsWith("duty_cycle:")) {
      signal.dutyCycle = line.substring(12).toFloat();
    } else if (line.startsWith("data:")) {
      signal.data = line.substring(6);
    }
  }

  irSignals.push_back(signal);

  Serial.printf("[%d] Codes were found. @ %s\n\n", codesFound, fileName.c_str());

  for (const auto& irSignal : irSignals) {
    Serial.println("Name: " + irSignal.name);
    Serial.println("Type: " + irSignal.type);
    Serial.println("Frequency: " + String(irSignal.frequency));
    Serial.println("Duty Cycle: " + String(irSignal.dutyCycle));
    Serial.println("Data: " + irSignal.data);
  }
}

void setup() {
  M5.begin();
  Serial.begin(115200);
  SPI2_SD.begin(SD_CLK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);
  Serial.print(logo);
  readFile();
  DISP.setRotation(1);
  DISP.fillScreen(BLACK);
  DISP.setTextColor(WHITE,BLACK);
  DISP.setTextSize(2);
  DISP.setCursor(2, 2);
  DISP.printf("[%d] Codes found.", codesFound);
}

void loop() {
  M5.update();
}