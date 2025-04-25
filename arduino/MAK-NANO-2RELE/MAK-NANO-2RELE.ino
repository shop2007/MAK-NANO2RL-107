String Versione = "MAK-NANO-2RELE";
/*

Note: 
kit https://www.ebay.it/itm/235901868338
pcb https://www.ebay.it/itm/375915351500
	
Per entrare nel menu scrivere "Menu" a 115200 baud

*/






/*
WATCHDOG32 BREAKOUT - NOTE HARDWARE https://www.ebay.it/itm/235363182136
Attenzione, ci sono tre versione di scheda WATCHDOG32 funzionalmente identiche
- 1) monta un transistor NPN Q2 con il lato piatto verso JP2, MARCATO BC317 (TO92 EBC)
- 2) monta un mosfet Q1 con il lato piatto veso R1/R2 , marcato BSS98 
- 3) monta un transistor PNP Q2 con il lato piatto verso JP2, MARCATO BC212 (TO92 CEB)

Le versioni 1 e 2 sono identiche
La versione 3 necessita di una inversione di segnale dichiarando la linea seguente
#define TransistorPNP 1

Il led di Arduino si accende quando ENABLE è attivo, ovvero, quando il reset di WATCHDOG32 è connesso al RESET di ARDUINO

Il tempo di sorveglianza del WATCHDOG è impostabile con un ponticello a goccia
https://www.analog.com/media/en/technical-documentation/data-sheets/ADM1232.pdf

- pin2 TD at GND = 150ms
- pin2 TD floating = 600msec
- pin2 TD at VCC = 1200ms
*/


//********************************************************************************************
// Costanti del circuito e della NTC
//dati rilevati 
//82 gradi fa 3500 ohm
//65 gradi fa 7600 ohm
//14 gradi fa 50000 ohm !!
//13 gradi fa 45200 ohm !!

const int ntcPin = A0; // Pin analogico a cui è collegato il partitore
const float rFixed = 22000.0; // Resistenza fissa in ohm
const float beta = 3578.0; // Coefficiente beta della NTC
const float t0 = 298.15; // Temperatura di riferimento (25°C in Kelvin)
const float r0 = 31583.0; // Resistenza della NTC a 25°C in ohm

int adcValue; //valore letto range 0-1023 bit
float voltage; // tensione al centrale del partitore
float rNtc; // resistenza della NTC
float temperatureK; // temperatura in Kelvin usando la formula di Steinhart-Hart semplificata
float temperatureC; // Converti la temperatura in Celsius

//********************************************************************************************
//Pinout di Arduino
#define Buzzer 2 // cicalino
#define Led_giallo 3
#define Led_verde 4 //led D1 r1
#define Rele_lettura_bulbo 5
#define Rele_bulbo 6
#define Trigger_Wdog 7  //TRIGGER CONNETTERE AL PIN 3 TRG della scheda WATCHDOG32
#define Free8 8 //non usato
#define Enable_Wdog 9   //ENABLE  CONNETTERE AL PIN 2 TRG della scheda WATCHDOG32
#define Tracepin3 10
#define Tracepin4 11
#define Tracepin5 12
#define Tracepin6 13
//
#define AdcTrimmer A0
#define AFree1 A1
#define AFree2 A2
#define AdcTeCentralina A3
#define AdcTeBulbo A4

// Definizione delle frequenze per le note (in Hz)
#define NOTE_DO 523  // Do - 523 Hz
#define NOTE_RE 587  // Re - 587 Hz
#define NOTE_MI 659  // Mi - 659 Hz
#define NOTE_FA 698  // Fa - 698 Hz
#define NOTE_SOL 784 // Sol - 784 Hz
#define NOTE_LA 880 // Sol - 784 Hz
#define NOTE_WAIT 1200 //

int noteDuration = 500; //durata delle note
//********************************************************************************************
// Dichiarazione della variabile per tenere traccia dello stato del pin


String inputString = "";      // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
bool ExitFromMenu = false;    // Used for exit from loop 
char Cestino;                 // 
const long interval = 1000;           // interval at which to blink (milliseconds)

bool debug = false;
String inputBuffer = "";       // Buffer per memorizzare i caratteri ricevuti

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;  // will store last time LED was updated

// Costanti del circuito e del NTC
const float VREF = 5.0; // Tensione di riferimento
const int ADC_MAX = 1023; // Valore massimo dell'ADC
const float R_FIXED = 22000.0; // Resistenza fissa in ohm

