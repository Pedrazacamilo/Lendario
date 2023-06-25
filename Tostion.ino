#include <Servo.h>
#include <SPI.h>
#include <max6675.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//CONFIG LDC
  LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
  // Variable booleana para indicar si es la primera vez que ingresamos al if
  bool cleartiempo = true;
  bool cleartemp = true;
  bool clearproceso = true;
  bool clearproceso1 = true;
  bool clearproceso2 = true;
  // Definir los pines de la pantalla LCD (I2C)
  const int pinSDA = A4;
  const int pinSCL = A5;

//CONFIG MAX6675
  int ktcSO = 9;
  int ktcCS = 10;
  int ktcCLK = 11;
  MAX6675 ktc(ktcCLK, ktcCS, ktcSO);

// Pin del pulsador
  const int pulsadorPinA = 2;
  const int pulsadorPinB = 8;
  const int pulsadorPin1 = 7;
  const int pulsadorPin2 = 4;
// Servomotor
  const int servoPin = 12;
  Servo servoMotor;
  int servoPosicionInicial = 100;
  int servoPosicionMinima = 90;
  int servoPosicionMaxima = 0;
  int servoPosicionActual = servoPosicionInicial;
  // Parámetros de ajuste
  int incrementoServo = 10;
// Declaración de variables
int minutos = 0;
int temp = 0;
int buzzerPin = 3;

