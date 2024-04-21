/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   constructors.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 19:36:48 by akovalev          #+#    #+#             */
/*   Updated: 2024/04/21 14:59:11 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*a function that constructs an exec node*/
t_cmd	*execcmd(void)
{
	t_execcmd	*cmd;

	cmd = ft_calloc(sizeof(*cmd), 1);
	check_malloc_failure(cmd);
	cmd->type = EXEC;
	return ((t_cmd *)cmd);
}

/*a function that constructs a redirect node*/
t_cmd	*redircmd(t_cmd *subcmd, t_redir_node_info *rni, int mode, int fd)
{
	t_redircmd	*cmd;

	cmd = ft_calloc(sizeof(*cmd), 1);
	check_malloc_failure(cmd);
	cmd->type = REDIR;
	cmd->cmd = subcmd;
	if (rni->heredoc_flag == 0)
	{
		cmd->file = rni->q;
		cmd->efile = rni->eq;
	}
	else
	{
		cmd->file = NULL;
		rni->heredoc_flag = 0;
	}
	cmd->mode = mode;
	cmd->fd = fd;
	return ((t_cmd *)cmd);
}

/*a function that constructs a pipe node*/
t_cmd	*pipecmd(t_cmd *left, t_cmd *right)
{
	t_pipecmd	*cmd;

	if (!right)
		return (NULL);
	cmd = ft_calloc(sizeof(*cmd), 1);
	check_malloc_failure(cmd);
	cmd->type = PIPE;
	cmd->left = left;
	cmd->right = right;
	return ((t_cmd *)cmd);
}

/*a function that builds a heredoc node. Nulls are passed as
a workaround to trigger the heredoc case when the node is executed later*/
t_cmd	*build_heredoc_node(t_cmd *cmd, t_redir_node_info *rni, t_line_info *li)
{
	*rni->eq = 0;
	rni->heredoc_flag = 1;
	cmd = redircmd(cmd, rni, O_RDONLY, 0);
	if (li->heredoc_buff)
	{
		free_and_null(li->heredoc_buff);
	}
	li->heredoc_buff = heredoc_builder(rni->q);
	return (cmd);
}
