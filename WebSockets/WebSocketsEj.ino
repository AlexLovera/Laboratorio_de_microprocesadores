#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncWebSocket.h>

const char* ssid = "Tu-SSID";
const char* password = "Tu-contraseña";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Variables de límite de alarma
float limiteInferiorTensiones = 36.0;
float limiteSuperiorTensiones = 44.0;

// Función para enviar actualizaciones de tensión a través de WebSockets
void sendVoltageUpdate(float voltage) {
  String json = "{\"voltage\": " + String(voltage) + "}";
  ws.textAll(json);
}

// Función para procesar mensajes recibidos a través de WebSockets
void onWebSocketMessage(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    // Enviar límites de alarma al cliente al conectar
    String json = "{\"lowerLimit\": " + String(limiteInferiorTensiones) + ", \"upperLimit\": " + String(limiteSuperiorTensiones) + "}";
    client->text(json);
  } else if (type == WS_EVT_DATA) {
    // Procesar mensajes de modificación de límites de alarma
    String message = String((char*)data);
    // Analizar el mensaje JSON recibido
    // Aquí debes implementar la lógica para actualizar los límites de alarma en base al mensaje recibido
    // Ejemplo: {"lowerLimit": 35.5, "upperLimit": 45.0}

    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->opcode == WS_TEXT) {
      // Convertir los datos recibidos a un objeto JSON
      DynamicJsonDocument doc(1024); // Tamaño del documento JSON
      DeserializationError error = deserializeJson(doc, data, len);

      // Verificar si hay un error en la deserialización
      if (error) {
        Serial.println("Error al analizar JSON");
        return;
      }

      // Obtener los valores del objeto JSON
      float newLowerLimit = doc["lowerLimit"];
      float newUpperLimit = doc["upperLimit"];

      // Hacer algo con los valores, por ejemplo, actualizar los límites de alarma
      // updateAlarmLimits(newLowerLimit, newUpperLimit);
  }
}

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Configurar rutas para manejar solicitudes GET y POST
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/update", HTTP_POST, [](AsyncWebServerRequest *request){
    // Procesar los valores enviados desde el formulario y actualizar los límites de alarma
    limiteInferiorTensiones = request->arg("lowerLimit").toFloat();
    limiteSuperiorTensiones = request->arg("upperLimit").toFloat();
    request->send(200);
  });

  // Configurar WebSocket
  ws.onEvent(onWebSocketMessage);
  server.addHandler(&ws);

  // Iniciar el servidor web
  server.begin();
}

void loop() {
  // Simulación de la lectura de tensión
  float voltage = random(360, 440) / 10.0;
  sendVoltageUpdate(voltage);
  
  delay(500); // Intervalo de tiempo para enviar actualizaciones de tensión
}
