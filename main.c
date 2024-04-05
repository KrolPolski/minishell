/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 18:01:40 by rboudwin          #+#    #+#             */
/*   Updated: 2024/04/05 12:11:24 by rboudwin         ###   ########.fr       */
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
	char	*str;
	int		line_count;
	int		i;

	i = 0;
	while (info->init_env[i])
		i++;
	info->curr_env = malloc(sizeof(char *) * (i + 1));
	i = 0;
	while (info->init_env[i])
	{
		info->curr_env[i] = ft_strdup(info->init_env[i]);
		i++;
	}
	info->curr_env[i] = NULL;
}

void	set_shell_level(t_info *info)
{
	int		i;
	int		lvl;
	char	*lvlstr;

	i = 0;
	while (info->curr_env[i])
	{
		if (ft_strnstr(info->curr_env[i], "SHLVL=", 6))
		{
			lvl = ft_atoi(info->curr_env[i] + 6);
			lvl++;
			free(info->curr_env[i]);
			lvlstr = ft_itoa(lvl);
			info->curr_env[i] = ft_strjoin("SHLVL=", lvlstr);
			free(lvlstr);
			return ;
		}
		i++;
	}
}

void	final_cleanup(t_info *info)
{
	write_history(".shell_history");
	free(info->username);
	free(info->init_dir);
	free(info->prompt);
	free_2d(info->curr_env);
}

int	main(int argc, char **argv, char **env)
{
	t_info		info;
	int			i;
	HIST_ENTRY	*hist_entry;

	signal(SIGQUIT, SIG_IGN);
	set_termios_settings();
	set_signal_action();
	info.argc = argc;
	info.argv = argv;
	info.init_env = env;
	populate_env_matrix(&info);
	set_shell_level(&info);
	i = 0;
	while (info.curr_env[i] != NULL)
	{
		if (ft_strnstr(info.curr_env[i], "USER=", 5))
			info.username = ft_strdup(info.curr_env[i] + 5);
		else if (ft_strnstr(info.curr_env[i], "PWD=", 4))
			info.init_dir = ft_strdup(info.curr_env[i] + 4);
		i++;
	}
	read_history(".shell_history");
	info.prompt = ft_prompt(info.username, "AR-Shell", info.init_dir);
	info.curr_dir = info.init_dir;
	parsing(&info);
	final_cleanup(&info);
}
