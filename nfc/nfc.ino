#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <Adafruit_CC3000.h>
//#include <ccspi.h>
//#include <SPI.h>
#include <cc3000_PubSubClient.h>
//#include <Adafruit_SleepyDog.h>
#include <avr/wdt.h>

#define RELAY_PIN   A5
#define BUZZER_PIN  8
#define RED  A0
#define GREEN  A1
#define BLUE   A2

#define PN532_IRQ   (4)
#define PN532_RESET (3)  // Not connected by default on the NFC Shield

char cardID[ 16 ];

uint32_t cards[] = { 296057479, 999246693 };

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
// also change #define in Adafruit_PN532.cpp library file
   #define Serial SerialUSB
#endif

// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   2  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  7
#define ADAFRUIT_CC3000_CS    10

// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT, SPI_CLOCK_DIVIDER);

#define WLAN_SSID       "Ocean Convidado"
#define WLAN_PASS       "ocean2901"
//#define WLAN_SSID       "Ocean Lab"
//#define WLAN_PASS       "ocean3101"

// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2

Adafruit_CC3000_Client client;

// We're going to set our broker IP and union it to something we can use
union ArrayToIp {
  byte array[4];
  uint32_t ip;
};

//ArrayToIp server = { 25, 0, 25, 172 };
ArrayToIp server = { 45, 235, 191, 179 };

void callback (char* topic, byte* payload, unsigned int len) {

    if (String((char *)payload).startsWith(String("S"))) {
      //Serial.println(F("ABREEE"));
      debug("Abrindo porta");
      
      analogWrite(RED, 0);  // os valores do analogRead variam de 0 a 1023, os valores do analogWrite variam de 0 a 255
      analogWrite(GREEN, 200);  // os valores do analogRead variam de 0 a 1023, os valores do analogWrite variam de 0 a 255
      analogWrite(BLUE, 0);  // os valores do analogRead variam de 0 a 1023, os valores do analogWrite variam de 0 a 255
      
      digitalWrite(RELAY_PIN, HIGH);
      delay(1000);
      digitalWrite(RELAY_PIN, LOW);

      
      analogWrite(RED, 150);  // os valores do analogRead variam de 0 a 1023, os valores do analogWrite variam de 0 a 255
      analogWrite(GREEN, 0);  // os valores do analogRead variam de 0 a 1023, os valores do analogWrite variam de 0 a 255
      analogWrite(BLUE, 150);  // os valores do analogRead variam de 0 a 1023, os valores do analogWrite variam de 0 a 255
    }
    else if (String((char *)payload).startsWith(String("A"))) {

      analogWrite(RED, 0);  // os valores do analogRead variam de 0 a 1023, os valores do analogWrite variam de 0 a 255
      analogWrite(GREEN, 200);  // os valores do analogRead variam de 0 a 1023, os valores do analogWrite variam de 0 a 255
      analogWrite(BLUE, 0);  // os valores do analogRead variam de 0 a 1023, os valores do analogWrite variam de 0 a 255
      
      //Serial.println(F("ABREEE"));
      debug("Abrindo porta");
      digitalWrite(RELAY_PIN, HIGH);
    }
    else if (String((char *)payload).startsWith(String("F"))) {
      //Serial.println(F("Fechando"));
      debug("Fechando porta");
      digitalWrite(RELAY_PIN, LOW);

      
      analogWrite(RED,150);  // os valores do analogRead variam de 0 a 1023, os valores do analogWrite variam de 0 a 255
      analogWrite(GREEN, 0);  // os valores do analogRead variam de 0 a 1023, os valores do analogWrite variam de 0 a 255
      analogWrite(BLUE, 150);  // os valores do analogRead variam de 0 a 1023, os valores do analogWrite variam de 0 a 255
    }
    else 
    {
      
      analogWrite(RED, 200);  // os valores do analogRead variam de 0 a 1023, os valores do analogWrite variam de 0 a 255
      analogWrite(GREEN, 0);  // os valores do analogRead variam de 0 a 1023, os valores do analogWrite variam de 0 a 255
      analogWrite(BLUE, 0);  // os valores do analogRead variam de 0 a 1023, os valores do analogWrite variam de 0 a 255
      debug("Entrada não permitida");
      delay(1000);
      
      analogWrite(RED, 150);  // os valores do analogRead variam de 0 a 1023, os valores do analogWrite variam de 0 a 255
      analogWrite(GREEN, 0);  // os valores do analogRead variam de 0 a 1023, os valores do analogWrite variam de 0 a 255
      analogWrite(BLUE, 150);  // os valores do analogRead variam de 0 a 1023, os valores do analogWrite variam de 0 a 255
    }
}

