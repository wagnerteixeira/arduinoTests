#ifdef DEBUG_ESP_PORT
#define DEBUG_MSG(...) DEBUG_ESP_PORT.printf( __VA_ARGS__ )
#else
#define DEBUG_MSG(...) 
#endif

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <ESP8266WiFiMulti.h>
#include <EEPROM.h>


char ssid[100];
char password[100];
 
char* ssid_ap     = "TESTE";  
char* password_ap = "12345678";
 
const char* host     = "wagnerteixeira.me"; // Your domain  
String path          = "/light.json";  
const int pin        = 2;
int attempsConnect = 0;

ESP8266WiFiMulti WiFiMulti;

String readStringUntil(String *str, char c){
  int index = str->indexOf(c);
  //Serial.println("index: " + String(index));
  String strRet = "";
  if (index > 0){
    strRet = str->substring(0, index);
    //Serial.println("strRet: " + strRet);
    *str = str->substring(index + 1);
    //Serial.println("str: " + *str);
    
  }
  else{
     strRet = *str; 
     *str ="";;//->substring(0);
  }
  return strRet;
}

void waitForNewCredentials(){
  int attemps = 0;
  Serial.println("Waiting for a new ssid and password");
  String ret = "";
  while (1){
    ret = readStringFromSerial(&Serial, 2000, true);
    //Serial.println("ret: " + ret + " attemp " + String(attemps));
    if (ret != ""){
      String at = readStringUntil(&ret, '|');
      String fssid = readStringUntil(&ret, '|');
      String fpassword = readStringUntil(&ret, '|');
      //Serial.println("fssid: " + fssid + " fpass: " + fpassword);
      if ((at== "AT+CW") && (fssid != "") && (fpassword != "")){
        //unsigned int len = 
        fssid.toCharArray(ssid, fssid.length() + 1);
        fpassword.toCharArray(password, fpassword.length() + 1);
        //Serial.println("ssid: " + String(ssid) + " pass: " + String(password));
        saveCredentials();
        return;
      }
    }  
    attemps++;     
    if (attemps == 15){
      Serial.println("Waiting for a new ssid and password");
      attemps = 0;
    }
  }
}


void upSoftAp(){
  Serial.println("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid_ap, password_ap);
  
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}


boolean connectWlan(){
  int attemps = 0;
  // We start by connecting to a WiFi network
  if (WiFiMulti.addAP(ssid, password)){
    Serial.println("Ap adicionado ");    
      
    Serial.print("Wait for WiFi... ");
  
    while(WiFiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        attemps++;
        if (attemps == 20){
          Serial.println(".");
          break;
        }
        delay(500);
    }
    if (WiFi.localIP() != (uint32_t)0){
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      printWifiData();  
      return true;
    }
    else{
     Serial.print("não conseguiu conectar no wifi " + String(ssid) + " com senha " + String(password));    
     return false;
    }
  }
}
void setup() {  

  Serial.begin(19200);
  Serial.println("Init...");
  WiFi.softAPdisconnect(true);
  //saveCredentials();
  loadCredentials();
  /*if (String(ssid) == ""){
    ssid = "..";
    password = "Senha123";
    saveCredentials();
  }*/
    
  //Serial.println("ssid: " + String(ssid));
  //Serial.println("password: " + String(password));
  boolean c = connectWlan();
  while (!c){
    waitForNewCredentials();
    c = connectWlan();
  }
  
  pinMode(pin, OUTPUT); 
  digitalWrite(pin, LOW);
}


void printWifiData() {
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);
  
  // print your MAC address:
  byte mac[6];  
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX);
 
}


String readStringFromSerial(Stream *s, const int timeout, boolean debug)
{
  // Envio dos comandos AT para o modulo
  String response = "";
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (s->available())
    {
      // The esp has data so display its output to the serial window
      char c = s->read(); // read the next character.
      response += c;
    }
  }
  if (debug)
  {
    Serial.println(response);
  }
  return response;
}

void loop() {  
  Serial.print("connecting to ");
  Serial.println(host);
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    attempsConnect++;
    boolean c = false;
    if (attempsConnect == 5){
      attempsConnect = 0;
      c = connectWlan();
      while (!c){
        waitForNewCredentials();
        c = connectWlan();
      }
    }
    if (!c)
      return;
  }
 
  client.print(String("GET ") + path + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: keep-alive\r\n\r\n");
 
  delay(500); // wait for server to respond
 
  // read response
  String section="header";
  while(client.available()){
    String line = client.readStringUntil('\r');
    // Serial.print(line);
    // we’ll parse the HTML body here
    if (section=="header") { // headers..
      Serial.print(".");
      if (line=="\n") { // skips the empty space at the beginning 
        section="json";
      }
    }
    else if (section=="json") {  // print the good stuff
      section="ignore";
      String result = line.substring(1);
 
      // Parse JSON
      int size = result.length() + 1;
      char json[size];
      result.toCharArray(json, size);
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& json_parsed = jsonBuffer.parseObject(json);
      if (!json_parsed.success())
      {
        Serial.println("parseObject() failed");
        return;
      }
 
      // Make the decision to turn off or on the LED
      if (strcmp(json_parsed["light"], "on") == 0) {
        digitalWrite(pin, HIGH); 
        Serial.println("LED ON");
      }
      else {
        digitalWrite(pin, LOW);
        Serial.println("led off");
      }
    }
  }
  Serial.print("closing connection. ");
}

void loadCredentials() {
  EEPROM.begin(512);
  EEPROM.get(0, ssid);
  EEPROM.get(0+sizeof(ssid), password);
  char ok[2+1];
  EEPROM.get(0+sizeof(ssid)+sizeof(password), ok);
  EEPROM.end();
  if (String(ok) != String("OK")) {
    ssid[0] = 0;
    password[0] = 0;
  }
  Serial.println("Recovered credentials:");
  Serial.println(ssid);
  Serial.println(strlen(password)>0?"********":"<no password>");
}

/** Store WLAN credentials to EEPROM */
void saveCredentials() {
  Serial.println("Saving new credentials:");
  EEPROM.begin(512);
  EEPROM.put(0, ssid);
  EEPROM.put(0+sizeof(ssid), password);
  char ok[2+1] = "OK";
  EEPROM.put(0+sizeof(ssid)+sizeof(password), ok);
  EEPROM.commit();
  EEPROM.end();
}
