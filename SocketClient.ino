//#include <SPI.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <math.h>
#include <EEPROM.h>


Ticker timer0;

const int oneSecInUsec = 1000000;   // A second in mirco second unit.
bool toggle = 0;                    // The LED status toggle
//int time;                           // the variable used to set the Timer

/************  Função PID  ************/
#define TEMPO_ACT_PID           250
#define GANHO_PROPORCIONAL      27
#define GANHO_INTEGRAL          15
#define GANHO_DERIVATIVO        18

//const char* ssid = "MasJaq";
//const char* password = "";

//const char* ssid = "bluedash";
//const char* password = "bluedash";

//const char* ssid = "thiago";
//const char* password = "thiagomoreira";

//const char* ssid = "Injetec";
//const char* password = "injetec123";

//const char* ssid = "teste";
//const char* password = "smartlogger2017";

//const char* ssid = "Magnesita_Maq100";
//const char* password = "smartlogger2017";

const char* ssid = "Injetec_Teste105";
const char* password = "smartlogger2017";

//const char* ssid = "desenvolvimento";
//const char* password = "smartrouter";
 

   //pino Entrada pos Chave
  const byte posChavePin = 14;
  
  //pino Entrada Sensor Cinto de Segurança
  const byte sensorCintoPin = 12;  
  
  //pino Entrada Sensor Luz de Marcha Ré
  const byte luzMarchaRePin = 16;  
    
  const byte Rele = 4;
  const byte LEDsinal = 5;
  const byte interruptPin = 13;
  //const byte eepromCS = 15;

  
/***********  PID   *****************/

//int Parcela_Proporcional,Parcela_Integral,Parcela_Derivativa,Erro_Velocidade = 6;
//int Erro_Velocidade_Ant = 0,Tempo_Acionamento,Tempo_Desacionamento;
//int Velocidade_Limite = 14;  
//int numeroDentesCoroa = 40;
int Flag200ms, contador1ms=0,contadorLoopInicial=0, contador1seg=0;
double DiametroPneu=0.7;
boolean Flag250ms=false,flag1s=false,resleStatus=0;

/***********************************/

int count=0; 
int countPulse=0;     //Contador de pulsos instantâneo
int pulsosContados=0; // Pulsos contados a cada 200ms
int valorAD=0;        // Valor de AD - referência 1Volt
int statusRele=0;     //status do Relé - 0= desatracado , 1= Atracado
int statusSinalizacao=0;   //status Sinalizacao - 0 = deslidgada , 1 = Ligada
int posChave=1;       //indicador de tensão pós chave - 1 com tensão, 0 máquina desligada.
int luzMarchaRe=0;       //indicador de Luz de marcha ré acesa - .
int sensorCinto=0;       //indicador de Sensor de Cinto de Segurança atracado
int timeoutWiFi=0;    //Timeout ponto de acesso WiFi
//int status = 0;       //Status do WiFI
int contEstabelecendoConn=0; //contador de tempo para socket não encontrado

int sensorPulsos = 0; //Contador de pulsos do sensor de Velocidade ou Rotação
int ligDesl=0;
int ligaDesligaReleCinto = 0;

unsigned long horimetro;// 4 bytes para contador de horimetro, em segundos
uint8_t ponteiroHorimetro;//
boolean posChaveEvent=false;
boolean SinalizaHoraEfetiva=false;
boolean SinalizaGoto=false;
boolean ReleCintoAlerta=false;
String  eventoComando="not";
byte addr=0; //endereço eeprom

String horimetroSub; 


/*********************************************/
/*********   mensagens  **********************/
const char* ID_NAO_IDENTIFICADO = "ID não identificado.";

// Use WiFiClient class to create TCP connections
WiFiClient client;



