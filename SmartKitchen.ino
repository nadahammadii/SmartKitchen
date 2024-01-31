#define BLYNK_TEMPLATE_ID "TMPL2-KHiuePn"
#define BLYNK_TEMPLATE_NAME "Temperature and humidity"
#define BLYNK_AUTH_TOKEN "OGIvhE1UryF2YzJ_-PSjTtn1_v9irfoQ"
#define BLYNK_PRINT Serial
#include <DHT.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <base64.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <UrlEncode.h>

#define DHTPIN 27  // Remplacez 27 par le numéro de la broche GPIO à laquelle le capteur est connecté
#define DHTTYPE DHT11

const char *senderEmail = "hanaahmedmanoubi@gmail.com";
const char *senderPassword = "rres yvny vfjv mxpw";
const char *recipientEmail = "hana.ahmed@isimg.tn";
const char *senderName = "esp_32";
const char *emailSubjectFlame = "Flame Detected";
const char *emailSubjectGaz = "Gas Detected";

String phoneNumber = "21650810748";
String apiKey = "7341257";

const int flameSensorPin = 34; 
const int gazSensorPin = 35;

DHT dht(DHTPIN, DHTTYPE);
const char *ssid = "pc";
const char *password = "hana202024";
const char auth[] = BLYNK_AUTH_TOKEN;
BlynkTimer timer;

void setup() {
  Serial.begin(115200);
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
    pinMode(gazSensorPin, INPUT);
   Blynk.begin(auth, ssid, password);
  //Blynk.begin(auth, ssid, password,"Blynk cloud",80);
  dht.begin();
 timer.setInterval(100L, sendSensor);
}
void loop() {
  // put your main code here, to run repeatedly:
readFlameSensor();
readGazSensor();
    Blynk.run();
  timer.run();
}
// pour l'application Blynk
void sendSensor() 
{
  float humidite = dht.readHumidity();
  float temperature = dht.readTemperature();
  if (isnan(humidite) || isnan(temperature)) {
    Serial.println("Erreur lors de la lecture du capteur DHT !");
    return;
  }
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidite);
}

void readFlameSensor() {
  int flameValue = analogRead(flameSensorPin);
  // Ajustez ce seuil en fonction de votre capteur et de votre environnement
  int flameThreshold = 50;
 
  if (flameValue > flameThreshold ) {
    Serial.print("Flamme détectée  ");
    // Appel de la fonction sendEmail
    sendEmail(senderEmail, senderPassword, recipientEmail, senderName, emailSubjectFlame, "Flame detected in kitchen");
    sendMessage(" Flame detected in kitchen ");
  } else {
    Serial.println("Aucune flamme détectée.");
  }
  delay(1000); // Attendez une seconde avant de lire à nouveau
}

void readGazSensor(){
   int gazValue = analogRead(gazSensorPin);
   Serial.print("Valeur du capteur MQ : ");
   Serial.println(gazValue);
  
  if (gazValue >  200) {
    Serial.println("Alerte !");
    sendEmail(senderEmail, senderPassword, recipientEmail, senderName, emailSubjectGaz, "Gaz detected in kitchen");
    sendMessage(" Gaz detected in kitchen ");
  }
   delay(1000);
}
// pour envoyer un email
void sendEmail(const char *from, const char *password, const char *to, const char *name, const char *subject, const char *message) {
  // Le corps de la fonction sendEmail
  WiFiClientSecure client;
  client.setInsecure();
  if (client.connect("smtp.gmail.com", 465)) {
    client.println("EHLO " + WiFi.localIP().toString());
    client.println("AUTH LOGIN");
    client.println(base64::encode(String(senderEmail)));
    client.println(base64::encode(String(password)));
    client.println("MAIL FROM: <" + String(senderEmail) + ">");
    client.println("RCPT TO: <" + String(to) + ">");
    client.println("DATA");
    client.println("From: " + String(name) + " <" + String(senderEmail) + ">");
    client.println("Subject: " + String(subject));
    client.println(message);
    client.println(".");
    client.println("QUIT");
    client.stop();
    Serial.println("Email sent!");
  } else {
    Serial.println("Failed to connect to SMTP server");
  }
}
// pour envoyer un message whatsApp 
void sendMessage(String message){
  String url = "https://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&apikey=" + apiKey + "&text=" + urlEncode(message);
  HTTPClient http;
  http.begin(url);
  // Specify content-type header
http.addHeader("Content-Type", "application/x-www-form-urlencoded");
 // Send HTTP POST request
int httpResponseCode = http.POST(url);
if (httpResponseCode == 200 )
{
  Serial.print("Message sent successfully");
}
  else {
  Serial.println("Error sending the message");
  Serial.print("HTTP response code: ");
  Serial.println(httpResponseCode);
}
// Free resources
http.end();
}
