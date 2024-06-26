# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/01/30 13:42:54 by rboudwin          #+#    #+#              #
#    Updated: 2024/04/22 15:35:01 by rboudwin         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	:= minishell 
CFLAGS := -I /Users/$(USER)/.brew/opt/readline/include -Wextra -Wall -Werror -Wunreachable-code
LDFLAGS := -L /Users/$(USER)/.brew/opt/readline/lib
SRCS	:= main.c signals.c parsing.c parsing2.c tools.c built_in.c \
			built_in2.c env_and_quote_handler.c heredoc.c \
			export.c export2.c unset.c parsing_main.c tools1.c tools2.c \
			friends_of_gettoken.c constructors.c execute_tools.c execute.c \
			parsing_tools_command.c parsing_tools.c
OBJS	:= ${SRCS:.c=.o} Libft/libft.a
BONUS_OBJS := ${BONUS_SRCS:.c=.o} Libft/libft.a 
all: $(NAME)

%.o: %.c
	@$(CC) $(CFLAGS) -o $@ -c $< 

$(NAME): $(OBJS) 
	@$(CC) $(OBJS) -o $(NAME) $(LDFLAGS) -lreadline  

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
