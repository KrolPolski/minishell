/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/20 12:23:49 by rboudwin          #+#    #+#             */
/*   Updated: 2024/04/21 15:08:03 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* uses readline to build a buffer to use for input for another 
command to emulate the heredoc functionality in bash */
char	*heredoc_builder(char *delimiter)
{
	char	*input;
	char	*buffer;
	char	*parking_one;
	char	*parking_two;

	buffer = ft_strdup("");
	check_malloc_failure(buffer);
	while (true)
	{
		input = readline("> ");
		if (!ft_strncmp(input, delimiter, ft_strlen(delimiter)))
			break ;
		parking_one = buffer;
		buffer = ft_strjoin(buffer, input);
		check_malloc_failure(buffer);
		parking_two = buffer;
		buffer = ft_strjoin(buffer, "\n");
		check_malloc_failure(buffer);
		free_and_null(parking_one);
		free_and_null(parking_two);
		free_and_null(input);
	}
	free_and_null(input);
	return (buffer);
}
