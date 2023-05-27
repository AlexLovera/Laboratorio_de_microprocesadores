#include "SevSeg.h"

SevSeg sevseg;

const double VALOR_ESCALA_CONVERSION_ADC_VOLT = (50.0/4095.0);
const int ledPin = 33;
const int ledPin2 = 32;
const int numSamples = 5; // Número de muestras para el promedio móvil
float samples[numSamples] = {0};
bool segundoLedEncendido = false;

//PULSADOR
const int buttonPin = 35;

//BUZZER Define el pin del zumbador y el pin de la alarma
int buzzerPin = 22;
// Define el estado de la alarma, TODAVIA NO HAY LOGICA PARA ESTO CASI
bool alarmOn = false;


//ALARMAS
// Definir las variables para los valores de tensión
float limiteInferiorTensiones = 36.0; // lo naranja
float limiteSuperiorTensiones = 44.0;
float minimoTensionNormal = 38.0; // inicio valor normal
float maximoTensionNormal = 42.0;
float tensionAlarmaLocalMinima = 36.0; // parte roja, debe contener a la normal...
float tensionAlarmaLocalMaxima = 44.0;

// Leer la tensión instantánea del sensor... funcion a crear
//float tensionReal = leertensionReal();


void IRAM_ATTR apagarLedConBotonInterrupcion(){
    // se asume que el debouncing es aplicado por hardware.
    Serial.print("Boton presionado ");
    digitalWrite(ledPin2, LOW);
}

void setup() {
  byte numDigits = 4;
  byte digitPins[] = {14, 15, 2, 5};
  byte segmentPins[] = {12, 4, 19, 26, 27, 13, 18, 25};
  byte hardwareConfig = COMMON_ANODE;

  bool resistorsOnSegments = false;
  bool updateWithDelays = false;
  bool leadingZeros = false;
  bool disableDecPoint = false;
  
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);

  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins);

  pinMode(34, INPUT);
  Serial.begin(115200);

  //PULSADOR
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(buttonPin, apagarLedConBotonInterrupcion, FALLING);

  //BUZZER Inicializa los pines
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  static unsigned long tiempoEjec = millis() + 1000;
  static unsigned long tiempoPromedio = millis() + 5000; // Tiempo para calcular el promedio móvil
  static int contador = 0;
  int tensionRealEnteraMultiplicada = 0;
  float promedio = 0;

  bool tensionEstaEnValoresNormales;
  bool tensionEstaEnValoresAlarmaLocal; // nombre para boolean...
  bool tensionEstaEnValoresMinimos; // para limite inferior
  bool tensionEstaEnValoresMaximos;

  if(millis() >= tiempoEjec){
    tiempoEjec += 1000;
    
    float value = analogRead(34);
    float tensionReal = value * VALOR_ESCALA_CONVERSION_ADC_VOLT;
    Serial.println(tensionReal,6);
    
    tensionRealEnteraMultiplicada = (int) (tensionReal * 100);

    tensionEstaEnValoresNormales = (tensionReal >= minimoTensionNormal) && (tensionReal <= maximoTensionNormal);

    tensionEstaEnValoresAlarmaLocal = (tensionReal >= tensionAlarmaLocalMinima) && (tensionReal < tensionAlarmaLocalMaxima);

    tensionEstaEnValoresMinimos = (tensionReal < limiteInferiorTensiones);

    tensionEstaEnValoresMaximos = (tensionReal > limiteSuperiorTensiones);
    
    // Presentar la medición en el display    
    //mostrarTension(tensionReal);
    sevseg.setNumber(tensionRealEnteraMultiplicada, 2);

    if(tensionEstaEnValoresNormales){
      
      Serial.println("Tension en valor normal");
      // VERIFICAR BOOL PARA VER SI VOLVIO A TENSION NORMAL y prender el segundo led
      if( alarmOn == true){
        digitalWrite(ledPin2, HIGH);
        segundoLedEncendido = true;
        alarmOn = false;
        
        digitalWrite(ledPin, LOW);
      } 
    }

    // Si no esta en valores de tension normal pero si de alarma local, parte roja
    if(!tensionEstaEnValoresNormales && tensionEstaEnValoresAlarmaLocal){
      digitalWrite(ledPin, HIGH);
      alarmOn = true;

      Serial.println("Tension entre "+ String(tensionAlarmaLocalMinima)+ " y " + String(tensionAlarmaLocalMaxima));
    }

    if(tensionEstaEnValoresMaximos){
      Serial.println("APP: Tensión instantánea superior a la máxima permitida.");
    }

    if(tensionEstaEnValoresMinimos){
      Serial.println("APP: Tensión instantánea inferior a la mínima permitida.");
    }
    
  }
  sevseg.refreshDisplay();
}
