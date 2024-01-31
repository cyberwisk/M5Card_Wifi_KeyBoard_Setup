/**
 * @file M5Cardputer_WebRadio.ino
 * @author Aurélio Avanzi
 * @brief M5Cardputer WebRadio
 * @version Apha 0.3
 * @date 2024-01-30
 *
 * @Hardwares: M5Cardputer - https://docs.m5stack.com/en/core/Cardputer
 * @Platform Version: Arduino M5Stack Board Manager
 * @Dependent Librarys:
 * M5GFX: https://github.com/m5stack/M5GFX
 * EEPROM: https://github.com/m5stack/m5-docs/blob/master/docs/en/api/eeprom.md
 **/

#include <M5Cardputer.h>
#include <WiFi.h>
#include <EEPROM.h>

// Define os endereços de memória EEPROM para SSID e Senha
#define SSID_ADDR 0
#define PASS_ADDR 32

String CFG_WIFI_SSID;
String CFG_WIFI_PASS;

void writeString(int addr, const String& data) {
    for (size_t i = 0; i < data.length(); i++) {
        EEPROM.write(addr + i, data[i]);
    }
    EEPROM.commit();
}

void readString(int addr, String& data, size_t maxLength) {
    data = "";  // Limpa a string antes de ler

    for (size_t i = 0; i < maxLength; i++) {
        char charRead = EEPROM.read(addr + i);
        if (charRead == '\0') {
            break;  // Encerra se encontrar o caractere nulo
        }
        data += charRead;
    }
    
    data.trim();  // Remove qualquer espaço em branco no final da string
}


String inputText() {
    String data = "> ";

    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.setTextScroll(true);
    M5Cardputer.Display.drawString(data, 4, M5Cardputer.Display.height() - 24);

    while (1) {
        M5Cardputer.update();

        if (M5Cardputer.Keyboard.isChange()) {
            if (M5Cardputer.Keyboard.isPressed()) {
                Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

                for (auto i : status.word) {
                    data += i;
                }

                if (status.del) {
                    data.remove(data.length() - 1);
                }

                if (status.enter) {
                    data.remove(0, 2);
                    M5Cardputer.Display.println(data);
                    return data;
                    
                }

                M5Cardputer.Display.fillRect(0, M5Cardputer.Display.height() - 28, M5Cardputer.Display.width(), 25, BLACK);
                M5Cardputer.Display.drawString(data, 4, M5Cardputer.Display.height() - 24);
            }
        }

        delay(20);
    }
}

void setup() {
    auto cfg = M5.config();
    M5Cardputer.begin(cfg, true);
    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.setTextSize(1.6);

    CFG_WIFI_SSID = "";
    CFG_WIFI_PASS = "";

    // Lê os valores da EEPROM e inicializa as variáveis
    readString(SSID_ADDR, CFG_WIFI_SSID,32);
    readString(PASS_ADDR, CFG_WIFI_PASS,10);

  // Se desejar, pode exibir os valores na tela
  //M5Cardputer.Display.setTextColor(PURPLE);
  WiFi.disconnect();
  WiFi.softAPdisconnect(true);

  M5Cardputer.Display.drawString("Conectando no Wifi.", 10, 1);
  M5Cardputer.Display.drawString("SSID: " + CFG_WIFI_SSID, 10, 15);
  M5Cardputer.Display.drawString("Senha: " + CFG_WIFI_PASS, 10, 30);
  WiFi.begin(CFG_WIFI_SSID, CFG_WIFI_PASS);

  // try forever but not too much
  int tm = 0;
  while (WiFi.status() != WL_CONNECTED) {
    M5Cardputer.Display.print(".");
    delay(100);
    if (M5Cardputer.Keyboard.isKeyPressed('m') || tm++ <30) {
      break;
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
  M5Cardputer.Display.println("");
  M5Cardputer.Display.println("WiFi conectada.");
  M5Cardputer.Display.println(WiFi.SSID());
  M5Cardputer.Display.println("Endereço de IP: ");
  M5Cardputer.Display.println(WiFi.localIP());
  M5Cardputer.Display.println("Potencia: ");
  M5Cardputer.Display.println(WiFi.RSSI());
  M5Cardputer.Speaker.tone(1000, 100);
  delay(2000);
  } else { 
    M5Cardputer.Display.clear();
    M5Cardputer.Display.drawString("Configuracao de WiFi",1,1);
    M5Cardputer.Display.setTextColor(GREEN);
    M5Cardputer.Display.drawString("Digite o SSID:", 1, 15);
    CFG_WIFI_SSID = inputText();
    M5Cardputer.Display.clear();
    M5Cardputer.Display.drawString("SSID: " + CFG_WIFI_SSID, 1, 15);
    M5Cardputer.Display.setTextColor(YELLOW);
    M5Cardputer.Display.drawString("Digite a senha:", 1, 32);
    CFG_WIFI_PASS = inputText();

    // Grava os valores na EEPROM
    writeString(SSID_ADDR, CFG_WIFI_SSID);
    writeString(PASS_ADDR, CFG_WIFI_PASS);
    M5Cardputer.Display.clear();
    M5Cardputer.Display.drawString("SSID: " + CFG_WIFI_SSID, 1, 15);
    M5Cardputer.Display.drawString("Senha: " + CFG_WIFI_PASS, 1, 32);
    M5Cardputer.Display.drawString("SSID e Senha gravados.", 1, 60);
    WiFi.begin(CFG_WIFI_SSID, CFG_WIFI_PASS);
      while (WiFi.status() != WL_CONNECTED) {
      M5Cardputer.Display.print(".");
      delay(100);
        if (M5Cardputer.Keyboard.isKeyPressed('m') || tm++ <30) {
          break;
        }
      }
  }
}

void loop() {
    //adicionar lógica aqui.
}
