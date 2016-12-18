#include <SoftwareSerial.h>
#include <SD.h>
#include <SPI.h>
#include <DS3231.h>
#define CS_PIN 10 //broche carte SD

SoftwareSerial esp8266(2, 3);//TX(jaune):2 RX(vert):3
DS3231  realTimeClock(A1, A0); //SDA, SCL

void setup() {
  Serial.begin(115200);
  realTimeClock.begin();
  initESP8266(); //wifi
  SD.begin();
}

void loop() {

  if (esp8266.available()) 
  {
    
    if (esp8266.find("+IPD,"))
    {
      delay(1000);
      int connectionId = esp8266.read() - 48; 
      Serial.println("xxxxconnection: " + String(connectionId));
      envoyerPageHtml(connectionId);
    }
    //delay(1000);
  }
}

void initESP8266() {
  esp8266.begin(115200);
  esp8266.write("AT+RST\r\n");
  receiveResponse();
  esp8266.write("AT+CWMODE=2\r\n");
  receiveResponse();
  esp8266.write("AT+CIFSR\r\n");
  receiveResponse();
  esp8266.write("AT+CIPMUX=1\r\n");
  receiveResponse();
  esp8266.write("AT+CIPSERVER=1,80\r\n");
  receiveResponse();
}

String sendData(String command, const int timeout)
{
  
  String response="";
  esp8266.print(command); 
  long int time = millis();

  while ( (time + timeout) > millis())
  {
    if (esp8266.available())
    {
      response += esp8266.readString();
     // Serial.print();
    }
  }
  
  return response;
}


void receiveResponse()
{
  
  long int time = millis();
  while ( (time + 3000) > millis())
  {
    while (esp8266.available())
    {
      Serial.print(esp8266.readString());
    }
  }
}


void enregistrerOuvertureDuRobinetCroquettes(){
  File data = SD.open("data.txt", FILE_WRITE);
  if(data){
    data.print("<li>" + String(realTimeClock.getDateStr()) + " a " + String(realTimeClock.getTimeStr()) + "</li>|");
    data.close();
  }
  delay(300);
}

String consulterNSChat(int connectionId){
  Serial.println("consultation NSChat");
  String contenu="";
  File data = SD.open("data.txt");
  while(data.available()){
    char r = data.read();
    if(r=='|'){
      envoyerChunkHtml(contenu, connectionId);
      contenu="";
    }else{
      contenu+=r;
    }
  }
  
  data.close();
  Serial.println(contenu);
  return contenu;
}



void envoyerPageHtml(int connectionId){
      envoyerChunkHtml("<html>",connectionId);
      envoyerChunkHtml("<head><title>:: NSChat IS WATCHING YOU ::</title></head>",connectionId);
      envoyerChunkHtml("<body>",connectionId);
      envoyerChunkHtml("<h1>NSChat State Department : Heures d'utilsation de ArduiChat^^ </h1>",connectionId);
      envoyerChunkHtml("<table>",connectionId);
      envoyerChunkHtml("<tr>",connectionId);
      envoyerChunkHtml("<td width=\"356px\">",connectionId);
      envoyerChunkHtml("<img src=\"http://localhost:8080/spy.jpg\" width=\"356px\">",connectionId);
      envoyerChunkHtml("<br/>",connectionId);
      envoyerChunkHtml("<img src=\"http://localhost:8080/c.png\">",connectionId);
      envoyerChunkHtml("</td>",connectionId);
      envoyerChunkHtml("<td>",connectionId);
      consulterNSChat(connectionId);
      envoyerChunkHtml("</td>",connectionId);
      envoyerChunkHtml("</tr>",connectionId);
      envoyerChunkHtml("</table>",connectionId);
      envoyerChunkHtml("</body>",connectionId);
      envoyerChunkHtml("</html>",connectionId);
      fermerConnection(connectionId);
}


void envoyerChunkHtml(String content, int connectionId){
      Serial.println("sending : " + content);
      String cipSend = "AT+CIPSEND=";
      cipSend += connectionId;
      cipSend += ",";
      cipSend += content.length();
      cipSend += "\r\n";

      sendData(cipSend, 100);
      sendData(content, content.length() * 20);
}


void fermerConnection(int connectionId){
      String closeCommand = "AT+CIPCLOSE=";
      closeCommand += connectionId;
      closeCommand += "\r\n";
      sendData(closeCommand, 1000);
}
