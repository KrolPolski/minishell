/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_tools.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 19:29:52 by akovalev          #+#    #+#             */
/*   Updated: 2024/04/19 19:48:19 by akovalev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*a function that puts null terminators at the 
end of each token in the parsed line*/
void	nullterminate_exec(t_cmd *cmd)
{
	int			i;
	t_execcmd	*ecmd;

	ecmd = (t_execcmd *)cmd;
	i = 0;
	while (ecmd->argv[i])
	{
		*ecmd->eargv[i] = 0;
		i++;
	}
}

/*a function that calls itself or nullterminate_exec
depending on the type of node encountered*/
t_cmd	*nullterminate(t_cmd *cmd)
{
	t_pipecmd	*pcmd;
	t_redircmd	*rcmd;

	if (cmd == 0)
		return (0);
	if (cmd->type == EXEC)
		nullterminate_exec(cmd);
	else if (cmd->type == REDIR)
	{
		rcmd = (t_redircmd *)cmd;
		nullterminate(rcmd->cmd);
		if (rcmd->efile)
			*rcmd->efile = 0;
	}
	else if (cmd->type == PIPE)
	{
		pcmd = (t_pipecmd *)cmd;
		nullterminate(pcmd->left);
		nullterminate(pcmd->right);
	}
	return (cmd);
}

/*a function that frees the previously built syntax tree*/
void	free_tree(t_cmd *cmd)
{
	t_execcmd	*ecmd;
	t_pipecmd	*pcmd;
	t_redircmd	*rcmd;

	if (!cmd)
		return ;
	else if (cmd->type == 1)
	{
		ecmd = (t_execcmd *)cmd;
		free_and_null(cmd);
	}
	else if (cmd->type == 2)
	{
		rcmd = (t_redircmd *)cmd;
		free_tree(rcmd->cmd);
		free_and_null(cmd);
	}
	else if (cmd->type == 3)
	{
		pcmd = (t_pipecmd *)cmd;
		free_tree(pcmd->left);
		free_tree(pcmd->right);
		free_and_null(cmd);
	}
}

/*a function that makes sure that there are no multiple sequential
operators in the parsed string or lone pipes at the end of it*/
bool	check_pipe_syntax(char *ps, char *es, t_line_info *li)
{
	int	i;

	i = 0;
	if (peek(&ps, es, "|><"))
	{
		ft_putstr_fd("AR-Shell: syntax error: multiple operators\n", 2);
		return (FALSE);
	}
	while (ps[i])
	{
		if (!ft_strchr(li->whitespace, ps[i]))
			return (TRUE);
		i++;
	}
	ft_putstr_fd("AR-Shell: syntax error near unexpected token `|'\n", 2);
	return (FALSE);
}

/*a function that checks what token is upcoming next in the parsed
line and whether it is a specific character*/
int	peek(char **ps, char *es, char *tokens)
{
	char	*s;
	char	*whitespace;

	whitespace = ft_strdup(" \t\r\n\v");
	s = *ps;
	while (s < es && ft_strchr(whitespace, *s))
		s++;
	*ps = s;
	free_and_null(whitespace);
	return (*s && ft_strchr(tokens, *s));
}
