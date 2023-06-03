TaskHandle_t taskHandle;

void secondCoreTask(void *parameter) {
  // Código para inicializar y ejecutar el servidor asíncrono con WebSockets

  // Bucle de ejecución del servidor
  while (true) {
    // Manejar conexiones, eventos, etc.
    server.handleClient();
    // Otras tareas relacionadas con el servidor
  }
}

void setup() {
  // Inicializar el ESP32 y otras configuraciones

  // Crear una tarea en el segundo núcleo
  xTaskCreatePinnedToCore(secondCoreTask, "SecondCoreTask", 8192, NULL, 1, &taskHandle, 1);
}

void loop() {
  // Código para el bucle principal del primer núcleo
}

