#include "esp_tcp_server.h"

bool EspTcpServer::create_tcp_server(const short& server_num)
{
    sock_server = socket(AF_INET, SOCK_STREAM, 0); //
    if (sock_server == -1) {
        printf("\ncreate socket fail...\n");
        return false;
    }
    s_in.sin_family = AF_INET;
    s_in.sin_port = htons(local_port);
    s_in.sin_addr.s_addr = INADDR_ANY;
    if (bind(sock_server, (struct sockaddr*)&s_in, sizeof(s_in)) < 0) { // listen all ipv4 address
        printf("\n error occur when using bind...\n");
        return false;
    }
    if (listen(sock_server, server_num) < 0) {
        printf("\nerror occur when using listen\n");
        return false;
    }
    printf("\nfinish create tcp server, local port is:%d\n, local ip is:%s", local_port, inet_ntoa(s_in.sin_addr));
    return true;
}

void EspTcpServer::multi_run()
{
    // std::string test_str = "from server, fuck you";
    printf("\nserver running...\n");
    sock_client = accept(sock_server, (struct sockaddr*)&c_in, &addr_len); // is a block function
    while (!task_exit_flag) {
        if (sock_client < 0) {
            printf("\nerror occur when using accept");
            sock_client = accept(sock_server, (struct sockaddr*)&c_in, &addr_len);
            continue;
        }
        memset(recv_buffer, '\0', SERVER_BUFFER_LEN);
        if (recv(sock_client, recv_buffer, SERVER_BUFFER_LEN, 0) < 0) {
            printf("\nrecieve data error...reuse accpet...\n");
            sock_client = accept(sock_server, (struct sockaddr*)&c_in, &addr_len);
            continue;
        }
        printf("accept from ip:%s, data seg:%s\n", inet_ntoa(c_in.sin_addr), recv_buffer);//log
        std::lock_guard<std::mutex> lock(server_mutex); /* data generator mutex */
        if (data_total_num >= server_data_queue.size()) {
            server_data_buffer.append(recv_buffer, std::strlen(recv_buffer)); 
            server_data_queue.push(server_data_buffer);
            server_data_buffer.clear(); /* clear cache */
        }
        lock.~lock_guard();
        // send(sock_client, test_str.c_str(), test_str.length(), 0);
        usleep(SERVER_RECV_WAIT_TIME_US);
    }
    printf("\nserver exit...\n");
}

std::string EspTcpServer::get_server_data()
{
    std::lock_guard<std::mutex> lock(server_mutex);
    if (server_data_queue.size() > 0) {
        std::string top = server_data_queue.front();
        server_data_queue.pop();
        lock.~lock_guard();
        return top;
    } else {
        lock.~lock_guard();
        return "";
    }
}

void EspTcpServer::init(const short& server_num)
{
    if (create_tcp_server(server_num) == false) {
        printf("\nfail to create server...\n");
    } else {
        std::thread t_server(&EspTcpServer::multi_run, this); /* run class function as a thread */
        t_server.detach();
    }
}

void EspTcpServer::stop()
{
    std::lock_guard<std::mutex> lock(server_mutex);
    task_exit_flag = true;
}
/* This entry only use for test*/
#if DEBUG_ESP_TCP_SERVER
int main()
{
    std::mutex main_mutex;
    EspTcpServer test_server(10, 28266);
    test_server.init();
    while (1) {
        std::lock_guard<std::mutex> lock(main_mutex);
        if (test_server.get_server_data_queue_size() >9) {
            // printf("data queue length is:%d\n", test_server.get_server_data_queue_size());//log
            test_server.stop();
            break;
        }
        lock.~lock_guard();
        usleep(5000);
    }
}
#endif