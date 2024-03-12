/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/05 14:36:57 by akovalev          #+#    #+#             */
/*   Updated: 2024/03/07 19:03:11 by akovalev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdio.h>
#include <stdlib.h>

void	panic(char *s)
{
	printf("%s\n", s);
	exit(1);
}

t_cmd	*execcmd(void)
{
	t_execcmd	*cmd;

	cmd = malloc(sizeof(*cmd));
	ft_memset(cmd, 0, sizeof(*cmd));
	cmd->type = EXEC;
	return ((t_cmd *)cmd);
}

t_cmd	*redircmd(t_cmd *subcmd, char *file, char *efile, int mode, int fd)
{
	t_redircmd	*cmd;

	cmd = malloc(sizeof(*cmd));
	ft_memset(cmd, 0, sizeof(*cmd));
	cmd->type = REDIR;
	cmd->cmd = subcmd;
	cmd->file = file;
	cmd->efile = efile;
	cmd->mode = mode;
	cmd->fd = fd;
	return ((t_cmd *)cmd);
}

t_cmd	*pipecmd(t_cmd *left, t_cmd *right)
{
	t_pipecmd	*cmd;

	cmd = malloc(sizeof(*cmd));
	ft_memset(cmd, 0, sizeof(*cmd));
	cmd->type = PIPE;
	cmd->left = left;
	cmd->right = right;
	return ((t_cmd *)cmd);
}

t_cmd	*listcmd(t_cmd *left, t_cmd *right)
{
	t_listcmd	*cmd;

	cmd = malloc(sizeof(*cmd));
	ft_memset(cmd, 0, sizeof(*cmd));
	cmd->type = LIST;
	cmd->left = left;
	cmd->right = right;
	return ((t_cmd *)cmd);
}

t_cmd	*backcmd(t_cmd *subcmd)
{
	t_backcmd	*cmd;

	cmd = malloc(sizeof(*cmd));
	ft_memset(cmd, 0, sizeof(*cmd));
	cmd->type = BACK;
	cmd->cmd = subcmd;
	return ((t_cmd *)cmd);
}


int	gettoken(char **pstr, char *end_str, char **q, char **eq)
{
	char	*str;
	int		ret;
	char	whitespace[] = " \t\r\n\v";
	char	symbols[] = "<|>&;()";

	str = *pstr;
	while (str < end_str && ft_strchr(whitespace, *str))
		str++;
	if (q)
		*q = str;
	ret = *str;
	if (*str == '|' || *str == '(' || *str == ')' || *str == ';' || *str == '&')
		str++;
	else if (*str == '>')
	{
		str++;
		if (*str == '>')
		{
			ret = '+';
			str++;
		}
	}
	else if (*str == '<')
	{
		str++;
		if (*str == '<')
		{
			ret = '-';
			str++;
		}
	}
	else if (*str != 0)
	{
		ret = 'a';
		while (str < end_str && !ft_strchr(whitespace, *str) && !ft_strchr(symbols, *str))
			str++;
	}
	if (eq)
		*eq = str;
	while (str < end_str && ft_strchr(whitespace, *str))
		str++;
	*pstr = str;
	return (ret);
}

int	peek(char **ps, char *es, char *tokens)
{
	char	*s;
	char	whitespace[] = " \t\r\n\v";
	char	symbols[] = "<|>&;()";

	s = *ps;
	while (s < es && ft_strchr(whitespace, *s))
		s++;
	*ps = s;
	return (*s && ft_strchr(tokens, *s));
}
t_cmd *parseline(char**, char*);
t_cmd *parsepipe(char**, char*);
t_cmd *parseexec(char**, char*);
t_cmd *nullterminate(t_cmd *);

t_cmd	*parsecommand(char *str)
{
	char	*end_str;
	t_cmd	*cmd;

	end_str = str + ft_strlen(str);
	cmd = parseline(&str, end_str);
	peek(&str, end_str, "");
	if (str != end_str)
	{
		printf("leftovers: %s\n", str);
		panic("syntax");
	}
	nullterminate(cmd);
	return (cmd);
}

t_cmd	*parseline(char **ps, char *es)
{
	t_cmd	*cmd;

	cmd = parsepipe(ps, es);
	while (peek(ps, es, "&"))
	{
		gettoken(ps, es, 0, 0);
		cmd = backcmd(cmd);
	}
	if (peek(ps, es, ";"))
	{
		gettoken(ps, es, 0, 0);
		cmd = listcmd(cmd, parseline(ps, es));
	}
	return (cmd);
}

t_cmd	*parsepipe(char **ps, char *es)
{
	t_cmd	*cmd;

	cmd = parseexec(ps, es);
	if (peek(ps, es, "|"))
	{
		gettoken(ps, es, 0, 0);
		cmd = pipecmd(cmd, parsepipe(ps, es));
	}
	return (cmd);
}

t_cmd*	parseredirs(t_cmd *cmd, char **ps, char *es)
{
	int		tok;
	char	*q;
	char	*eq;

	while (peek(ps, es, "<>"))
	{
		tok = gettoken(ps, es, 0, 0);
		if (gettoken(ps, es, &q, &eq) != 'a')
			panic("missing file for redirection");
		if (tok == '<')
			cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
		else if (tok == '>')
			cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREAT, 1);
		else if (tok == '+')
			cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREAT, 1);
	}
	return (cmd);
}

t_cmd	*parseblock(char **ps, char *es)
{
	t_cmd	*cmd;

	if (!peek(ps, es, "("))
		panic("parseblock");
	gettoken(ps, es, 0, 0);
	cmd = parseline(ps, es);
	if (!peek(ps, es, ")"))
		panic("syntax - missing )");
	gettoken(ps, es, 0, 0);
	cmd = parseredirs(cmd, ps, es);
	return (cmd);
}

t_cmd	*parseexec(char **ps, char *es)
{
	char		*q;
	char		*eq;
	int			tok;
	int			argc;
	t_execcmd	*cmd;
	t_cmd		*ret;

	if (peek(ps, es, "("))
		return (parseblock(ps, es));
	ret = execcmd();
	cmd = (t_execcmd *)ret;

	argc = 0;
	ret = parseredirs(ret, ps, es);
	while (!peek(ps, es, "|)&;"))
	{
		if ((tok = gettoken(ps, es, &q, &eq)) == 0)
			break ;
		if (tok != 'a')
			panic("syntax");
		cmd->argv[argc] = q;
		cmd->eargv[argc] = eq;
		argc++;
		if (argc >= MAXARGS)
		{
			printf("argc: %d", argc);
			panic("too many args");
		}
		ret = parseredirs(ret, ps, es);
	}
	cmd->argv[argc] = 0;
	cmd->eargv[argc] = 0;
	return (ret);
}

t_cmd	*nullterminate(t_cmd *cmd)
{
	int			i;
	t_backcmd	*bcmd;
	t_execcmd	*ecmd;
	t_listcmd	*lcmd;
	t_pipecmd	*pcmd;
	t_redircmd	*rcmd;

	if (cmd == 0)
		return (0);
	if (cmd->type == EXEC)
	{
		ecmd = (t_execcmd *)cmd;
		i = 0;
		while (ecmd->argv[i])
		{
			*ecmd->eargv[i] = 0;
			i++;
		}
	}
	else if (cmd->type == REDIR)
	{
		rcmd = (t_redircmd *)cmd;
		nullterminate(rcmd->cmd);
		*rcmd->efile = 0;
	}
	else if (cmd->type == PIPE)
	{
		pcmd = (t_pipecmd *)cmd;
		nullterminate(pcmd->left);
		nullterminate(pcmd->right);
	}
	else if (cmd->type == LIST)
	{
		lcmd = (t_listcmd *)cmd;
		nullterminate(lcmd->left);
		nullterminate(lcmd->right);
	}
	else if (cmd->type == BACK)
	{
		bcmd = (t_backcmd *)cmd;
		nullterminate(bcmd->cmd);
	}
	return (cmd);
}

int fork1(void)
{
	int	pid;

	pid = fork();
	if (pid == -1)
		panic("fork");
	return (pid);
}

void runcmd(t_cmd *cmd)
{
	int			p[2];
	t_backcmd	*bcmd;
	t_execcmd	*ecmd;
	t_listcmd	*lcmd;
	t_pipecmd	*pcmd;
	t_redircmd	*rcmd;

	exit(0);
}


int	main	(void)
{
	int			fd;
	char		*str;
	t_cmd		*cmd;
	int			status;
	t_execcmd	*ecmd;
	char		buf[100];

	fd = 0;
	while (str != NULL)
	{
		str = get_next_line(fd);
		//printf("> ");
		cmd = parsecommand(str);
		if (cmd->type == 1)
		{
			ecmd = (t_execcmd *)cmd;
			if (ft_strncmp(ecmd->argv[0], "cd", 3) == 0)
			{
				if (chdir(ecmd->argv[1]) < 0)
					printf("cannot cd %s\n", ecmd->argv[1]);
			}
			else if (ft_strncmp(ecmd->argv[0], "pwd", 4) == 0)
			{
				if (!(getcwd(buf, sizeof(buf))))
					printf("pwd error\n");
				else
					(printf("%s%s", buf, "\n"));
			}
		}
		// if (fork1() == 0)
		// 	runcmd(cmd);
		// wait(&status);
		//printf("type %d\n", cmd->type);
		free(str);
	}
	return (0);
}