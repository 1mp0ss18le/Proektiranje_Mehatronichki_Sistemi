#define IN1 10
#define IN2 11
#define ENA 9
#define IN3 6
#define IN4 5
#define ENB 3
#define OUTA 12
#define OUTB 13
#define ENCROUTA 8
#define ENCROUTB 12
#define ENCLOUTA 2
#define ENCLOUTB 7

#define PI 3.14159265359 


// Vrednosti za halov senzor na enkoder
bool val_LA;
bool val_LB;
bool val_RA;
bool val_RB;

// Prethodni vrednosti za halov senzor na enkoder
bool prev_val_LA;
bool prev_val_LB;
bool prev_val_RA;
bool prev_val_RB;

// Prvi vrednosti za halov senzor na enkoder
bool prva_val_LA;
bool prva_val_LB;
bool prva_val_RA;
bool prva_val_RB;

// Promenlivi za detektiranje promeni kaj enkoderite 
bool promena_lev_enkoder = 0;
bool promena_desen_enkoder = 0;

// Promenlivi za pametenje na brojot na promeni vo kombinacijata na pulsevite na enkoderite
long broj_promeni_lev_enkoder = 0;
long broj_promeni_desen_enkoder = 0;

// Promenlivi za pametenje na brojot na ciklusi kaj enkoderite
long broj_ciklusi_lev_enkoder = 0;
long broj_ciklusi_desen_enkoder = 0;

// Promenlivi za pametenje na brojot na vrtezhi na trkalata
long broj_vrtezhi_levo_trkalo = 0;
long broj_vrtezhi_desno_trkalo = 0;


// Promenliva za chuvanje na brojot na loops
long broj_loop = 0;

// Promenlivi za dali se meri vremeto
bool merenje_vreme_levo = 0;
bool merenje_vreme_desno = 0;


// Promenlivi za chuvanje na vremeto
double pochetno_vreme_levo = 0;
double pochetno_vreme_desno = 0;

double segashno_vreme_levo = 0;
double segashno_vreme_desno = 0;
double dt_levo = 0;
double dt_desno = 0;


// Promenlivi za chuvanje na agolni brzini
double agolna_brzina_levo = 0;
double agolna_brzina_desno = 0;
 
// Bafer za chuvanje na reprezentacija na agolna brzina
char buffer[20];

// Vrednosti za PWM signali
float maxPWM = 255;
float incrPWM = 12.75;
float currentPWM = 255;

 


// Funkcija za presmetka na agolna brzina so izlez [rad/s]
double presmetka_agolna_brzina(double n, double dt)
{
  double agolna_brzina = (2 * PI) / (dt / 1000000) ;
  // Serial.print("vrtezhi ");
  // Serial.println(n);
  // Serial.print("vremenski interval ");
  // Serial.println(dt/1000000);
  return agolna_brzina;
}

void setup() 
{
  // Postavi baud rate 
  Serial.begin(9600);

  
  // Izlezi za L298N

  // Izlezi za desen motor
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  // Izlezi za lev motor
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Vlezovi za pulsevi od enkoder

  // Vlezovi za pulsevi od lev motor
  pinMode(ENCLOUTA,INPUT);
  pinMode(ENCLOUTB,INPUT);

  // Vlezovi za pulsevi od desen motor
  pinMode(ENCROUTA,INPUT);
  pinMode(ENCROUTB,INPUT);

  // Bitovi za odreduvanje nasoka na motorite

  // Bitovi za odreduvanje nasoka na desen motor
  analogWrite(ENA, 255);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  // Bitovi za odreduvanje nasoka na lev motor
  analogWrite(ENB, 0);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  // Start na merenje na vremeto
  pochetno_vreme_levo = micros();
  pochetno_vreme_desno = micros();
}

