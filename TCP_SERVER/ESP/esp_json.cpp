#include "esp_json.h"

void EspJson::add_key_str_to_tree(const std::string& key, const std::string& str)
{
    json_root[key.c_str()] = str.c_str();
}

void EspJson::write_json_to_file()
{
    json_file.open(file_name, std::ios::out);
    while (json_file.is_open() == false) { /* open file */
        json_file.open(file_name, std::ios::out);
        printf("\nwait the file to open...\n");
        usleep(WAIT_FILE_TIME_US);
    }
    write_buffer = json_root.toStyledString();
    json_file << write_buffer;
    write_buffer.clear(); /* release cache */
    json_file.close(); /* close file */
}

#if DEBUG_ESP_JSON
int main()
{
    EspJson my_json("testfile.json");
    my_json.add_key_str_to_tree("fuck_you", "you mother fucker 1000");
    my_json.write_json_to_file();
    // std::fstream test("testfile.json", std::ios::out);
    // test.close();
    return 0;
}
#endif 