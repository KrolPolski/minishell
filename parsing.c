/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/05 14:36:57 by akovalev          #+#    #+#             */
/*   Updated: 2024/04/19 16:05:42 by akovalev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdio.h>
#include <stdlib.h>

char	**parse_paths(char **env)
{
	int		i;
	char	**paths;

	paths = NULL;
	i = 0;
	while (env[i])
	{
		if (ft_strnstr(env[i], "PATH=", 5))
			paths = ft_split(env[i] + 5, ':');
		i++;
	}
	return (paths);
}

char	*check_absolute_path(char *com)
{
	if (access(com, X_OK) != -1)
		return (ft_strdup(com));
	else
	{
		ft_putstr_fd("AR-Shell: ", 2);
		ft_putstr_fd(com, 2);
		ft_putstr_fd(": ", 2);
		perror("");
		return (NULL);
	}
}

char	*check_command(char *com, char **env)
{
	char	*com_slash;
	char	*command;
	int		i;
	char	**paths;

	paths = parse_paths(env);
	i = 0;
	if (ft_strchr(com, '/'))
		return (check_absolute_path(com));
	while (paths && paths[i])
	{
		com_slash = ft_strjoin(paths[i], "/");
		command = ft_strjoin(com_slash, com);
		free_and_null(com_slash);
		if (access(command, X_OK) != -1)
			return (command);
		i++;
		free_and_null(command);
	}
	ft_putstr_fd("AR-Shell: ", 2);
	ft_putstr_fd(com, 2);
	ft_putstr_fd(": command not found\n", 2);
	return (NULL);
}

int	fork1(void)
{
	int	pid;

	pid = fork();
	if (pid == -1)
		panic("fork");
	return (pid);
}

void	execute(t_cmd *cmd, char **env, t_info *info, t_line_info *li)
{
	int			p[2];
	t_execcmd	*ecmd;
	t_pipecmd	*pcmd;
	t_redircmd	*rcmd;
	int			status;
	char		*command;
	char		*builtins[8];
	int			i;

	i = 0;
	builtins[0] = "cd";
	builtins[1] = "pwd";
	builtins[2] = "echo";
	builtins[3] = "export";
	builtins[4] = "unset";
	builtins[5] = "exit";
	builtins[6] = "env";
	builtins[7] = NULL;
	if (cmd == NULL)
		exit (1);
	if (!cmd->type)
		panic("execute");
	if (cmd->type == PIPE)
	{
		pcmd = (t_pipecmd *)cmd;
		if (pipe(p) < 0)
			panic("pipe");
		li->pid = fork1();
		if (li->pid == 0)
		{
			close(1);
			dup(p[1]);
			close(p[0]);
			close(p[1]);
			execute(pcmd->left, env, info, li);
		}
		li->pid = fork1();
		if (li->pid == 0)
		{
			close(0);
			dup(p[0]);
			close(p[0]);
			close(p[1]);
			execute(pcmd->right, env, info, li);
		}
		close(p[0]);
		close(p[1]);
		wait(&status);
		if (WIFEXITED(status))
			info->exit_code = WEXITSTATUS(status);
		wait(&status);
		if (WIFEXITED(status))
			info->exit_code = WEXITSTATUS(status);
	}
	else if (cmd->type == EXEC)
	{
		ecmd = (t_execcmd *)cmd;
		if (ecmd->argv[0] == NULL)
			exit(1);
		while (builtins[i])
		{
			if (ft_strlen(builtins[i]) == ft_strlen(ecmd->argv[0]) && !ft_strncmp(ecmd->argv[0], builtins[i], ft_strlen(builtins[i])))
			{
				handle_builtins(ecmd, builtins[i], info);
				exit(1);
			}
			i++;
		}
		command = check_command(ecmd->argv[0], env);
		if (command)
			execve(command, ecmd->argv, env);
		exit(1);
	}
	else if (cmd->type == REDIR)
	{
		rcmd = (t_redircmd *)cmd;
		close (rcmd->fd);
		if (rcmd->file == NULL)
		{
			if (pipe(p) < 0)
			{
				printf("Pipe creation failed\n");
				exit(1);
			}
			if (write(p[1], li->heredoc_buff, ft_strlen(li->heredoc_buff)) < 0)
			{
				printf("Writing to the pipe failed\n");
				exit(1);
			}
			close(p[1]);
			rcmd->fd = p[0];
		}
		else if (open(rcmd->file, rcmd->mode, 0666) < 0)
		{
			ft_putstr_fd("AR-Shell: ", 2);
			perror(rcmd->file);
			exit(1);
		}
		execute (rcmd->cmd, env, info, li);
	}
	exit(info->exit_code);
}

/*a function that constructs an exec node*/
t_cmd	*execcmd(void)
{
	t_execcmd	*cmd;

	cmd = ft_calloc(sizeof(*cmd), 1);
	cmd->type = EXEC;
	return ((t_cmd *)cmd);
}

/*a function that constructs a redirect node*/
t_cmd	*redircmd(t_cmd *subcmd, char *file, char *efile, int mode, int fd)
{
	t_redircmd	*cmd;

	cmd = ft_calloc(sizeof(*cmd), 1);
	cmd->type = REDIR;
	cmd->cmd = subcmd;
	cmd->file = file;
	cmd->efile = efile;
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
	cmd->type = PIPE;
	cmd->left = left;
	cmd->right = right;
	return ((t_cmd *)cmd);
}

