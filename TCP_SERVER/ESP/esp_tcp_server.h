#ifndef ESP_TCP_SERVER_H
#define ESP_TCP_SERVER_H
/*
    This is use for esp server's test and get data
*/
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <string>
#include <thread>
#include <mutex>
#include <memory> // enable smart pointer
#include <queue>

#define DEBUG_ESP_TCP_SERVER 0
#define SERVER_BUFFER_LEN 256
// #define SERVER_RECV_WAIT_TIME_US 500000s
/* This is server socker data class*/
class ServerBasic
{
protected:
    const unsigned short local_port;
    int sock_server;
    int sock_client;
    socklen_t addr_len;
    struct sockaddr_in s_in;
    struct sockaddr_in c_in;
    ServerBasic(const int& new_local_port=0) : local_port(new_local_port) {}
    virtual ~ServerBasic() {
        close(sock_server);
        close(sock_client);
    }
public:
    char recv_buffer[SERVER_BUFFER_LEN];
};
/* set the rules for server's multitask */
class ServerMultiTask
{
protected:
    explicit ServerMultiTask() {}
    virtual ~ServerMultiTask() {}
    bool task_exit_flag=false;
    std::mutex server_mutex;
    /* thread use depatch mod */
    virtual void multi_run()=0;
    virtual void stop()=0;
    virtual std::string get_server_data()=0;
};
/* provide complete esp server functions */
class EspTcpServer : ServerBasic, ServerMultiTask
{
private:
    const unsigned short data_total_num;
    std::string server_data_buffer;
    std::queue<std::string> server_data_queue;
    void multi_run() override;
    bool create_tcp_server(const short& server_num=1); 
public:
    explicit EspTcpServer(const unsigned short& data_total_num_, const unsigned short& new_local_port=0) : ServerBasic(new_local_port), data_total_num(data_total_num_) {}
    virtual ~EspTcpServer() {}
    const int SERVER_RECV_WAIT_TIME_US = 500000; 
    void init(const short& server_num=1);
    unsigned short get_server_data_queue_size() {return server_data_queue.size();}
    void stop() override;
    std::string get_server_data() override;
};
#endif