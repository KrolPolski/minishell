/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/05 14:36:57 by akovalev          #+#    #+#             */
/*   Updated: 2024/04/17 17:26:34 by rboudwin         ###   ########.fr       */
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

char	*check_command(char *com, char **env)
{
	char	*com_slash;
	char	*command;
	int		i;
	char	**paths;

	paths = parse_paths(env);
	i = 0;
	//ft_printf("We got inside check_command\n");
	//ft_printf("com is '%s'\n", com);
	if (ft_strchr(com, '/'))
	{
		//ft_printf("slash detected\n");
		if (access(com, X_OK) != -1)
			return (ft_strdup(com));
		else
		{
			//ft_printf("Nope");
			ft_putstr_fd("AR-Shell: ", 2);
			ft_putstr_fd(com, 2);
			ft_putstr_fd(": ", 2);
			perror("");
			return (NULL);
		}
	}
	while (paths[i])
	{
		com_slash = ft_strjoin(paths[i], "/");
		command = ft_strjoin(com_slash, com);
		free(com_slash);
		com_slash = NULL;
		if (access(command, X_OK) != -1)
			return (command);
		i++;
		free(command);
	}
	ft_putstr_fd("AR-Shell: ", 2);
	ft_putstr_fd(com, 2);
	ft_putstr_fd(": command not found\n", 2);
	return (NULL);
}

int fork1(void)
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
	//int fd;

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
		{
			info->exit_code = WEXITSTATUS(status);
			//ft_printf("Exit code of pipe fork is %d\n", info->exit_code);
		}
		wait(&status);
		if (WIFEXITED(status))
		{
			info->exit_code = WEXITSTATUS(status);
			//ft_printf("Exit code of pipe fork2 is %d\n", info->exit_code);
		}
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
	//	ft_printf("return value of check command was '%s'\n", command);
		//printf("execve failed\n");
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
			printf("open %s failed\n", rcmd->file);
			exit(1);
		}
		execute (rcmd->cmd, env, info, li);
	}
	//free(cmd);
	exit(info->exit_code);
	//exit (0); //remember to manually free memory on all exits
}

t_cmd	*execcmd(void)
{
	t_execcmd	*cmd;

	cmd = ft_calloc(sizeof(*cmd), 1);
	cmd->type = EXEC;
	return ((t_cmd *)cmd);
}

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

t_cmd	*pipecmd(t_cmd *left, t_cmd *right)
{
	t_pipecmd	*cmd;

	if (!right)
		return NULL;
	cmd = ft_calloc(sizeof(*cmd), 1);
	cmd->type = PIPE;
	cmd->left = left;
	cmd->right = right;
	return ((t_cmd *)cmd);
}

t_cmd	*listcmd(t_cmd *left, t_cmd *right)
{
	t_listcmd	*cmd;

	cmd = ft_calloc(sizeof(*cmd), 1);
	cmd->type = LIST;
	cmd->left = left;
	cmd->right = right;
	return ((t_cmd *)cmd);
}

t_cmd	*backcmd(t_cmd *subcmd)
{
	t_backcmd	*cmd;

	cmd = ft_calloc(sizeof(*cmd), 1);
	cmd->type = BACK;
	cmd->cmd = subcmd;
	return ((t_cmd *)cmd);
}

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

int	peek(char **ps, char *es, char *tokens)
{
	char	*s;
	char	*whitespace;
	//ft_printf("entered peek\n");
	whitespace = ft_strdup(" \t\r\n\v");
	s = *ps;
	while (s < es && ft_strchr(whitespace, *s))
		s++;
	*ps = s;
	free(whitespace);
	//ft_printf("exiting peek\n");
	return (*s && ft_strchr(tokens, *s));
}
//t_cmd *parsepipe(char**, char*);

void	print_exec(t_execcmd *ecmd);

