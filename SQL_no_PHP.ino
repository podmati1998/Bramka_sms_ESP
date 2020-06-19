#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char* ssid = "asdfg";
const char* password = "sylwia0202";

char user[] = "db_user"; // MySQL user login username
char passwordSQL[] = "123"; // MySQL user login password

char INSERT_SQL[] = "INSERT INTO esp_data.przyklad(nazwa,wartosc) VALUES ('ja',5);";
char SELECT_SQL[] = "SELECT * FROM esp_data.przyklad;";
IPAddress server_addr(192, 168, 1, 113); // IP of the MySQL server here


WiFiServer  server(80);
void setup() {

  Serial.begin(115200);

  connectToNetwork();
  mySqlLoop();
}

void loop() {
}
void connectToNetwork() {
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }
 
  Serial.println("Connected to network");
 
}
void mySqlLoop(){
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
}
