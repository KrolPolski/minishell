# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/01/30 13:42:54 by rboudwin          #+#    #+#              #
#    Updated: 2024/03/19 18:13:03 by akovalev         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	:= minishell 
#CFLAGS	:= -Wextra -Wall -Werror -Wunreachable-code -Ofast 
CFLAGS := -I /Users/$(USER)/.brew/opt/readline/include
LDFLAGS := -L /Users/$(USER)/.brew/opt/readline/lib
SRCS	:= main.c signals.c parsing.c env_handling.c tools.c built_in.c \
			built_in2.c quote_handler.c
OBJS	:= ${SRCS:.c=.o} Libft/libft.a
BONUS_OBJS := ${BONUS_SRCS:.c=.o} Libft/libft.a 
all: $(NAME)

%.o: %.c
	@$(CC) $(CFLAGS) -o $@ -c $< 

$(NAME): $(OBJS) 
	@$(CC) $(OBJS) -o $(NAME) $(LDFLAGS) -lreadline -ltermcap -lncurses
#-ltermcap -lncurses might be needed for termcap stuff like save/restore cursor

Libft/libft.a: 
	$(MAKE) -C Libft/ all
	$(MAKE) -C Libft/ bonus

clean:
	@rm -rf $(OBJS)
	rm -f Libft/.bonus;
	$(MAKE) -C Libft/ clean

fclean: clean
	@rm -rf $(NAME)

re: clean fclean all

.PHONY: all, clean, fclean, re 
