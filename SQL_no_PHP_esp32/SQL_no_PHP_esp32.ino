
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#define HTTP_REST_PORT 8040
#define SELECT_MAX_SIZE 200
const char* ssid = "asdfg";
const char* password = "sylwia0202";
WebServer http_rest_server(HTTP_REST_PORT);
char user[] = "db_user"; // MySQL user login username
char passwordSQL[] = "123"; // MySQL user login password

char INSERT_SQL[] = "INSERT INTO esp_data.przyklad(nazwa,wartosc) VALUES ('ja',5);";

char QUERRY_BUFF[SELECT_MAX_SIZE];
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
    http_rest_server.on("/users", HTTP_GET, getUsers);    
    http_rest_server.on("/user", HTTP_POST, postUser);
    http_rest_server.on("/user", HTTP_GET, getUser);
}

                              //GET USERS
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
  Serial.println(QUERRY_BUFF);
  // Execute the query
  cur_mem->execute(QUERRY_BUFF);  
  column_names *columns = cur_mem->get_columns();
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
            String batchnumber_str = "";
            for (int f = 0; f < columns->num_fields; f++)
            {
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
            }
        }
    } while (row != NULL);
    Serial.println("Done reading users");
  // Note: since there are no results, we do not need to read any data
  // Deleting the cursor also frees up memory used
  delete cur_mem;
for (int x=0;x<i;x++){
  Serial.println(users[x].id);
  Serial.println(users[x].name);
  Serial.println(users[x].surname);
  Serial.println(users[x].phone_number);
  Serial.println(users[x].group_id);
}
//to json and send response
{
  DynamicJsonDocument doc((i+1)*114+2);
  for (int x=0;x<i;x++){
    doc[x]["id"]=users[x].id;
    doc[x]["name"]=users[x].name;
    doc[x]["surname"]=users[x].surname;
    doc[x]["phone_number"]=users[x].phone_number;
    doc[x]["group_id"]=users[x].group_id;
  }  
  memset(jsonChar,0, sizeof(jsonChar));
  serializeJson(doc,jsonChar);
}
  Serial.println(jsonChar);
  http_rest_server.send(200, "application/json", jsonChar);
  client.stop();
}
                              //GET USER
void getUser(){
  WiFiClient client;
  MySQL_Connection conn((Client *)&client);
  if (conn.connect(server_addr, 3306, user, passwordSQL)) {
    Serial.println("Database connected.");
  }
  else{
    Serial.println("Connection failed.");
    Serial.println("Recording data.");
  }
    String id = http_rest_server.arg("id");
     
          // Initiate the query class instance
        String SELECT_USERS = "SELECT * FROM esp_data.users WHERE esp_data.users.id=";
        SELECT_USERS+=id;

        Serial.println(SELECT_USERS);
        memset(QUERRY_BUFF,0, sizeof(QUERRY_BUFF));
        SELECT_USERS.toCharArray(QUERRY_BUFF,SELECT_MAX_SIZE);
        MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
        // Execute the query
        DynamicJsonDocument doc(150);
        cur_mem->execute(QUERRY_BUFF);
        column_names *columns = cur_mem->get_columns();
        row_values *row = NULL;
         row = cur_mem->get_next_row();
        if (row != NULL)
        {
        String batchnumber_str = "";
        for (int f = 0; f < columns->num_fields; f++)
            {
                batchnumber_str = String(row->values[f]);
                if(f==0)
                doc["id"]=batchnumber_str.toInt();
                if(f==1)
                doc["name"]=batchnumber_str;
                if(f==2)
                doc["surname"]=batchnumber_str;
                if(f==3)
                doc["phone_number"]=batchnumber_str;
                if(f==4)
                doc["group_id"]=batchnumber_str.toInt();
            }
        }      
        serializeJson(doc,jsonChar);
        Serial.println(jsonChar);
        http_rest_server.send(200, "application/json", jsonChar);
        client.stop();    
    }
                              //POST USER
void postUser(){
  WiFiClient client;
  MySQL_Connection conn((Client *)&client);
  if (conn.connect(server_addr, 3306, user, passwordSQL)) {
    Serial.println("Database connected.");
  }
  else{
    Serial.println("Connection failed.");
    Serial.println("Recording data.");
  }
  //read body
  StaticJsonDocument<200> doc;
    String post_body = http_rest_server.arg("plain");
    Serial.println(post_body);
    DeserializationError err = deserializeJson(doc, http_rest_server.arg("plain"));
    if (err!=DeserializationError::Ok) {
        Serial.println("error in parsin json body");
        http_rest_server.send(400);
    }
    else {   
      if(doc["name"]!=0 && doc["surname"]!=0 && doc["phone_number"]!=0 && doc["group_id"]!=0){
        const char* name = doc["name"];
        const char* surname = doc["surname"];
        const char* phone_number = doc["phone_number"];
        int group_id = doc["group_id"];

          // Initiate the query class instance
        String SELECT_USERS = "INSERT INTO esp_data.users (name,surname,phone_number,group_id) VALUES";
        SELECT_USERS+=" (\"";
        SELECT_USERS+=name;
        SELECT_USERS+="\",\"";
        SELECT_USERS+=surname;
        SELECT_USERS+="\",\"";
        SELECT_USERS+=phone_number;
        SELECT_USERS+="\",\"";
        SELECT_USERS+=group_id;
        SELECT_USERS+="\")";

        Serial.println(SELECT_USERS);
        memset(QUERRY_BUFF,0, sizeof(QUERRY_BUFF));
        SELECT_USERS.toCharArray(QUERRY_BUFF,SELECT_MAX_SIZE);
        MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
        // Execute the query
        cur_mem->execute(QUERRY_BUFF);
        int last_id = cur_mem->get_last_insert_id();
        Serial.println(last_id);
        getUserFromId(last_id);
      }
    }
}

void getUserFromId(int id){
  WiFiClient client;
  MySQL_Connection conn((Client *)&client);
  if (conn.connect(server_addr, 3306, user, passwordSQL)) {
    Serial.println("Database connected.");
  }
  else{
    Serial.println("Connection failed.");
    Serial.println("Recording data.");
  }
        String SELECT_USERS = "SELECT * FROM esp_data.users WHERE esp_data.users.id=";
        SELECT_USERS+=id;
        
        Serial.println(SELECT_USERS);
        memset(QUERRY_BUFF,0, sizeof(QUERRY_BUFF));
        SELECT_USERS.toCharArray(QUERRY_BUFF,SELECT_MAX_SIZE);
        MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
        // Execute the query
        DynamicJsonDocument doc(150);
        cur_mem->execute(QUERRY_BUFF);
        column_names *columns = cur_mem->get_columns();
        row_values *row = NULL;
         row = cur_mem->get_next_row();
        if (row != NULL)
        {
        String batchnumber_str = "";
        for (int f = 0; f < columns->num_fields; f++)
            {
                batchnumber_str = String(row->values[f]);
                if(f==0)
                doc["id"]=batchnumber_str.toInt();
                if(f==1)
                doc["name"]=batchnumber_str;
                if(f==2)
                doc["surname"]=batchnumber_str;
                if(f==3)
                doc["phone_number"]=batchnumber_str;
                if(f==4)
                doc["group_id"]=batchnumber_str.toInt();
            }
        }      
        serializeJson(doc,jsonChar);
        Serial.println(jsonChar);
        http_rest_server.send(200, "application/json", jsonChar);
        client.stop();
}

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
