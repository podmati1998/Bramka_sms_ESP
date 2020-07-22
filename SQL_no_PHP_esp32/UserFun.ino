                              //GET USERS
void getUsers(){
  
  connectToDB();
  // Initiate the query class instance
  String sql = "SELECT * FROM esp_data.users;";
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
}
//to json and send response
{
  DynamicJsonDocument doc((i+1)*114+2);
  for (int x=0;x<i;x++){
    doc[x]["id"]=users[x].id;
    doc[x]["name"]=users[x].name;
    doc[x]["surname"]=users[x].surname;
    doc[x]["phone_number"]=users[x].phone_number;
  }  
  memset(jsonChar,0, sizeof(jsonChar));
  serializeJson(doc,jsonChar);
}
  Serial.println(jsonChar);
  http_rest_server.sendHeader("Access-Control-Allow-Origin", "*");
  http_rest_server.send(200, "application/json", jsonChar);
  client.stop();
}
                              //GET USER
void getUser(){
   connectToDB();
    String id = http_rest_server.arg("id");
     
          // Initiate the query class instance
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
                              //POST USER
void postUser(){
  connectToDB();
  //read body
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
      if(doc["name"]!=0 && doc["surname"]!=0 && doc["phone_number"]!=0){
        const char* name = doc["name"];
        const char* surname = doc["surname"];
        const char* phone_number = doc["phone_number"];

          // Initiate the query class instance
        String sql = "INSERT INTO esp_data.users (name,surname,phone_number) VALUES";
        sql+=" (\"";
        sql+=name;
        sql+="\",\"";
        sql+=surname;
        sql+="\",\"";
        sql+=phone_number;
        sql+="\")";

        setQuery(sql);
        MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
        // Execute the query
        cur_mem->execute(QUERRY_BUFF);
        int last_id = cur_mem->get_last_insert_id();
        delete cur_mem;
        Serial.println(last_id);
        getUserFromId(last_id);
      }
    }
}

void updateUser(){
  connectToDB();
  //read body
  StaticJsonDocument<300> doc;
    String post_body = http_rest_server.arg("plain");
    Serial.println(post_body);
    DeserializationError err = deserializeJson(doc, http_rest_server.arg("plain"));
    if (err!=DeserializationError::Ok) {
        Serial.println("error in parsin json body");
        http_rest_server.send(400);
    }
    else {   
      if(doc["id"]!=0 && doc["name"]!=0 && doc["surname"]!=0 && doc["phone_number"]!=0){
        int id = doc["id"];
        const char* name = doc["name"];
        const char* surname = doc["surname"];
        const char* phone_number = doc["phone_number"];

          // Initiate the query class instance
        String sql = "UPDATE esp_data.users SET name = \"";
        sql+=name;
        sql+="\", surname = \"";
        sql+=surname;
        sql+="\", phone_number = \"";
        sql+=phone_number;
        sql+="\" WHERE id = ";
        sql+=id;

        setQuery(sql);
        MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
        // Execute the query
        cur_mem->execute(QUERRY_BUFF);
        delete cur_mem;
        Serial.println(id);
        getUserFromId(id);
      }
    }
}
           //DELETE USER
void deleteUser(){
   connectToDB();
    String id = http_rest_server.arg("id");
     
          // Initiate the query class instance
        String sql = "DELETE FROM esp_data.users WHERE esp_data.users.id=";
        sql+=id;
        setQuery(sql);
        MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
        // Execute the query
        cur_mem->execute(QUERRY_BUFF);
        delete cur_mem;
        http_rest_server.sendHeader("Access-Control-Allow-Origin", "*");
        http_rest_server.send(200, "application/json", "{ \"id\": \""+id+"\"}");
        client.stop();    
    }

                //GET USER GROUPS
void getUserGroups(){
    connectToDB();
    String id = http_rest_server.arg("id"); 
      // Initiate the query class instance
    String sql = "SELECT g.id, g.name FROM esp_data.group_to_user gu JOIN esp_data.users u ";
    sql+="ON gu.user_id=u.id ";
    sql+="JOIN esp_data.groups g ";
    sql+="ON g.id=gu.group_id ";
    sql+="WHERE u.id= ";
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
                groups[i].id=batchnumber_str.toInt();
                if(f==1)
                groups[i].name=batchnumber_str;
            }
        }
    } while (row != NULL);
    Serial.println("Done reading group details");
  // Note: since there are no results, we do not need to read any data
  // Deleting the cursor also frees up memory used
    delete cur_mem;          
     DynamicJsonDocument doc((i+1)*50+20);     
     for (int x=0;x<i;x++){
      doc[x]["id"]=groups[x].id;
      doc[x]["name"]=groups[x].name;

     }
     memset(jsonChar,0, sizeof(jsonChar));
     serializeJson(doc,jsonChar);
     Serial.println(jsonChar);
     http_rest_server.sendHeader("Access-Control-Allow-Origin", "*");
     http_rest_server.send(200, "application/json", jsonChar);
     client.stop();    
}
