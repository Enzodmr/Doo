#include <SoftwareSerial.h>
#include <Ethernet.h>

byte MAC[] = {0x90, 0xA2, 0xD1, 0x0E, 0xA5, 0x7E};
//IPAddress IP(192, 168, 0, 27);
IPAddress IP(192, 168, 1, 27);
EthernetServer serveurHTTP(80);
EthernetClient client;
char serveur[] = "file:///C:/Users/blanc/Desktop/test.html";

const byte rxPin = 3;
const byte txPin = 4;

SoftwareSerial SerialRS = SoftwareSerial(rxPin, txPin);

const byte address = 1; //camera address
byte speed = 100; // can be changed by pressing left+up or left+down

const byte C_STOP = 0x00;
const byte C_UP = 0x08;
const byte C_DOWN = 0x10;
const byte C_LEFT = 0x04;
const byte C_RIGHT = 0x02;

const byte C_SET_PAN_POSITION = 0x4B;  // 1/100ths of degree
const byte C_SET_TILT_POSITION = 0x4D; // 1/100ths of degree

bool stoped = false;

void setup()
{
  Ethernet.begin(MAC, IP);
  serveurHTTP.begin();
  Serial.begin(9600);
  SerialRS.begin(9600);
  Serial.println("Pelco D controller");

  sendPelcoDFrame(C_STOP, 0, 0);
  delay(10);
  sendPelcoDFrame(C_SET_PAN_POSITION, 23, 28);

}

void loop()
{
  EthernetClient client = serveurHTTP.available();
  if (client)
  {
    if (client.connected())
    {
     String reception;
     while (client.available())
     {
        char carLu = client.read();
        if (carLu != 10)
        {
          reception += carLu;
        }
        else
        {
          break;
        }
     }
        Serial.println(reception);
        reception.trim();
        if ((reception.startsWith("GET /")) && (reception.endsWith(" HTTP/1.1")))
        {
          if (reception.indexOf("haut=haut") != -1) UP();
          if (reception.indexOf("bas=bas") != -1) DOWN();
          if (reception.indexOf("gauche=gauche") != -1) LEFT();
          if (reception.indexOf("droite=droite") != -1) RIGHT();
          if (reception.indexOf("arret=arret") != -1)  STOP();
          client.println(F("HTTP/1.1 200 OK"));
          client.println(F("Content-Type: text/html"));
          client.println(F("Access-Control-Allow-Origin:*"));
          client.println(F("Access-Control-Allow-Credentials: true"));
          client.println(F("Access-Control-Max-Age: 86400"));
          client.println(F("Connection: close"));
          client.println();
          client.println(F( "<!DOCTYPE html>"
                            "<html lang='fr'>"
                            "<head>"
                            "    <meta charset='UTF-8'>"
                            "    <meta name='viewport' content='width=device-width, initial-scale=1.0'>"
                            "    <title>Document</title>"
                            "</head>"
                            "<body>"
                            "<center>"
                            "    <form method='get'>"
                            "        <input style='font-size: 10em' type='submit' value='haut' name='haut'><br>"
                            "        <input style='font-size: 10em' type='submit' value='bas' name='bas'>"
                            "        <input style='font-size: 10em' type='submit' value='gauche' name='gauche'><br>"
                            "        <input style='font-size: 10em' type='submit' value='droite' name='droite'>"
                            "        <input style='font-size: 10em' type='submit' value='arret' name='arret'><br>"
                            "    </form>"
                            "</center>"
                            " </body>"
                            " </html>"));
        }
        client.stop();
      }
  }
}
void sendPelcoDFrame(byte command, byte data1, byte data2)
{
  byte bytes[7] = {0xFF, address, 0x00, command, data1, data2, 0x00};
  byte crc = (bytes[1] + bytes[2] + bytes[3] + bytes[4] + bytes[5]) % 0x100;
  bytes[6] = crc;

  for (int i = 0; i < 7; i++)
  {
    SerialRS.write(bytes[i]);
    // Serial.print(bytes[i], HEX); //debug
  }
  // Serial.println(); //debug
}

  void LEFT()
  {
    sendPelcoDFrame(C_LEFT, speed, speed);
    Serial.println("left");
    stoped = false;
    delay(500);
    sendPelcoDFrame(C_STOP, 0, 0);
    Serial.println("stop");
    stoped = true;
  }
  void RIGHT()
  {
    sendPelcoDFrame(C_RIGHT, speed, speed);
    Serial.println("right");
    stoped = false;
    delay(500);
    sendPelcoDFrame(C_STOP, 0, 0);
    Serial.println("stop");
    stoped = true;
  }
  void UP()
  {
    sendPelcoDFrame(C_UP, speed, speed);
    Serial.println("up");
    stoped = false;
    delay(500);
    sendPelcoDFrame(C_STOP, 0, 0);
    Serial.println("stop");
    stoped = true;
  }
  void DOWN()
  {
    sendPelcoDFrame(C_DOWN, speed, speed);
    Serial.println("down");
    stoped = false;
    delay(500);
    sendPelcoDFrame(C_STOP, 0, 0);
    Serial.println("stop");
    stoped = true;
  }
  
  void STOP()
    {
      sendPelcoDFrame(C_STOP, 0, 0);
      delay(10);
      sendPelcoDFrame(C_STOP, 0, 0);
      Serial.println("stop");
      stoped = true;
    }
