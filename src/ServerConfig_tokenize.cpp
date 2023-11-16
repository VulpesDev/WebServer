#include <ServerConfig.hpp>

/// @brief Compare a string to the macro KEYWORDS
/// @param s The string to compare
/// @return Either 0 or 1, depending if the string is found in the macro KEYWORDS
bool isKeyword(std::string s) {
    const std::string keywords = KEYWORDS;
    return keywords.find(s) == std::string::npos ? 0 : 1;
}

/// @brief Check if a char is a symbol
/// @param ch The char to check
/// @return Either 0 or 1
bool isSymbol(char ch) {
    const std::string symbols = SYMBOLS;
    return symbols.find(ch) == std::string::npos ? 0 : 1;
}

/// @brief Operations to apply in order to tokenize a single line
/// @param input The line
/// @param tokens The vector to modify
/// @param linen The number of the current line
void    tokenizeLine(std::string input, std::vector<Token>& tokens, unsigned int linen)
{
    Token       currentToken;
    std::string currentTokenVal;
    bool insideQuotes = false;
    char quoteType = '\0';

    currentToken.line = linen;
    currentToken.type = WORD;
    for (std::string::iterator it = input.begin(); it != input.end(); ++it) {
        char ch = *it;
        if (!insideQuotes && ch == '#')
            break;
        if (ch == '\'' || ch == '"') {
            if (!insideQuotes) {
                insideQuotes = true;
                currentToken.type = STRING;
                quoteType = ch;
            } else if (quoteType == ch) {
                insideQuotes = false;
                quoteType = '\0';
            }
        }

        if (!insideQuotes && (std::isspace(ch) || isSymbol(ch))) {
            if (!currentTokenVal.empty()) {
                currentToken.value = currentTokenVal;
                currentToken.type = isKeyword(currentTokenVal) ? KEYWORD : WORD;
                tokens.push_back(currentToken);
                currentTokenVal.clear();
            }

            if (isSymbol(ch)) {
                currentToken.type = SYMBOL;
                currentToken.value = std::string(1, ch);
                tokens.push_back(currentToken);
            }
        } else {
            currentTokenVal += ch;
        }
    }

    if (!currentTokenVal.empty()) {
        currentToken.value = currentTokenVal;
        tokens.push_back(currentToken);
    }
}

/// @brief Converts a string into tokens line by line split by symbols and spaces then labled
/// @param file the file to read the string from
/// @return std::vector<Token> tokens, if something goes wrong it returns empty vector
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