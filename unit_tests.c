/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unit_tests.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 14:53:37 by rboudwin          #+#    #+#             */
/*   Updated: 2024/04/09 13:48:38 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	test(t_info *info)
{
	int fd;
	char *str;
	char *bash_str;

	ft_printf("Wouldn't it be lovely to have some tests\n");
	fd = open("test_input", O_RDONLY);
	if (fd > 0)
	{
		str = get_next_line(fd);
		bash_str = ft_strjoin("bash -c ", str);
		ft_printf("test string:\n%s", str);
		ft_printf("Minishell output:\n");
		alt_parsing(info, str);
		ft_printf("bash output:\n");
		
		system(bash_str);
		//free(str);
		free(bash_str);
	}
}
int	alt_parsing(t_info *info, char *str)
{
	int			fd;
	t_cmd		*cmd;
	int			status;
	t_execcmd	*ecmd;
	char		*expanded;
	int			tree_prisoner;
	char		exp_wants_freedom;
	char		*ptr_parking;

	ptr_parking = str;
	expanded = expand_env_remove_quotes(str, info->curr_env);
	if (expanded == ptr_parking)
		exp_wants_freedom = 0;
	else
		exp_wants_freedom = 1;
	cmd = parsecommand(expanded);
	if (cmd->type == 1)
	{
		ecmd = (t_execcmd *)cmd;
		if (ecmd->argv[0] && ft_strncmp(ecmd->argv[0], "cd", 3) == 0)
			ft_cd(ecmd, info);
		else if (ecmd->argv[0] && ft_strncmp(ecmd->argv[0], "export", 7) == 0) // should it not have the last condition removed? otherwise it does not run in parent without args
			ft_export(ecmd, info);
		else if (ecmd->argv[0] && ft_strncmp(ecmd->argv[0], "exit", 5) == 0)
			ft_exit(ecmd, info);
		else if (ecmd->argv[0] && ft_strncmp(ecmd->argv[0], "unset", 6) == 0)
			ft_unset(ecmd, info);
		else if (ecmd->argv[0] && ft_strncmp(ecmd->argv[0], "pwd", 4) == 0)
			ft_pwd(ecmd, info);
		else if (fork1() == 0)
		{
			signal(SIGQUIT, SIG_DFL);
			execute(cmd, info->curr_env, info);
		}
		tree_prisoner = 0;
		}
		else
		{
		if (fork1() == 0)
		{
			signal(SIGQUIT, SIG_DFL);
			execute(cmd, info->curr_env, info);	
		}
		tree_prisoner = 1;
	}
	signal(SIGINT, SIG_IGN);
	wait(&status);
	set_signal_action();
	if (WIFEXITED(status))
	{
		info->exit_code = WEXITSTATUS(status);
	}
	if (tree_prisoner)
	{
		free_tree(cmd);
	}
	else
		free(cmd);
	free(ptr_parking);
	if (exp_wants_freedom)
		free(expanded);
	return (0);
}