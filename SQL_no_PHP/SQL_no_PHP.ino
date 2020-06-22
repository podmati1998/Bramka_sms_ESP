#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <stdio.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <ListLib.h>

#define HTTP_REST_PORT 8040
#define SELECT_MAX_SIZE 100
const char* ssid = "asdfg";
const char* password = "sylwia0202";
ESP8266WebServer http_rest_server(HTTP_REST_PORT);
char user[] = "db_user"; // MySQL user login username
char passwordSQL[] = "123"; // MySQL user login password

char INSERT_SQL[] = "INSERT INTO esp_data.przyklad(nazwa,wartosc) VALUES ('ja',5);";

char QUERRY_BUFF[SELECT_MAX_SIZE];


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
class History_element{
 public:
  int id;
  int user_id;
  int group_id;
  String phone_number;
  String text;
};
WiFiServer  server(80);

void setup() {

  Serial.begin(115200);

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
        http_rest_server.send(200, "text/html",
            "Welcome to the ESP8266 REST Web Server");
    });
    http_rest_server.on("/get_users", HTTP_GET, getUsers);
    
   // http_rest_server.on("/leds", HTTP_POST, post_put_leds);
   // http_rest_server.on("/leds", HTTP_PUT, post_put_leds);
}



void getUsers(){
  WiFiClient client;
  MySQL_Connection conn((Client *)&client);
  if (conn.connect(server_addr, 3306, user, passwordSQL)) {
    Serial.println("Database connected.");
  }
  else{
    Serial.println("Connection failed.");
    Serial.println("Recording data.");
  }
  // Initiate the query class instance
  String SELECT_USERS = "SELECT * FROM esp_data.users;";
  memset(QUERRY_BUFF,0, sizeof(QUERRY_BUFF));
  SELECT_USERS.toCharArray(QUERRY_BUFF,SELECT_MAX_SIZE);
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  // Execute the query

  cur_mem->execute(QUERRY_BUFF);
  
  column_names *columns = cur_mem->get_columns();
  //testowe aby sprawdzic czy nazwy sie zgadzaja
    for (int f = 0; f < columns->num_fields; f++)
    {
        Serial.print(columns->fields[f]->name);
        if (f < columns->num_fields - 1)
        {
            Serial.print(',');
        }
    }
    Serial.println("Done");

    // Read the rows and print them
    int i=-1;
    Serial.println("Fetching with Rows");
    row_values *row = NULL;
    do
    {   
      i++;   
        row = cur_mem->get_next_row();
        if (row != NULL)
        {
            //e.g convert to a float value
            // float batchnumber = 0;
            //   batchnumber = atol(row->values[0]);
            //   Serial.print("float value: ");
            //   Serial.println(batchnumber,2);
            String batchnumber_str = "";
            for (int f = 0; f < columns->num_fields; f++)
            {
                //just print the String value
                // Serial.print(row->values[f]);
                // convert value to String
                batchnumber_str = String(row->values[f]);
                if(f==0)
                users[i].id=batchnumber_str.toInt();
                if(f==1)
                users[i].name=batchnumber_str;
                if(f==2)
                users[i].surname=batchnumber_str;
                if(f==3)
                users[i].phone_number=batchnumber_str;
                if(f==4)
                users[i].group_id=batchnumber_str.toInt();
                Serial.print("batchnumber_str: ");
                Serial.println(batchnumber_str);
                if (f < columns->num_fields - 1)
                {
                    Serial.print(',');
                }
            }
            Serial.println();
        }
    } while (row != NULL);
    Serial.println("Done");
  // Note: since there are no results, we do not need to read any data
  // Deleting the cursor also frees up memory used
  delete cur_mem;
  Serial.println("closing connection\n");
//write all DEBUG
for (int x=0;x<2;x++){
  Serial.println(users[x].id);
  Serial.println(users[x].name);
  Serial.println(users[x].surname);
  Serial.println(users[x].phone_number);
  Serial.println(users[x].group_id);
}
  
  //client.stop();
}


/*void mySqlLoop(){
  WiFiClient client;
  MySQL_Connection conn((Client *)&client);
  if (conn.connect(server_addr, 3306, user, passwordSQL)) {
    Serial.println("Database connected.");
  }
  else{
    Serial.println("Connection failed.");
    Serial.println("Recording data.");
  }
  // Initiate the query class instance
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  // Execute the query
  cur_mem->execute(SELECT_SQL);
  
  column_names *columns = cur_mem->get_columns();

    for (int f = 0; f < columns->num_fields; f++)
    {
        Serial.print(columns->fields[f]->name);
        if (f < columns->num_fields - 1)
        {
            Serial.print(',');
        }
    }
    Serial.println("Done");

    // Read the rows and print them
    Serial.println("Fetching with Rows");
    row_values *row = NULL;
    do
    {
        row = cur_mem->get_next_row();
        if (row != NULL)
        {
            //e.g convert to a float value
            // float batchnumber = 0;
            //   batchnumber = atol(row->values[0]);
            //   Serial.print("float value: ");
            //   Serial.println(batchnumber,2);
            String batchnumber_str = "";
            for (int f = 0; f < columns->num_fields; f++)
            {
                //just print the String value
                // Serial.print(row->values[f]);
                // convert value to String
                batchnumber_str = String(row->values[f]);
                Serial.print("batchnumber_str: ");
                Serial.println(batchnumber_str);
                if (f < columns->num_fields - 1)
                {
                    Serial.print(',');
                }
            }
            Serial.println();
        }
    } while (row != NULL);
    Serial.println("Done");
  // Note: since there are no results, we do not need to read any data
  // Deleting the cursor also frees up memory used
  delete cur_mem;
  Serial.println("closing connection\n");
  //client.stop();
}*/

void connectToNetwork() {
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }
 
  Serial.print("Connetted to ");
        Serial.print(ssid);
        Serial.print("--- IP: ");
        Serial.println(WiFi.localIP());
 
}
