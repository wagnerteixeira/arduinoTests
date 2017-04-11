#include <SoftwareSerial.h>
#include <ArduinoJson.h>

//RX pino 2, TX pino 3
SoftwareSerial esp8266(2, 3);


#define DEBUG true

#define BAUD_RATE 19200
String SSID = "NET_2G6E01F3";
String PASS = "0F6E01F3";

String getIp(String response) {
  int i = response.indexOf("CIFSR:STAIP,");
  if ( i > 0)
  {
    response = response.substring(i + 13, i + 28);
    i = response.indexOf("\"");
    if (i > 0) {
      response = response.substring(0, i);
    }
    return response;
  }
  else
    return "0.0.0.0";
}

void setup()
{
  /*  String ret = "AT+CIFSR\r\n+CIFSR:APIP,\"192.168.4.1\"\r\nCIFSR:APMAC,\"1a:fe:34:cf:eb:c3\"\r\nCIFSR:STAIP,\"192.168.0.23\"\r\nCIFSR:STAMAC,\"18:fe:34:cf:eb:c3\"\r\nOK\r\n";
    Serial.println(ret);
    getIp(ret);*/
  Serial.begin(BAUD_RATE);
  Serial.println("** inicio **");
  delay(1000);
  // Configure na linha abaixo a velocidade inicial do
  // modulo ESP8266
  esp8266.begin(BAUD_RATE);
  delay(1000);
  //setAp(&esp8266);
  //verificaAT(&esp8266, DEBUG);
  //sendData(&esp8266, "AT+GMR", 2000, DEBUG);
  sendData(&esp8266, "AT+CIOBAUD=19200", 2000, DEBUG);
  /*if (verificaAT(&esp8266, DEBUG)){
    CheckMode(&esp8266);
    //setSta(&esp8266);
    //setAp(&esp8266);
    setStaAndAp(&esp8266);
    CheckMode(&esp8266);
    }*/
  //sendData(&esp8266, "AT+CIOBAUD=19200", 2000, DEBUG);
  if (verificaAT(&esp8266, DEBUG)) {
    //Serial.println("** Shield conectado **");
    //sendData(&esp8266, "AT+RST", 2000, DEBUG);
    //delay(1000);
    //Serial.println("Versao de firmware");
    // delay(3000);
    //sendData(&esp8266, "AT+GMR", 2000, DEBUG); // rst
    // Configure na linha abaixo a velocidade desejada para a
    // comunicacao do modulo ESP8266 (9600, 19200, 38400, etc)
    // sendData(&esp8266, "AT+CIOBAUD=19200", 2000, DEBUG);
    //ListAccessPoints(&esp8266);
    // sendData(&esp8266, "AT+RST\r\n", 2000, DEBUG); // rst
    // Conecta a rede wireless
    //CheckMode(&esp8266);
    quitConnect(&esp8266);
    //CheckMode(&esp8266);
    sendData(&esp8266, "AT+CWJAP=\"" + SSID + "\",\"" + PASS + "\"", 2000, DEBUG);
    delay(3000);
    //sendData(&esp8266, "AT+CWMODE=1", 1000, DEBUG);
    // Mostra o endereco IP

    String ip = checkConnected(&esp8266);
    Serial.println("IP: " + ip);
    // if (!ip.equals("0.0.0.0")){
    //   checkConnections(&esp8266);
    //   httpGet(&esp8266);
    // }

  }
  else {
    Serial.println("** Shield não conectado bosta**");
  }
  Serial.println("** Final **");
}


void loop() {
  // put your main code here, to run repeatedly:

}


String sendData(SoftwareSerial *sw, String command, const int timeout, boolean debug)
{
  // Envio dos comandos AT para o modulo
  String response = "";
  sw->println(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (sw->available())
    {
      // The esp has data so display its output to the serial window
      char c = sw->read(); // read the next character.
      response += c;
    }
  }
  if (debug)
  {
    Serial.println(response);
  }
  return response;
}

String readStringFromSerial(SoftwareSerial *sw, const int timeout, boolean debug)
{
  // Envio dos comandos AT para o modulo
  String response = "";
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (sw->available())
    {
      // The esp has data so display its output to the serial window
      char c = sw->read(); // read the next character.
      response += c;
    }
  }
  if (debug)
  {
    Serial.println(response);
  }
  return response;
}

bool verificaAT(SoftwareSerial *sw, boolean debug)
{
  sw->println("AT+RST");
  String response = "";
  response = readStringFromSerial(sw, 2000, debug);
  if (debug)
    Serial.println(response);

  return response.indexOf("OK") >= 0;
}

void ListAccessPoints(SoftwareSerial *sw)
{
  sw->println("AT+CWLAP");
  String response = "";
  long int time = millis();
  while ( (time + 10000) > millis())
  {
    while (sw->available())
    {
      // The esp has data so display its output to the serial window
      char c = sw->read(); // read the next character.
      response += c;
    }
  }
  Serial.println(response);
}

void CheckMode(SoftwareSerial *sw)
{
  Serial.println("** Mode **");
  sw->println("AT+CWMODE?");
  String response = "";
  long int time = millis();
  while ( (time + 10000) > millis())
  {
    while (sw->available())
    {
      // The esp has data so display its output to the serial window
      char c = sw->read(); // read the next character.
      response += c;
    }
  }
  Serial.println(response);
}