void loop() 
{
  //------------------- MOTOR -------------------------------- //
  
  

  
  // Otchituvanje na pulsevi od enkoder na lev motor
  val_LA = digitalRead(ENCLOUTA);
  val_LB = digitalRead(ENCLOUTB);

  // Ako e prv loop zachuvaj gi vrednostite na leviot enkoder
  if (broj_loop == 0)
  {
    prva_val_LA = val_LA;
    prva_val_LB = val_LB;
  }
  
  // proverka dali ima promena kaj lev enkoder 
  promena_lev_enkoder = (prev_val_LA != val_LA) || (prev_val_LB != val_LB);

  // Ako ima promena, ispechati ja promenata na leviot enkoder
  if (promena_lev_enkoder)
  {
    // Otchituvanje na kombinacii na lev enkoder
    // Serial.print("Enkoder levo A: ");
    // Serial.println(val_LA);
    // Serial.print("Enkoder levo B: ");
    // Serial.println(val_LB);
    // Serial.println();
    
    // Zgolemuvanje na brojot na promenite na leviot enkoder
    broj_promeni_lev_enkoder++;

    // Ako brojot na promeni na leviot enkoder e deliv so chetiri, togash zavrshil eden ciklus na pulsevi
    if (broj_promeni_lev_enkoder % 4 == 0)
    {
      broj_ciklusi_lev_enkoder++;
    }
    if (broj_ciklusi_lev_enkoder % 300 == 0 && broj_ciklusi_lev_enkoder != 0 && val_LA == prva_val_LA && val_LB == prva_val_LB)
    {
      broj_vrtezhi_levo_trkalo++;
      // Serial.println(broj_vrtezhi_levo_trkalo);
      // analogWrite(ENB, 0);


      // Presmetka na vreme za promena na sostojba na enkoder na levo trkalo
      // merenje_vreme_levo = false;
      segashno_vreme_levo = micros();
      dt_levo = (segashno_vreme_levo - pochetno_vreme_levo);

      // Pochni novo vreme
      pochetno_vreme_levo = micros();
      // Serial.println(pochetno_vreme_levo);
      // merenje_vreme_levo = true;

      // Presmetka na agolna brzina na levo trkalo
      agolna_brzina_levo = presmetka_agolna_brzina(broj_vrtezhi_levo_trkalo, dt_levo);
      currentPWM -= incrPWM;
      analogWrite(ENB, currentPWM);
      
      delay(2000);
      // dtostrf(dt_levo, 15, 9, buffer);
      // Serial.print("Agolna brzina levo: ");
      // Serial.println(agolna_brzina_levo);
    }

    
  }
  
  // Otchituvanje na pulsevi od enkoder na desen motor
  val_RA = digitalRead(ENCROUTA);
  val_RB = digitalRead(ENCROUTB);

   // Ako e prv loop zachuvaj gi vrednostite na desniot enkoder
  if (broj_loop == 0)
  {
    prva_val_RA = val_RA;
    prva_val_RB = val_RB;
  }

  // proverka dali ima promena kaj lev enkoder 
  promena_desen_enkoder = (prev_val_RA != val_RA) || (prev_val_RB != val_RB);

  // Ako ima promena, ispechati ja promenata na desniot enkoder
  if (promena_desen_enkoder)
  {

    // Otchituvanje na kombinacii na desen enkoder
    // Serial.print("Enkoder desno A: ");
    // Serial.println(val_RA);
    // Serial.print("Enkoder desno B: ");
    // Serial.println(val_RB);
    // Serial.println();

    // Zgolemuvanje na brojot na promenite na desniot enkoder
    broj_promeni_desen_enkoder++;

    // Ako brojot na promeni na desniot enkoder e deliv so chetiri, togash zavrshil eden ciklus na pulsevi
    if (broj_promeni_desen_enkoder % 4 == 0)
    {
      broj_ciklusi_desen_enkoder++;
    }
    
    // Posle sekoi 3 vrtezhi enkoderot pravi eden vrtezh, naporedno na sekoi 100 vrtezhi na enkoderot trkaloto pravi 1 vrtezh
    if (broj_ciklusi_desen_enkoder % 300 == 0 && broj_ciklusi_desen_enkoder != 0 && val_RA == prva_val_RA && val_RB == prva_val_RB)
    {
      broj_vrtezhi_desno_trkalo++;
      // Serial.println(broj_vrtezhi_desno_trkalo);
      // analogWrite(ENA, 0);

      // Presmetka na vreme za promena na sostojba na enkoder na desno trkalo
      // merenje_vreme_desno = false;
      segashno_vreme_desno = micros();
      dt_desno = (segashno_vreme_desno - pochetno_vreme_desno);
      pochetno_vreme_desno = micros();
      // Presmetka na agolna brzina na desno trkalo
      agolna_brzina_desno = presmetka_agolna_brzina(broj_vrtezhi_desno_trkalo, dt_desno);
      // Ispechati gi informaciite za PWM i agolna brzina
      Serial.print("| ");
      Serial.print(currentPWM);
      Serial.print(" | ");
      Serial.print(agolna_brzina_levo);
      Serial.print(" | ");
      Serial.print(agolna_brzina_desno);
      Serial.println(" |");
      currentPWM -= incrPWM;
      analogWrite(ENA, currentPWM);
      delay(2000);
      // dtostrf(agolna_brzina_desno, 15, 9, buffer);
      // Serial.print("Agolna brzina desno: ");
      // Serial.println(agolna_brzina_desno);
    }

    
  }
  
  // Zachuvuvanje na posledni vrednosti od halov senzor na enkoder
  prev_val_LA = val_LA;
  prev_val_LB = val_LB;
  prev_val_RA = val_RA;
  prev_val_RB = val_RB;


  // Inkrementiraj go brojot na loop
  broj_loop++;

  

  // Inkrementiraj / dekrementiraj PWM signal


}
