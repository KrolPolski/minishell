/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 18:01:40 by rboudwin          #+#    #+#             */
/*   Updated: 2024/04/15 15:14:57 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*ft_prompt(char *username, char *hostname, char *path)
{
	char	*prompt;
	char	*ptr_parking;

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
			info->username = ft_strdup(info->curr_env[i] + 5);
		else if (ft_strnstr(info->curr_env[i], "PWD=", 4))
			info->init_dir = ft_strdup(info->curr_env[i] + 4);
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
	if (argv[1] && !ft_strncmp(argv[1], "test", 5))
		test(&info);
	else
		parsing(&info);
	final_cleanup(&info);
}
