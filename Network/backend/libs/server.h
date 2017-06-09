#ifndef __SERVER_H__

#include <string>

typedef int (*handle_request_f)(const std::string& request);

class Server {
    public:
        Server(const std::string& ip, int port);
        Server(const std::string& ip, int port, handle_request_f handler);
        void setHandler(handle_request_f new_handler);

        int start();

    private:
        int config_socket();

    private:
        handle_request_f _handler;

        int _sock_fd;
        int _port;
        std::string _ip;
};

#endif
