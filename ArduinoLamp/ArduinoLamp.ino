//this will be used for bluetooth bt-04 module #include <SoftwareSerial.h>

#define sonidoin 14
#define brightness 5
#define redPin 4
#define greenPin 3
#define bluePin 2

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

  pinMode(sonidoin, INPUT_PULLUP);
  pinMode(brightness, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

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
}

void loop() {
  if (Serial.available() > 0) {
    readBluetoothComms();
  }
  if (menu != 10) {
    switch (menu) {
      case 1:
        //apagar lampara
        analogWrite(greenPin, 0);
        analogWrite(redPin, 0);
        analogWrite(bluePin, 0);        
        menu = 10;
        break;
      case 2:
        //encender lampara
        analogWrite(redPin, red);
        analogWrite(greenPin, green);
        analogWrite(bluePin, blue);
        menu = 10;
        break;
      case 3:
        rainbow();
        break;
      case 4:
        breathe();
        break;
      case 5:
        TIMSK5 = 0b00000010;//activa la interrupt del audio
        menu = 10;
        break;
      case 6:
        TIMSK5 = 0b00000000; //desactiva la interrupt de audio
        digitalWrite(brightness, LOW);
        menu = 10;
        break;
      case 7: //poner cierto color rgb quiza redirigir al caso 2
        analogWrite(redPin, red);
        analogWrite(greenPin, green);
        analogWrite(bluePin, blue);
        menu = 10;
        break;
    }
  }
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

ISR(TIMER5_COMPA_vect)
{
  digitalWrite(brightness, digitalRead(sonidoin));
}