cc3000_PubSubClient mqttclient(server.ip, 1883, callback, client, cc3000);

void debug(char *s) {
  mqttclient.publish("/ocean/nfc/debug",s);
}


void setup(void) {
  //wdt_enable(WDTO_8S);

  comeco:

  pinMode(RED, OUTPUT);   // pré-determina o pino como saída
  pinMode(GREEN, OUTPUT);   // pré-determina o pino como saída
  pinMode(BLUE, OUTPUT);   // pré-determina o pino como saída
  
  analogWrite(RED, 150);  // os valores do analogRead variam de 0 a 1023, os valores do analogWrite variam de 0 a 255
  analogWrite(GREEN, 0);  // os valores do analogRead variam de 0 a 1023, os valores do analogWrite variam de 0 a 255
  analogWrite(BLUE, 150);  // os valores do analogRead variam de 0 a 1023, os valores do analogWrite variam de 0 a 255

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  
  #ifndef ESP8266
    while (!Serial); // for Leonardo/Micro/Zero
  #endif
  Serial.begin(115200);
  Serial.println(F("Hello OCEAN NFC Reader!"));

  Serial.println(F("Inicializando NFC Reader...."));
  
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print(F("Didn't find PN53x board"));
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print(F("Found chip PN5")); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print(F("Firmware ver. ")); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print(F(".")); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc.SAMConfig();
  
  Serial.println(F("Waiting for an ISO14443A Card ...\n\n"));

  Serial.println(F("Inicializando Wifi CC3000"));
  
  //displayDriverMode();

  Serial.println(F("\nInicializando o CC3000 ..."));
  if (!cc3000.begin()) {
    Serial.println(F("Nao foi possivel inicializar o CC3000! Os cabos estao conectados corretamente?"));
    for(;;);
  }
  else {
    Serial.println(F("Carregou"));
  }

  //uint16_t firmware = checkFirmwareVersion();
/*  if ((firmware != 0x113) && (firmware != 0x118)) {
    Serial.println(F("Versao do firmware errada!"));
    for(;;);
  }*/
  
  //displayMACAddress();
  
  Serial.println(F("\nDeletando perfis de conexoes antigas"));
  if (!cc3000.deleteProfiles()) {
    Serial.println(F("Falhou!"));
    while(1);
  }

  /* Attempt to connect to an access point */
  char *ssid = WLAN_SSID;             /* Max 32 chars */
  Serial.print(F("\nTentando conectar-se a(o) ")); Serial.println(ssid);
  
  /* NOTE: Secure connections are not available in 'Tiny' mode! */
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Falhou!"));
    while(1);
  }
   
  Serial.println(F("Conectado!"));
  
  /* Wait for DHCP to complete */
  Serial.println(F("Requisitando DHCP"));
  int timeOut = 0;
  while (!cc3000.checkDHCP()) {
    delay(100); // ToDo: Insert a DHCP timeout!
    Serial.print(".");
    if (timeOut++ == 120) {
      goto comeco;
    }
  }

  /* Display the IP address DNS, Gateway, etc. */  
  /*while (!displayConnectionDetails()) {
    delay(1000);
  }*/
   
   // connect to the broker
   if (!client.connected()) {
     client = cc3000.connectTCP(server.ip, 1883);
     Serial.println(F("Conectando em TCP"));
   }
   
   // did that last thing work? sweet, let's do something
   if(client.connected()) {
    if (mqttclient.connect("ocean_nfc_reader_1", "iotocean", "NZZOLd0O66oLS0vpajiA123")) {
      Serial.println(F("Publicando"));
      mqttclient.publish("/ocean/nfc/debug","ocean_nfc_reader_1 está online");
      mqttclient.subscribe("/ocean/nfc/porta");
    }
    else {
      Serial.println(F("Ouve um erro ao conectar-se ao broker"));
    }
   } 
  nfc.setPassiveActivationRetries(0x1E);

  wdt_enable(WDTO_8S);
}


