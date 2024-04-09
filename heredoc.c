/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/20 12:23:49 by rboudwin          #+#    #+#             */
/*   Updated: 2024/04/09 20:26:49 by akovalev         ###   ########.fr       */
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
	/*
	input = readline("> ");
	new_buffer = input;
	//free(input);
	while (ft_strncmp(input, delimiter, ft_strlen(delimiter)))
	{
		
		//if (!ft_strncmp(input, delimiter, ft_strlen(delimiter)))
		//	break ;
		old_buffer = ft_strjoin(new_buffer, "\n");
		//new_buffer = ft_strjoin(old_buffer, input);
		free(input);
		new_buffer = ft_strjoin(old_buffer, "\n");
		free(old_buffer);
		ft_printf("%s", new_buffer);
		//free(new_buffer);
		input = readline("> ");
		//input = readline("> ");
	//	free(old_buffer);
	}
	ft_printf("Inside heredoc builder check\n");
	system("leaks -q minishell");
	
	
	return (old_buffer);*/
}