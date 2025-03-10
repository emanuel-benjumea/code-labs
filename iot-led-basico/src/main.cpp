#include <Arduino.h>
#include <WiFi.h>

const char *ssid = "virus5";
const char *password = "a1b2c3d4";
WiFiServer server(80);

// Función para conectar a la red WiFi
void conectarRedWifi() {
    WiFi.begin(ssid, password);
    Serial.print("Estableciendo conexion WiFi..");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nConectado a la red WiFi");
    Serial.print("IP local: ");
    Serial.println(WiFi.localIP());
    server.begin();
}

// Función que devuelve el tipo de cifrado de la red WiFi
String getTipoCifrado(wifi_auth_mode_t tipoCifrado) {
    switch (tipoCifrado) {
        case WIFI_AUTH_OPEN: return "Abierta";
        case WIFI_AUTH_WEP: return "WEP";
        case WIFI_AUTH_WPA_PSK: return "WPA_PSK";
        case WIFI_AUTH_WPA2_PSK: return "WPA2_PSK";
        case WIFI_AUTH_WPA_WPA2_PSK: return "WPA_WPA2_PSK";
        case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2_ENTERPRISE";
        default: return "Desconocido";
    }
}

// Función para escanear redes WiFi
void escanearRedes() {
    int numberOfNetworks = WiFi.scanNetworks();
    Serial.print("Numero de redes encontradas: ");
    Serial.println(numberOfNetworks);
    for (int i = 0; i < numberOfNetworks; i++) {
        Serial.print("Nombre de red: ");
        Serial.println(WiFi.SSID(i));
        Serial.print("Fuerza de la señal: ");
        Serial.println(WiFi.RSSI(i));
        Serial.print("Direccion MAC: ");
        Serial.println(WiFi.BSSIDstr(i));
        Serial.print("Tipo de cifrado: ");
        Serial.println(getTipoCifrado(WiFi.encryptionType(i)));
        Serial.println("-----------------------");
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(2, OUTPUT);
    escanearRedes();
    conectarRedWifi();
}

void loop() {
    WiFiClient cliente = server.available();
    if (cliente) {
        String mensaje = "";
        Serial.println("Llego un nuevo cliente");
        while (cliente.connected()) {
            if (cliente.available()) {
                char letra = cliente.read();
                Serial.write(letra);
                if (letra == '\n') {  // Detecta un salto de línea
                    if (mensaje.length() == 0) {
                        cliente.println("HTTP/1.1 200 OK");
                        cliente.println("Content-type:text/html");
                        cliente.println();
                        cliente.println("<br>Clic <a href=\"H\">aqui</a> para encender la lampara<br>");
                        cliente.println("Clic <a href=\"L\">aqui</a> para apagar la lampara");
                        cliente.println();
                        break;
                    } else {
                        mensaje = "";
                    }
                } else if (letra != '\r') {
                    mensaje += letra;
                }
                if (mensaje.endsWith("GET /H")) {
                    digitalWrite(2, HIGH);
                }
                if (mensaje.endsWith("GET /L")) {
                    digitalWrite(2, LOW);
                }
            }
        }
        Serial.println("Fin de conexion");
        cliente.stop();
    }
}