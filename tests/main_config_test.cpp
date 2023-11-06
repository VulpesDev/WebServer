#include <ServerConfig.hpp>

int main(void)
{
    conf_t t;
    std::cerr << "return value: " << ngx_conf_parse(&t, "webserv.default.conf") << std::endl;
}