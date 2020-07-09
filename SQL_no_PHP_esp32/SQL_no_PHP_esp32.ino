
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include "time.h"
//config TinyGSM library and set modem constants
#define TINY_GSM_MODEM_SIM800      // Modem is SIM800
#define TINY_GSM_RX_BUFFER   1024  // Set RX buffer to 1Kb
#include <TinyGsmClient.h>
#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23
#define MODEM_TX             27
#define MODEM_RX             26
#define SerialAT  Serial1
//set some other constants
#define HTTP_REST_PORT 8040
#define QUERY_MAX_SIZE 500
//USERS AND PASSWORDS
const char* ssid = "asdfg";
const char* password = "sylwia0202";
char user[] = "db_user"; // MySQL user login username
char passwordSQL[] = "123"; // MySQL user login password

char QUERRY_BUFF[QUERY_MAX_SIZE]; //bufor zapytań
char jsonChar[10000]; //plik dla eksportu jsonów (max rozmiar to 10kB)

IPAddress server_addr(192, 168, 1, 113); // IP of the MySQL server here
class User{
 public:
  int id;
  String name;
  String surname;
  String phone_number;
  int group_id;
  User(){
  }
};
User users[100];
class Group{
 public:
  int id;
  String name;
  Group(){
  }
};
Group groups[10];
class History_element{
 public:
  int id;
  int user_id;
  int group_id;
  String phone_number;
  String text;
  String timestamp;
  History_element(){
  }
};
History_element history[50];

//stworzenie instancji obiektow globalnych
WebServer http_rest_server(HTTP_REST_PORT);
WiFiServer  server(80);
WiFiClient client;
TinyGsm modem(SerialAT);
MySQL_Connection conn((Client *)&client);
void setup() {

  Serial.begin(115200);
// Set modem reset, enable, power pins
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);
  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  // Restart SIM800 module, it takes quite some time
  // To skip it, call init() instead of restart()
  Serial.println("Initializing modem...");
  modem.restart();
  connectToNetwork();
  config_rest_server_routing();
  http_rest_server.begin();
  Serial.println("HTTP REST Server Started");
  //mySqlLoop();
}
void loop() {
  http_rest_server.handleClient();
}
//konfiguracja serwera REST
void config_rest_server_routing() {
    http_rest_server.on("/", HTTP_GET, []() {
      http_rest_server.sendHeader("Access-Control-Allow-Origin", "*");
        http_rest_server.send(200, "text/html",
            "Welcome to the ESP8266 REST Web Server");
    });
    http_rest_server.on("/api/users", HTTP_GET, getUsers);    
    http_rest_server.on("/api/users", HTTP_POST, postUser);
    http_rest_server.on("/api/user", HTTP_GET, getUser);
    http_rest_server.on("/api/users", HTTP_DELETE, deleteUser);
    http_rest_server.on("/api/users", HTTP_PUT, updateUser);   
    http_rest_server.on("/api/groups", HTTP_GET, getGroups);
    http_rest_server.on("/api/groups", HTTP_POST, postGroup);    
    http_rest_server.on("/api/group", HTTP_GET, getGroup);
    http_rest_server.on("/api/group_users", HTTP_GET, getGroupUsers);
    http_rest_server.on("/api/groups", HTTP_DELETE, deleteGroup);
    http_rest_server.on("/api/groups", HTTP_PUT, updateGroup);
    http_rest_server.on("/api/send_number", HTTP_POST, sendToNumber);
    http_rest_server.on("/api/send_user", HTTP_POST, sendToUser);
    http_rest_server.on("/api/send_group", HTTP_POST, sendToGroup);
    http_rest_server.on("/api/history_element", HTTP_GET, getHistoryElement);
    http_rest_server.on("/api/history", HTTP_GET, getHistory);
}
