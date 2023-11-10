#include <ServerConfig.hpp>
#include <stack>
#include <unordered_map>

bool ServerConfig::isValidBraces(std::vector<Token> tokens) {
    std::stack<char> stack;
    std::unordered_map<char, char> bracePairs = {
        {')', '('},
        {'}', '{'},
        {']', '['}
    };
    std::unordered_map<char, char> bracePairs2 = {
        {'(', ')'},
        {'{', '}'},
        {'[', ']'}
    };

    for (std::vector<Token>::iterator it = tokens.begin(); it != tokens.end(); it++) {
        if (it->type != SYMBOL)
            continue;
        char ch = it->value.at(0);
        if (ch == '(' || ch == '{' || ch == '[') {
            stack.push(ch);
        } else if (ch == ')' || ch == '}' || ch == ']') {
            if (stack.empty() || stack.top() != bracePairs[ch]) {
                std::cerr << "Unexpected \'" << ch << "\' at line " << it->line << "!" << std::endl;
                return false;
            }
            stack.pop();
        }
    }
    if (!stack.empty())
        std::cerr << "Expected \'" << bracePairs2[stack.top()] << "\' at line " << tokens.back().line << "!" << std::endl;
    return stack.empty();
}

bool ServerConfig::isValidSemicolon(std::vector<Token> tokens) {
    Token lastOfLine = *tokens.begin();
    unsigned int line= lastOfLine.line;

    for (std::vector<Token>::iterator it = tokens.begin(); it != tokens.end(); it++) {
        if (lastOfLine.value == ";" && it->line == lastOfLine.line){
            std::cerr << "Unexpected \';\' at line " << lastOfLine.line << std::endl;
            return false;
        }
        if (line != it->line) {
        std::cerr << "line: " << line << " itline: " << it->line << " itvalue: " << lastOfLine.value << std::endl;
            if (lastOfLine.value != ";" && lastOfLine.value != "{" && lastOfLine.value != "}"){
                std::cerr << "Expected \';\' at end of line " << lastOfLine.line << std::endl;
                return false;
            }
            line = it->line;
        }
        lastOfLine = *it;
    }
    return true;
}

bool ServerConfig::isValidEncapsulation(std::vector<Token> tokens) {
    enum    Context {
        NONE_CON,
        HTTP_CON,
        SERVER_CON,
        LOCATION_CON
    };

    Context con = NONE_CON;

    for (std::vector<Token>::iterator it = tokens.begin(); it != tokens.end(); it++) {
        if (it->type == KEYWORD) {
            if (it->value == "http") {
                if (con == NONE_CON)
                    con = HTTP_CON;
                else {
                    std::cerr << "Invalid http context at line " << it->line << std::endl;
                    return false;
                }
            } else if (it->value == "server") {
                if (con == HTTP_CON)
                    con = SERVER_CON;
                else {
                    std::cerr << "Invalid server context at line " << it->line << std::endl;
                    return false;
                }
            } else if (it->value == "location") {
                if (con == SERVER_CON)
                    con = LOCATION_CON;
                else {
                    std::cerr << "Invalid location context at line " << it->line << std::endl;
                    return false;
                }
            }
        } else if (it->type == SYMBOL && it->value == "}")
            con = static_cast<Context>(static_cast<int>(con)-1);
    }
    return true;
}

