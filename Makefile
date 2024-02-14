# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tvasilev <tvasilev@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/10/25 01:05:19 by mcutura           #+#    #+#              #
#    Updated: 2024/02/14 22:24:00 by tvasilev         ###   ########.fr        #
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

# --- Targets ---
NAME := webserv

# --- Directories ---
SRCDIR := src/
OBJDIR := build/
HDRDIR := include/

# --- Sources ---
# SRC := ServerConfig.cpp ServerConfig_class.cpp ServerConfig_validation.cpp LocationConfig_class.cpp main_config_test.cpp ServerConfig_tokenize.cpp
SRC := HttpResponse.cpp HttpRequest.cpp Server_main.cpp ServerConfig.cpp Server_class.cpp Location_class.cpp
SRCS := $(addprefix $(SRCDIR), $(SRC))

# --- Objects ---
OBJS := $(SRC:%.cpp=$(OBJDIR)%.o)

# --- Headers ---
HDR := HttpMessage.hpp Location_class.hpp Server_class.hpp ServerConfig.hpp
HDRS := $(addprefix $(HDRDIR), $(HDR))

# --- Compilers ---
CC := cc
CXX := c++

# --- Flags ---
CFLAGS := -Wall -Wextra -Wpedantic -Werror -O3
CXXFLAGS := -g3
INCLUDES := -I$(HDRDIR)
debug: CFLAGS += -ggdb3 -Og
debug: CXXFLAGS += -ggdb3 -Og

# --- Utils --
RM := rm -fr
MKDIR := mkdir -pm 775

# --- Rules ---
all: $(NAME)

$(NAME): $(HDRS) $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

$(OBJDIR)%.o : $(SRCDIR)%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR):
	@$(MKDIR) $(OBJDIR)

debug: all

# tests: $(NAME) TODO

clean:
	$(RM) $(OBJDIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean debug fclean re
