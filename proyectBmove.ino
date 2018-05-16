#include "Servo.h"
Servo servoR, servoL;
Servo servoBat;

int TP = 4, EP = 5; // TP = trig pin, EP = Echo pin Sensor Bat

int ldrIzq = A2, ldrDrch = A3; //pines ldr
int irIzq = 2, irDrch = 3; // pines infrarrojos

//Delays
int defaultDelay = 200, lineasDelay = 0;
int giro_180 = 1028, giro_90 = 514, giro_45 = 257, giro_30 = 200; //Delays para diferentes ángulos

int pinBuz = 12;

String inString = ""; // String para lectura/escritura desde Protocoder
void setup() {

	Serial.begin(19200); //NO MODIFICAR!!!!!!!!!!!
	Serial.flush();

	servoL.attach(8);
	servoR.attach(9);
	servoBat.attach(11);

	servoR.write(90);
	servoL.write(90);
	servoBat.write(90);

	//pines bat
	pinMode(EP, INPUT);
	pinMode(TP, OUTPUT);

	//pines ldr
	pinMode(ldrDrch, INPUT);
	pinMode(ldrIzq, INPUT);

	//pines ir
	pinMode(irDrch, INPUT);
	pinMode(irIzq, INPUT);
        pinMode(pinBuz, OUTPUT);  
}


//Funciones lectura/escritura desde Protocoder
void readFromAndroid() {
	char inChar;
	while (Serial.available() > 0) {
		inChar = (char) Serial.read();
		Serial.flush();
		if (inChar == '=')
			inString = "";
		else if (inChar != '+')
			inString += inChar;
	}
}
void writeData() {
	if (inString == "avanzar") {
		avanzar();
	}
	if (inString == "retroceder") {
		retroceder();
	}
	if (inString == "izquierda") {
		izquierda();
		delay(defaultDelay);
		parar();
		inString = "";
	}
	if (inString == "derecha") {
		derecha();
		delay(defaultDelay);
		parar();
		inString = "";
	}
	if (inString == "parar") {
		parar();
	}
	if (inString == "obstaculos") {
		obstaculos();
	}
	if (inString == "siguelineas") {
		sigueLineas();
	}
	if (inString == "sigueluz") {
		sigueLuz();
	}
	if (inString == "180") {
		giro_180_grados();
		inString = "";
	}
        if (inString == "Musica") {
                musica();
        }
}
//Funciones de movimiento
void avanzar () {
	servoR.write(180);
	servoL.write(0);
}

void retroceder() {
	servoR.write(0);
	servoL.write(180);
}
void parar() {
	servoR.write(90);
	servoL.write(90);
}
void izquierda() {
	servoR.write(180);
	servoL.write(90);
}
void derecha() {
	servoR.write(90);
	servoL.write(0);
}

void giro_180_grados() {
	servoR.write(180);
	servoL.write(180);
	delay(giro_180);
	parar();
}

void izquierdaLineas() {
	servoR.write(180);
	servoL.write(90);
	delay(lineasDelay);
	parar();
}

void derechaLineas() {
	servoR.write(90);
	servoL.write(0);
	delay(lineasDelay);
	parar();
}

//Funciones obstaculos
void obstaculos() {

	int izq = 140, drch = 50, cent = 90;

	// Comprueba si hay obstaculos
	boolean obsIzq = buscaObstaculo(izq); //Devuelve true si hay obstaculo
	if (obsIzq) {
		parar();
	}

	boolean obsCent = buscaObstaculo(cent);
	if (obsCent) {
		parar();
	}

	boolean obsDrch = buscaObstaculo(drch);
	if (obsDrch) {
		parar();
	}

	//Se mueve si no hay obstaculos
	if (!obsCent) {
		avanzar();
	} else if (!obsDrch) {
		derecha();
		delay(giro_45);
		parar();
	} else if (!obsIzq) {
		izquierda();
		delay(giro_45);
		parar();
	} else {
		retroceder();
		delay(giro_30);
		giro_180_grados();
	}
}

boolean buscaObstaculo(int angulo) {

	int distancia_max = 25;
	int distancia;

	servoBat.write(angulo);
	delay(200);

	distancia = Distance();

	if (distancia != 0 && distancia < distancia_max) {
		tone(pinBuz, 261, 100);
		return true;
	} else {
		return false;
	}
}

long TP_init() {
	
	digitalWrite( TP , LOW);
	delayMicroseconds(2);
	digitalWrite( TP , HIGH);
	delayMicroseconds(10);
	digitalWrite( TP , LOW);
	long microseconds = pulseIn( EP , HIGH);
	return microseconds;
}
long Distance() {
	long microseconds = TP_init();
	int distancia = 	microseconds / 29 / 2; // Distance_CM  = ((Duration of high level)*(Sonic :340m/s))/2
	//                                                     = ((Duration of high level)*(Sonic :0.034 cm/us))/2
	//                                                     = ((Duration of high level)/(Sonic :29.4 cm/us))/2
	return distancia;
}