/*a fucntion handling quote removal and setting of quote flags back to 0*/
void	handle_quote_removal(char **str, t_line_info *li, bool qflag)
{
	if (qflag == 0)
	{
		remove_quotes(li->begsq, li->endsq);
		li->sfl = 0;
		li->begsq = NULL;
		li->endsq = NULL;
		li->in_quotes = 0;
		(*str)--;
		(*str)--;
	}
	else if (qflag == 1)
	{
		remove_quotes(li->begdq, li->enddq);
		li->dfl = 0;
		li->begdq = NULL;
		li->enddq = NULL;
		li->in_quotes = 0;
		(*str)--;
		(*str)--;
	}
}

/*a function that handles the tokens that are not special characters
and makes sure that the quotes are handled and removed appropriately*/
void	handle_regular_chars(char **str, t_line_info *li)
{
	while (*str < li->end_str && (!(ft_strchr(li->whitespace, **str))) \
		&& ((!(ft_strchr(li->symbols, **str)))))
	{
		check_quotes(str, li);
		if (li->sfl == 1 && li->in_quotes)
		{
			while (*str <= li->endsq)
			{
				if (li->sfl == 1 && *str == li->endsq)
					handle_quote_removal(str, li, 0);
				(*str)++;
			}
		}
		else if (li->dfl == 1 && li->in_quotes)
		{
			while (*str <= li->enddq)
			{
				if (li->dfl == 1 && *str == li->enddq)
					handle_quote_removal(str, li, 1);
				(*str)++;
			}
		}
		else
			(*str)++;
	}
}

/*a function that sets the return value to the type of encountered token
and moves the string pointer to the character past the end of the token*/
void	handle_current_char(char **str, int *ret, t_line_info *li)
{
	if (**str == '|' && (!li->in_quotes))
		(*str)++;
	else if (**str == '>' && (!li->in_quotes))
	{
		(*str)++;
		if (**str == '>' && (!li->in_quotes))
		{
			*ret = '+';
			(*str)++;
		}
	}
	else if (**str == '<' && (!li->in_quotes))
	{
		(*str)++;
		if (**str == '<' && (!li->in_quotes))
		{
			*ret = '-';
			(*str)++;
		}
	}
	else if (**str != 0)
	{
		*ret = 'a';
		handle_regular_chars(str, li);
	}
}

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

/*a fucntion that checks what token is upcoming next in the parsed
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

/*a fucntion that sets appropriate flags based on whether
there are single or double quotes to be encountered*/
void	handle_quote_flags(t_line_info *li, bool qflag)
{
	if (qflag == 0)
	{
		li->endsq = ft_strchr(li->begsq + 1, '\'');
		if (li->endsq)
		{
			li->sfl = 1;
			li->dfl = 0;
			li->begdq = NULL;
			li->enddq = NULL;
		}
	}
	else if (qflag == 1)
	{
		li->enddq = ft_strchr(li->begdq + 1, '\"');
		if (li->enddq)
		{
			li->dfl = 1;
			li->sfl = 0;
			li->begsq = NULL;
			li->endsq = NULL;
		}
	}
}

/*a function that looks for quotes and sets flags appropriately
depending on whether the beginning or end of quotes is discovered*/
void	check_quotes(char **ps, t_line_info *li)
{
	char		*str;

	str = *ps;
	if (!li->sfl)
		li->begsq = ft_strchr(str, '\'');
	if (!li->dfl)
		li->begdq = ft_strchr(str, '\"');
	if ((!li->endsq) && (li->begsq) && ((li->begsq < li->begdq \
		|| li->begdq == NULL)) && li->dfl != 1 && li->sfl == 0)
	{
		handle_quote_flags(li, 0);
	}
	if ((!li->enddq) && (li->begdq) && ((li->begdq < li->begsq \
		|| li->begsq == NULL)) && li->sfl != 1 && li->dfl == 0)
	{
		handle_quote_flags(li, 1);
	}
	if (li->endsq && (*ps >= li->begsq && *ps <= li->endsq))
		li->in_quotes = 1;
	if (li->enddq && (*ps >= li->begdq && *ps <= li->enddq))
		li->in_quotes = 1;
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

/*a function that builds a heredoc node. Nulls are passed as
a workaround to trigger the heredoc case when the node is executed later*/
t_cmd	*build_heredoc_node(t_cmd *cmd, char *q, char *eq, t_line_info *li)
{
	*eq = 0;
	cmd = redircmd(cmd, NULL, NULL, O_RDONLY, 0);
	if (li->heredoc_buff)
	{
		free_and_null(li->heredoc_buff);
	}
	li->heredoc_buff = heredoc_builder(q);
	return (cmd);
}

/*a function that looks for redirect tokens and builds
redirect nodes and also handles the heredoc case*/
t_cmd	*parseredirs(t_cmd *cmd, char **ps, char *es, t_line_info *li)
{
	char	*q;
	char	*eq;

	if ((!li->in_quotes))
	{
		while (peek(ps, es, "<>") && !li->in_quotes)
		{
			li->redir_tok = gettoken(ps, 0, 0, li);
			if (gettoken(ps, &q, &eq, li) != 'a')
			{
				ft_putstr_fd("missing file for redirection\n", 2);
				free_and_null(cmd);
				return (NULL);
			}
			if (li->redir_tok == '<')
				cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
			else if (li->redir_tok == '-')
				cmd = build_heredoc_node(cmd, q, eq, li);
			else if (li->redir_tok == '>')
				cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREAT | O_TRUNC, 1);
			else if (li->redir_tok == '+')
				cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREAT | O_APPEND, 1);
		}
	}
	return (cmd);
}

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

/*a function that puts null terminators at the 
end of each token in the parsed line*/
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

void	one_time_init(t_line_info *li, t_parsing *p, t_info *info)
{
	p->ptr_parking = p->str;
	li->info = info;
	li->whitespace = ft_strdup(" \t\r\n\v");
	li->symbols = ft_strdup("<|>");
}
