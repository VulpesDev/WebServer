# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tvasilev <tvasilev@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/12/08 16:50:22 by tvasilev          #+#    #+#              #
#    Updated: 2023/10/01 17:08:01 by tvasilev         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = c++

CFLAGS = -Wall -Werror -Wextra -std=c++98

NAME = webserv

NAME_C = client

SRC = main.cpp Server.cpp

SRC_CLIENT = main.client.cpp

OBJ_DIR = objs

OBJ = $(addprefix $(OBJ_DIR)/,$(SRC:.cpp=.o))

OBJ_CLIENT = $(addprefix $(OBJ_DIR)/,$(SRC_CLIENT:.cpp=.o))

all: $(NAME)

$(OBJ_DIR)/%.o : %.cpp
	$(CC) $(CFLAGS) -g3 -c $< -o $@

$(NAME): $(OBJ)
	$(CC) $(OBJ) -o $(NAME)

$(NAME_C): $(OBJ_CLIENT)
	$(CC) $(OBJ_CLIENT) -o $(NAME_C)

clean:
	-rm -f $(OBJ_DIR)/$(OBJ)
	-rm -f $(OBJ_DIR)/$(OBJ_CLIENT)

fclean: clean
	-rm -f $(NAME)
	-rm -f $(NAME_C)

re: fclean all

$(shell mkdir -p $(OBJ_DIR))
