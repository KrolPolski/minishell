/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/20 12:23:49 by rboudwin          #+#    #+#             */
/*   Updated: 2024/03/20 15:38:38 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	heredoc_handler(char *terminator)
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
	while (ft_strncmp(input, terminator, ft_strlen(terminator)))
	{
		input = readline("> ");
		if (!ft_strncmp(input, terminator, ft_strlen(terminator)))
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
	// The problem is, we are also appending the EOF marker, which we should not.
}