/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_tools_command.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 19:32:24 by akovalev          #+#    #+#             */
/*   Updated: 2024/04/19 19:48:20 by akovalev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*a function that populates the path array*/
char	**parse_paths(char **env)
{
	int		i;
	char	**paths;

	paths = NULL;
	i = 0;
	while (env[i])
	{
		if (ft_strnstr(env[i], "PATH=", 5))
			paths = ft_split(env[i] + 5, ':');
		i++;
	}
	return (paths);
}

/*a function that checks the access to an absolute
path command*/
char	*check_absolute_path(char *com)
{
	if (access(com, X_OK) != -1)
		return (ft_strdup(com));
	else
	{
		ft_putstr_fd("AR-Shell: ", 2);
		ft_putstr_fd(com, 2);
		ft_putstr_fd(": ", 2);
		perror("");
		return (NULL);
	}
}

/*a function that creates the paths array and looks for 
the command based on that or an absolute path*/
char	*check_command(char *com, char **env)
{
	char	*com_slash;
	char	*command;
	int		i;
	char	**paths;

	paths = parse_paths(env);
	i = 0;
	if (ft_strchr(com, '/'))
		return (check_absolute_path(com));
	while (paths && paths[i])
	{
		com_slash = ft_strjoin(paths[i], "/");
		command = ft_strjoin(com_slash, com);
		free_and_null(com_slash);
		if (access(command, X_OK) != -1)
			return (command);
		i++;
		free_and_null(command);
	}
	ft_putstr_fd("AR-Shell: ", 2);
	ft_putstr_fd(com, 2);
	ft_putstr_fd(": command not found\n", 2);
	return (NULL);
}