void setup() {
  servoMotor.attach(servoPin);
  servoMotor.write(servoPosicionInicial);
  pinMode(pulsadorPinA, INPUT_PULLUP);
  pinMode(pulsadorPinB, INPUT_PULLUP);
  pinMode(pulsadorPin1, INPUT_PULLUP);
  pinMode(pulsadorPin2, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  lcd.begin(20, 4);

  // Titila la pantalla 3 veces
  for (int i = 0; i < 3; i++) {
    lcd.backlight();
    delay(250);
    lcd.noBacklight();
    delay(250);
  }
  lcd.backlight();

  lcd.setCursor(6, 0);
  lcd.print("LENDARIO");
  delay(1000);
  lcd.setCursor(5, 1);
  lcd.print("MAQUINA DE");
  delay(1000);
  lcd.setCursor(2, 2);
  lcd.print("TOSTION DE CACAO");
  delay(3000);
  //configuracion
  lcd.clear();
}

void loop() {
//MODO PREPARACION
  if ((minutos == 0) && (temp == 0) && (digitalRead(pulsadorPin1) == HIGH) && (digitalRead(pulsadorPin2) == HIGH)){
  if (clearproceso) {
      lcd.clear();  // Borra el contenido de la pantalla LCD solo en la primera vez
      clearproceso = false;  // Cambia el valor de la variable para que no se borre la pantalla en las siguientes iteraciones
    }
  lcd.setCursor(3, 0);
  lcd.print("Configuracion");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("-Tiempo");
  delay(1000);
  lcd.setCursor(0, 3);
  lcd.print("-Temperatura");
  delay(1000);
    cleartiempo = true;
    cleartemp = true;
    clearproceso1 = true;
    clearproceso2 = true;
  }
//PROCESO 1 CALENTAMIENTO
  if ((minutos == 0) && (temp > 0) && (digitalRead(pulsadorPin1) == HIGH) && (digitalRead(pulsadorPin2) == HIGH)) {
    if (clearproceso1) {
      lcd.clear();  // Borra el contenido de la pantalla LCD solo en la primera vez
      clearproceso1 = false;  // Cambia el valor de la variable para que no se borre la pantalla en las siguientes iteraciones
    }
    lcd.setCursor(3, 0);
    lcd.print("PROCESO 1");
    delay(1000);
    lcd.setCursor(3, 1);
    lcd.print("CALENTAMIENTO");
    delay(1000);
    double temperaturaMaxima = temp;
    double temperaturaActual = obtenerTemperatura();
    lcd.setCursor(0,2);
    lcd.print("Temp Actual:");
    delay(1000);
    lcd.setCursor(12, 2);
    lcd.print(temperaturaActual);
    char temperaturaActualStr[8];
    sprintf(temperaturaActualStr, "%.2", temperaturaActual); 
    lcd.print(temperaturaActualStr);
    delay(1000);
    lcd.setCursor(0,3);
    lcd.print("Temp maxima:");
    delay(1000);
    lcd.setCursor(12, 3);
    lcd.print(temperaturaMaxima);
    char temperaturaMaximaStr[8];  // Variable para almacenar el número de minutos como cadena
    sprintf(temperaturaMaximaStr, "%.2", temperaturaMaxima);  
    lcd.print(temperaturaMaximaStr);
  // Control del servo motor
  if (temperaturaActual < temperaturaMaxima) {
    servoPosicionActual -= incrementoServo;
    servoPosicionActual = max(servoPosicionActual, servoPosicionMaxima);
  } else {
    servoPosicionActual += incrementoServo;
    servoPosicionActual = min(servoPosicionActual, servoPosicionMinima);
  }

  servoMotor.write(servoPosicionActual);
  
    clearproceso = true;
    cleartiempo = true;
    cleartemp = true;
  }
//PROCESO 2 TOSTION
  if ((minutos > 0) && (temp > 0) && (digitalRead(pulsadorPin1) == HIGH) && (digitalRead(pulsadorPin2) == HIGH)) {
    if (clearproceso2) {
      lcd.clear();  // Borra el contenido de la pantalla LCD solo en la primera vez
      clearproceso2 = false;  // Cambia el valor de la variable para que no se borre la pantalla en las siguientes iteraciones
    }
    lcd.setCursor(3, 0);
    lcd.print("PROCESO 2");
    delay(1000);
    lcd.setCursor(3, 1);
    lcd.print("TOSTION");
    delay(1000);
    clearproceso = true;
    cleartiempo = true;
    cleartemp = true;
  }
// CONFIGURACION TIEMPO
  if (digitalRead(pulsadorPin1) == LOW) {
    if (cleartiempo) {
      lcd.clear();  // Borra el contenido de la pantalla LCD solo en la primera vez
      cleartiempo = false;  // Cambia el valor de la variable para que no se borre la pantalla en las siguientes iteraciones
    }
    lcd.setCursor(3, 0);
    lcd.print("Tiempo");

    if (digitalRead(pulsadorPinA) == LOW) {
      delay(50);  // Retardo para evitar rebotes
      if (digitalRead(pulsadorPinA) == LOW) {
        minutos++;  // Incrementar los minutos
        if (minutos > 60){
          minutos = 60;
        }
      }
    }
    if (digitalRead(pulsadorPinB) == LOW) {
      delay(50);  // Retardo para evitar rebotes
      if (digitalRead(pulsadorPinB) == LOW) {
        minutos--;  // Decrementar los minutos
        if (minutos < 0){
          minutos = 0;
        }
      }
    }
    lcd.setCursor(5, 2);  // Posicionar el cursor en la tercera línea
    lcd.print("          ");  // Borrar el contenido anterior
    lcd.setCursor(0, 3);  // Posicionar el cursor en la tercera línea
    lcd.print("Minutos: ");
    char minutosStr[3];  // Variable para almacenar el número de minutos como cadena
    sprintf(minutosStr, "%02d", minutos);  // Formatear el número de minutos con tres dígitos
    lcd.print(minutosStr);
    cleartemp = true;
    clearproceso = true;
    clearproceso1 = true;
    clearproceso2 = true;

  }
// CONFIGURACION TEMPERATURA
  if (digitalRead(pulsadorPin2) == LOW) {
    if (cleartemp) {
      lcd.clear();  // Borra el contenido de la pantalla LCD solo en la primera vez
      cleartemp = false;  // Cambia el valor de la variable para que no se borre la pantalla en las siguientes iteraciones
    }
    lcd.setCursor(3, 0);
    lcd.print("Temperatura");

    if (digitalRead(pulsadorPinA) == LOW) {
      delay(50);  // Retardo para evitar rebotes
      if (digitalRead(pulsadorPinA) == LOW) {
        temp++;  // Incrementar los minutos
        if (temp > 800){
          temp = 800;
        }
      }
    }
    // Verificar si se presiona el segundo pulsador
    if (digitalRead(pulsadorPinB) == LOW) {
      delay(50);  // Retardo para evitar rebotes
      if (digitalRead(pulsadorPinB) == LOW) {
        temp--;  // Decrementar los minutos
        if (temp < 0){
          temp = 0;
        }
      }
    }
    lcd.setCursor(5, 2);  // Posicionar el cursor en la tercera línea
    lcd.print("          ");  // Borrar el contenido anterior
    lcd.setCursor(0, 3);  // Posicionar el cursor en la tercera línea
    lcd.print("Temperatura: ");
    char tempStr[4];  // Variable para almacenar el número de minutos como cadena
    sprintf(tempStr, "%03d", temp);  // Formatear el número de minutos con tres dígitos
    lcd.print(tempStr);
    cleartiempo = true;
    clearproceso = true;
    clearproceso1 = true;
    clearproceso2 = true;
  }
  }
double obtenerTemperatura() {
  double temp = ktc.readCelsius();

  // Si se produce un error de lectura del sensor, asignar un valor negativo como indicador de error
  if (isnan(temp)) {
    temp = -1.0;
    servoMotor.write(servoPosicionInicial);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Error de lectura de temperatura");
    emitirError();

  }

  return temp;
}
void emitirError() {
  digitalWrite(buzzerPin, HIGH); // Activar el buzzer
  delay(200); // Mantener el buzzer activado durante 200 ms
  digitalWrite(buzzerPin, LOW); // Apagar el buzzer
  delay(200); // Pausa de 200 ms entre tonos
  
  // Repetir el patrón de sonido tres veces
  for (int i = 0; i < 2; i++) {
    delay(500); // Pausa de 500 ms antes de repetir el patrón
    digitalWrite(buzzerPin, HIGH); // Activar el buzzer
    delay(200); // Mantener el buzzer activado durante 200 ms
    digitalWrite(buzzerPin, LOW); // Apagar el buzzer
    delay(200); // Pausa de 200 ms entre tonos
  }
}
