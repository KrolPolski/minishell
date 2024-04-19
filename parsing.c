/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/05 14:36:57 by akovalev          #+#    #+#             */
/*   Updated: 2024/04/19 19:45:50 by akovalev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdio.h>
#include <stdlib.h>

/*a fucntion that fetches the type of the current token and sets its
beginning and end at q and eq respectively while moving the string pointer*/
int	gettoken(char **pstr, char **q, char **eq, t_line_info *li)
{
	char	*str;
	int		ret;

	check_quotes(pstr, li);
	str = *pstr;
	while (str < li->end_str && ft_strchr(li->whitespace, *str))
		str++;
	if (q)
		*q = str;
	ret = *str;
	handle_current_char(&str, &ret, li);
	if (eq)
		*eq = str;
	while (str < li->end_str && ft_strchr(li->whitespace, *str))
		str++;
	*pstr = str;
	return (ret);
}

/*a function that begins the parsing and after the line is parsed puts
null terminators after each token in it. Not sure if the leftovers check is 
needed anymore*/
t_cmd	*parsecommand(char *str, t_line_info *li)
{
	char	*end_str;
	char	*beg_str;
	t_cmd	*cmd;

	beg_str = str;
	end_str = str + ft_strlen(str);
	cmd = parseline(&str, end_str, li);
	if (!cmd)
	{
		return (NULL);
	}
	peek(&str, end_str, "");
	if (str != end_str)
	{
		printf("leftovers: %s\n", str);
		panic("syntax");
	}
	nullterminate(cmd);
	return (cmd);
}

/*a function that parses the line for executables and pipes, 
creating pipe nodes when appropriate*/
t_cmd	*parseline(char **ps, char *es, t_line_info *li)
{
	t_cmd		*cmd;
	int			tok;
	t_cmd		*ptr_parking;

	init_line_info(li, ps);
	cmd = parseexec(ps, es, li);
	if (!cmd)
		return (NULL);
	if (peek(ps, es, "|"))
	{
		tok = gettoken(ps, 0, 0, li);
		if (!check_pipe_syntax(*ps, es, li))
		{
			free_tree(cmd);
			return (NULL);
		}
		ptr_parking = cmd;
		cmd = pipecmd(cmd, parseline(ps, es, li));
		if (!cmd)
		{
			free_tree(ptr_parking);
			return (NULL);
		}
	}
	return (cmd);
}

/*a function that parses the line for executables and 
creates nodes accordingly. While looking for executables it also
needs to consider encountering potential redirections*/

t_cmd	*parseexec(char **ps, char *es, t_line_info *li)
{
	char		*q;
	char		*eq;
	int			argc;
	t_execcmd	*cmd;
	t_cmd		*ret;

	ret = execcmd();
	cmd = (t_execcmd *)ret;
	argc = 0;
	ret = parseredirs(ret, ps, es, li);
	if (!ret)
		return (NULL);
	while ((((**ps != '|') && (!li->in_quotes)) || (li->in_quotes)) && **ps)
	{
		gettoken(ps, &q, &eq, li);
		cmd->argv[argc] = q;
		cmd->eargv[argc] = eq;
		argc++;
		ret = parseredirs(ret, ps, es, li);
		if (!ret)
			return (NULL);
	}
	cmd->argv[argc] = 0;
	cmd->eargv[argc] = 0;
	return (ret);
}

/*a function that looks for redirect tokens and builds
redirect nodes and also handles the heredoc case*/
t_cmd	*parseredirs(t_cmd *cmd, char **ps, char *es, t_line_info *li)
{
	t_redir_node_info	rni;

	rni.heredoc_flag = 0;
	if ((!li->in_quotes))
	{
		while (peek(ps, es, "<>") && !li->in_quotes)
		{
			li->redir_tok = gettoken(ps, 0, 0, li);
			if (gettoken(ps, &rni.q, &rni.eq, li) != 'a')
			{
				ft_putstr_fd("missing file for redirection\n", 2);
				free_and_null(cmd);
				return (NULL);
			}
			if (li->redir_tok == '<')
				cmd = redircmd(cmd, &rni, O_RDONLY, 0);
			else if (li->redir_tok == '-')
				cmd = build_heredoc_node(cmd, &rni, li);
			else if (li->redir_tok == '>')
				cmd = redircmd(cmd, &rni, O_WRONLY | O_CREAT | O_TRUNC, 1);
			else if (li->redir_tok == '+')
				cmd = redircmd(cmd, &rni, O_WRONLY | O_CREAT | O_APPEND, 1);
		}
	}
	return (cmd);
}
