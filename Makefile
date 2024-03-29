# --- Directories ---
SRCDIR := src/
SRCDIR_HANDLEDATA := HandleData/
SRCDIR_HTTP := HTTP/
SRCDIR_PARSING := Parsing/
OBJDIR := build/
HDRDIR := include/

# --- Sources ---
SRC := Server_main.cpp CGI.cpp
SRC_HANDLEDATA := HandleData.cpp Handle_Delete.cpp Handle_Post.cpp Handle_Get.cpp Handle_Request_Checks.cpp Handle_Receive_Data.cpp
SRC_HTTP := HttpResponse.cpp HttpRequest.cpp
SRC_PARSING :=  ServerConfig.cpp Server_class.cpp Location_class.cpp
SRCS := $(addprefix $(SRCDIR), $(SRC)) $(addprefix $(SRCDIR), $(addprefix $(SRCDIR_HANDLEDATA), $(SRC_HANDLEDATA))) \
        $(addprefix $(SRCDIR), $(addprefix $(SRCDIR_HTTP), $(SRC_HTTP))) \
        $(addprefix $(SRCDIR), $(addprefix $(SRCDIR_PARSING), $(SRC_PARSING)))

# --- Objects ---
OBJS := $(patsubst $(SRCDIR)%.cpp, $(OBJDIR)%.o, $(SRCS))

# --- Headers ---
HDR := HttpMessage.hpp Location_class.hpp Server_class.hpp ServerConfig.hpp CGI.hpp HandleData.hpp
HDRS := $(addprefix $(HDRDIR), $(HDR))

# --- Compilers ---
CXX := c++

# --- Flags ---
CXXFLAGS := -g3 #-Wall -Wextra -Wpedantic -Werror -O3 -std=c++98
debug: CXXFLAGS += -g3 -Og

# --- Utils ---
RM := rm -fr
MKDIR := mkdir -pm 775

# --- Targets ---
NAME := webserv

# --- Rules ---
all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJDIR)%.o: $(SRCDIR)%.cpp $(HDRS) | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -I$(HDRDIR) -c $< -o $@

$(OBJDIR)HandleData/%.o: $(SRCDIR)HandleData/%.cpp $(HDRS) | $(OBJDIR)HandleData/
	$(CXX) $(CXXFLAGS) -I$(HDRDIR) -c $< -o $@

$(OBJDIR)HandleData/:
	@$(MKDIR) $@

$(OBJDIR)HTTP/%.o: $(SRCDIR)HTTP/%.cpp $(HDRS) | $(OBJDIR)HTTP/
	$(CXX) $(CXXFLAGS) -I$(HDRDIR) -c $< -o $@

$(OBJDIR)HTTP/:
	@$(MKDIR) $@

$(OBJDIR)Parsing/%.o: $(SRCDIR)Parsing/%.cpp $(HDRS) | $(OBJDIR)Parsing/
	$(CXX) $(CXXFLAGS) -I$(HDRDIR) -c $< -o $@

$(OBJDIR)Parsing/:
	@$(MKDIR) $@


$(OBJDIR):
	@$(MKDIR) $@

debug: CXXFLAGS += -ggdb3
debug: all

clean:
	$(RM) $(OBJDIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean debug fclean re
