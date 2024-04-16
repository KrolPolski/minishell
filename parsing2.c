/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 14:24:52 by rboudwin          #+#    #+#             */
/*   Updated: 2024/04/16 15:30:56 by rboudwin         ###   ########.fr       */
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
		li->pid = fork1();
		if (li->pid == 0)
		{
			signal(SIGQUIT, SIG_DFL);
			execute(cmd, info->curr_env, info, li);
		}
		p->tree_prisoner = 0;
	}
}

void	fork_and_execute(t_cmd *cmd, t_info *info, t_line_info *li, t_parsing *p)
{
	li->pid = fork1();
	if (li->pid == 0)
	{
		signal(SIGQUIT, SIG_DFL);
		execute(cmd, info->curr_env, info, li);	
	}
	p->tree_prisoner = 1;
}

int	parsing(t_info *info)
{
	t_cmd		*cmd;
	t_parsing	p;
	t_line_info	li;

	p.str = readline(info->prompt);
	p.ptr_parking = p.str;
	li.info = info;
	one_time_init(&li);
	while (p.str != NULL)
	{
		add_history(p.str);
		li.heredoc_buff = NULL;
		p.expanded = expand_env_remove_quotes(p.str, info->curr_env, &li);
		if (p.expanded == p.ptr_parking)
			p.exp_wants_freedom = 0;
		else
			p.exp_wants_freedom = 1;
		cmd = parsecommand(p.expanded, &li);
		if (cmd->type == EXEC)
			single_command_handler(cmd, info, &p, &li);
		else
			fork_and_execute(cmd, info, &li, &p);
		signal(SIGINT, SIG_IGN);
		wait(&p.status);
		set_signal_action();
		if (WIFEXITED(p.status))
			info->exit_code = WEXITSTATUS(p.status);
		if (p.tree_prisoner)
			free_tree(cmd);
		else
			free(cmd);
		free(p.str);
		if (li.heredoc_buff)
			free(li.heredoc_buff);
		if (p.exp_wants_freedom)
			free(p.expanded);
		system("leaks -q minishell");
		p.str = readline(info->prompt);
		p.ptr_parking = p.str;
	}
	free_and_null(li.whitespace);
	free_and_null(li.symbols);
	ft_printf("We bid you all a very fond farewell.\n");
	return (0);
}