// Caratteristiche del sensore NTC
const float T1 = 65.0; // Temperatura a cui il NTC misura R1
const float R1 = 7600.0; // Resistenza del NTC a T1
const float T2 = 13.0; // Temperatura a cui il NTC misura R2
const float R2 = 45200.0; // Resistenza del NTC a T2

// Costante beta del sensore
const float BETA = log(R1 / R2) / ((1.0 / (T1 + 273.15)) - (1.0 / (T2 + 273.15)));

/*
const int CicliPrimoFiltro = 2; // Numero di campioni per ogni lettura
const int CicliGranMedia = 2; // Numero totale di cicli
unsigned long adcGrandTotal = 0; // Somma totale ADC
float voltageGrandTotal = 0.0; // Somma totale delle tensioni
float temperatureGrandTotal = 0.0; // Somma totale delle temperature
*/
/*
// Calcolo delle grandi medie
float adcGrandAverage = (float)adcGrandTotal / CicliGranMedia;
float voltageGrandAverage = voltageGrandTotal / CicliGranMedia;
float temperatureGrandAverage = temperatureGrandTotal / CicliGranMedia;
*/
//------------------------------------

//PROTOTIPI
void Azione1(void);
void Azione2(void);
void Azione3(void);
void Azione4(void);
void Azione5(void);
void Azione6(void);
void Azione7(void);
void Azione8(void);
void Azione9(void);
void Azione10(void);
void Azione11(void);
void Azione12(void);
void Azione13(void);
void Azione14(void);
void Azione15(void);
void Azione16(void);
void Azione17(void);
void Azione18(void);
void Azione19(void);
void Azione20(void);
void Azione99(void);

//------------------------------------


// Funzione per suonare una nota
void playTone(int frequency, int duration) {
  tone(Buzzer, frequency, duration); // Genera il suono sul pin
  delay(duration);                      // Attendi la durata della nota
  noTone(Buzzer);                    // Ferma il suono
}
//********************************
//verifica se devo entrare nel menu
void CheckIfMenu(){
  // Controlla i dati ricevuti sulla seriale
  if (Serial.available() > 0) {
    char receivedChar = Serial.read(); // Legge un carattere
    inputBuffer += receivedChar;       // Aggiunge il carattere al buffer

    // Verifica se il buffer contiene la parola "Menu"
    if (inputBuffer.endsWith("Menu")) {
      Serial.println("Comando 'Menu' rilevato! Entrando nella funzione Menu().");
      Menu(); // Chiama la funzione Menu()
      inputBuffer = ""; // Resetta il buffer
    }

    // Evita che il buffer diventi troppo lungo
    if (inputBuffer.length() > 20) {
      inputBuffer = ""; // Resetta il buffer in caso di overflow
    }
  }

}


//-------------------------------------------------------------------
void loop(){

//SCRIVI QUI IL TUO LOOP




}

//------------------------------------
/*
Nei sistemi che utilizzano numeri a virgola mobile, confrontare due numeri float direttamente usando l'operatore di uguaglianza == può 
 essere problematico a causa delle imprecisioni legate alla rappresentazione in virgola mobile.
Per confrontare due numeri float, è meglio verificare se la differenza assoluta tra i due numeri è inferiore a una soglia (detta epsilon). 
Questo metodo consente di considerare due numeri "uguali" se sono sufficientemente vicini tra loro.
*/
bool compareFloats(float a, float b, float epsilon = 0.0001) {
  return fabs(a - b) < epsilon;
}
//------------------------------------

void LeggeAdcGrezzo(int QualeAdc){
  Serial.print("Lettura ADC ");Serial.println(QualeAdc);
  ClearSerialBuffer();
  while (true) {
    // Controlla se c'è un dato disponibile sulla seriale
    if (Serial.available() > 0) {
      char receivedChar = Serial.read(); // Legge il carattere inviato via seriale
      if (receivedChar == 'Q' || receivedChar == 'q') {
        Serial.println("Interruzione della lettura.");
        break; // Esce dal ciclo e dalla funzione
      }
    }
    Serial.println(analogRead(QualeAdc));
  }
}
//------------------------------------
void ReadTemperature(int QualeAdc, bool repeat, bool printdebug){
  ClearSerialBuffer();
  while (true) {
    // Controlla se c'è un dato disponibile sulla seriale
    if (Serial.available() > 0) {
      char receivedChar = Serial.read(); // Legge il carattere inviato via seriale
      if (receivedChar == 'Q' || receivedChar == 'q') {
        Serial.println("Interruzione della lettura");
        break; // Esce dal ciclo e dalla funzione
      }
    }
    Adc2Temperature(QualeAdc, printdebug);
    if (!repeat){
      break; // Esce dal ciclo e dalla funzione
    }
  }
}




