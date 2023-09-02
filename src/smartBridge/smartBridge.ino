// Librerie
#include <Ultrasonic.h>
#include <Servo.h>
#include <TaskScheduler.h>
#include <LiquidCrystal_I2C.h>

// Definizione di MACRO
#define SERVO_SBARRA 7
#define SERVO_PONTESX 8
#define SERVO_PONTEDX 9
#define PIN_ROSSO 12
#define PIN_VERDE 13
#define PIN_BUZZER 2
#define SENSORE_ACQUA A0 

LiquidCrystal_I2C display(0x27, 16, 2);
Ultrasonic ultrasonicEntrata(10, 11);
Ultrasonic ultrasonicUscita(4, 3);

// Definizione di costanti
const short DISTANZA_MAX = 20; // Soglia massima di distanza espressa in centimetri per rilevare un'imbarcazione
const unsigned int frequenza = 480; // Frequenza utilizzata per il buzzer espressa in Hertz
const unsigned long durata = 80; // Durata utilizzata per il buzzer espressa in millisecondi 

// Definizioni di variabili
bool sbarraAperta = true; // true significa sbarra lato auto aperta, false chiusa
short numBarcheinAttesa = 0; // Tiene il conteggio delle barche che sono in attesa
int livelloH2o = 0; // Valore corrente del livello dell'acqua
short marea = 0; // 0 -> BASSA marea con altezza consigliata di 3 metri, 1 -> MEDIA marea con altezza consigliata di 2 metri, 2 -> ALTA marea, impossibile passare
long timeCars = 60000; // Valore di default in millisecondi per chiusura della sbarra auto e apertura del ponte
long timeBoats; // Valore in millisecondi per chiusura del ponte e apertura della sbarra auto
bool first = false; // Indica se viene rilevata la prima imbarcazione dopo la chiusura del ponte

Scheduler scheduler;
Servo servoSbarra; // Servo della sbarra che dirige il traffico
Servo servosx; // Servo sinistro del ponte
Servo servodx; // Servo destro del ponte

// Prototipi di funzione
void sbarraDirigiTrafficoOpenCallback();
void sbarraDirigiTrafficoCloseCallback();
void ponteOpenCallback();
void ponteCloseCallback();
void ledCallback();
void buzzerCallback();
void displayCallback();
void ultraFirstCallback();
void ultraSecondCallback();
int readWaterSensorCallback();
void mareaCallback();
void regolazioneFlussoCallback();

// Task 
Task sbarraDirigiTrafficoOpenTask(0, TASK_ONCE, &sbarraDirigiTrafficoOpenCallback);
Task sbarraDirigiTrafficoCloseTask(0, TASK_ONCE, &sbarraDirigiTrafficoCloseCallback);
Task ponteOpenTask(0, TASK_ONCE, &ponteOpenCallback);
Task ponteCloseTask(0, TASK_ONCE, &ponteCloseCallback);
Task ledTask(200, TASK_FOREVER, &ledCallback);
Task buzzerTask(200, TASK_FOREVER, &buzzerCallback);
Task displayTask(500, TASK_FOREVER, &displayCallback);
Task ultraFirstTask(3000, TASK_FOREVER, &ultraFirstCallback);
Task ultraSecondTask(3000, TASK_FOREVER, &ultraSecondCallback);
Task readWaterSensorTask(0, TASK_ONCE, &readWaterSensorCallback);
Task mareaTask(5000, TASK_FOREVER, &mareaCallback);
Task regolazioneFlussoTask(2000, TASK_FOREVER, &regolazioneFlussoCallback);