/************************** Interrupt 1ms **************************************/
// Interrupção de 1ms - Intervalo de contagem de pulsos
void timer()
{
  //Serial.print ( "Interrupção Timer at 1ms" );

  contadorLoopInicial++;
  if(contadorLoopInicial==250){
    Flag250ms=true;
    contadorLoopInicial=0;
  }

  contador1seg++;
  if(contador1seg>=1000){
    contador1seg=0;
    flag1s=true; 
    //incrementa horimetro se posChave estiver ligado
    if(posChave==0){
      horimetro++;
    }
    //sirene intermitente hora efetifa
    if(SinalizaHoraEfetiva && posChave==0){
      if(ligDesl==0){
        ligDesl=1;
      }else{
        ligDesl=0;
      }
      digitalWrite(LEDsinal, ligDesl);
    }else{
      digitalWrite(LEDsinal, 0);
    }

    //sirene intermitente cinto
    if(ReleCintoAlerta && posChave==0){
      if(ligaDesligaReleCinto==0){
        ligaDesligaReleCinto=1;
      }else{
        ligaDesligaReleCinto=0;
      }
      digitalWrite(Rele, ligaDesligaReleCinto);
    }else{
      //digitalWrite(Rele, 0);
    }
    
    //sirene on indicando Goto
    if(SinalizaGoto){
      digitalWrite(LEDsinal, 1);
    }else{
      if(!SinalizaHoraEfetiva){
        digitalWrite(LEDsinal, 0);
      }
    }

    
  }
 
  contador1ms++;
  if(contador1ms==250){
    //recomeça a contagem
    contador1ms=0;
    
    //Salva contador dinâmico
    pulsosContados=countPulse;
    //Zera Contador dinâmico
    countPulse=0;

    /***** Media *****/

//    entPos9 = entPos8;
//    entPos8 = entPos7;
//    entPos7 = entPos6;
//    entPos6 = entPos5;
//    entPos5 = entPos4;
//    entPos4 = entPos3;
//    entPos3 = entPos2;
//    entPos2 = entPos1;
//    entPos1 = entPos0;
//    entPos0 = pulsosContados;
//    //media = (entPos9 + entPos8 + entPos7 + entPos6 + entPos5 + entPos4 + entPos3 + entPos2 + + entPos1 + entPos0)/10;
//    media = ( entPos4 + entPos3 + entPos2 + + entPos1 + entPos0)/5;
//    /***** PID ******/
//    double VelocidadeCalculada = (PI * DiametroPneu * media / numeroDentesCoroa) *3.6 * 4;
//    
//    //Serial.println(pulsosContados);
//    
//   /**************  PID   ********************************/
//    Erro_Velocidade_Ant=Erro_Velocidade;
//    
//    if(VelocidadeCalculada < Velocidade_Limite)
//    {
//      Erro_Velocidade=5-(Velocidade_Limite - VelocidadeCalculada);
//      if(Erro_Velocidade > 4 || Erro_Velocidade < 0 ) Erro_Velocidade = 0;
//    }else{
//      Erro_Velocidade=6;
//    }
//    
//    Parcela_Proporcional = GANHO_PROPORCIONAL * Erro_Velocidade;
//    Parcela_Integral = GANHO_INTEGRAL * ( Erro_Velocidade_Ant + Erro_Velocidade) ;
//    Parcela_Derivativa = GANHO_DERIVATIVO * ( Erro_Velocidade - Erro_Velocidade_Ant) ;
//    Tempo_Acionamento = Parcela_Proporcional + Parcela_Integral + Parcela_Derivativa;
//
//    if(Tempo_Acionamento<0) Tempo_Acionamento=0;
//    if(Tempo_Acionamento>230) Tempo_Acionamento=230;
//    
//    Tempo_Desacionamento = TEMPO_ACT_PID - Tempo_Acionamento;
  
//    Serial.print("Vel:");
//    Serial.println(VelocidadeCalculada);
//    Serial.print("ON ");
//    Serial.println(Tempo_Acionamento);
//    Serial.print("OFF ");
//    Serial.println(Tempo_Desacionamento);
    
  }

   
  
//  if(Tempo_Acionamento > 0 ){
//      Tempo_Acionamento--;
//      
//      //digitalWrite(Rele, 1);//liga rele
//      
//    }
//    if(Tempo_Desacionamento > 0 && Tempo_Acionamento==0){
//      Tempo_Desacionamento--;
//      //digitalWrite(Rele, 0);
//      
//    }
}

