#include <ServerConfig.hpp>

////////////////////////////////////////////////////////////////////////////////
// --- CTORs / DTORs ---


ServerConfig::ServerConfig(std::string const &config_path)
{
	std::ifstream	file(config_path);

	if (!file.is_open()) {
		throw ConfigException("Cannot open config file: " + config_path);
		this->is_valid_ = false;
		return ;
	}
	this->is_valid_ = this->parse(file);
	if (!this->is_valid_) {
		file.close();
		throw ConfigException("Invalid configuration file: " + config_path);
		return ;
	}
	file.close();
	
}

ServerConfig::ServerConfig(ServerConfig const &other)
{}

ServerConfig const &ServerConfig::operator=(ServerConfig const &rhs)
{
	return *this;
}

ServerConfig::~ServerConfig()
{}

////////////////////////////////////////////////////////////////////////////////
// --- GETTERS ---

bool ServerConfig::is_valid() const
{
	return this->is_valid_;
}

////////////////////////////////////////////////////////////////////////////////
// --- INTERNAL ---

//////////////////////////////////////////////
//// --- TOKENIZE ---

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
    return tokens;
}

//////////////////////////////////////////////
//// --- VALIDATION ---

/// @brief Validate bracket placing
/// @param tokens The tokens to validate
/// @return True if valid and false if not
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

/// @brief Validate semicolons at the end of line
/// @param tokens The tokens to validate
/// @return True if valid and false if not
bool ServerConfig::isValidSemicolon(std::vector<Token> tokens) {
    Token lastOfLine = *tokens.begin();
    unsigned int line= lastOfLine.line;

    for (std::vector<Token>::iterator it = tokens.begin(); it != tokens.end(); it++) {
        if (lastOfLine.value == ";" && it->line == lastOfLine.line){
            std::cerr << "Unexpected \';\' at line " << lastOfLine.line << std::endl;
            return false;
        }
        if (line != it->line) {
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

/// @brief Validate encapsulation
/// @param tokens The tokens to validate
/// @return True if valid and false otherwise
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

//////////////////////////////////////////////
//// --- PARSING MAIN PART ---

//is parsing configuration locations and saving the values in Location class object
//with key value pair for the information.
Location	parseLocations(std::vector<Token>::iterator& it, std::vector<Token> tokens) {
	Location l;
	it+=1;
	l.setPath(it->value);
	it += 2;
	while ((it != tokens.end() && (it->type != SYMBOL && it->value != "}")) && (it->type == WORD || it->type == NUMBER)) {
		std::string					val_key;
		std::vector<std::string>	val_values;
		
		val_key = it->value;
        // std::cout << "\n this is val_key in ServerConfig.cpp " << val_key << std::endl;
		it++;
		while (it != tokens.end() && (it->type != SYMBOL && it->value != ";")) {
			val_values.push_back(it->value);
            // std::cout << "\n this is it_val in ServerConfig.cpp " << it->value << std::endl;
			it++;
		}
		if (!val_key.empty())
			l.other_vals.insert(std::pair<std::string, std::vector<std::string>>(val_key, val_values));
		it++;
	} l.mapToValues();
    return l;
}

//is parsing configuration servers and saving the values in Server class object, before location
Server				parseServer(std::vector<Token>::iterator& it, std::vector<Token> tokens) {
	Server s;
	it+=2;
	while (it != tokens.end() && (it->type != SYMBOL && it->value != "}")) {
		if (it->type == KEYWORD && it->value == "location") {
            Location l = parseLocations(it, tokens);
			s.locations.push_back(l);
		} else if (it->type == WORD) {
			std::string					val_key;
			std::vector<std::string>	val_values;
			
			val_key = it->value;
            // std::cout << "\n this is val_key in ServerConfig.cpp " << val_key << std::endl;
			it++;
			while (it != tokens.end() && (it->type != SYMBOL && it->value != ";")) {
				val_values.push_back(it->value);
                // std::cout << "\n this is it_val in ServerConfig.cpp " << it->value << std::endl;
				it++;
			}
			if (!val_key.empty()) {
				s.other_vals.insert(std::pair<std::string, std::vector<std::string>>(val_key, val_values));
			}
		} it++;
	}   s.mapToValues();
     return s;
}


Http				parseHttp(std::vector<Token>::iterator& it, std::vector<Token> tokens) {
	Http h;
	it+=2;
	while (it != tokens.end() && (it->type != SYMBOL && it->value != "}")) {
		if (it->type == KEYWORD && it->value == "server") {
            Server s = parseServer(it, tokens);
			h.servers.push_back(s);

		} else if (it->type == WORD) {
			std::string					val_key;
			std::vector<std::string>	val_values;
			
			val_key = it->value;
			it++;
			while (it != tokens.end() && (it->type != SYMBOL && it->value != ";")) {
				val_values.push_back(it->value);
				it++;
			}
			if (!val_key.empty())
			h.other_vals.insert(std::pair<std::string, std::vector<std::string>>(val_key, val_values));
		} it++;
	} return h;
}

bool				ServerConfig::parse(std::ifstream& file) {
    tokens = tokenize(file);
    if(!isValidBraces(tokens) || !isValidSemicolon(tokens) || !isValidEncapsulation(tokens))
        return false;
	for (std::vector<Token>::iterator it = tokens.begin(); it != tokens.end(); it++) {
		if (it->type == KEYWORD && it->value == "http") {
			https.push_back(parseHttp(it, tokens));
		}
	}

    return true;
}

std::vector<Http> ServerConfig::GetHttps( void ) const {
    return this->https;
}
Server 				ServerConfig::GetFirstServer( void ) {
    https[0].servers[0].mapToValues();
    return https[0].servers[0];
}
