/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 18:01:40 by rboudwin          #+#    #+#             */
/*   Updated: 2024/03/11 13:15:05 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_prompt(char *username, char *hostname, char *path)
{
	ft_printf("[%s@%s %s]$", username, hostname, path);
	//readline("");
}
int	main(int argc, char **argv, char **env)
{
	char	*username;
	char	*init_dir;
	int		i;

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
	ft_prompt(username, "AR-Shell", init_dir);
	while (i)
		sleep(1);
	//readline();
	//cleanup
	free(username);
	free(init_dir);
}