//--------------------------------------------------------------------------
void Adc2Temperature(int QualeAdc, bool printdebug){
    adcValue = analogRead(QualeAdc);

    //inverte il senso del trimmer
    if(QualeAdc==AdcTrimmer){
      adcValue=1023-adcValue;
    }
   
    voltage = (adcValue / 1023.0) * 5.0; // tensione al centrale del partitore
    rNtc = (rFixed * voltage) / (5.0 - voltage); // resistenza della NTC
    temperatureK = 1.0 / ((1.0 / beta) * log(rNtc / r0) + (1.0 / t0)); // temperatura in Kelvin usando la formula di Steinhart-Hart semplificata
    temperatureC = temperatureK - 273.15; // Converti la temperatura in Celsius

    // Stampa la temperatura sul monitor seriale
    if (printdebug){
      Serial.print("ADC: ");Serial.print(QualeAdc);
	    Serial.print(" - adcValue: "); Serial.print(adcValue);
      Serial.print(" - voltage: "); Serial.print(voltage);
      Serial.print(" - rNtc: "); Serial.print(rNtc);
      Serial.print(" - temperatureC: "); Serial.print(temperatureC);   
      Serial.println(" °C");
    }

}












//-------------------------------------------------------------------

// Funzione per calcolare tensione e temperatura dalla media ADC
void CalculateValues(int adcValue, float &voltage, float &temperature) {
  voltage = (adcValue * VREF) / ADC_MAX; // Calcolo della tensione
  float rNtc = R_FIXED * ((VREF / voltage) - 1.0); // Resistenza del NTC
  float tempK = 1.0 / ((1.0 / (T1 + 273.15)) + (log(rNtc / R1) / BETA)); // Temperatura in Kelvin
  temperature = tempK - 273.15; // Conversione in Celsius
}
//-------------------------------------------------------------------



// funzioni
//---------------------------------------------------------
void SirenaFrancese(){


  noteDuration=400;
  Retrigger_Wdog();
  playTone(NOTE_MI, noteDuration);
  Retrigger_Wdog();
  playTone(NOTE_LA, noteDuration);
  Retrigger_Wdog();
  playTone(NOTE_MI, noteDuration);
  Retrigger_Wdog();
  playTone(NOTE_LA, noteDuration);
  Retrigger_Wdog();
  noTone(8); //fermo il tono

}
//---------------------------------------------------------
void SirenaFranceseVeloce(){

  noteDuration=250;
  Retrigger_Wdog();
  playTone(NOTE_MI, noteDuration);
  Retrigger_Wdog();
  playTone(NOTE_LA, noteDuration);
  Retrigger_Wdog();
  playTone(NOTE_MI, noteDuration);
  Retrigger_Wdog();
  playTone(NOTE_LA, noteDuration);
  Retrigger_Wdog();
  playTone(NOTE_MI, noteDuration);
  Retrigger_Wdog();
  playTone(NOTE_LA, noteDuration);
  Retrigger_Wdog();
  noTone(8); //fermo il tono

}

//---------------------------------------------------------
//lampeggia ed attendein loop il tasto Q per continuare
void BlinkWaitQ(void){
  while (1) {
    char inChar = (char)Serial.read();

    if (inChar == 'q'){
      break;
    }

    delay(500);                       // wait for a second

    if (inChar == 'Q'){
      break;
    }
    
    delay(500); 
  
  }
}
//---------------------------------------------------------
void BlinkRetriggerWaitQ(){
  while (1) {
    digitalWrite(Tracepin5, HIGH);

    Retrigger_Wdog();
    char inChar = (char)Serial.read();
    if (inChar == 'q'){
      break;
    }

    delay(200);                       // wait for a second

    Retrigger_Wdog();    
    if (inChar == 'Q'){
      break;
    }
    delay(200); 
  }
  

  
  
}




//*******************************************************


void Retrigger_Wdog(void) {
  digitalWrite(Trigger_Wdog, HIGH);
  delay(40);
  digitalWrite(Trigger_Wdog, LOW);
  delay(5);
}
//---------------------------------------------------------

void Abilita_Wdog(void) {

  Serial.println("WDOG abilitato");
    digitalWrite(LED_BUILTIN, HIGH);    // turn the ARDUINO LED ON by making the PIN voltage HIGH
  #ifdef TransistorPNP
    digitalWrite(Enable_Wdog, LOW);
  #else
    digitalWrite(Enable_Wdog, HIGH);
  #endif    

}
//---------------------------------------------------------

