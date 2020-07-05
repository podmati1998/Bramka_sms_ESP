void getUserFromId(int id){
   connectToDB();
        String sql = "SELECT * FROM esp_data.users WHERE esp_data.users.id=";
        sql+=id;    
        setQuery(sql);
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
        delete cur_mem;
        serializeJson(doc,jsonChar);
        Serial.println(jsonChar);
        http_rest_server.send(200, "application/json", jsonChar);
        client.stop();
}

void getHistoryElementFromId(int id){
   connectToDB();
        String sql = "SELECT * FROM esp_data.history WHERE esp_data.history.id=";
        sql+=id;    
        setQuery(sql);
        MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
        // Execute the query
        DynamicJsonDocument doc(500);
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
                doc["user_id"]=batchnumber_str.toInt();
                if(f==2)
                doc["group_id"]=batchnumber_str.toInt();
                if(f==3)
                doc["phone_number"]=batchnumber_str;
                if(f==4)
                doc["text"]=batchnumber_str;
                if(f==5){
                doc["timestamp"]=batchnumber_str;
                }
            }
        }      
        delete cur_mem;
        serializeJson(doc,jsonChar);
        Serial.println(jsonChar);
        http_rest_server.send(200, "application/json", jsonChar);
        client.stop();
}
void connectToDB(){

  while (!conn.connect(server_addr, 3306, user, passwordSQL)) {
    Serial.println("Connection failed.");
    Serial.println("Recording data.");
    delay(1000); 
  }
  Serial.println("Database connected.");
}
void setQuery(String query){
  Serial.println(query);
        memset(QUERRY_BUFF,0, sizeof(QUERRY_BUFF));
        query.toCharArray(QUERRY_BUFF,QUERY_MAX_SIZE);
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
void sendSMS(String phone_number,String text){
  
}
