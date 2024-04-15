/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/20 12:23:49 by rboudwin          #+#    #+#             */
/*   Updated: 2024/04/15 14:41:51 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*heredoc_builder(char *delimiter)
{
	char	*input;
	char	*buffer;
	char	*parking_one;
	char	*parking_two;

	buffer = ft_strdup("");
	while (true)
	{
		input = readline("> ");
		if (!ft_strncmp(input, delimiter, ft_strlen(delimiter)))
			break ;
		parking_one = buffer;
		buffer = ft_strjoin(buffer, input);
		parking_two = buffer;
		buffer = ft_strjoin(buffer, "\n");
		free(parking_one);
		free(parking_two);
		free(input);
	}
	free(input);
	return (buffer);
}
