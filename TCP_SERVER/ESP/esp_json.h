#ifndef ESP_JSON_H
#define ESP_JSON_H
/*
    This is use for esp server's json operation
*/
#include <unistd.h>
#include <cstdio>
#include <string>
#include <fstream>
#include <json/json.h>

#define DEBUG_ESP_JSON 0
#define WAIT_FILE_TIME_US 1000
#define JSON_BUFFER_LEN 65536
/* provide read json file functions */
class JsonReader
{
protected:
    // Json::Reader json_reader;
    JsonReader() {}
    virtual ~JsonReader() {}
};
/* provide write json file functions */
class JsonWriter
{
protected:
    std::string write_buffer;
    JsonWriter() {}
    virtual ~JsonWriter() {}
    virtual void add_key_str_to_tree(const std::string& key, const std::string& str)=0;
    virtual void write_json_to_file()=0;
};
/* class of esp json functions */
class EspJson : JsonWriter, JsonReader
{
private:
    Json::Value json_root;
    std::fstream json_file;
    std::string file_name;
public:
    explicit EspJson(const std::string& file_name_) : file_name(file_name_) {
        write_buffer.clear();
        json_file.open(file_name, std::ios::in);
        while (json_file.is_open() == false) { /* open file */
            json_file.open(file_name, std::ios::out);
            printf("\nwait the file to open...\n");
            usleep(WAIT_FILE_TIME_US);
        }
        json_file >> json_root;
        json_file.close();
    }
    virtual ~EspJson() {
        if (json_file.is_open())
            json_file.close();
    }
    /* writer functions */
    void add_key_str_to_tree(const std::string& key, const std::string& str) override;
    void write_json_to_file() override;
    /* reader functions */
};
#endif 