t_cmd	*parsecommand(char *str, t_line_info *li)
{
	char	*end_str;
	char	*beg_str;
	t_cmd	*cmd;

	ft_printf("we have entered parsecommand\n");
	beg_str = str;
	end_str = str + ft_strlen(str);
	cmd = parseline(&str, end_str, li);
	ft_printf("cmd pointer is currently %p\n", cmd);
	if (!cmd)
	{
		ft_putstr_fd("parsecommand concludes we have an error somewhere(received a null pointer)\n", 2);
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

bool	check_pipe_syntax(char *ps, t_line_info *li)
{
	int i;
 
	//ft_printf("ps is '%s'\n and es is '%s'\n", *ps, es);
	//ft_printf("li->beg_str is '%s'\n", li->beg_str);
	i = 0;
	while (ps[i])
	{
		if (!ft_strchr(li->whitespace, ps[i]))
			return (TRUE);
		i++;
	}
	ft_putstr_fd("AR-Shell: syntax error near unexpected token `|'\n", 2);
	return (FALSE);
}

t_cmd	*parseline(char **ps, char *es, t_line_info *li)
{
	t_cmd		*cmd;
	int			tok;
	bool		pipe_syntax;
	
	init_line_info(li, ps);
	cmd = parseexec(ps, es, li);

	ft_printf("welcome to parseline, we hope you enjoy your stay with us\n");
	if (!cmd)
		return (NULL);
	if (peek(ps, es, "|"))
	{
		tok = gettoken(ps, 0, 0, li);
		pipe_syntax = check_pipe_syntax(*ps, li);
		if (!pipe_syntax)
		{
			ft_printf("We are giving up on life, the universe, everything\n");
			free(cmd);
			cmd = NULL;
			ft_printf("returning null from parseline\n, in theory. but cmd is actually:\n%p\n", cmd);
			return (NULL);
		}
		ft_putstr_fd("Out of pipe_syntax\n", 1);
		if (peek(ps, es, "|><"))
		{
			ft_putstr_fd("AR-Shell: syntax error: multiple redirect operators\n", 2);
			free(cmd);
			return (NULL);
		}
		ft_printf("here comes a recursive parseline call\n");
		cmd = pipecmd(cmd, parseline(ps, es, li));
		if (!cmd)
			return (NULL);
	}
	return (cmd);
}

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
	int			tok;
	int			argc;
	t_execcmd	*cmd;
	t_cmd		*ret;

	ft_printf("All hail the mighty ParseExec\n");
	ret = execcmd();
	cmd = (t_execcmd *)ret;
	argc = 0;
	ret = parseredirs(ret, ps, es, li);
	if (!ret)
		return (NULL);
	while ((((**ps != '|') && (!li->in_quotes)) || (li->in_quotes)) && **ps)
	{
		if ((tok = gettoken(ps, &q, &eq, li)) == 0)
			break ;
		if (tok != 'a')
			panic("syntax");
		cmd->argv[argc] = q;
		cmd->eargv[argc] = eq;
		argc++;
		ret = parseredirs(ret, ps, es, li);
		if (!ret)
			return NULL;
	}
	cmd->argv[argc] = 0;
	cmd->eargv[argc] = 0;
	return (ret);
}

t_cmd	*build_heredoc_node(t_cmd *cmd, char *q, char *eq, t_line_info *li)
{
	*eq = 0;
	cmd = redircmd(cmd, NULL, NULL, O_RDONLY, 0);
	if (li->heredoc_buff)
	{
		free(li->heredoc_buff);
		li->heredoc_buff = NULL;
	}
	li->heredoc_buff = heredoc_builder(q);
	return (cmd);
}

/*a function that looks for redirect tokens and builds
redirect nodes and also handles the heredoc case*/
t_cmd	*parseredirs(t_cmd *cmd, char **ps, char *es, t_line_info *li)
{
	int		tok;
	char	*q;
	char	*eq;

	ft_printf("Behold the majesty of parseredirs\n");
	if ((!li->in_quotes))
	{
		while (peek(ps, es, "<>") && !li->in_quotes)
		{
			tok = gettoken(ps, 0, 0, li);
			if (gettoken(ps, &q, &eq, li) != 'a')
			{
				ft_putstr_fd("missing file for redirection\n", 2);
				free(cmd);
				return (NULL);
			}
			if (tok == '<')
				cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
			else if (tok == '-')
				cmd = build_heredoc_node(cmd, q, eq, li);
			else if (tok == '>')
				cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREAT | O_TRUNC, 1);
			else if (tok == '+')
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

	if (cmd->type == 1)
	{
		ecmd = (t_execcmd *)cmd;
		free(cmd);
	}
	if (cmd->type == 2)
	{
		rcmd = (t_redircmd *)cmd;
		free_tree(rcmd->cmd);
		free(cmd);
	}
	if (cmd->type == 3)
	{
		pcmd = (t_pipecmd *)cmd;	
		free_tree(pcmd->left);
		free_tree(pcmd->right);
		free(cmd);
	}
}
void	one_time_init(t_line_info *li, t_parsing *p, t_info *info)
{
	p->ptr_parking = p->str;
	li->info = info;
	li->whitespace = ft_strdup(" \t\r\n\v");
	li->symbols = ft_strdup("<|>");
}