void Disabilita_Wdog(void) {

  Serial.println("WDOG disabilitato");
  digitalWrite(LED_BUILTIN, LOW);    // turn the ARDUINO LED off by making the pin voltage LOW
  #ifdef TransistorPNP
    digitalWrite(Enable_Wdog, HIGH);
  #else
    digitalWrite(Enable_Wdog, LOW);  
  #endif
}
//---------------------------------------------------------

//---------------------------------------------------------

//---------------------------------------------------------



void ConnettiBulbo(){
  digitalWrite(Rele_lettura_bulbo, LOW); // 
  digitalWrite(Rele_bulbo, LOW); 
}

void SconnettiBulbo(){
  digitalWrite(Rele_lettura_bulbo, HIGH); // 
  digitalWrite(Rele_bulbo, HIGH); 
}




//----------------------
void ClearSerialBuffer() {
  while (Serial.available() > 0) {
    Serial.read(); // Legge e scarta i dati nel buffer
  }
}
//----------------------
void Buzzer_on(){
  digitalWrite(Buzzer, HIGH); // 4
}  

void Buzzer_off(){
  digitalWrite(Buzzer, LOW); // 4
}  
//---------------
void Led_giallo_on(){
  digitalWrite(Led_giallo, HIGH); // 4
}  

void Led_giallo_off(){
  digitalWrite(Led_giallo, LOW); // 4
}  
//---------------
void Led_verde_on(){
  digitalWrite(Led_verde, HIGH); // 
}  

void Led_verde_off(){
  digitalWrite(Led_verde, LOW); // 
}  
//---------------
void Rele_lettura_bulbo_on(){
  digitalWrite(Rele_lettura_bulbo, HIGH); // 
}  

void Rele_lettura_bulbo_off(){
  digitalWrite(Rele_lettura_bulbo, LOW); // 
}  
//---------------
void Rele_bulbo_on(){
  digitalWrite(Rele_bulbo, HIGH); // 
}  

void Rele_bulbo_off(){
  digitalWrite(Rele_bulbo, LOW); // 
}  
//---------------

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void setup() {

  //pinMode(Buzzer, OUTPUT); // 3 // cicalino
  pinMode(Led_giallo, OUTPUT); // 4
  pinMode(Led_verde, OUTPUT); // 5
  pinMode(Rele_lettura_bulbo, OUTPUT); // 6
  pinMode(Rele_bulbo, OUTPUT); // 7
  pinMode(Trigger_Wdog, OUTPUT); // 8  //TRIGGER CONNETTERE AL PIN 3 TRG della scheda WATCHDOG32
  pinMode(Enable_Wdog, OUTPUT); // 9   //ENABLE  CONNETTERE AL PIN 2 TRG della scheda WATCHDOG32
  pinMode(Tracepin3, OUTPUT);
  pinMode(Tracepin4, OUTPUT);
  pinMode(Tracepin5, OUTPUT);
  pinMode(Tracepin6, OUTPUT);

  digitalWrite(Buzzer, LOW); // 3 // cicalino
  digitalWrite(Led_giallo, LOW); // 4
  digitalWrite(Led_verde, LOW); // 5
  digitalWrite(Rele_lettura_bulbo, LOW); // 6
  digitalWrite(Rele_bulbo, LOW); // 7
  digitalWrite(Trigger_Wdog, LOW); // 8  //TRIGGER CONNETTERE AL PIN 3 TRG della scheda WATCHDOG32
  digitalWrite(Enable_Wdog, LOW); // 9   //ENABLE  CONNETTERE AL PIN 2 TRG della scheda WATCHDOG32
  digitalWrite(Tracepin3, LOW); //
  digitalWrite(Tracepin4, LOW);
  digitalWrite(Tracepin5, LOW);
  digitalWrite(Tracepin6, LOW);


   // set the digital pin as output:
   Disabilita_Wdog();
   pinMode(LED_BUILTIN, OUTPUT);

//erial.println( F ("Attivato"));    
   //start serial connection
  Serial.begin(115200); // terminale
  Serial.println();
  Serial.println( F ("|******   SISTEMA RESETTATO  ******|"));
  Serial.println( F (" Sketch build xxxxx "));
  Serial.print( F ("Versione "));Serial.println(Versione);
  Serial.println();
    Serial.println( F ("|buzzer disabilitato|"));


}

//--******************************************************************************************

void(* resetFunc) (void) = 0; //declare reset function @ address 0

//-------------------------------------------


