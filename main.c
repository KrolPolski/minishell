/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 18:01:40 by rboudwin          #+#    #+#             */
/*   Updated: 2024/04/21 16:03:05 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* builds a prompt that includes a username and current path */
char	*ft_prompt(char *username, char *hostname, char *path)
{
	char	*prompt;
	char	*ptr_parking;

	prompt = ft_strjoin("[", username);
	check_malloc_failure(prompt);
	ptr_parking = prompt;
	prompt = ft_strjoin(prompt, "@");
	check_and_free(prompt, ptr_parking);
	ptr_parking = prompt;
	prompt = ft_strjoin(prompt, hostname);
	check_and_free(prompt, ptr_parking);
	ptr_parking = prompt;
	prompt = ft_strjoin(prompt, " ");
	check_and_free(prompt, ptr_parking);
	ptr_parking = prompt;
	prompt = ft_strjoin(prompt, path);
	check_and_free(prompt, ptr_parking);
	ptr_parking = prompt;
	prompt = ft_strjoin(prompt, "]$ ");
	check_and_free(prompt, ptr_parking);
	return (prompt);
}

/*suppresses echo of control characters, using a bitwise
compound assignment to invert the ECHOCTL flag with a 
bitwise NOT */
void	set_termios_settings(void)
{
	struct termios		term;

	tcgetattr(0, &term);
	term.c_lflag &= ~ECHOCTL;
	tcsetattr(0, TCSADRAIN, &term);
}

void	populate_env_matrix(t_info *info)
{
	int	i;

	i = 0;
	while (info->init_env[i])
		i++;
	info->curr_env = ft_calloc(sizeof(char *) * (i + 1), 1);
	i = 0;
	while (info->init_env[i])
	{
		info->curr_env[i] = ft_strdup(info->init_env[i]);
		check_malloc_failure(info->curr_env[i]);
		i++;
	}
	info->curr_env[i] = NULL;
}

void	set_username_dir(t_info *info)
{
	int	i;

	i = 0;
	while (info->curr_env[i] != NULL)
	{
		if (ft_strnstr(info->curr_env[i], "USER=", 5))
		{
			info->username = ft_strdup(info->curr_env[i] + 5);
			check_malloc_failure(info->username);
		}
		else if (ft_strnstr(info->curr_env[i], "PWD=", 4))
		{
			info->init_dir = ft_strdup(info->curr_env[i] + 4);
			check_malloc_failure (info->init_dir);
		}
		i++;
	}
}

int	main(int argc, char **argv, char **env)
{
	t_info		info;

	signal(SIGQUIT, SIG_IGN);
	set_termios_settings();
	set_signal_action();
	info.argc = argc;
	info.argv = argv;
	info.init_env = env;
	populate_env_matrix(&info);
	set_shell_level(&info);
	set_username_dir(&info);
	read_history(".shell_history");
	info.prompt = ft_prompt(info.username, "AR-Shell", info.init_dir);
	info.curr_dir = info.init_dir;
	parsing(&info);
	final_cleanup(&info);
	ft_printf("We bid you all a very fond farewell.\n");
	exit(0);
}