void checkBaudRate(SoftwareSerial *sw)
{
  Serial.println("** Baud Rate **");
  sw->println("AT+CIOBAUD?");
  String response = "";
  long int time = millis();
  while ( (time + 10000) > millis())
  {
    while (sw->available())
    {
      // The esp has data so display its output to the serial window
      char c = sw->read(); // read the next character.
      response += c;
    }
  }
  Serial.println(response);
}

void setSta(SoftwareSerial *sw)
{
  Serial.println("** Set in Sta mode **");
  sw->println("AT+CWMODE=1");
  String response = "";
  long int time = millis();
  while ( (time + 10000) > millis())
  {
    while (sw->available())
    {
      // The esp has data so display its output to the serial window
      char c = sw->read(); // read the next character.
      response += c;
    }
  }
  Serial.println(response);
}


void setAp(SoftwareSerial *sw)
{
  Serial.println("** Set in Ap mode **");
  sw->println("AT+CWMODE=2");
  String response = "";
  long int time = millis();
  while ( (time + 10000) > millis())
  {
    while (sw->available())
    {
      // The esp has data so display its output to the serial window
      char c = sw->read(); // read the next character.
      response += c;
    }
  }
  Serial.println(response);
}

void setStaAndAp(SoftwareSerial *sw)
{
  Serial.println("** Set in Sta and Ap mode **");
  sw->println("AT+CWMODE=3");
  String response = "";
  long int time = millis();
  while ( (time + 10000) > millis())
  {
    while (sw->available())
    {
      // The esp has data so display its output to the serial window
      char c = sw->read(); // read the next character.
      response += c;
    }
  }
  Serial.println(response);
}

void quitConnect(SoftwareSerial *sw) {
  Serial.println("** Quitar conexao**");
  sw->println("AT+CWQAP");
  String response = readStringFromSerial(sw, 2000, DEBUG);
  if (DEBUG)
    Serial.println(response);
}

void checkConnections(SoftwareSerial *sw) {
  Serial.println("** Check Connections**");
  sw->println("AT+CIPMUX?");
  String response = readStringFromSerial(sw, 2000, DEBUG);
  if (DEBUG)
    Serial.println(response);
}


String checkConnected(SoftwareSerial *sw)
{
  Serial.println("** Checar conexao**");
  int tentativas = 10;
  Serial.println("Tentativa " + tentativas);
  delay(100);
  String response = "";
  String ip = "";
  while (tentativas) {
    //response = sendData(sw, "AT+CIFSR", 2000, DEBUG);
    sw->println("AT+CIFSR");
    response = readStringFromSerial(sw, 2000, DEBUG);
    Serial.println("response " + String(tentativas) + " " + response);
    ip = getIp(response);
    if (ip.equals("0.0.0.0"))
    {
      delay(500);
      tentativas--;
    }
    else {
      return ip;
    }
  }
}

void httpGet(SoftwareSerial *sw) {
  Serial.println("** Http Get ** será?");
  const String server = "www.google.com";
  sw->println("AT+CIPSTART=\"TCP\",\"" + server + "\",80");//start a TCP connection.
  while (sw->available()) {
    String tmpResp = sw->readString();
    Serial.println("CIPSTART: " + tmpResp);
  }
  String postRequest = ">GET / HTTP/1.0\r\nHost: www.google.com\r\nCache-Control: no-cache\r\n";
  Serial.println(postRequest);
  String sendCmd = "AT+CIPSEND=";//determine the number of caracters to be sent.
  sw->print(sendCmd);
  sw->println(postRequest.length());
  String tot = "";
  while (sw->available()) {
    String tmpResp = sw->readString();
    tot = tot + tmpResp;
    Serial.println("CIPSEND: " + tmpResp);
  }
  delay(500);
  int tentativas = 50;
  while (!sw->find(">")) {
    if (sw->find(">")) {
      Serial.println("Sending..");
      sw->print(postRequest);
      if ( sw->find("SEND OK")) {
        Serial.println("Packet sent");
        while (sw->available()) {
          String tmpResp = sw->readString();
          Serial.println(tmpResp);
        }
      }
    }
    else
      Serial.println("tentativa: " + String(tentativas));
    delay(200);
    tentativas--;
  }
  sw->println("AT+CIPCLOSE");
}
/*void httppost () {
  esp.println("AT+CIPSTART=\"TCP\",\"" + server + "\",80");//start a TCP connection.

  if( esp.find("OK")) {
    Serial.println("TCP connection ready");
  }
  delay(1000);

  String postRequest =
  "POST " + uri + " HTTP/1.0\r\n" +
  "Host: " + server + "\r\n" +
  "Accept: *" + "/" + "*\r\n" +
  "Content-Length: " + data.length() + "\r\n" +
  "Content-Type: application/x-www-form-urlencoded\r\n" +
  "\r\n" + data;
  String sendCmd = "AT+CIPSEND=";//determine the number of caracters to be sent.
  esp.print(sendCmd);
  esp.println(postRequest.length() );
  delay(500);
  if(esp.find(">")) { Serial.println("Sending.."); esp.print(postRequest);
  if( esp.find("SEND OK")) { Serial.println("Packet sent");
  while (esp.available()) {
  String tmpResp = esp.readString();
  Serial.println(tmpResp);
  }

  // close the connection

  esp.println("AT+CIPCLOSE");

  }

  }}

*/
