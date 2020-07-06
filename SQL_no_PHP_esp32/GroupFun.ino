void getGroups(){
  
  connectToDB();
  // Initiate the query class instance
  String sql = "SELECT * FROM esp_data.groups;";
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
                groups[i].id=batchnumber_str.toInt();
                if(f==1)
                groups[i].name=batchnumber_str;
            }
        }
    } while (row != NULL);
    Serial.println("Done reading groups");
  // Note: since there are no results, we do not need to read any data
  // Deleting the cursor also frees up memory used
  delete cur_mem;
for (int x=0;x<i;x++){
  Serial.println(groups[x].id);
  Serial.println(groups[x].name);

}
//to json and send response
{
  DynamicJsonDocument doc((i+1)*114+2);
  for (int x=0;x<i;x++){
    doc[x]["id"]=groups[x].id;
    doc[x]["name"]=groups[x].name;
  }  
  memset(jsonChar,0, sizeof(jsonChar));
  serializeJson(doc,jsonChar);
}
  Serial.println(jsonChar);
  http_rest_server.send(200, "application/json", jsonChar);
  client.stop();
}
                              //GET GROUP
void getGroup(){
    connectToDB();
    String id = http_rest_server.arg("id"); 
      // Initiate the query class instance
    String sql = "SELECT u.id,u.name,u.surname,u.phone_number FROM esp_data.groups g JOIN esp_data.users u ";
    sql+="ON g.id=u.group_id WHERE g.id=";
    sql+=id;
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
     DynamicJsonDocument doc((i+1)*100);     
     for (int x=0;x<i;x++){
      doc[x]["id"]=users[x].id;
      doc[x]["name"]=users[x].name;
      doc[x]["surname"]=users[x].surname;
      doc[x]["phone_number"]=users[x].phone_number;
     }
     memset(jsonChar,0, sizeof(jsonChar));
     serializeJson(doc,jsonChar);
     Serial.println(jsonChar);
     http_rest_server.send(200, "application/json", jsonChar);
     client.stop();    
}
                              //POST USER
void postGroup(){
  connectToDB();
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
      if(doc["name"]!=0){
        const char* name = doc["name"];
          // Initiate the query class instance
        String sql = "INSERT INTO esp_data.groups (name) VALUES";
        sql+=" (\"";
        sql+=name;
        sql+="\")";
        setQuery(sql);
        MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
        // Execute the query
        cur_mem->execute(QUERRY_BUFF);
        int last_id = cur_mem->get_last_insert_id();
        delete cur_mem;
        Serial.println(last_id);
        getGroupFromId(last_id);
      }
    }
}

void updateGroup(){
  connectToDB();
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
      if(doc["id"]!=0 && doc["name"]!=0){
        int id = doc["id"];
        const char* name = doc["name"];
          // Initiate the query class instance
        String sql = "UPDATE esp_data.groups SET name = \"";
        sql+=name;
        sql+="\" ";
        sql+=" WHERE id = ";
        sql+=id;
        setQuery(sql);
        MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
        // Execute the query
        cur_mem->execute(QUERRY_BUFF);
        delete cur_mem;
        Serial.println(id);
        getGroupFromId(id);
      }
    }
}
           //DELETE GROUP
void deleteGroup(){
   connectToDB();
    String id = http_rest_server.arg("id");
     
          // Initiate the query class instance
        String sql = "DELETE FROM esp_data.groups WHERE esp_data.groups.id=";
        sql+=id;
        setQuery(sql);
        MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
        // Execute the query
        cur_mem->execute(QUERRY_BUFF);
        delete cur_mem;
        http_rest_server.send(200, "application/json", "{ \"id\": \""+id+"\"}");
        client.stop();    
    }
