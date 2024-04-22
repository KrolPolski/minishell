/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 14:24:52 by rboudwin          #+#    #+#             */
/*   Updated: 2024/04/22 15:18:48 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	single_command_handler(t_cmd *cmd, t_info *info,
	t_parsing *p, t_line_info *li)
{
	t_execcmd	*ecmd;

	ecmd = (t_execcmd *)cmd;
	if (ecmd->argv[0] && ft_strncmp(ecmd->argv[0], "cd", 3) == 0)
		ft_cd(ecmd, info);
	else if (ecmd->argv[0] && ft_strncmp(ecmd->argv[0], "export", 7) == 0)
		ft_export(ecmd, info);
	else if (ecmd->argv[0] && ft_strncmp(ecmd->argv[0], "exit", 5) == 0)
		ft_exit(ecmd, info);
	else if (ecmd->argv[0] && ft_strncmp(ecmd->argv[0], "unset", 6) == 0)
		ft_unset(ecmd, info);
	else if (ecmd->argv[0] && ft_strncmp(ecmd->argv[0], "pwd", 4) == 0)
		ft_pwd();
	else
	{
		li->pid = fork_with_protection();
		if (li->pid == 0)
		{
			signal(SIGQUIT, SIG_DFL);
			execute(cmd, info->curr_env, info, li);
		}
		p->tree_prisoner = 0;
	}
}

void	fork_and_execute(t_cmd *cmd, t_info *info,
			t_line_info *li, t_parsing *p)
{
	li->pid = fork_with_protection();
	if (li->pid == 0)
	{
		signal(SIGQUIT, SIG_DFL);
		execute(cmd, info->curr_env, info, li);
	}
	p->tree_prisoner = 1;
}

void	parsing_cleanup(t_parsing *p, t_cmd *cmd, t_line_info *li)
{
	if (p->tree_prisoner && cmd)
		free_tree(cmd);
	else
		free_and_null(cmd);
	free_and_null(p->str);
	if (li->heredoc_buff)
		free_and_null(li->heredoc_buff);
	if (p->exp_wants_freedom)
		free_and_null(p->expanded);
}

void	parsing_signal_exit_codes(t_parsing *p, t_info *info)
{
	signal(SIGINT, SIG_IGN);
	wait(&p->status);
	set_signal_action();
	if (WIFEXITED(p->status))
		info->exit_code = WEXITSTATUS(p->status);
}