extern "C" {
  #include "user_interface.h"
}
void setup() {

   pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output

  //SPI EEPROM Externa
//  pinMode(eepromCS, OUTPUT);
//  digitalWrite(eepromCS,HIGH);

  //SPI PORT
  //Seta pino CS automatico
  //SPI.setHwCs(true);
  //Seta SPI frenquencia para 1MHz
  //SPI.setFrequency(1000000);

  //Relé
  pinMode(Rele, OUTPUT);
  digitalWrite(Rele, 0);
  statusRele=0;

  //LED Sinalizacao
  pinMode(LEDsinal, OUTPUT);
  digitalWrite(LEDsinal, 0);
  statusSinalizacao=0;

  //Sensor de temperatura
  pinMode(0, INPUT);
  analogRead(A0);

  //pino Interrupt Sensor
 
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), interruptPulse, FALLING);

  //pino Entrada pos Chave
  //const byte posChavePin = 14;
  pinMode(posChavePin, INPUT_PULLUP);

//pino Entrada Luz de Marcha Ré
  pinMode(luzMarchaRePin, INPUT_PULLUP);

  
//pino Entrada Sensor Cinto Segurança
  pinMode(sensorCintoPin, INPUT_PULLUP);

  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);

  EEPROM.begin(512);
  delay(500);
   
  Serial.begin(115200);

//Le horimetro da EEPROM
//  Serial.println(" ");
//  Serial.println(EEPROM.read(addr));
//  Serial.println(EEPROM.read(addr+1));
//  Serial.println(EEPROM.read(addr+2));
//  Serial.println(EEPROM.read(addr+3));
  
  horimetro = EEPROM.read(addr)<<8;
  horimetro += EEPROM.read(addr+1)<<8;
  horimetro += EEPROM.read(addr+2)<<8;
  horimetro += EEPROM.read(addr+3);
  Serial.print("horimetro EEPROM ");
  Serial.println(horimetro);
  
  Serial.println();
  Serial.print("connecting to: ");
  Serial.println(ssid);

  // Timer the pin every 0.001s
  timer0.attach(0.001, timer);

//  String StrChega="AjusteHorimetro:17459";
//  Serial.println(StrChega);
//  String valAdjString=StrChega.substring(16);
//  Serial.println(valAdjString);
//  int horimetroADJ =valAdjString.toInt();
//  Serial.println(horimetroADJ);
  
  WiFi.mode(WIFI_STA); 
  delay(1000);
  WiFi.begin(ssid, password);
  
