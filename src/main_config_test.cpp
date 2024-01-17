#include <ServerConfig.hpp>

int main() {
    std::string nginxConfig = "./data/webserv.default.conf";
    ServerConfig sc(nginxConfig);

    return 0;
}
