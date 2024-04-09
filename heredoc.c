/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/20 12:23:49 by rboudwin          #+#    #+#             */
/*   Updated: 2024/04/09 17:51:52 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*heredoc_builder(char *delimiter)
{
	char	*input;
	char	*buffer;
	char	*ptr_parking;

	buffer = ft_strdup("");
	while (true)
	{
		input = readline("> ");
		if (!ft_strncmp(input, delimiter, ft_strlen(delimiter)))
			break ;
		ptr_parking = buffer;
		buffer = ft_strjoin(buffer, input);
		free(input);
		free(ptr_parking);
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