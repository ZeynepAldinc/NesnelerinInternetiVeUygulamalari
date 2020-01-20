/***************************************************
 * IOT Ev Otomasyon Projesi
 * Röle ile Priz ve Lamba Kontrolü 
 * 
 * Hazırlayanlar: Zeynep Aldinç B181210390 zeynep.aldinc1@ogr.sakarya.edu.tr
 *                Hüsne İrem Kaya B171210309 husneirem.kaya@ogr.sakarya.edu.tr
 *                Uğur Çelik B171210042 ugur.celik10@ogr.sakarya.edu.tr
 * Version : 1.00.0
 ****************************************************/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/*** Kablosuz Ağ Bilgileri ***/
#define   WLAN_SSID     "EV"   // "Kablosuz Ağ Adı"
#define   WLAN_PASSWORD "12344321"  // "Kablosuz Ağ Şifresi"
/************************* Adafruit.io Kurulumu *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "PlaLoho"
#define AIO_KEY         "0e5859df0c0e4bcb8519985b246c7fe3"

/************ Global State (you don't need to change this!) ******************/

// MQTT bağlantısı için gerekli WifiClient sınıf nesnesidir.
WiFiClient client;

// WifiClient sınıfını wifi client ve yetkilendirme giriş bilgilerine göre ayarlar.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Yayınlar ***************************************/

//Lamba1 yayınının dinlenebilmesi için gerekli kullanıcı ve yayın bilgileri ayarlanır.
Adafruit_MQTT_Subscribe lamba1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/lamba1");

//Priz1 yayınının dinlenebilmesi için gerekli kullanıcı ve yayın bilgileri ayarlanır.
Adafruit_MQTT_Subscribe priz1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/priz1");

/*************************** Taslak Kod ************************************/
//MQTT bağlantı durummu kontrol eden fonksiyon. 
void MQTT_connect();

void setup() {
  Serial.begin(115000);
  delay(10);
  WiFi_Setup();
  //Lamba rölesi kontrol pini
  pinMode(D1,OUTPUT);
  digitalWrite(D1,1);
  //Priz rölesi kontrol pini
  pinMode(D2,OUTPUT);
  digitalWrite(D2,1);
  mqtt.subscribe(&lamba1); 
  mqtt.subscribe(&priz1);
}

void loop() {
  MQTT_connect();
  Adafruit_MQTT_Subscribe *subscription;
  
  while ((subscription = mqtt.readSubscription(2000))) {
    if (subscription == &lamba1) {
      Serial.print(F("LambaDurum: "));
      Serial.println((char *)lamba1.lastread);
      char *oku;
     oku=(char *)lamba1.lastread;
     if (strcmp(oku,"ON")==0)digitalWrite(D2,0);
     if (strcmp(oku,"OFF")==0)digitalWrite(D2,1);
    }
      if (subscription == &priz1) {
        Serial.print(F("PrizDurum: "));
        Serial.println((char *)priz1.lastread);
        char *oku2;
       oku2=(char *)priz1.lastread;
       if (strcmp(oku2,"ON")==0)digitalWrite(D1,0);
       if (strcmp(oku2,"OFF")==0)digitalWrite(D1,1);
        
      }
    }
}

void WiFi_Setup(){
 delay(10);
 Serial.println(); Serial.print(WLAN_SSID);
 Serial.print(" Kablosuz Agina Baglaniyor");
 WiFi.begin(WLAN_SSID, WLAN_PASSWORD);
 // WiFi durum kontrolü
 while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();Serial.print(WLAN_SSID);
  Serial.println("Kablosuz Agina Baglandi");
  Serial.println("IP adresi: "); 
  Serial.println(WiFi.localIP());
}

// Bu fonksiyon gerekli bağlantının durumunu kontrol eder. Bağlantı olmaması durumunda bağlanmayı yineler.
void MQTT_connect() {
  uint8_t ret;
  // Bağlı ise çağrıldığı yere döner
  if (mqtt.connected()) return;
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // 0 bilgisi ile bağlantı anlaşılır.
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
