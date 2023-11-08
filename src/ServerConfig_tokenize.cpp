#include <ServerConfig.hpp>

bool isNumber(const std::string& str) {
    for (size_t i = 0; i < str.length(); ++i) {
        if (!std::isdigit(str[i]) && str[i] != '-' && str[i] != '+') {
            return false;
        }
    }
    return true;
}

int strChrArr(std::string token, const std::string symbols)
{
    int pos = 0;
    bool found = false;

    for (size_t i = 0; i < symbols.size(); i++) {
        pos = token.find(symbols[i]);
        if (pos != std::string::npos) {
            found = true;
            break;
        }
    }
    return found ? pos : -1;
}

void    tokenizer_core(std::string token, std::vector<Token>& tokens, unsigned int linen)
{
    Token t;
    int      pos = 0;
    const std::string symbols = "{};=,#";
    const std::string keywords = "http server location root";

    if (token[0] == '"' || token[0] == '\'') {
        t.type = STRING;
        t.value = token.substr(1, token.length() - 2);
    } else if (isNumber(token)) {
        t.type = NUMBER;
        t.value = token;
    } else if ((pos = strChrArr(token, symbols)) >= 0) {
        if (token.length() > 1 && pos == 0)
            pos = 1;
        if (pos == 0) {
            t.type = SYMBOL;
            t.value = token;
        } else {
            tokenizer_core(token.substr(0, pos), tokens, linen);
            tokenizer_core(token.substr(pos), tokens, linen);
            return;
        }
    } else if (keywords.find(token) != std::string::npos){
        t.type = KEYWORD;
        t.value = token;
    } else {
        t.type = WORD;
        t.value = token;
    }
    t.line = linen;
    tokens.push_back(t);
}

void tokenizeLine(const std::string& line, std::vector<Token>& tokens, unsigned int linen) {
    std::stringstream stream(line);
    std::string token;
    

    while (stream >> token) {
        if (tokens.size() > 0 && tokens.back().type == SYMBOL && tokens.back().value == "#" && linen == tokens.back().line){
            continue;
        }
        tokenizer_core(token, tokens, linen);
    }
}

std::vector<Token> ServerConfig::tokenize(std::ifstream& file)
{
    std::string fileContent;
    fileContent.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    std::istringstream configStream(fileContent);
    std::vector<Token> tokens;
    std::string line;
    unsigned int linen = 1;
    while (std::getline(configStream, line)) {
        tokenizeLine(line, tokens, linen++);
    }
    for (const Token& token : tokens) {
        std::cout << "Type: " << token.type << " Value: " << token.value << " Line: " << token.line << std::endl;
    }
    return tokens;
}