//      Serial.println("Zerando horimetro...");
//      EEPROM.write(addr,0);
//      delay(20);
//      EEPROM.write(addr+1,0);
//      delay(20);
//      EEPROM.write(addr+2,0);
//      delay(20);
//      EEPROM.write(addr+3,0);
//      delay(20);          
//      EEPROM.commit();    //Store data to EEPROM
//       Serial.println("Horimetro zerado.");

  
}

  
void loop() {

//    while (status != WL_CONNECTED) {
//    
//  
//       status = WiFi.status();
//       delay(500);
//      if (status == WL_NO_SHIELD) {
//        Serial.println("WiFi shield not present");
//      }else if (status == WL_CONNECTION_LOST){
//        Serial.println("assigned when the connection is lost");  
//      }else if (status == WL_IDLE_STATUS){
//        Serial.println("number of attempts expires");  
//      }else if (status == WL_NO_SSID_AVAIL){
//        Serial.println("assigned when no SSID are available");  
//      }else if (status == WL_SCAN_COMPLETED){
//        Serial.println("assigned when the scan networks is completed");  
//      }else if (status == WL_CONNECT_FAILED){
//        Serial.println("assigned when the connection fails for all the attempts");  
//      }else if (status == WL_DISCONNECTED){
//        Serial.println("assigned when disconnected from a network");  
//      }
//      status = WiFi.status();
//      Serial.print("."); 
//  
//    
//  }

//  Serial.println("");
//  Serial.println("WiFi connected");  
//  Serial.println("IP address: ");
//  Serial.println(WiFi.localIP());

 
 // pinMode(interruptPin, INPUT_PULLUP);
//  attachInterrupt(digitalPinToInterrupt(interruptPin), interruptPulse, FALLING);
   
  // This will send the request to the server
  while(true){
    //CurieTimerOne.start(time, &timedBlinkIsr);  // set timer and callback
    delay(1);

    if(Flag250ms){
      Flag250ms=false;
      //Serial.println("250ms Evento");
      //Leitura do Status Pos Chave - Gerador do evento poschave
      if(posChave != digitalRead(posChavePin)){
        posChave=digitalRead(posChavePin);
        posChaveEvent=true;
        Serial.println("Pos Chave Evento");
      }

      //se teve evento posChave e foi posChave desliagdo, salva horimetro
      if(posChaveEvent && posChave==1){
        posChaveEvent=false;
        salvaHorimetro(horimetro);
        
      }

      //Leitura do Status Luz de Marcha Ré
      luzMarchaRe=digitalRead(luzMarchaRePin);

      //Leitura do Status Sensor cinto
      sensorCinto=digitalRead(sensorCintoPin);

      //Leitura do A/D
      valorAD=analogRead(A0);

      if(WiFi.status() != WL_CONNECTED){
        if(WIFI_Connect()==1) {
          delay ( 250 );
          socket_comm();
        }
        
      }else{
        socket_comm();
      }
      
    }
  }
}