void setup() {
  Serial.begin(9600);

  scheduler.init();
  
  Serial.println("INIZIO");
  servoSbarra.attach(SERVO_SBARRA);
  Serial.println("Aggangio servo sbarra");
  servosx.attach(SERVO_PONTESX);
  Serial.println("Aggangio servo ponte sx");
  servodx.attach(SERVO_PONTEDX);
  Serial.println("Aggangio servo ponte dx");
  pinMode(PIN_VERDE, OUTPUT);
  pinMode(PIN_ROSSO, OUTPUT);
  Serial.println("Led settati");
  pinMode(PIN_BUZZER, OUTPUT);
  Serial.println("Buzzer settato");
  display.init();
  display.backlight();
  Serial.println("Display lcd abilitato");
  analogWrite(SENSORE_ACQUA, LOW);
  Serial.println("Sensore acqua abilitato");

  servoSbarra.write(0);
  servosx.write(0);
  servodx.write(0);

  // Situazione iniziale: ponte chiuso, viene quindi settato (acceso) il semaforo verde per le auto e la sbarra traffico viene aperta
  digitalWrite(PIN_VERDE, HIGH);
  digitalWrite(PIN_ROSSO, LOW);
  display.setCursor(1, 0);
  display.print("Ponte chiuso: ");
  display.setCursor(1, 1);
  display.print("attendere");
  delay(15000);
  Serial.println("Fase di inizializzazione completata");

  scheduler.addTask(sbarraDirigiTrafficoOpenTask);
  scheduler.addTask(sbarraDirigiTrafficoCloseTask);
  scheduler.addTask(ponteOpenTask);
  scheduler.addTask(ponteCloseTask);
  scheduler.addTask(ledTask);
  scheduler.addTask(buzzerTask);
  scheduler.addTask(displayTask);
  scheduler.addTask(ultraFirstTask);
  scheduler.addTask(ultraSecondTask);
  scheduler.addTask(readWaterSensorTask);
  scheduler.addTask(mareaTask);
  scheduler.addTask(regolazioneFlussoTask);

  sbarraDirigiTrafficoOpenTask.enable();
  ponteCloseTask.enable();
  ledTask.enable();
  buzzerTask.enable();
  displayTask.enable();
  ultraFirstTask.enable();
  ultraSecondTask.enable();
  readWaterSensorTask.enable();
  mareaTask.enable();
  regolazioneFlussoTask.enable();
  
}

void loop() {
  // Esecuzione dei task
  scheduler.execute();
}

// Regolazione dell'apertura della sbarra auto
void sbarraDirigiTrafficoOpenCallback() {
  sbarraAperta = true;
  // Serial.println("ATTIVATO"); 
  // Apro la sbarra auto di 90 gradi 
  for (int i = 90; i >= 0; i--) {
    servoSbarra.write(i);
  }
}

// Regolazione della chiusura della sbarra auto
void sbarraDirigiTrafficoCloseCallback() {
  sbarraAperta = false;
  // Chiudo la sbarra auto di 90 gradi verso il terreno
  for (int i = 0; i <= 90; i++) {
    servoSbarra.write(i);
  }
}

// Regolazione dell'apertura del ponte 
void ponteOpenCallback() {
  // Apro il ponte di 70 gradi
  for (int i = 0; i < 70; i++) {
    servosx.write(i);
    servodx.write(i);
  }
  ponteCloseTask.enable();
  ponteCloseTask.restartDelayed(timeBoats);
  sbarraDirigiTrafficoOpenTask.enable();
  sbarraDirigiTrafficoOpenTask.restartDelayed(timeBoats + 1500);
}

// Regolazione della chiusura del ponte
void ponteCloseCallback() {
  // Chiudo il ponte di 70 gradi verso il terreno
  for (int i = 70; i >= 0; i--) {
    servosx.write(i);
    servodx.write(i);
  }
}

// Controllo del led: rimane verde se la sbarra auto è aperta, rosso altrimenti
void ledCallback() {
  if (sbarraAperta) {
    digitalWrite(PIN_VERDE, HIGH);
    digitalWrite(PIN_ROSSO, LOW);
  } else {
    digitalWrite(PIN_ROSSO, HIGH);
    digitalWrite(PIN_VERDE, LOW);
  }
}

// Se il ponte delle barche è aperto emette un suono utile al fine di avvisare sia
// i mezzi di terra che i mezzi di mare
void buzzerCallback() {
  if (!sbarraAperta) {
    tone(PIN_BUZZER, frequenza, durata);
  }
}

// Se il ponte è aperto viene emesso un segnale luminoso con la seguente scritta: Ponte aperto: procedere,
// se il ponte è chiuso consiglia in base al tipo di imbarcazione il passaggio o meno, con bassa marea possono bassare barche avente un albero/altezza (dipende
// se a vela o motore) di modeste dimensioni, con la marea leggermente più alta il passaggio è consentito alle sole più piccole, con l'alta marea il passaggio 
// non è consentito
void displayCallback() {
  display.clear();
  if (sbarraAperta) {
    display.setCursor(1, 0);
    display.print("Ponte chiuso: ");
    switch (marea) {
      case 0: 
        display.setCursor(1, 1);
        display.print("Alt. consgl. 3m");
        break;

      case 1:
        display.setCursor(1, 1);
        display.print("Alt. consgl. 1.5m");
        break;

      case 2:
        display.setCursor(1, 1);
        display.print("attendere");
        break;
    }
  } else {
      display.setCursor(1, 0);
      display.print("Ponte aperto: ");
      display.setCursor(1, 1);
      display.print("procedere");
  }
}

