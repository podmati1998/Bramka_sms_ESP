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
            }
        }      
        delete cur_mem;
        serializeJson(doc,jsonChar);
        Serial.println(jsonChar);
        http_rest_server.sendHeader("Access-Control-Allow-Origin", "*");
        http_rest_server.send(200, "application/json", jsonChar);
        client.stop();
}

void historyElementResponse(int code, int id){
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
        http_rest_server.sendHeader("Access-Control-Allow-Origin", "*");
        http_rest_server.send(code, "application/json", jsonChar);
        client.stop();
}

void getGroupFromId(int id){
   connectToDB();    
          // Initiate the query class instance
        String sql = "SELECT * FROM esp_data.groups ";
        sql+="WHERE id=";
        sql+=id;
        setQuery(sql);
        MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
        // Execute the query
        
        cur_mem->execute(QUERRY_BUFF);
        column_names *columns = cur_mem->get_columns();
        row_values *row = NULL;
        row = cur_mem->get_next_row();
        DynamicJsonDocument doc(100);
        if (row != NULL)
        {
          String batchnumber_str = "";
        for (int f = 0; f < columns->num_fields; f++)
            {
              batchnumber_str = String(row->values[f]);
              if(f==0)
              doc["group_id"]=batchnumber_str.toInt();
              if(f==1)
              doc["group_name"]=batchnumber_str;
            }           
        }      
        delete cur_mem;      
        memset(jsonChar,0, sizeof(jsonChar));
        serializeJson(doc,jsonChar); 
        Serial.println(jsonChar);
        http_rest_server.sendHeader("Access-Control-Allow-Origin", "*");
        http_rest_server.send(200, "application/json", jsonChar);
        client.stop();  
}

void getGroupUsersFromId(int group_id){
    connectToDB();
      // Initiate the query class instance
    String sql = "SELECT u.id,u.name,u.surname,u.phone_number FROM esp_data.group_to_user g JOIN esp_data.users u ";
    sql+="ON g.user_id=u.id WHERE g.group_id=";
    sql+=group_id;
    setQuery(sql);
    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
      // Execute the query   
    cur_mem->execute(QUERRY_BUFF);
    column_names *columns = cur_mem->get_columns();
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
            }
        }
    } while (row != NULL);
    Serial.println("Done reading group details");
  // Note: since there are no results, we do not need to read any data
  // Deleting the cursor also frees up memory used
    delete cur_mem;          
     DynamicJsonDocument doc((i+1)*100+10);     
     for (int x=0;x<i;x++){
      doc[x]["id"]=users[x].id;
      doc[x]["name"]=users[x].name;
      doc[x]["surname"]=users[x].surname;
      doc[x]["phone_number"]=users[x].phone_number;
     }
     memset(jsonChar,0, sizeof(jsonChar));
     serializeJson(doc,jsonChar);
     Serial.println(jsonChar);
     http_rest_server.sendHeader("Access-Control-Allow-Origin", "*");
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
int sendSMS(String phone_number,String text){
  
}
