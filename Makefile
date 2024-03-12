# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tvasilev <tvasilev@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/10/25 01:05:19 by mcutura           #+#    #+#              #
#    Updated: 2024/03/12 23:12:33 by tvasilev         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# --- Usage ---
# make			compile all sources into binaries and link them into executable
# make all		^ same
# make webserv	^ same
# make clean	remove compiled object files
# make fclean	remove all compiled binaries
# make re		remove all compiled binaries and recompile the project again
# make debug	compile with debug flags and reduced compiler optimizations

# --- Directories ---
SRCDIR := src/
OBJDIR := build/
HDRDIR := include/

# --- Sources ---
SRC := HttpResponse.cpp HttpRequest.cpp Server_main.cpp ServerConfig.cpp Server_class.cpp Location_class.cpp CGI.cpp
SRC_HANDLEDATA := HandleData.cpp Handle_Delete.cpp Handle_Post.cpp Handle_Get.cpp
SRCS := $(addprefix $(SRCDIR), $(SRC)) $(addprefix $(SRCDIR)HandleData/, $(SRC_HANDLEDATA))

# --- Objects ---
OBJS := $(addprefix $(OBJDIR), $(SRC:.cpp=.o)) $(addprefix $(OBJDIR)HandleData/, $(SRC_HANDLEDATA:.cpp=.o))

# --- Headers ---
HDR := HttpMessage.hpp Location_class.hpp Server_class.hpp ServerConfig.hpp CGI.hpp HandleData.hpp
HDRS := $(addprefix $(HDRDIR), $(HDR))

# --- Compilers ---
CXX := c++

# --- Flags ---
CXXFLAGS := #-Wall -Wextra -Wpedantic -Werror -O3 -std=c++98
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

$(OBJDIR) $(OBJDIR)HandleData/:
	@$(MKDIR) $@

debug: CXXFLAGS += -ggdb3
debug: all

clean:
	$(RM) $(OBJDIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean debug fclean re
