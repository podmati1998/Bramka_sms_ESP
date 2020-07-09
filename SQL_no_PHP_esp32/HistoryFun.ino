void getHistoryElement(){
  String id = http_rest_server.arg("id"); 
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
        http_rest_server.send(200, "application/json", jsonChar);
        client.stop();
}

void getHistory(){
  String id = http_rest_server.arg("id");
  if((id = http_rest_server.arg("id"))==NULL){
    id="0";
  }
   connectToDB();
  // Initiate the query class instance
  int high_id =id.toInt();
  high_id+=50;
  String sql = "SELECT * FROM esp_data.history h WHERE h.id >= ";
  sql+=id;
  sql+=" AND h.id < ";
  sql+=high_id;
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
                history[i].id=batchnumber_str.toInt();
                if(f==1)
                history[i].user_id=batchnumber_str.toInt();
                if(f==2)
                history[i].group_id=batchnumber_str.toInt();
                if(f==3)
                history[i].phone_number=batchnumber_str;
                if(f==4)
                history[i].text=batchnumber_str;
                if(f==5)
                history[i].timestamp=batchnumber_str;
            }
        }
    } while (row != NULL);
    Serial.println("Done reading users");
  // Note: since there are no results, we do not need to read any data
  // Deleting the cursor also frees up memory used
  delete cur_mem;
/*for (int x=0;x<i;x++){
  Serial.println(users[x].id);
  Serial.println(users[x].name);
  Serial.println(users[x].surname);
  Serial.println(users[x].phone_number);
  Serial.println(users[x].group_id);
}*/
//to json and send response
{
  DynamicJsonDocument doc((i+1)*600+2);
  for (int x=0;x<i;x++){
    doc[x]["id"]=history[x].id;
    doc[x]["user_id"]=history[x].user_id;
    doc[x]["group_id"]=history[x].group_id;
    doc[x]["phone_number"]=history[x].phone_number;
    doc[x]["text"]=history[x].text;
    doc[x]["timestamp"]=history[x].timestamp;
  }  
  memset(jsonChar,0, sizeof(jsonChar));
  serializeJson(doc,jsonChar);
}
  Serial.println(jsonChar);
  http_rest_server.sendHeader("Access-Control-Allow-Origin", "*");
  http_rest_server.send(200, "application/json", jsonChar);
  client.stop();
}