void loopDebug() {
  //--------------------------------------
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
  }
  //--------------------------------------
  
  Menu();
}

//*******************************************************
/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
void TypeMenuList(void){
    Serial.println();
    Serial.println( F ("|*******************************************"));
    Serial.println( F ("|             ʘ‿ʘ   Menù   (◡_◡)          "));
      Serial.print( F ("|  Ver. "));Serial.println(Versione);
    Serial.println( F ("|  nessuno sta retriggerando il Watchdog32, "));
    Serial.println( F ("|  il led rosso WDOG32 continua a blinkare  "));
    Serial.println( F ("|*******************************************"));
    Serial.println( F ("  0 Reset"));
    Serial.println( F ("  1 Retriggera Watchdog32 ogni 100msec"));
    Serial.println( F ("  2 Abilita Watchdog32 senza retriggerare"));
    Serial.println( F ("  3 Abilita, triggera 5 secondi e poi ferma il trigger, deve resettare"));
    Serial.println( F ("  4 Abilita, triggera con tempi crescenti, deve resettare"));
    Serial.println( F ("  5 Aziona relè bulbo per 3 secondi"));
    Serial.println( F ("  6 Aziona relè lettura bulbo per 3 secondi"));
    Serial.println( F ("  7 Accende led giallo per 3 secondi"));
    Serial.println( F ("  8 Accende led verde per 3 secondi"));
    Serial.println( F ("  9 Suona il cicalino"));
    Serial.println( F ("  10 Temperatura trimmer gradi (Q)"));
    Serial.println( F ("  11 Legge bit ADC trimmer (Q)"));
    Serial.println( F ("  12 Temperatura bulbo sconnesso (Q)"));
    Serial.println( F ("  13 Legge bit ADC bulbo sconnesso (Q)"));
    Serial.println( F ("  14 Temperatura bulbo connesso (Q)"));
    Serial.println( F ("  15 Legge bit ADC bulbo connesso (Q)"));
    Serial.println( F ("  16 Loop principale"));
      Serial.print( F ("  17 Debug is "));
      if(debug){
        Serial.println(" ON");
      }else{
        Serial.println(" OFF");
      }
        
   Serial.println( F ("  18 Legge tutti gli ADC"));
   Serial.println( F ("  19 Simula funz. NTC 0..1023 bit adc"));
   Serial.println( F ("  20 Test tutti gli output di nano, da D2 a d13"));
   Serial.println( F (" 99 Torna al loop senza reset"));
  
}
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void Menu() {
  Disabilita_Wdog(); //ALTRIMENTI RESETA
  //resta nel menu fino a che premi 0
  while (!ExitFromMenu) {
    Disabilita_Wdog();
    TypeMenuList();

    //svuoto il buffer
    while (Serial.available()) {
      Cestino = Serial.read();
    }

    Serial.println();
    // qui ciclo attesa e Flash fino a che non c'è un carattere sulla seriale
    while (!Serial.available()) {
      
      //--------------------------------------
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval/4) {
        previousMillis = currentMillis;   
      }     
    }

    // arrivato un carattere
    int CmdMenu = Serial.parseInt();
    Serial.print( F ("ricevuto CmdMenu "));Serial.println(CmdMenu);

    switch (CmdMenu){
      //-------------------------------------------------
      case 0:

        Serial.println( F ("reset tra 1 sec"));
        delay(1000);               // wait for a second
        resetFunc();  //call reset

        delay(100);
        Serial.println( F ("Il reset non ha funzionato"));
      break;
      //-------------------------------------------------
      case 1:
        //    Serial.println("  1 Retriggera Watchdog32 ogni 200msec");
        Azione1();
      break;
      //-------------------------------------------------
      case 2:
        //    Serial.println("  2 Abilita Watchdog32 senza retriggerare");
        Azione2();
      break;
      //-------------------------------------------------
      case 3:
        //Serial.println( F ("  3 Abilita, triggera 5 secondi e poi ferma il trigger, deve resettare"));
        Azione3();
      break;
      //-------------------------------------------------
      case 4:
        //    Serial.println( F ("  4 Abilita, triggera con tempi crescenti, deve resettare"));      
        Azione4();
      break;
      //-------------------------------------------------
      case 5:
        //    5 Aziona relè bulbo per 3 secondi      
        Azione5();
      break;
      //-------------------------------------------------
      case 6:
        //    6 Aziona relè lettura bulbo per 3 secondi"     
        Azione6();
      break;
      //-------------------------------------------------
      case 7:
        //     7 Accende led giallo per 3 secondi"));  
        Azione7();
      break;
      //-------------------------------------------------
      case 8:
        //    8 Accende led verde per 3 secondi"));      
        Azione8();
      break;
      //-------------------------------------------------
      case 9:
        //    9 Suona il cicalino"));      
        Azione9();
      break;
      //-------------------------------------------------
      case 10:
        //     10 Legge il trimmer"));     
        Azione10();
      break;
      //-------------------------------------------------
      case 11:
        //     11 Legge il bulbo"));
        Azione11();
      break;
      //-------------------------------------------------
      case 12:
        //     11 Legge il bulbo"));
        Azione12();
      break;
      //-------------------------------------------------
      case 13:
        //     11 Legge il bulbo"));
        Azione13();
      break;
      //-------------------------------------------------
      case 14:
        //     11 Legge il bulbo"));
        Azione14();
      break;
      //-------------------------------------------------
      case 15:
        //     11 Legge il bulbo"));
        Azione15();
      break;
      //-------------------------------------------------
      case 16:
        //     11 Legge il bulbo"));
        Azione16();
      break;
      //-------------------------------------------------
      case 17:
        //     swap debug;
        Azione17();
      break;
      //-------------------------------------------------
      case 18:
        //     swap debug;
        Azione18();
      break;
      //-------------------------------------------------
      case 19:
        //     swap debug;
        Azione19();
      break;
      //-------------------------------------------------
      case 20:
        //     swap debug;
        Azione20();
      break;
      //-------------------------------------------------


      case 99:
        Azione99();
      break;
      //-------------------------------------------------

      default:
        //Serial.println( F ("Unrecognized command, try again!"));
        Serial.println( F ("Comando errato! Riprova"));
    } //Serial.available
  } // ExitFromMenu
} // Menu