void socket_comm(){
  const int httpPort = 8080;
  
  if(!client.connected()){
    if (!client.connect(WiFi.gatewayIP(), httpPort)) {
      Serial.println("Estabelecendo conexao com socket");
      contEstabelecendoConn++;
      if(posChave==0){
        if(contEstabelecendoConn==480){
          contEstabelecendoConn=0;
          Serial.println("Rele Acionado TimeOut Socket"); 
          digitalWrite(Rele, 1);
          statusRele=1;
        }
      }else{
        contEstabelecendoConn=0;
          Serial.println("TimeOut Socket Reestabelecido"); 
          digitalWrite(Rele, 0);
          statusRele=0;
      }
      
      return;
    }else{
      Serial.print("client.connect: ");
      Serial.print(WiFi.gatewayIP());
      Serial.println(httpPort);
      contEstabelecendoConn=0;
      Serial.println("Rele Desliga TimeOut Socket"); 
      digitalWrite(Rele, 0);
      statusRele=0;
    }
  }else{
    
    String readString = "";
    for(int c = client.read(); c != -1  ;c = client.read()){
       //store characters to string 
       readString += char(c);
    }

    if(readString.equals("")){
      //Serial.print ( "Tx String" );
      //Serial.print ( "\n" );
      
      client.print(pulsosContados);
      client.print("|");
      client.print(valorAD);
      client.print("|");
      client.print(statusRele);
      client.print("|");
      client.print(statusSinalizacao);
      client.print("|");
      client.print(posChave);
      client.print("|");
      client.print(sensorCinto);
      client.print("|");
      client.print(luzMarchaRe);
      client.print("|");
      client.print(eventoComando);
      //String resp = eventoComando.substring(eventoComando.length() - 2);
      
      if(eventoComando.equals("OK")){
        eventoComando="not";
      }

    client.print("|");
    client.print(horimetro);

    
//    Serial.println(horimetro);
//    EEPROM.write(addr,horimetro>> 24);
//    Serial.print("horimetro>> 24 ");
//    Serial.println(horimetro>> 24);
//    EEPROM.write(addr,horimetro>> 24);
//    
//    Serial.print("horimetro>> 16 ");
//    Serial.println(horimetro>> 16);
//    EEPROM.write(addr+1,horimetro>> 16);
//
//    Serial.print("horimetro>> 8 ");
//    Serial.println(horimetro>> 8);
//    EEPROM.write(addr+2,horimetro>> 8);
//    
//    EEPROM.write(addr+3,horimetro & 0xFF);
//    Serial.print("horimetro & 0xFF ");
//    Serial.println(horimetro & 0xFF);
      
      client.print("\n");
      client.print("\0");
            
      //Serial.print(pulsosContados);
    }else{
      //Reservado para quando receber um comando do smartphone
      readString.trim();
      client.print(readString + "\n");
      Serial.println(readString.length());
      Serial.println(readString);
      //Comando para rele
      if(readString=="ReleOn"){
             digitalWrite(Rele, 1);
             statusRele=1; 
             Serial.println("Rele Habilitado");
             eventoComando="ReleOnOK";
             Serial.println(eventoComando);
      }
      if(readString=="ReleOff"){
              Serial.println("Rele Desabilitado"); 
              digitalWrite(Rele, 0);
              statusRele=0; 
              eventoComando="ReleOffOK";
              Serial.println(eventoComando);
      }
      //comando para sinalizacao
      if(readString=="LedSinalOn"){
             digitalWrite(LEDsinal, 1);
             statusSinalizacao=1; 
             eventoComando="LedSinalOnOK";
             Serial.println("Sinalização Habilitada");
             Serial.println(eventoComando);
      }
      if(readString=="LedSinalOff"){
              Serial.println("Sinalização Desabilitada"); 
              digitalWrite(LEDsinal, 0);
              statusSinalizacao=0; 
              eventoComando="LedSInalOffOK";
              Serial.println(eventoComando);
      }
      if(readString=="HoraEfetivaOn"){
              Serial.println("Sinalização H.Ef. habilitada"); 
              SinalizaHoraEfetiva=true;
              statusSinalizacao=2;// 2 LED indica contagem hora efetiva 
              eventoComando="HoraEfetivaOnOK";
              Serial.println(eventoComando);
      }
      if(readString=="HoraEfetivaOff"){
              Serial.println("Sinalização H.Ef. Desabilitada"); 
              SinalizaHoraEfetiva=false;
              statusSinalizacao=3;// 3 LED indica nao contando hora efetiva
              eventoComando="HoraEfetivaOffOK";
              Serial.println(eventoComando);
      }
      if(readString=="gotoRecebido"){
              Serial.println("GoTo Recebido"); 
              SinalizaGoto=true;
              statusSinalizacao=4;// 4 LED indica GoTo recebidido mas nao respondido
              eventoComando="gotoRecebidoOK";
              Serial.println(eventoComando);
      }
      if(readString=="gotoRespondido"){
              Serial.println("GoTo Respondido"); 
              SinalizaGoto=false;
              statusSinalizacao=5;// 5 LED indica GoTo Respondido
              eventoComando="gotoRespondidoOK";
              Serial.println(eventoComando);
      }
      if(readString=="ReleCintoOff"){
              Serial.println("Rele Cinto Alerta OFF"); 
              ReleCintoAlerta=false;
              statusSinalizacao=6;// 6 LED indica Rele Cinto desligado
              eventoComando="ReleCintoOffOK";
              digitalWrite(Rele, 0);
              Serial.println(eventoComando);
      }
      if(readString=="ReleCintoOn"){
              Serial.println("Rele Cinto Alerta ON"); 
              ReleCintoAlerta=true;
              statusSinalizacao=7;// 7 LED indica Rele Cinto ligado
              eventoComando="ReleCintoOnOK";
              Serial.println(eventoComando);
      }
      if(readString.startsWith("espHori")){
              Serial.println("Ajuste de Horimetro"); 
              statusSinalizacao=8;// 
              horimetroSub = readString.substring(7,readString.length());
              float ght;
              ght =horimetroSub.toFloat();
              horimetro =(unsigned int) ght;
              //horimetro=atol(horimetroSub);
              Serial.print("Horimetro ");
              Serial.println(horimetro);  
              eventoComando="espHoriOK";
              Serial.println(eventoComando);
      }
      //zera horimetro
//      if(readString=="ZeraHorimetro"){
//            Serial.println("Zerando horimetro...");
//            EEPROM.write(addr,0);
//            delay(20);
//            EEPROM.write(addr+1,0);
//            delay(20);
//            EEPROM.write(addr+2,0);
//            delay(20);
//            EEPROM.write(addr+3,0);
//            delay(20);          
//            EEPROM.commit();    //Store data to EEPROM
//             Serial.println("Horimetro zerado.");
//             eventoComando="ZeraHorimetroOK";
//             Serial.println(eventoComando);
//      }
      //ajusta horimetro
//      if(readString.substring(0,15)=="AjusteHorimetro:"){
//        String valorAdjString=readString.substring(16);
//        float horimetroADJrecebido =valorAdjString.toFloat();
//        
//        //TODO converter float para horimetro
//        
//        unsigned long horimetroADJ = (unsigned long) horimetroADJrecebido;
//        salvaHorimetro(horimetroADJ);
//        Serial.println("Ajustando horimetro...");
//
//        EEPROM.write(addr,0);
//        delay(20);
//        EEPROM.write(addr+1,0);
//        delay(20);
//        EEPROM.write(addr+2,0);
//        delay(20);
//        EEPROM.write(addr+3,0);
//        delay(20);          
//        EEPROM.commit();    //Store data to EEPROM
//        Serial.println("Horimetro Ajustado.");
//        eventoComando="AjusteHorimetroOK";
//        Serial.println(eventoComando);
//      }
    }
    
  }
}


