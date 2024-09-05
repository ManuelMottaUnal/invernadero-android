#include <WiFi.h>
#include <HTTPClient.h>
#include <DHTesp.h>
#include <VARSTEP_ultrasonic.h>
#include <ESP32Servo.h> 
#include <ArduinoJson.h>

const char *ssid = "Gibo";
const char *password = "aladar12";

#define FIREBASE_HOST "greenguard-e386d-default-rtdb.firebaseio.com"
#define FIREBASE_URL "/" 

String ruta = "Invernadero";
String ruta2 = "Puerta";
String ruta3 = "mojar1";
String ruta4 = "mojar2";
String ruta5 = "mojar3";
String ruta6 = "mojar4";

#define trigPin 2
#define echoPin 4
#define Led 5
#define motorPin1 12
#define motorPin2 14 
#define motorPin3 27
#define motorPin4 26 

const int humsuelo1 = 33; 
const int humsuelo2 = 32; 
const int humsuelo3 = 35; 

Servo myservo; 
const int servoPin = 13; 

float t = 0.0; 
float h = 0.0;
int u = 0; 
String p;
int r1 = 0;  
int r2 = 0;
int r3 = 0;
int r4 = 0;
int a = 0;
int valHumsuelo1, valHumsuelo2, valHumsuelo3;

VARSTEP_ultrasonic ultrasonic(trigPin, echoPin);
DHTesp dht;

