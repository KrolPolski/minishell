/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 18:01:40 by rboudwin          #+#    #+#             */
/*   Updated: 2024/03/13 12:47:55 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


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
	prompt = ft_strjoin(prompt, "]$ ");
	free(ptr_parking);
	return (prompt);
}

int	main(int argc, char **argv, char **env)
{
	t_info	info;	
	int		i;
	HIST_ENTRY	*hist_entry;

	set_signal_action();
	info.argc = argc;
	info.argv = argv;
	info.env = env;
	i = 0;
	while (env[i] != NULL)
	{
		if (ft_strnstr(env[i], "USER=", 5))
			info.username = ft_strdup(env[i] + 5);
		else if (ft_strnstr(env[i], "PWD=", 4))
			info.init_dir = ft_strdup(env[i] + 4);
		i++;
	}
	read_history(".shell_history");
	
	info.prompt = ft_prompt(info.username, "AR-Shell", info.init_dir);
	info.curr_dir = info.init_dir;
	//info.input = readline(info.prompt);

	//while (i)
	//	sleep(1);


	//input = readline("\nEnter something: ");

	/*if (info.input != NULL)
	{
		printf("\nYou entered: %s\n", info.input);
		add_history(info.input);
		free(info.input);
	}
	write_history(".shell_history");
	i = 0;
	while (i < history_length + 1)
	{
		hist_entry = history_get(i);
		if (hist_entry != NULL)
			printf("Line %d: %s\n", i, hist_entry->line);
		i++;
	}*/
	parsing(&info);
	//readline();
	//cleanup

	free(info.username);
	free(info.init_dir);
	free(info.prompt);
}