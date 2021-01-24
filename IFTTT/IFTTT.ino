#include <M5Stack.h>
#include <WiFi.h>
#include <WiFiClient.h>

float ondo = 38.3;
const char* ssid     = "yutayのiPhone";  //変更点
const char* password = "Tsu10711";      //変更点

String makerEvent = "thermal_mail"; // Maker Webhooks
String makerKey = "brc6Zplml_dErA2kakoJrQ"; // Maker Webhooks

const char* server = "maker.ifttt.com";  // Server URL
WiFiClient client;

bool checkWifiConnected() {
  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    // wait 1 second for re-trying
    delay(1000);
  }

  Serial.print("Connected to ");
  Serial.println(ssid);
  return true;
}

void send( float value1 ) {  //, String value2, String value3
  while (!checkWifiConnected()) {
    Serial.print("Attempting to connect to WiFi");
    WiFi.begin(ssid, password);
  }

  Serial.println("\nStarting connection to server...");
  if (!client.connect(server, 80)) {
    Serial.println("Connection failed!");
  } else {
    Serial.println("Connected to server!");
    // Make a HTTP request:
    String url = "/trigger/" + makerEvent + "/with/key/" + makerKey;
    url += "?value1=" + String(value1);
    //+ "&value2=" + value2 + "&value3=" + value3;
    client.println("GET " + url + " HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();
    Serial.print("Waiting for response "); //WiFiClientSecure uses a non blocking implementation

    int count = 0;
    while (!client.available()) {
      delay(50); //
      Serial.print(".");
    }
    // if there are incoming bytes available
    // from the server, read them and print them:
    while (client.available()) {
      char c = client.read();
      Serial.write(c);
    }

    // if the server's disconnected, stop the client:
    if (!client.connected()) {
      Serial.println();
      Serial.println("disconnecting from server.");
      client.stop();
    }
  }
}

void setup() {
  //Initialize serial and wait for port to open:
    M5.begin();

  /*
    Power chip connected to gpio21, gpio22, I2C device
    Set battery charging voltage and current
    If used battery, please call this function in your project
  */
  M5.Power.begin();
  Serial.begin(115200);
  delay(100);
  M5.Lcd.println("yen");

   WiFi.begin(ssid, password);
  while (!checkWifiConnected()) {
    WiFi.begin(ssid, password);
  }
  delay(100);
  /*
  起動テスト
  send("38.2");
   M5.Lcd.println("send");
  */
}

void loop() {
  M5.update();
  if (M5.BtnA.wasReleased()) {  //テスト
  
  //if () {
    makerEvent = "thermal_mail";
    send(ondo);
    makerEvent = "thermal-over";
    send(ondo);
        
    M5.Lcd.println("send");
  }
  
  delay(20);
}
