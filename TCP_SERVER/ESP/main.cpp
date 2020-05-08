#include <unistd.h>
#include <cstdio>
#include <string>
#include <iostream>
#include "esp_tcp_server.h"
#include "esp_json.h"

#define FILE_NAME "esp_data.json"
#define LOOP_WAIT_TIME_US 10

EspTcpServer esp_server(10, 28266);
EspJson* esp_json;
std::string server_data;

bool data_form_check(std::string& server_data)
{
    if (server_data.length() == 0)
        return false;
    int length = server_data.length();
    if ( (server_data[0] == '#' && server_data[1] == '#' && server_data[2] == ';') && 
        (server_data[length-1] == '$' && server_data[length-2] == '$' &&  server_data[length-3] == ';') ) {
        server_data.erase(0,3);
        length = server_data.length();
        server_data.erase(length-3, 3);
        return true;
    } else {
        printf("\nfail checking func's data string:%s\n", server_data.c_str());//log
        return false;
    }
}

void get_pair_data(std::string& server_data)
{
    // printf("\nbegin get_pair_data...str is:%s\n", server_data.c_str());//log
    int length = server_data.length();
    short i = 0;
    std::string key = "";
    std::string value = "";
    /* has bug below */
    for (; (i < length) && (server_data[i] != ':') ; ++i) {
        key.append(1, server_data[i]);
    }
    server_data = server_data.substr(++i);
    length = server_data.length();
    for (i = 0; (i < length) && (server_data[i] != ';') ; ++i) { /* must reset the 'i' to 0 */
        value.append(1, server_data[i]);
    }
    if (i+1 < length)
        server_data = server_data.substr(++i);
    else
        server_data.clear();
    esp_json->add_key_str_to_tree(key, value);
}

int main()
{
    
    /* init tcp server*/
    esp_server.init();
    while (1) {
        /*
            get data from server and write them to json file
        */
        server_data = esp_server.get_server_data();
        if (data_form_check(server_data)) {
            esp_json = new EspJson(FILE_NAME);
            while (server_data.size() != 0) {
                get_pair_data(server_data);
                usleep(LOOP_WAIT_TIME_US);
            }
            esp_json->write_json_to_file();
            esp_json->~EspJson();
        }
       usleep(esp_server.SERVER_RECV_WAIT_TIME_US);
    }
    return 0;
}
