void sendToGroup(){
   connectToDB();
  StaticJsonDocument<200> doc;
    String post_body = http_rest_server.arg("plain");
    Serial.println(post_body);
    DeserializationError err = deserializeJson(doc, http_rest_server.arg("plain"));
    if (err!=DeserializationError::Ok) {
        Serial.println("error in parsin json body");
        http_rest_server.sendHeader("Access-Control-Allow-Origin", "*");
        http_rest_server.send(400);
    }
    else {   
      if(doc["group_id"]!=0 && doc["text"]!=0){
        int group_id = doc["group_id"];
        const char* text = doc["text"];
        //GET USER NUMBER
        String sql = "SELECT u.id,u.name,u.surname,u.phone_number FROM esp_data.users u JOIN esp_data.group_to_user gu ";
        sql+="ON u.id=gu.user_id WHERE gu.group_id=";
        sql+=group_id;   
        setQuery(sql);
        MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
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
            }
          }
        }while (row != NULL);
        Serial.println("Done reading users");
        delete cur_mem;
        boolean sendSuccess = true;
        //SEND THE MESSAGE
        for(int x=0;x<i;x++){
          if(!modem.sendSMS(users[x].phone_number, text))
            sendSuccess=false;        
        }
          Serial.println(text);
          // QUERRY INITIALIZER        
          sql = "INSERT INTO esp_data.history (group_id,text,timestamp) VALUES";
          sql+=" (\"";
          sql+=group_id;
          sql+="\",\"";
          sql+=text;
          sql+="\",";
          sql+="CURRENT_TIMESTAMP";
          sql+=")";
          setQuery(sql);
          // Execute the query
          connectToDB();
          cur_mem = new MySQL_Cursor(&conn);
          cur_mem->execute(QUERRY_BUFF);
          int last_id = cur_mem->get_last_insert_id();
          Serial.println(last_id);
          if(sendSuccess)
            historyElementResponse(200,last_id);
          else
            historyElementResponse(206,last_id);
          client.stop();               
      }
    }   
}

void sendToNumber(){
   connectToDB();
  StaticJsonDocument<200> doc;
    String post_body = http_rest_server.arg("plain");
    Serial.println(post_body);
    DeserializationError err = deserializeJson(doc, http_rest_server.arg("plain"));
    if (err!=DeserializationError::Ok) {
        Serial.println("error in parsin json body");
        http_rest_server.sendHeader("Access-Control-Allow-Origin", "*");
        http_rest_server.send(400);
    }
    else {   
      if(doc["phone_number"]!=0 && doc["text"]!=0){
        const char* phone_number = doc["phone_number"];
        const char* text = doc["text"];
        //SEND THE MESSAGE
        if(modem.sendSMS(phone_number, text)){
          Serial.println(text);
          // QUERRY INITIALIZER
        String sql = "INSERT INTO esp_data.history (phone_number,text,timestamp) VALUES";
        sql+=" (\"";
        sql+=phone_number;
        sql+="\",\"";
        sql+=text;
        sql+="\",";
        sql+="CURRENT_TIMESTAMP";
        sql+=")";

        setQuery(sql);
        MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
        // Execute the query
        cur_mem->execute(QUERRY_BUFF);
        int last_id = cur_mem->get_last_insert_id();
        delete cur_mem;
        Serial.println(last_id);
        historyElementResponse(200, last_id);
        client.stop();       
        }
        else{
          Serial.println("SMS failed to send");
        }         
      }
    }   
}

void sendToUser(){
   connectToDB();
  StaticJsonDocument<200> doc;
    String post_body = http_rest_server.arg("plain");
    Serial.println(post_body);
    DeserializationError err = deserializeJson(doc, http_rest_server.arg("plain"));
    if (err!=DeserializationError::Ok) {
        Serial.println("error in parsin json body");
        http_rest_server.send(400);
    }
    else {   
      if(doc["user_id"]!=0 && doc["text"]!=0){
        int user_id = doc["user_id"];
        const char* text = doc["text"];
        //GET USER NUMBER
        String sql = "SELECT phone_number FROM esp_data.users WHERE esp_data.users.id=";
        sql+=user_id;
    
        setQuery(sql);
        MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
        cur_mem->execute(QUERRY_BUFF);
        column_names *columns = cur_mem->get_columns();
        row_values *row = NULL;
        row = cur_mem->get_next_row();
        String phone_number = "";
        if (row != NULL)
        phone_number = String(row->values[0]);  
        delete cur_mem;              
        //SEND THE MESSAGE
        if(modem.sendSMS(phone_number, text)){
          Serial.println(text);
          // QUERRY INITIALIZER
          
          String sql = "INSERT INTO esp_data.history (user_id,text,timestamp) VALUES";
          sql+=" (\"";
          sql+=user_id;
          sql+="\",\"";
          sql+=text;
          sql+="\",";
          sql+="CURRENT_TIMESTAMP";
          sql+=")";
          setQuery(sql);
          // Execute the query
          connectToDB();
          cur_mem = new MySQL_Cursor(&conn);
          cur_mem->execute(QUERRY_BUFF);
          int last_id = cur_mem->get_last_insert_id();
          Serial.println(last_id);
          historyElementResponse(200,last_id);
          client.stop();       
        }
        else{
          Serial.println("SMS failed to send");
        }         
      }
    }   
}
