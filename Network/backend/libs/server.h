#ifndef __SERVER_H__

#include <string>

class ClientConn {

    public:
        ClientConn(int fd, std::string& data) : _fd(fd), _data(data) {}

    public:
        const std::string& getData() { return _data; }
        int getFD() { return _fd; }

    private:
        int _fd;
        std::string _data;
};

typedef int (*handle_request_f)(ClientConn conn);

class Server {
    public:
        Server(const std::string& ip, int port);
        Server(const std::string& ip, int port, handle_request_f handler);
        void setHandler(handle_request_f new_handler);

        int start();

    private:
        int _config_socket();
        int _set_nonblock(int fd);

    private:
        handle_request_f _handler;

        int _sock_fd;
        int _port;
        std::string _ip;
};

#endif
