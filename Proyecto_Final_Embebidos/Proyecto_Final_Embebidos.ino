#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7);

char hexaKeys[4][4] = { //definos los símbolos en los botones de nuestro teclado
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte colPins[4] = {35, 34, 33, 32}; //conecta a los pines de las fila del teclado
byte rowPins[4] = {39, 38, 37, 36}; //conecta a los pines de las columnas del teclado

Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, 4, 4); //inicializar una instancia de libreria Keypad.h

#define sonidoin 14
#define brightness 5
#define redPin 4
#define greenPin 3
#define bluePin 2
#define tecladoPin 19
#define S0 8
#define S1 9
#define S2 10
#define S3 11
#define sensorOut 12

volatile bool keyPadFlag = false;

byte red = 255, green = 255, blue = 255;
int itime = 10, menu = 2;
/*
  void readComms();
  void breathe();
  void rainbow();
  void readSound();
  int teclado();
  void interTeclado();*/

void setup() {
  Serial.begin(9600);

  lcd.setBacklightPin(3, POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.begin(16, 2);//Inicia el lcd 16x2

  pinMode(tecladoPin, INPUT_PULLUP);
  pinMode(sonidoin, INPUT_PULLUP);
  pinMode(brightness, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  // Set Sensor output as input
  pinMode(sensorOut, INPUT);
  // Set Pulse Width scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  /*TCCR registro de 16 bits partido en A y B que controla cosas
    como que clock se usa, si falling edge o raising edge, el preescaler
    En estas 2 lineas limpiamos el registro en sus 2 bytes*/
  TCCR5A = 0; //Queda en operacion normal
  TCCR5B = 0;
  TCNT5 = 0;   //Contador del timer a 0
  OCR5A = 1000;//Registro de comparacion cuando el contador llegue a 3200 se produce la interrupcion al timer le toma 0.5 ms llegar a esta cantidad
  TCCR5B |= 0b00001010; //
  /*Aqui ponemos del bit 4 al 3 el tipo de waveform generado como la velocidal,
     en el bit 3 le ponemos que se resetee la cuenta cuando llegue al Borde superior
     del bit 2 al 0 Que clock usaremos  y el preescaler en este caso usamos el de 16MHz con preescaler de 8*/
  //TIMSK5 |= 0b00000010;   //Aqui finalmente se activa la interrupcion por comparacion, donde en el bit 2 le decimos que compare al registro b
  TIMSK5 |= 0b00000000;

  attachInterrupt(digitalPinToInterrupt(tecladoPin), interTeclado, LOW);

}

void loop() {
  if (keyPadFlag == true) {
    bool invalidOption;
    do {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("1OFF 2ON 3Ef 4Ef");
      lcd.setCursor(0, 1);
      lcd.print("5Ef 6Ef 7SetRGB");
      delay(3000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Ingresa la opc");
      menu = teclado();
      invalidOption = menu < 1 || menu > 8;
      if (invalidOption) {
        lcd.setCursor(0, 1);
        lcd.print("Opcion Invalida");
        delay(2000);
      }
    } while (invalidOption);
    if (menu == 7) {
      int auxiliar[3] = {0, 0, 0};
      char* color[3] = {"rojo", "verde", "azul"};
      for (int i = 0; i < 3; i++) {
        do {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Ingresa el ");
          lcd.print(color[i]);
          auxiliar[i] = teclado();
          invalidOption = auxiliar[i] > 255;
          if (invalidOption) {
            lcd.setCursor(0, 1);
            lcd.print("Opcion Invalida");
            delay(2000);
          }
        } while (invalidOption);
        red = auxiliar[0];
        green = auxiliar[1];
        blue = auxiliar[2];
      }
    }
    keyPadFlag = false;
    lcd.clear();
  }
  if (Serial.available() > 0) {
    readBluetoothComms();
  }
  if (menu != 10) {
    lcd.clear();
    lcd.setCursor(0, 0);
    switch (menu) {
      case 1:
        analogWrite(greenPin, 0);
        analogWrite(redPin, 0);
        analogWrite(bluePin, 0);
        lcd.print("Lampara apagada");
        menu = 10;
        break;
      case 2:
        analogWrite(redPin, red);
        analogWrite(greenPin, green);
        analogWrite(bluePin, blue);
        lcd.print("Lampara encendida");
        menu = 10;
        break;
      case 3:
        lcd.print("Efecto arcoiris");
        rainbow();
        break;
      case 4:
        lcd.print("Efecto Respiracion");
        breathe();
        break;
      case 5:
        TIMSK5 = 0b00000010;//activa la interrupt del audio
        lcd.print("Efecto Audio");
        menu = 10;
        break;
      case 6:
        TIMSK5 = 0b00000000; //desactiva la interrupt de audio
        digitalWrite(brightness, LOW);
        lcd.print("Efecto Audio Off");
        menu = 10;
        break;
      case 7:
        lcd.print("Color");
        lcd.setCursor(0, 1);
        lcd.print("R");
        lcd.print(red);
        lcd.print(" G");
        lcd.print(green);
        lcd.print(" B");
        lcd.print(blue);
        analogWrite(redPin, red);
        analogWrite(greenPin, green);
        analogWrite(bluePin, blue);
        menu = 10;
        break;
      case 8:
        colorInput();
        menu = 10;
        break;
    }
  }
}
void colorInput() {
  bool salir;
  int redPW = 0;
  int greenPW = 0;
  int bluePW = 0;
  do {
    salir = true;
    // Read Red Pulse Width
    redPW = getRedPW();
    // Delay to stabilize sensor
    delay(200);
    // Read Green Pulse Width
    greenPW = getGreenPW();
    // Delay to stabilize sensor
    delay(200);
    // Read Blue Pulse Width
    bluePW = getBluePW();
    // Delay to stabilize sensor
    delay(200);
    /*if (redPW > 550 && greenPW > 640 && bluePW > 500) {
      lcd.print("Negro");
      red = 0;
      green = 0;
      blue = 0;
      analogWrite(redPin, red);
      analogWrite(greenPin, green);
      analogWrite(bluePin, blue);
    }*/
    if ((redPW < 150 && greenPW < 440 && bluePW < 200) && (redPW > 85 && greenPW > 213 && bluePW > 105) ) {
      lcd.print("Morado");
      red = 255;
      green = 0;
      blue = 255;
      analogWrite(redPin, red);
      analogWrite(greenPin, green);
      analogWrite(bluePin, blue);
    }
    else if ((redPW < 65 && greenPW < 160 && bluePW < 130) && (redPW > 40 && greenPW > 130 && bluePW > 100) ) {
      lcd.print("Rojo");
      red = 255;
      green = 0;
      blue = 0;
      analogWrite(redPin, red);
      analogWrite(greenPin, green);
      analogWrite(bluePin, blue);
    }
    else if ((redPW < 65 && greenPW < 100 && bluePW < 110) && (redPW > 40 && greenPW > 70 && bluePW > 85) ) {
      lcd.print("Naranja");
      red = 255;
      green = 255;
      blue = 0;
      analogWrite(redPin, red);
      analogWrite(greenPin, green);
      analogWrite(bluePin, blue);
    }
    else if ((redPW < 170 && greenPW < 410 && bluePW < 210) && (redPW > 140 && greenPW > 110 && bluePW > 180) ) {
      lcd.print("Verde");
      red = 0;
      green = 255;
      blue = 0;
      analogWrite(redPin, red);
      analogWrite(greenPin, green);
      analogWrite(bluePin, blue);
    }
    else if (redPW < 50 && greenPW < 50 && bluePW < 50) {
      lcd.print("Blanco");
      red = 255;
      green = 255;
      blue = 255;
      analogWrite(redPin, red);
      analogWrite(greenPin, green);
      analogWrite(bluePin, blue);
    } else {
      salir = false;
    }
    /*
      // Print output to Serial Monitor
      Serial.print("Red PW = ");
      Serial.print(redPW);
      Serial.print(" - Green PW = ");
      Serial.print(greenPW);
      Serial.print(" - Blue PW = ");
      Serial.println(bluePW);*/
  } while (salir == false);
}

int teclado() {
  int i = 0;
  int numericInput;
  lcd.setCursor(0, 1);
  char input[10];
  do {
    input[i] = customKeypad.getKey();
    if (input[i]) {
      lcd.print(input[i]);
      i++;
    }
  } while (input[i - 1] != '*');
  numericInput = atoi(input);
  return numericInput;
}

void readBluetoothComms() { //quiza hacerla interrupcion?
  String aux = Serial.readString();
  if (aux == "Apagar") {
    menu = 1;
  } else if (aux == "Encender") {
    menu = 2;
  } else if (aux == "secuencia1") {
    menu = 3;
  } else if (aux == "Breathe") {
    TIMSK5 = 0b00000000; //desactiva la interrupt de audio
    menu = 4;
    digitalWrite(brightness, LOW);
  } else if (aux == "Audio") {
    menu = 5;
  } else if (aux == "OffAudio") {
    menu = 6;
  }  else if (aux[0] == 'R') {
    red = atoi(aux.substring(1, aux.indexOf('G')).c_str());
    green = atoi(aux.substring(aux.indexOf('G') + 1, aux.indexOf('B')).c_str());
    blue = atoi(aux.substring(aux.indexOf('B') + 1).c_str());
    menu = 7;
  }
}

void rainbow() {
  red = 255;
  green = 0;
  blue = 0;
  analogWrite(redPin, 255);
  analogWrite(greenPin, 0);
  analogWrite(bluePin, 0);
  for (int i = 0; i < 255; i++) {
    red--;
    green++;
    analogWrite(redPin, red);
    analogWrite(greenPin, green);
    delay(itime);
  }
  for (int i = 0; i < 255; i++) {
    green--;
    blue++;
    analogWrite(greenPin, green);
    analogWrite(bluePin, blue);
    delay(itime);
  }
  for (int i = 0; i < 255; i++) {
    blue--;
    red++;
    analogWrite(bluePin, blue);
    analogWrite(redPin, red);
    delay(itime);
  }
}

void breathe() {
  for (int i = 0; i < 255; i++) {
    analogWrite(brightness, i);
    delay(itime);
  }
  for (int i = 255; i > 0; i--) {
    analogWrite(brightness, i);
    delay(itime);
  }
}

// Function to read Red Pulse Widths
int getRedPW() {

  // Set sensor to read Red only
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  // Define integer to represent Pulse Width
  int PW;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut, LOW);
  // Return the value
  return PW;

}

// Function to read Green Pulse Widths
int getGreenPW() {

  // Set sensor to read Green only
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  // Define integer to represent Pulse Width
  int PW;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut, LOW);
  // Return the value
  return PW;

}

// Function to read Blue Pulse Widths
int getBluePW() {

  // Set sensor to read Blue only
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  // Define integer to represent Pulse Width
  int PW;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut, LOW);
  // Return the value
  return PW;

}

void interTeclado() {
  keyPadFlag = true;
}

ISR(TIMER5_COMPA_vect)
{
  digitalWrite(brightness, digitalRead(sonidoin));
}
