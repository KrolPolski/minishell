/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_main.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 15:50:22 by rboudwin          #+#    #+#             */
/*   Updated: 2024/04/22 15:39:11 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	null_command_handler(t_parsing *p, t_line_info *li)
{
	free_and_null(p->str);
	if (p->exp_wants_freedom)
		free_and_null(p->expanded);
	if (li->heredoc_buff)
	{
		free_and_null(li->heredoc_buff);
	}
}

void	parsing_loop(t_parsing *p, t_line_info *li, t_info *info)
{
	t_cmd	*cmd;

	cmd = NULL;
	add_history(p->str);
	li->heredoc_buff = NULL;
	p->expanded = expand_main(p->str, info->curr_env, li);
	if (p->expanded == p->ptr_parking)
		p->exp_wants_freedom = 0;
	else
		p->exp_wants_freedom = 1;
	cmd = parsecommand(p->expanded, li);
	if (!cmd)
	{
		null_command_handler(p, li);
		return ;
	}
	if (cmd->type == EXEC)
		single_command_handler(cmd, info, p, li);
	else
		fork_and_execute(cmd, info, li, p);
	parsing_signal_exit_codes(p, info);
	parsing_cleanup(p, cmd, li);
}

int	parsing(t_info *info)
{
	t_parsing	p;
	t_line_info	li;

	p.str = readline(info->prompt);
	one_time_init(&li, &p, info);
	while (p.str != NULL)
	{
		parsing_loop(&p, &li, info);
		p.str = readline(info->prompt);
		p.ptr_parking = p.str;
	}
	free_and_null(li.whitespace);
	free_and_null(li.symbols);
	return (0);
}
