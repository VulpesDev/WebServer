#include <ServerConfig.hpp>

int main() {
    std::string nginxConfig = R"(
        server {
            listen 80;
            server_name "example.com";
            location / {
                proxy_pass "http://backend_server/";
            }
        }
    )";

    std::istringstream configStream(nginxConfig);
    std::string line;
    while (std::getline(configStream, line)) {
        std::vector<Token> tokens = tokenizeLine(line);

        for (const Token& t : tokens) {
            std::string typeName;
            switch (t.type) {
                case WORD: typeName = "WORD"; break;
                case NUMBER: typeName = "NUMBER"; break;
                case SYMBOL: typeName = "SYMBOL"; break;
                case STRING: typeName = "STRING"; break;
            }
            std::cout << "Type: " << typeName << ", Value: " << t.value << std::endl;
        }
    }

    return 0;
}
