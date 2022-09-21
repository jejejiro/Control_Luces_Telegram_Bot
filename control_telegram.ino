#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif

#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   // Libreria de Universal Telegram 
#include <ArduinoJson.h>


// Coloca Aqui los credenciales del Wifi
const char* ssid = "..........";
const char* password = ".........";

#define BOTtoken ".........."  // Coloca el Token de Telegram

// Coloc aqui la ID del Chat Telegram
#define CHAT_ID "........."

#ifdef ESP8266
  X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Verifica si hay un nuevo mensaje cada segundo.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

//salidas a utilizar
const int PinRele1 = D0; //Pin conectado al IN del Rele
const int PinRele2 = D1; //Pin conectado al IN del Rele
bool ledState = HIGH;

// Manejo de mensajes 
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Imprime el mensaje recibido
    String text = bot.messages[i].text;
    Serial.println(text);

//mensaje que vera el usuario 

    String from_name = bot.messages[i].from_name;
    
    if (text == "/start") {
      String welcome = "Hola, " + Tu nombre + ".\n";
      welcome += "Comandos (copia o pega los siguientes comandos. \n\n";
      welcome += "/sala_on to turn GPIO ON \n";
      welcome += "/sala_off to turn GPIO OFF \n";
      welcome += "/cuarto_on to turn GPIO ON \n";
      welcome += "/cuarto_off to turn GPIO OFF \n";;
      welcome += "/estado to request current GPIO state \n";
      bot.sendMessage(chat_id, Bienvenido, "");
    }
    
//condiciones y respuestas en el chat

    if (text == "/sala_on") {
      bot.sendMessage(chat_id, "El Bombillo de la sala esta Encendido", "");
      ledState = LOW;
      digitalWrite(PinRele1, ledState);
    }
    
    if (text == "/sala_off") {
      bot.sendMessage(chat_id, "El Bombillo de la sala esta Apagado", "");
      ledState = HIGH;
      digitalWrite(PinRele1, ledState);
    }
    if (text == "/cuarto_on") {
      bot.sendMessage(chat_id, "El Bombillo del cuarto esta Encendido", "");
      ledState = LOW;
      digitalWrite(PinRele2, ledState);
    }
    if (text == "/cuarto_off") {
      bot.sendMessage(chat_id, "El Bombillo del cuarto esta Apagado", "");
      ledState = HIGH;
      digitalWrite(PinRele2, ledState);
    }
    
    if (text == "/estado") {
      if (digitalRead(PinRele1)){
        bot.sendMessage(chat_id, "El Bombillo de la sala esta Encendido", "");
      }
      else{
        bot.sendMessage(chat_id, "El Bombillo de la sala esta Apagado", "");
      }
      if (digitalRead(PinRele2)){
        bot.sendMessage(chat_id, "El Bombillo del cuarto esta Encendido", "");
      }
      else{
        bot.sendMessage(chat_id, "El Bombillo del cuarto esta Apagado", "");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);

  #ifdef ESP8266
    configTime(0, 0, "pool.ntp.org");      // obtenemos el tiempo a traves del protocolo NTP
    client.setTrustAnchors(&cert);        // Agregamos el certificado Raiz api.telegram.org
  #endif

//configuramos los pin del procesador como salida
  pinMode(PinRele1, OUTPUT);
  digitalWrite(PinRele1, ledState);
   pinMode(PinRele2, OUTPUT);
  digitalWrite(PinRele2, ledState);
  
  // Se conecta a WIFI en modo estacion de trabajo
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Agregamos el certificado raiz para api.telegram.org
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Imprimimos la direccion IP por consola
  Serial.println(WiFi.localIP());

}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