int WIFI_Connect()
{
  //WiFi.disconnect();
  Serial.println("Recuperando conexao");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //incrementa contador timeout WiFi indicando falta de ponto de Acesso SmartPhone
  timeoutWiFi++;
  if(timeoutWiFi==20){
     Serial.println("Rele Acionado TimeOut WiFi"); 
     digitalWrite(Rele, 1);
     statusRele=1;
     timeoutWiFi=0;
   } 
  for (int i = 0; i < 25; i++)
  {
    if ( WiFi.status() != WL_CONNECTED ) {
      //delay ( 250 );
      //Verifica se pos chave está desligado
      //posChave=digitalRead(posChavePin);
      if(posChave==1){
        //zera timeout WiFi access point
        timeoutWiFi =0;
        //Serial.println("Rele Desabilitado Pos Chave Desligado"); 
        digitalWrite(Rele, 0);
        statusRele=0;
      }
           
      Serial.print ( "." );
      delay ( 250 );
    }else{
      Serial.println ( "Reconectado " );
      //zera timeout WiFi access point
      timeoutWiFi =0;
      Serial.println("Rele Desabilitado TimeOut WiFi"); 
      digitalWrite(Rele, 0);
      statusRele=0;
      
      return 1;
    }
  }
  return 0;
}

void salvaHorimetro(unsigned long horasSeg){
    horimetro = horasSeg;
    Serial.println(horimetro);
    //EEPROM.write(addr,horimetro>> 24);
    Serial.print("horimetro>> 24 ");
    Serial.println(horimetro>> 24 & 0xFF);
    EEPROM.write(addr,horimetro>> 24);
    
    Serial.print("horimetro>> 16 ");
    Serial.println(horimetro>> 16 & 0xFF);
    EEPROM.write(addr+1,horimetro>> 16);

    Serial.print("horimetro>> 8 ");
    Serial.println(horimetro>> 8 & 0xFF);
    EEPROM.write(addr+2,horimetro>> 8);
    
    EEPROM.write(addr+3,horimetro & 0xFF);
    Serial.print("horimetro 0 ");
    Serial.println(horimetro & 0xFF);
          
    EEPROM.commit();    //Store data to EEPROM
}
void interruptPulse(){
  //Serial.print ( "Interrupção Externa" );
  countPulse++;

}