//*******************************************************

//--------------------------------------------------------------------------
void Azione1(){
  pinMode(Trigger_Wdog, OUTPUT);
  Serial.println( F ("  il led rosso della scheda Watchdog32 si deve spegnere"));
  Serial.println( F ("  Arduino non si deve resettare"));
  Serial.println( F ("invia Q quando vuoi per uscire"));
  BlinkRetriggerWaitQ(); 
}
//--------------------------------------------------------------------------
void Azione2(){
  Serial.println( F (" il Watchdog32 viene connesso al reset"));
  Serial.println( F (" nessuno triggera il Watchdog32"));
  Serial.println( F (" il led rosso della scheda Watchdog32 continua a blinkare"));
  //Serial.println( F (" tra un secondo dovrebbe resettarsi Arduino"));
  Serial.println( F (" togli il jumper W1 e invia Q"));
  BlinkWaitQ();

  
  delay(1000);
  pinMode(Enable_Wdog, OUTPUT);
  Abilita_Wdog();
  Serial.println( F (" inserisci jumper W1, e in un secondo dovrebbe resettarsi Arduino"));
  delay(2000);
  Serial.println( F (" se non si è resettato invia Q"));
  BlinkWaitQ();
  Serial.println( F (" qualcosa non ha funzionato, invia Q per tornare al menu"));
  BlinkWaitQ();
  Disabilita_Wdog();
 
  
}
//--------------------------------------------------------------------------
void Azione3(){
  pinMode(Trigger_Wdog, OUTPUT);
  Serial.println( F (" il led rosso della scheda Watchdog32 si deve spegnere"));
  Serial.println( F (" inserisci il jumper W1 e poi invia Q per continuare"));
  pinMode(Enable_Wdog, OUTPUT);
  digitalWrite(Tracepin3, HIGH);

  Abilita_Wdog();
  BlinkRetriggerWaitQ();
  digitalWrite(Tracepin5, LOW);

  Serial.println( F (" tra 5 secondi il trigger verrà tolto"));
  digitalWrite(Tracepin6, HIGH);
  for(int i = 1; i<6; i++){
    for(int j = 1; j<10; j++){
      Retrigger_Wdog();
      delay(100);
      Serial.print(".");  
    }
    Serial.print(i);
    
    if(i==5){
      digitalWrite(Tracepin4, HIGH);

      Serial.println();
      Serial.println( F ("Tra un secondo si deve resettare"));
      Serial.println( F ("Se non resetta invia Q per continuare"));
      digitalWrite(Tracepin6, LOW);

    }
  }


  BlinkWaitQ();
  Serial.println( F (" qualcosa non ha funzionato, invia Q per tornare al menu"));
  BlinkWaitQ();
  Disabilita_Wdog();
  digitalWrite(Tracepin3, LOW);
  digitalWrite(Tracepin4, LOW);
  digitalWrite(Tracepin5, LOW);
  digitalWrite(Tracepin6, LOW);  
  
}
//--------------------------------------------------------------------------
void Azione4(){
  pinMode(Trigger_Wdog, OUTPUT);
  Serial.println( F (" il led rosso della scheda Watchdog32 si deve spegnere"));
  Serial.println( F (" inserisci il jumper W1 e poi invia Q per continuare"));
  
  pinMode(Enable_Wdog, OUTPUT);
  for(int i = 1; i<10; i++){
    Retrigger_Wdog();
    delay(100);
  }

  
  Abilita_Wdog();
  BlinkRetriggerWaitQ();
  Serial.println( F (" inizio a triggerare a tempi cresenti,deve resettare"));


  for (int t=250;t<3000;t=t+10){
    Serial.println(t);
    Retrigger_Wdog();
    Abilita_Wdog();

    //accelera
    if (t>750){
      t=t+15;
    }

    //accelera
    if (t>1000){
      t=t+50;
    }
     
    delay(t);
      
    // Utilizza digitalWrite() per impostare lo stato del pin
    if (t>1500){
      Serial.println();
      Serial.println("  XXXXXXXXXXXXXXX errore XXXXXXXXXXXXXXX  ");
      Serial.println(" (◡_◡) il tempo ha superato i 1500msec, e non si è generato un Reset");
      Serial.println(" - verifica se è stato inserito il jumper W1");
      Serial.println(" - la scheda non sta funzionando bene");
      Serial.println(" - verifica saldature e montaggio");
      while(1){
              //attesa infinita di un reset
      }
    }
  }
                    // wait for a second
  BlinkWaitQ();
  Serial.println( F (" qualcosa non ha funzionato, invia Q per tornare al menu"));
  BlinkWaitQ();
  Disabilita_Wdog();
}
//--------------------------------------------------------------------------   
void Azione5(){
  //Serial.println( F ("  5 Aziona relè bulbo per 3 secondi"));
  Rele_bulbo_on();
  delay(3000);
  Rele_bulbo_off();
}
//--------------------------------------------------------------------------
void Azione6(){
  //Serial.println( F ("  6 Aziona relè lettura bulbo per 3 secondi"));
  Rele_lettura_bulbo_on();
  delay(3000);
  Rele_lettura_bulbo_off();
}
//--------------------------------------------------------------------------
void Azione7(){
  //Serial.println( F ("  7 Accende led giallo per 3 secondi"));
  Led_giallo_on();
  delay(3000);
  Led_giallo_off();  
}
//--------------------------------------------------------------------------
void Azione8(){
  //Serial.println( F ("  8 Accende led verde per 3 secondi")); 
  Led_verde_on();
  delay(3000);
  Led_verde_off();     
}
//--------------------------------------------------------------------------
void Azione9(){


  SirenaFranceseVeloce();

  
}

