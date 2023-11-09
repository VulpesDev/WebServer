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