//Funciones sigueLuz
void sigueLuz() {

	int lecturaIzq = analogRead(ldrIzq);
	int lecturaDrch = analogRead(ldrDrch);

	if (lecturaIzq > lecturaDrch) {
		izquierda();
		delay(250);
		avanzar();
		delay(1000);
	} else {
		derecha();
		delay(250);
		avanzar();
		delay(1000);
	}
}

//Funciones sigueLineas
void sigueLineas() {
  	int lecturaIzq = digitalRead(irIzq);
	int lecturaDrch = digitalRead(irDrch);

  if ((lecturaIzq == 0) && (lecturaDrch == 0)){ // los dos encima linea
        avanzar();
}else if  ((lecturaIzq < 1) && (lecturaDrch > 0)){  //pierde derecha, izquierda ok
izquierdaLineas();
}else if  ((lecturaIzq > 0) && (lecturaDrch < 1)){   // pierde izquierda-derecha ok, pierden los dos
derechaLineas();
}else if  ((lecturaIzq == 1) && (lecturaDrch == 1)){
parar();
  }

}
//Funciones sigue luz
void loop() {
	if (Serial.available() > 0) {
		readFromAndroid();
	}
	writeData();
}
//Funciones Musica
void beep (unsigned char pinBuz, int frequencyInHertz, long timeInMilliseconds)  //code for working out the rate at which each note plays and the frequency.
{
  int x;      
  long delayAmount = (long)(1000000/frequencyInHertz);
  long loopTime = (long)((timeInMilliseconds*1000)/(delayAmount*2));
  for (x=0;x<loopTime;x++)    
  {    
    digitalWrite(pinBuz,HIGH);
    delayMicroseconds(delayAmount);
    digitalWrite(pinBuz,LOW);
    delayMicroseconds(delayAmount);
  }    
  delay(20);
} 
void musica() {
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

beep(pinBuz, NOTE_D5, 100);  //   beep( -PIN OF SPEAKER-, -THE NOTE WANTING TO BE PLAYED-, -DURATION OF THE NOTE IN MILISECONDS- )
  delay(80);
  beep(pinBuz, NOTE_F5, 100); 
  delay(80);
  beep(pinBuz, NOTE_D6, 200); 
  delay(250);
  
  beep(pinBuz, NOTE_D5, 100); 
  delay(80);
  beep(pinBuz, NOTE_F5, 100); 
  delay(80);
  beep(pinBuz, NOTE_D6, 200);
  delay(250);
  
  beep(pinBuz, NOTE_E6, 200); 
  delay(200);
  beep(pinBuz, NOTE_F6, 100); 
  delay(100);
  beep(pinBuz, NOTE_E6, 100); 
  delay(80);
  beep(pinBuz, NOTE_F6, 100); 
  delay(80);
  beep(pinBuz, NOTE_E6, 100); 
  delay(80);
  beep(pinBuz, NOTE_C6, 100); 
  delay(80);
  beep(pinBuz, NOTE_A5, 100); 
  delay(300);
  
  beep(pinBuz, NOTE_A5, 200); 
  delay(100);
  beep(pinBuz, NOTE_D5, 200); 
  delay(100);
  beep(pinBuz, NOTE_F5, 100); 
  delay(100);
  beep(pinBuz, NOTE_G5, 100); 
  delay(100);
  beep(pinBuz, NOTE_A5, 100); 
  delay(500);
  
  beep(pinBuz, NOTE_A5, 200); 
  delay(100);
  beep(pinBuz, NOTE_D5, 200); 
  delay(100);
  beep(pinBuz, NOTE_F5, 100); 
  delay(100);
  beep(pinBuz, NOTE_G5, 100); 
  delay(100);
  beep(pinBuz, NOTE_E5, 100); 
  delay(500);
  
  
  
  beep(pinBuz, NOTE_D5, 100); 
  delay(80);
  beep(pinBuz, NOTE_F5, 100); 
  delay(80);
  beep(pinBuz, NOTE_D6, 200); 
  delay(250);
  
  beep(pinBuz, NOTE_D5, 100); 
  delay(80);
  beep(pinBuz, NOTE_F5, 100); 
  delay(80);
  beep(pinBuz, NOTE_D6, 200);
  delay(250);
  
  beep(pinBuz, NOTE_E6, 200); 
  delay(200);
  beep(pinBuz, NOTE_F6, 100); 
  delay(100);
  beep(pinBuz, NOTE_E6, 100); 
  delay(80);
  beep(pinBuz, NOTE_F6, 100); 
  delay(80);
  beep(pinBuz, NOTE_E6, 100); 
  delay(80);
  beep(pinBuz, NOTE_C6, 100); 
  delay(80);
  beep(pinBuz, NOTE_A5, 100); 
  delay(300);
  
  beep(pinBuz, NOTE_A5, 200); 
  delay(100);
  beep(pinBuz, NOTE_D5, 200); 
  delay(100);
  beep(pinBuz, NOTE_F5, 100); 
  delay(100);
  beep(pinBuz, NOTE_G5, 100); 
  delay(100);
  beep(pinBuz, NOTE_A5, 300);
  delay(100);
  beep(pinBuz, NOTE_A5, 200);
  delay(100);
  beep(pinBuz, NOTE_D5, 300);
  delay(2000);
  
}