//--------------------------------------------------------------------------
void Azione10(){
    //10 Legge il trimmer gradi (Q)
  ReadTemperature(AdcTrimmer, true, true);
}
//--------------------------------------------------------------------------
void Azione11(){
  //11 Legge il trimmer grezzo (Q)
  LeggeAdcGrezzo(AdcTrimmer);  
}
//--------------------------------------------------------------------------
//#define AdcTeCentralina A3
//#define AdcTeBulbo A4

void Azione12(){
  //12 Legge il bulbo sconnesso (Q)
  SconnettiBulbo();
  ReadTemperature(AdcTeBulbo, true, true);
  ConnettiBulbo();
    
}
//--------------------------------------------------------------------------
//#define AdcTeCentralina A3
//#define AdcTeBulbo A4

void Azione13(){
  //13 Legge il bulbo grezzo sconnesso (Q)
  SconnettiBulbo();
  LeggeAdcGrezzo(AdcTeBulbo);
  ConnettiBulbo();
}
//--------------------------------------------------------------------------
//#define AdcTeCentralina A3
//#define AdcTeBulbo A4

void Azione14(){
  //14 Legge il bulbo connesso (Q)
  ConnettiBulbo();
  ReadTemperature(AdcTeCentralina, true, true);
}
//--------------------------------------------------------------------------


void Azione15(){
  //15 Legge il bulbo grezzo connesso (Q)
  ConnettiBulbo();
  LeggeAdcGrezzo(AdcTeCentralina); 
}


//--------------------------------------------------------------------------

void Azione16(){
  //
  //Loop();    
  Serial.println("Salto al loop eliminato dopo il debug");
}
//--------------------------------------------------------------------------

