# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mcutura <mcutura@student.42berlin.de>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/10/25 01:05:19 by mcutura           #+#    #+#              #
#    Updated: 2023/10/25 01:05:19 by mcutura          ###   ########.fr        #
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
HDRDIR := includes/

# --- Sources ---
SRC := main.cpp Server.cpp
SRCS := $(addprefix $(SRCDIR), $(SRC))

# --- Objects ---
OBJS := $(SRC:%.cpp=$(OBJDIR)%.o)

# --- Headers ---
HDR := Server.hpp
HDRS := $(addprefix $(HDRDIR), $(HDR))

# --- Compilers ---
CC := cc
CXX := c++

# --- Flags ---
CFLAGS := -Wall -Wextra -Werror -pedantic-errors -O3
CXXFLAGS := -Wall -Wextra -Werror -std=c++98 -pedantic-errors -O3
INCLUDES := -I$(HDRDIR)
debug: CFLAGS += -ggdb3 -Og
debug: CXXFLAGS += -ggdb3 -Og

# --- Utils --
RM := rm -fr
MKDIR := mkdir -pm 775

# --- Rules ---
all: $(NAME)

$(NAME):$(HDRS) $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

$(OBJDIR)%.o : $(SRCDIR)%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR):
	$(MKDIR) $(OBJDIR)

debug: all

clean:
	$(RM) $(OBJDIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean debug fclean re