void setup() {
  Serial.begin(115200);
  pinMode(Led, OUTPUT);
  pinMode(humsuelo1, INPUT);
  pinMode(humsuelo2, INPUT);
  pinMode(humsuelo3, INPUT);
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  
  dht.setup(15, DHTesp::DHT11);

  // Inicialización del servo
  myservo.attach(servoPin);

  // Configurar la conexión WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a la red WiFi");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi no conectado, reiniciando...");
    ESP.restart(); // Reiniciar el ESP32 si no está conectado
  }

  long distance = ultrasonic.distance_m();
  Serial.print("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");

  valHumsuelo1 = map(analogRead(humsuelo1), 0, 4095, 100, 0);
  valHumsuelo2 = map(analogRead(humsuelo2), 0, 4095, 100, 0);
  valHumsuelo3 = map(analogRead(humsuelo3), 0, 4095, 100, 0);

  Serial.print("Humedad del suelo 1: ");
  Serial.print(valHumsuelo1);
  Serial.println(" %");

  Serial.print("Humedad del suelo 2: ");
  Serial.print(valHumsuelo2);
  Serial.println(" %");

  Serial.print("Humedad del suelo 3: ");
  Serial.print(valHumsuelo3);
  Serial.println(" %");

  u = distance;

  h = dht.getHumidity();
  t = dht.getTemperature();
  t += 26; 

  Serial.print("Humedad: ");
  Serial.print(h);
  Serial.print(", Temperatura: ");
  Serial.println(t);

  // Configuración de Firebase usando HTTPClient
  HTTPClient http;
  String url = "https://" + String(FIREBASE_HOST) + FIREBASE_URL + ruta + ".json";
  http.begin(url); // URL de Firebase

  // Crea un objeto JSON para almacenar los datos a enviar a Firebase
  String data = "{\"Temperatura\":" + String(t) + ",";
  data += "\"Humedad\":" + String(h) + ",";
  data += "\"Agua\":" + String(u) + ",";
  data += "\"Humedad de piso 1\":" + String(valHumsuelo1) + ",";
  data += "\"Humedad de piso 2\":" + String(valHumsuelo2) + ",";
  data += "\"Humedad de piso 3\":" + String(valHumsuelo3) + "}";

  // Realiza una solicitud PUT para actualizar los datos en Firebase
  int httpCode = http.PUT(data);

  if (httpCode > 0) {
    Serial.printf("[HTTP] PUT... code: %d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] PUT... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();

  // Leer datos desde Firebase para la ruta2 (puerta)
  String readUrl2 = "https://" + String(FIREBASE_HOST) + FIREBASE_URL + ruta2 + ".json";
  http.begin(readUrl2); // URL de Firebase para lectura

  int readHttpCode2 = http.GET(); // Realiza una solicitud GET para leer datos

  if (readHttpCode2 > 0) {
    Serial.printf("[HTTP] GET... code: %d\n", readHttpCode2);
    if (readHttpCode2 == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println("Datos leídos desde Firebase:");
      Serial.println(payload);
      p = payload;
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(readHttpCode2).c_str());
  }

  http.end();

  if (p == "1") {
    myservo.write(90); 
    Serial.println(p);
  } else {
    myservo.write(0);
  }

  // Leer datos desde Firebase para la ruta3 (mojar1)
  String readUrl3 = "https://" + String(FIREBASE_HOST) + FIREBASE_URL + ruta3 + ".json";
  http.begin(readUrl3); // URL de Firebase para lectura

  int readHttpCode3 = http.GET(); // Realiza una solicitud GET para leer datos

  if (readHttpCode3 > 0) {
    Serial.printf("[HTTP] GET... code: %d\n", readHttpCode3);
    if (readHttpCode3 == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println("Datos leídos desde Firebase:");
      Serial.println(payload);
      r1 = payload.toInt(); // Convertir String a int
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(readHttpCode3).c_str());
  }

  http.end();

  // Leer datos desde Firebase para la ruta4 (mojar2)
  String readUrl4 = "https://" + String(FIREBASE_HOST) + FIREBASE_URL + ruta4 + ".json";
  http.begin(readUrl4); // URL de Firebase para lectura

  int readHttpCode4 = http.GET(); // Realiza una solicitud GET para leer datos

  if (readHttpCode4 > 0) {
    Serial.printf("[HTTP] GET... code: %d\n", readHttpCode4);
    if (readHttpCode4 == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println("Datos leídos desde Firebase:");
      Serial.println(payload);
      r2 = payload.toInt(); // Convertir String a int
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(readHttpCode4).c_str());
  }

  http.end();

  // Leer datos desde Firebase para la ruta5 (mojar3)
  String readUrl5 = "https://" + String(FIREBASE_HOST) + FIREBASE_URL + ruta5 + ".json";
  http.begin(readUrl5); // URL de Firebase para lectura

  int readHttpCode5 = http.GET(); // Realiza una solicitud GET para leer datos

  if (readHttpCode5 > 0) {
    Serial.printf("[HTTP] GET... code: %d\n", readHttpCode5);
    if (readHttpCode5 == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println("Datos leídos desde Firebase:");
      Serial.println(payload);
      r3 = payload.toInt(); // Convertir String a int
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(readHttpCode5).c_str());
  }

  http.end();

  // Leer datos desde Firebase para la ruta6 (mojar4)
  String readUrl6 = "https://" + String(FIREBASE_HOST) + FIREBASE_URL + ruta6 + ".json";
  http.begin(readUrl6); // URL de Firebase para lectura

  int readHttpCode6 = http.GET(); // Realiza una solicitud GET para leer datos

  if (readHttpCode6 > 0) {
    Serial.printf("[HTTP] GET... code: %d\n", readHttpCode6);
    if (readHttpCode6 == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println("Datos leídos desde Firebase:");
      Serial.println(payload);
      r4 = payload.toInt(); // Convertir String a int
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(readHttpCode6).c_str());
  }

  http.end();

  // Controlar motores basados en valores de Firebase
  if (a >= 8640) { 
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW); 
    digitalWrite(motorPin3, LOW); 
    digitalWrite(motorPin4, LOW);
    a = 0;
    delay(5000); 
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, HIGH); 
    digitalWrite(motorPin3, HIGH); 
    digitalWrite(motorPin4, HIGH); 
  } else {
    a++;
  }

  if (r1 > 0) {
    digitalWrite(motorPin1, LOW);
    delay(5000); 
    digitalWrite(motorPin1, HIGH);
    Serial.println(r1);

    HTTPClient httpUpdate;
    String updateUrl = "https://" + String(FIREBASE_HOST) + FIREBASE_URL + ruta3 + ".json";
    httpUpdate.begin(updateUrl); 
    
    String updateData = "0"; 
    int httpUpdateCode = httpUpdate.PUT(updateData);
    
    if (httpUpdateCode > 0) {
      Serial.printf("[HTTP] PUT... code: %d\n", httpUpdateCode);
      if (httpUpdateCode == HTTP_CODE_OK) {
        String payload = httpUpdate.getString();
        Serial.println("Valor actualizado en Firebase:");
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] PUT... failed, error: %s\n", httpUpdate.errorToString(httpUpdateCode).c_str());
    }
    
    httpUpdate.end();
  }

  if (r2 > 0) {
    digitalWrite(motorPin2, LOW);
    delay(5000); 
    digitalWrite(motorPin2, HIGH);

    HTTPClient httpUpdate;
    String updateUrl = "https://" + String(FIREBASE_HOST) + FIREBASE_URL + ruta4 + ".json";
    httpUpdate.begin(updateUrl); 
    
    String updateData = "0"; 
    int httpUpdateCode = httpUpdate.PUT(updateData);
    
    if (httpUpdateCode > 0) {
      Serial.printf("[HTTP] PUT... code: %d\n", httpUpdateCode);
      if (httpUpdateCode == HTTP_CODE_OK) {
        String payload = httpUpdate.getString();
        Serial.println("Valor actualizado en Firebase:");
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] PUT... failed, error: %s\n", httpUpdate.errorToString(httpUpdateCode).c_str());
    }
    
    httpUpdate.end();
  }

  if (r3 > 0) {
    digitalWrite(motorPin3, LOW);
    delay(5000); 
    digitalWrite(motorPin3, HIGH);

    HTTPClient httpUpdate;
    String updateUrl = "https://" + String(FIREBASE_HOST) + FIREBASE_URL + ruta5 + ".json";
    httpUpdate.begin(updateUrl); 
    
    String updateData = "0"; 
    int httpUpdateCode = httpUpdate.PUT(updateData);
    
    if (httpUpdateCode > 0) {
      Serial.printf("[HTTP] PUT... code: %d\n", httpUpdateCode);
      if (httpUpdateCode == HTTP_CODE_OK) {
        String payload = httpUpdate.getString();
        Serial.println("Valor actualizado en Firebase:");
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] PUT... failed, error: %s\n", httpUpdate.errorToString(httpUpdateCode).c_str());
    }
    
    httpUpdate.end();
  }

  if (r4 > 0) {
    digitalWrite(motorPin4, LOW);
    delay(5000); 
    digitalWrite(motorPin4, HIGH);

    HTTPClient httpUpdate;
    String updateUrl = "https://" + String(FIREBASE_HOST) + FIREBASE_URL + ruta6 + ".json";
    httpUpdate.begin(updateUrl); 
    
    String updateData = "0"; 
    int httpUpdateCode = httpUpdate.PUT(updateData);
    
    if (httpUpdateCode > 0) {
      Serial.printf("[HTTP] PUT... code: %d\n", httpUpdateCode);
      if (httpUpdateCode == HTTP_CODE_OK) {
        String payload = httpUpdate.getString();
        Serial.println("Valor actualizado en Firebase:");
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] PUT... failed, error: %s\n", httpUpdate.errorToString(httpUpdateCode).c_str());
    }
    
    httpUpdate.end();
  }

  delay(10000); 
}
