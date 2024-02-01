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

String inputText(const String& prompt, int x, int y) {
    String data = "> ";

    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.setTextScroll(true);
    M5Cardputer.Display.drawString(prompt, x, y);

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

                M5Cardputer.Display.fillRect(0, y - 4, M5Cardputer.Display.width(), 25, BLACK);
                M5Cardputer.Display.drawString(data, 4, y);
            }
        }

        delay(20);
    }
}

void displayWiFiInfo() {
    M5Cardputer.Display.clear();
    M5Cardputer.Display.setCursor(1, 1);
    M5Cardputer.Display.drawString("WiFi conectada.", 35, 1);
    M5Cardputer.Display.drawString("SSID: " + WiFi.SSID(), 1, 15);
    M5Cardputer.Display.drawString("IP: " + WiFi.localIP().toString(), 1, 30);
    int8_t rssi = WiFi.RSSI();
    M5Cardputer.Display.drawString("RSSI: " + String(rssi) + " dBm", 1, 45);
}

void connectToWiFi() {
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
        displayWiFiInfo();
    } else {      
        M5Cardputer.Display.clear();
        M5Cardputer.Display.drawString("Configuracao de WiFi", 1, 1);
        M5Cardputer.Display.drawString("Digite o SSID:", 1, 15);
        CFG_WIFI_SSID = inputText("> ", 4, M5Cardputer.Display.height() - 24);
        M5Cardputer.Display.clear();
        M5Cardputer.Display.drawString("SSID: " + CFG_WIFI_SSID, 1, 15);
        M5Cardputer.Display.drawString("Digite a senha:", 1, 32);
        CFG_WIFI_PASS = inputText("> ", 4, M5Cardputer.Display.height() - 24);

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
        displayWiFiInfo();
    }
}

void scanAndDisplayNetworks() {
    int numNetworks = WiFi.scanNetworks();

    if (numNetworks == 0) {
        M5Cardputer.Display.drawString("Nenhuma rede encontrada.", 1, 15);
    } else {
        M5Cardputer.Display.drawString("Redes disponíveis:", 1, 1);

        for (int i = 0; i < numNetworks; ++i) {
            String ssid = WiFi.SSID(i);
            M5Cardputer.Display.drawString(ssid, 1, 15 + i * 15);
        }

        M5Cardputer.Display.drawString("Selecione uma rede.", 1, 15 + numNetworks * 15);

        while (1) {
            M5Cardputer.update();

            if (M5Cardputer.Keyboard.isChange()) {
                if (M5Cardputer.Keyboard.isPressed()) {
                    Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

                    if (status.enter) {
                        return;
                    }
                }
            }
            delay(20);
        }
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

    M5Cardputer.Display.drawString("Pressione 'S' para escanear", 1, 1);

    while (1) {
        M5Cardputer.update();

        if (M5Cardputer.Keyboard.isChange()) {
            if (M5Cardputer.Keyboard.isPressed()) {
                Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

                if (M5Cardputer.Keyboard.isKeyPressed('s')) {
                    scanAndDisplayNetworks();
                }

                break;
            }
        }

        delay(20);
    }

    connectToWiFi();
}



/*
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

    connectToWiFi();
}
*/
void loop() {
    // adicionar lógica aqui.
}
