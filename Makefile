# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tvasilev <tvasilev@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/12/08 16:50:22 by tvasilev          #+#    #+#              #
#    Updated: 2023/10/01 15:10:11 by tvasilev         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = c++

CFLAGS = -Wall -Werror -Wextra -std=c++98

NAME = webserv

SRC = main.cpp Server.cpp

OBJ_DIR = objs

OBJ = $(addprefix $(OBJ_DIR)/,$(SRC:.cpp=.o))

all: $(NAME)

$(OBJ_DIR)/%.o : %.cpp
	$(CC) $(CFLAGS) -g3 -c $< -o $@

$(NAME): $(OBJ)
	$(CC) $(OBJ) -o $(NAME)

clean:
	-rm -f $(OBJ_DIR)/$(OBJ)

fclean: clean
	-rm -f $(NAME)

re: fclean all

$(shell mkdir -p $(OBJ_DIR))