void loop(void) {
  //Watchdog.reset();
  wdt_reset();

  Serial.println(millis());
  /*
  if (millis() > 300000) {
    Serial.println(F("Resetando!!!"));
    delay(1000);
    asm volatile ("jmp 0");
  }*/
  // Make sure to reset watchdog every loop iteration!
  
  //delay(1000);
  
  //Serial.println(F("Entrei no loop\n"));
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {
    /*/ Display some basic information about the card
    Serial.println(F("Found an ISO14443A card"));
    Serial.print(F("  UID Length: "));Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print(F("  UID Value: "));
    nfc.PrintHex(uid, uidLength);*/
    
    if (uidLength == 4)
    {
      // We probably have a Mifare Classic card ... 
      uint32_t cardid = uid[0];
      cardid <<= 8;
      cardid |= uid[1];
      cardid <<= 8;
      cardid |= uid[2];  
      cardid <<= 8;
      cardid |= uid[3]; 
      //Serial.print(F("Seems to be a Mifare Classic card #"));
      //Serial.println(cardid);
      for (int i = 0; i < 2; i++) {
        if(cards[i] == cardid)
        {
          byte perm[5];
          String("S").getBytes(perm, 5);
          callback (NULL, perm, 0);
          break;
        }
      }
        
          // Testa se ainda esta conectado
          if (!client.connected()) {
            Serial.println(F("!client.connected()"));
             client = cc3000.connectTCP(server.ip, 1883);
             
             if(client.connected()) {
               if (mqttclient.connect("ocean_nfc_reader_1")) {
                  mqttclient.publish("/ocean/nfc/debug","ocean_nfc_reader_1 está novamente online");
               }
             } 
          } else {
            //Serial.println(F("else !client.connected()"));
            // as per comment from LS_dev, platform is int 16bits
            sprintf(cardID,"%lu", cardid);

            
            mqttclient.publish("/ocean/nfc/id", cardID); delay(1000);
          }
          //delay(1000);

    }
    Serial.println(F(""));
  }

  mqttclient.loop(); // nunca esqueca disso!
  
  //Serial.println(F("Sai do loop\n"));
  //digitalWrite(RELAY_PIN,HIGH);
}
/*
void displayDriverMode(void)
{
  #ifdef CC3000_TINY_DRIVER
    Serial.println(F("CC3000 is configure in 'Tiny' mode"));
  #else
    Serial.print(F("RX Buffer : "));
    Serial.print(CC3000_RX_BUFFER_SIZE);
    Serial.println(F(" bytes"));
    Serial.print(F("TX Buffer : "));
    Serial.print(CC3000_TX_BUFFER_SIZE);
    Serial.println(F(" bytes"));
  #endif
}

void charToByte(char* chars, byte* bytes, unsigned int count){
    for(unsigned int i = 0; i < count; i++)
      bytes[i] = (byte)chars[i];
}

void byteToChar(byte* bytes, char* chars, unsigned int count){
    for(unsigned int i = 0; i < count; i++)
       chars[i] = (char)bytes[i];
}

uint16_t checkFirmwareVersion(void)
{
  uint8_t major, minor;
  uint16_t version;
  
#ifndef CC3000_TINY_DRIVER  
  if(!cc3000.getFirmwareVersion(&major, &minor))
  {
    Serial.println(F("Unable to retrieve the firmware version!\r\n"));
    version = 0;
  }
  else
  {
    Serial.print(F("Firmware V. : "));
    Serial.print(major); Serial.print(F(".")); Serial.println(minor);
    version = major; version <<= 8; version |= minor;
  }
#endif
  return version;
}

**************************************************************************/
/*!
    @brief  Tries to read the 6-byte MAC address of the CC3000 module

**************************************************************************
void displayMACAddress(void)
{
  uint8_t macAddress[6];
  
  if(!cc3000.getMacAddress(macAddress))
  {
    Serial.println(F("Unable to retrieve MAC Address!\r\n"));
  }
  else
  {
    Serial.print(F("MAC Address : "));
    cc3000.printHex((byte*)&macAddress, 6);
  }
}


*************************************************************************
*!
    @brief  Tries to read the IP address and other connection details
*/
/**************************************************************************
bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    Serial.print(F("\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
    Serial.println();
    return true;
  }
}
*/