void Azione17(){
  //swap debug
  if (debug){
    debug=false;
  }else {
    debug=true;
  }
}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void Azione18(){

  uint16_t adcValues[8];
  ClearSerialBuffer();
  while (true) {
    // Controlla se c'è un dato disponibile sulla seriale
    if (Serial.available() > 0) {
      char receivedChar = Serial.read(); // Legge il carattere inviato via seriale
      if (receivedChar == 'Q' || receivedChar == 'q') {
        Serial.println("Interruzione della lettura.");
        break; // Esce dal ciclo e dalla funzione
      }
    }

    // Leggi i valori dai pin analogici A0-A7
    for (int i = 0; i < 8; i++) {
      adcValues[i] = analogRead(i); // Leggi il valore ADC dal pin i
    }
    
    Serial.println();
    Serial.println("  A0    A1    A2    A3    A4    A5    A6    A7");

    // Invia i valori in esadecimale su una riga al terminale seriale
    for (int i = 0; i < 8; i++) {
      Serial.print("0x"); // Aggiungi uno spazio tra i valori
      if (adcValues[i] < 0x100) {
        Serial.print("0"); // Aggiungi uno zero iniziale se il valore è inferiore a 0x10
      }
      if (adcValues[i] < 0x10) {
        Serial.print("0"); // Aggiungi uno zero iniziale se il valore è inferiore a 0x10
      }
      Serial.print(adcValues[i], HEX); // Stampa il valore in esadecimale
      if (i < 7) {
        Serial.print(" "); // Aggiungi uno spazio tra i valori
      }
    }
    Serial.println(); // Vai a capo dopo aver inviato tutti i valori

    delay(250); // Aspetta 500 ms prima della prossima lettura

  }
}

//--------------------------------------------------------------------------

void Azione19(){

  Serial.println("Prima lo simulo da 0 a 0x3ff");
  for(int adcValue=0; adcValue<1024; adcValue++){

    // Calcola la tensione al centrale del partitore
    voltage = (adcValue / 1023.0) * 5.0;

    // Calcola la resistenza della NTC
    rNtc = (rFixed * voltage) / (5.0 - voltage);

    // Calcola la temperatura in Kelvin usando la formula di Steinhart-Hart semplificata
    temperatureK = 1.0 / ((1.0 / beta) * log(rNtc / r0) + (1.0 / t0));

    // Converti la temperatura in Celsius
    temperatureC = temperatureK - 273.15;

    // Stampa la temperatura sul monitor seriale
    Serial.print("adcValue: "); Serial.print(adcValue);
    Serial.print(" - voltage: "); Serial.print(voltage);
    Serial.print(" - rNtc: "); Serial.print(rNtc);
    Serial.print(" - temperatureC: "); Serial.print(temperatureC);   
    Serial.println(" °C");
	}	


  for(int adcValue=0; adcValue<1024; adcValue++){
    Adc2Temperature(AdcTrimmer, true);
	}	


}


//--------------------------------------------------------------------------

void Azione20(){
  const int startPin = 2;  // Primo pin digitale disponibile
  const int endPin = 13;  // Ultimo pin digitale disponibile

  Serial.println("Testa gli outut a rotazione (solo quelli configuati)... q termina");

  ClearSerialBuffer();
  while (true) {
    //Serial.print(".");
    // Controlla se c'è un dato disponibile sulla seriale
    if (Serial.available() > 0) {
      char receivedChar = Serial.read(); // Legge il carattere inviato via seriale
      if (receivedChar == 'Q' || receivedChar == 'q') {
        Serial.println("Interruzione della lettura.");
        break; // Esce dal ciclo e dalla funzione
      }


    }
    Serial.println();
    for (int pin = startPin; pin <= endPin; pin++) {
        digitalWrite(pin, HIGH); // Accendi il pin corrente
        delay(250);        // Aspetta per 200 ms
        digitalWrite(pin, LOW);  // Spegni il pin corrente
        Serial.print(" D");Serial.print(pin);
    }
  }
}


//--------------------------------------------------------------------------



void Azione99(){
  Serial.println( F ("E' stata scelta l'azione n. 99"));
  delay(2000);
  Serial.println( F ("torno al loop'"));
  delay(2000);
  ExitFromMenu = true;

  
}
//--------------------------------------------------------------------------
void CollaudoFunzione(int QualeAdc){
  Serial.println("----------------");
  Serial.print("ADC:"); Serial.println(analogRead(QualeAdc));

  Serial.print(" - altro "); 
}