// Tiene il conteggio delle barche in attesa PRIMA dell'apertura del ponte
void ultraFirstCallback() {
  int distance = ultrasonicEntrata.read(CM);
  // Se la sbarra delle auto è chiusa significa che il ponte è aperto e non è necessario
  // conteggiare il numero delle barche in attesa dato che sono in transito
  if (!sbarraAperta) {
    numBarcheinAttesa = 0;
  }
  // Se la sbarra della auto è aperta e viene rilevato un movimento il numero di barche in attesa viene incrementato di 1 dato che
  // il passaggio prima di entrare nell'area di attesa è consentito ad una sola imbarcazione
  if ((sbarraAperta) & ((distance > 0) & (distance < DISTANZA_MAX))) {
    numBarcheinAttesa++;
  }
  Serial.println("Num. barche: " + String(numBarcheinAttesa));
}

// Tiene il conteggio delle barche in uscita MENTRE il ponte è chiuso, il significato deriva dal fatto che essendo "piccola" l'imbarcazione è riuscita 
// a passare al di sotto del ponte
void ultraSecondCallback() {
  int distance2 = ultrasonicUscita.read(CM);
  //Serial.println(distance2);
  // Se la sbarra del traffico è aperta e viene rilevata una barca uscire dal ponte decremento il numero di barche in attesa 
  // perchè la barca è riuscita a passare in caso di bassa/media marea
  if((sbarraAperta) & ((distance2 > 0) & (distance2 < DISTANZA_MAX))) {
    numBarcheinAttesa--;
  } 
}

// Misurazione del livello dell'acqua 
int readWaterSensorCallback() {
  //analogWrite(waterSensor, HIGH);
	livelloH2o = analogRead(SENSORE_ACQUA);		// Lettura del valore analogico dal sensore
  Serial.print("Water level: ");
	Serial.println(livelloH2o);							 // stampa il valore corrente del livello di H2o
  //analogWrite(waterSensor, LOW); 
  return livelloH2o;
}

// Funzione che mi indica la tipologia di marea 0/1/2 in base all'altezza del livello dell'acqua
void mareaCallback() {

  int liv = readWaterSensorCallback();

  if ((liv > 0) && (liv < 220)) {
    Serial.println("Livello: " + String(liv));
    Serial.println("Bassa marea 0");
    marea = 0;
  } else if ((liv >= 220) && (liv < 250)) {
    Serial.println("Livello: " + String(liv));
    Serial.println("Media marea 1");
    marea = 1;
  } else if (liv >= 250) {
    Serial.println("Livello: " + String(liv));
    Serial.println("Alta marea");
    marea = 2;
  }
}

// In base al numero di barche che ci sono in attesa viene regolato il timer di apertura del ponte:
// 0 barche: ponte chiuso
// 1 barca: ponte aperto per 30 secondi
// 2/3 barche: ponte aperto per 1 min.
// >= 4 barche: ponte aperto per 1 min. e 30 s.
// Prima della chiusura del ponte, appena viene rilevata la prima imbarcazione, trascorre un tempo di 60 secondi
void regolazioneFlussoCallback() {

  if((numBarcheinAttesa <= 0) & (first)) {
    // if utile nel caso in cui viene rilevata un'imbarcazione o più dopo la chiusura del ponte e tale/i imbarcazione/i è/sono stata/e in grado di passare sotto il
    // ponte a causa delle piccole dimensioni
    Serial.println("Disabilitato");
    sbarraDirigiTrafficoCloseTask.disable();
    ponteOpenTask.disable();
    first = false;
  } else if((!first) & (numBarcheinAttesa > 0)) {
    first = true;
    Serial.println("PASSATO"); 
    sbarraDirigiTrafficoCloseTask.enable();
    sbarraDirigiTrafficoCloseTask.restartDelayed(timeCars);
    ponteOpenTask.enable();
    ponteOpenTask.restartDelayed(timeCars + 1500);
  }

  if ((numBarcheinAttesa > 0) & (numBarcheinAttesa < 2)) {
    timeBoats = 30000;
  } else if ((numBarcheinAttesa >= 2) & (numBarcheinAttesa < 4)) {
    timeBoats = 60000;
  } else if ((numBarcheinAttesa >= 4)) {
    timeBoats = 90000;
  } else {
    numBarcheinAttesa = 0;
  }

  Serial.println("Time boats: " + String(timeBoats)); 
  Serial.println("Time cars: " + String(timeCars)); 
  Serial.println("First: " + String(first)); 
}