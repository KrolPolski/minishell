/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 18:01:40 by rboudwin          #+#    #+#             */
/*   Updated: 2024/03/12 12:25:10 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

char	*ft_prompt(char *username, char *hostname, char *path)
{
//	ft_printf("[%s@%s %s]$", username, hostname, path);
	char *prompt;
	char *ptr_parking;

	prompt = ft_strjoin("[", username);
	ptr_parking = prompt;
	prompt = ft_strjoin(prompt, "@");
	free(ptr_parking);
	ptr_parking = prompt;
	prompt = ft_strjoin(prompt, hostname);
	free(ptr_parking);
	ptr_parking = prompt;
	prompt = ft_strjoin(prompt, " ");
	free(ptr_parking);
	ptr_parking = prompt; 
	prompt = ft_strjoin(prompt, path);
	free(ptr_parking);
	ptr_parking = prompt;
	prompt = ft_strjoin(prompt, "]$");
	free(ptr_parking);
	return (prompt);
}
int	main(int argc, char **argv, char **env)
{
	char		*username;
	char		*init_dir;
	int			i;
	char		*input;
	HIST_ENTRY	*hist_entry;
	char		*prompt;


	set_signal_action();

	i = 0;
	while (env[i] != NULL)
	{
		if (ft_strnstr(env[i], "USER=", 5))
			username = ft_strdup(env[i] + 5);
		else if (ft_strnstr(env[i], "PWD=", 4))
			init_dir = ft_strdup(env[i] + 4);
		i++;
	}
	read_history(".shell_history");
	
	prompt = ft_prompt(username, "AR-Shell", init_dir);
	readline(prompt);

	//while (i)
	//	sleep(1);


	/*input = readline("\nEnter something: ");

	if (input != NULL)
	{
		printf("\nYou entered: %s\n", input);
		add_history(input);
		free(input);
	}*/
	write_history(".shell_history");
	i = 0;
	while (i < history_length + 1)
	{
		hist_entry = history_get(i);
		if (hist_entry != NULL)
			printf("Line %d: %s\n", i, hist_entry->line);
		i++;
	}

	//readline();
	//cleanup

	free(username);
	free(init_dir);
	free(prompt);
}