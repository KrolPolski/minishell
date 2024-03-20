/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/20 12:23:49 by rboudwin          #+#    #+#             */
/*   Updated: 2024/03/20 15:49:16 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*heredoc_builder(char *delimiter)
{
	char *input;
	char *old_buffer;
	char *ptr_parking;
	char *new_buffer;
	
	ft_printf("You want to get heredoc? Because that's how you get heredocs\n");
	input = readline("> ");
	old_buffer = ft_strjoin(input, "\n");
	free(input);
	//ft_printf("Our first strjoin finished");
	while (ft_strncmp(input, delimiter, ft_strlen(delimiter)))
	{
		input = readline("> ");
		if (!ft_strncmp(input, delimiter, ft_strlen(delimiter)))
			break ;
		new_buffer = ft_strjoin(old_buffer, input);
		free(old_buffer);
		old_buffer = ft_strjoin(new_buffer, "\n");
		free(new_buffer);
		free(input);
		input = readline("> ");
	//	free(old_buffer);
	}
	
	ft_printf("%s", old_buffer);
	return (old_buffer);
}