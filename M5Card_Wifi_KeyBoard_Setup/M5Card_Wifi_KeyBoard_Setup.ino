/**
 * @file M5Card_Wifi_KeyBoard_Setup.ino
 * @author Aurélio Avanzi
 * @brief https://github.com/cyberwisk/M5Card_Wifi_KeyBoard_Setup/tree/main/M5Card_Wifi_KeyBoard_Setup
 * @version Apha 0.3
 * @date 2024-01-30
 *
 * @Hardwares: M5Cardputer - https://docs.m5stack.com/en/core/Cardputer
 * @Dependent Librarys:
 * M5Cardputer: https://github.com/m5stack/M5Cardputer
 * WiFi: https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi
 * Preferences: https://github.com/m5stack/azure_iothub_arduino_lib_esp32/blob/master/hardware/espressif/esp32/libraries/Preferences/src/Preferences.h
 **/

#include <M5Cardputer.h>
#include <WiFi.h>
#include <Preferences.h>

#define NVS_SSID_KEY "wifi_ssid"
#define NVS_PASS_KEY "wifi_pass"

String CFG_WIFI_SSID;
String CFG_WIFI_PASS;

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

    Preferences preferences;
    preferences.begin("wifi_settings", false);
    CFG_WIFI_SSID = preferences.getString(NVS_SSID_KEY, "");
    CFG_WIFI_PASS = preferences.getString(NVS_PASS_KEY, "");
    preferences.end();

    WiFi.disconnect();
    WiFi.softAPdisconnect(true);
    WiFi.begin(CFG_WIFI_SSID.c_str(), CFG_WIFI_PASS.c_str());

    int tm = 0;
    while (tm++ < 60 && WiFi.status() != WL_CONNECTED) {
        M5Cardputer.Display.print(".");
        delay(100);
      if (M5Cardputer.Keyboard.isKeyPressed('´')) {
        break;
      }
    }
    if (WiFi.status() == WL_CONNECTED) {
    M5Cardputer.Display.clear();
    M5Cardputer.Display.setCursor(0, 0);
    M5Cardputer.Display.println("WiFi conectada.");
    M5Cardputer.Display.println("SSID: " + WiFi.SSID());
    M5Cardputer.Display.println("IP: " + WiFi.localIP());
    M5Cardputer.Display.println("Power: " + WiFi.RSSI());
    M5Cardputer.Speaker.tone(800, 100);
    delay(200);
    } else {
        M5Cardputer.Display.clear();
        M5Cardputer.Display.drawString("Configuracao de WiFi", 1, 1);
        M5Cardputer.Display.drawString("Digite o SSID:", 1, 15);
        CFG_WIFI_SSID = inputText();
        M5Cardputer.Display.clear();
        M5Cardputer.Display.drawString("SSID: " + CFG_WIFI_SSID, 1, 15);
        M5Cardputer.Display.drawString("Digite a senha:", 1, 32);
        CFG_WIFI_PASS = inputText();

        Preferences preferences;
        preferences.begin("wifi_settings", false);
        preferences.putString(NVS_SSID_KEY, CFG_WIFI_SSID);
        preferences.putString(NVS_PASS_KEY, CFG_WIFI_PASS);
        preferences.end();

        M5Cardputer.Display.clear();
        M5Cardputer.Display.drawString("SSID: " + CFG_WIFI_SSID, 1, 15);
        M5Cardputer.Display.drawString("Senha: " + CFG_WIFI_PASS, 1, 32);
        M5Cardputer.Display.drawString("SSID e Senha gravados.", 1, 60);
        WiFi.begin(CFG_WIFI_SSID.c_str(), CFG_WIFI_PASS.c_str());

        M5Cardputer.Display.clear();
        M5Cardputer.Display.setCursor(1,1);
        M5Cardputer.Display.drawString("WiFi conectada.", 35,1);
        M5Cardputer.Display.drawString("SSID: " + WiFi.SSID(), 1,15);
        M5Cardputer.Display.drawString("IP: " + WiFi.localIP(), 1,32);
        M5Cardputer.Display.drawString("Power: " + WiFi.RSSI(), 1,60);
        M5Cardputer.Speaker.tone(6000, 100);
        delay(200);
    }
}

void loop() {
    // adicionar lógica aqui.
}
