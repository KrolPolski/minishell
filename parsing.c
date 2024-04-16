/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/05 14:36:57 by akovalev          #+#    #+#             */
/*   Updated: 2024/04/16 15:43:28 by rboudwin         ###   ########.fr       */
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
	{
		(*str)++;
		if (**str == '|')
			panic("Multiple sequential pipes");
	}
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

	//ft_printf("We have entered parsecommand\n");
	//system("leaks -q minishell");
	beg_str = str;
	end_str = str + ft_strlen(str);
	cmd = parseline(&str, end_str, li);
//	write(1, "After parseline", 15);
	//ft_printf("After parseline\n");
	//system("leaks -q minishell");
	peek(&str, end_str, "");
	if (str != end_str)
	{
		printf("leftovers: %s\n", str);
		panic("syntax");
	}
	nullterminate(cmd);
	//print_exec(cmd);
	//ft_printf("exiting parsecommand\n");
	return (cmd);
}

t_cmd	*parseline(char **ps, char *es, t_line_info *li)
{
	t_cmd		*cmd;
	int			tok;

	//ft_printf("we entered parseline\n");
	//system("leaks -q minishell");
	init_line_info(li, ps);
	cmd = parseexec(ps, es, li);
	if (peek(ps, es, "|"))
	{
		tok = gettoken(ps, 0, 0, li);
		cmd = pipecmd(cmd, parseline(ps, es, li));
	}
	//ft_printf("we are about to exit parseline\n");
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
		//printf("Single quotes found starting at %s\nand ending at %s\n", li->begsq, li->endsq);
	}
	if ((!li->enddq) && (li->begdq) && ((li->begdq < li->begsq \
		|| li->begsq == NULL)) && li->sfl != 1 && li->dfl == 0)
	{
		handle_quote_flags(li, 1);
		//printf("Double quotes found starting at %s\nand ending at %s\n", li->begdq, li->enddq);
	}
	if (li->endsq && (*ps >= li->begsq && *ps <= li->endsq))
		li->in_quotes = 1;
	if (li->enddq && (*ps >= li->begdq && *ps <= li->enddq))
		li->in_quotes = 1;
}

t_cmd	*parseexec(char **ps, char *es, t_line_info *li)
{
	char		*q;
	char		*eq;
	int			tok;
	int			argc;
	t_execcmd	*cmd;
	t_cmd		*ret;

	ret = execcmd();
	cmd = (t_execcmd *)ret;
	argc = 0;
	ret = parseredirs(ret, ps, es, li);
	//check_quotes(ps, li);
	while ((((**ps != '|') && (!li->in_quotes)) || (li->in_quotes)) && **ps)
	{
		if ((tok = gettoken(ps, &q, &eq, li)) == 0)
			break ;
		if (tok != 'a')
			panic("syntax");
		cmd->argv[argc] = q;
		cmd->eargv[argc] = eq;
		//printf("Current token's start and end: %s\n%s\n", q, eq);
		argc++;
		if (argc >= MAXARGS)
		{
			printf("argc: %d", argc);
			panic("too many args");
		}
		ret = parseredirs(ret, ps, es, li);
	}
	cmd->argv[argc] = 0;
	cmd->eargv[argc] = 0;
	return (ret);
}

t_cmd*	parseredirs(t_cmd *cmd, char **ps, char *es, t_line_info *li)
{
	int		tok;
	char	*q;
	char	*eq;
	//int fd;

	//printf("String is now at %s\n, and begq is %s\n and end is %s\n and the flags are: %d, %d\n", *ps, li->begdq, li->enddq, li->sfl, li->dfl);
	if ((!li->in_quotes)) // && ((*ps > li->endsq && *ps < li->begsq) || (*ps > li->enddq && *ps < li->begdq))
	{
		while (peek(ps, es, "<>") && !li->in_quotes)
		{
			tok = gettoken(ps, 0, 0, li);
			if (gettoken(ps, &q, &eq, li) != 'a')
				ft_putstr_fd("missing file for redirection\n", 2);
			//printf("Redir was fould to be beginning and %s\nand ending at %s\n", q, eq);
			if (tok == '<')
				cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
			else if (tok == '-')
			{
				//li->hdfl = 1;
				*eq = 0;
				//printf("The delimiter is %s\n", q);
				//fd = open(q, 1, O_CREAT | O_RDWR);
				cmd = redircmd(cmd, NULL, NULL, O_RDONLY, 0);
				
				//gettoken(ps, &q, &eq, li);
				if(li->heredoc_buff)
				{
					free(li->heredoc_buff);
					li->heredoc_buff = NULL;
				}
				li->heredoc_buff = heredoc_builder(q);
				//ft_printf("Assembled buffer is:\n%s", li->heredoc_buff);
				//write(fd, li->heredoc_buff, ft_strlen(li->heredoc_buff));
			}
			else if (tok == '>')
				cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREAT | O_TRUNC, 1);
			else if (tok == '+')
				cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREAT | O_APPEND, 1);
		}
	}
	return (cmd);
}

t_cmd	*nullterminate(t_cmd *cmd)
{
	int			i;
	t_execcmd	*ecmd;
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
/*
void runcmd(t_cmd *cmd)
{
	int			p[2];
	t_backcmd	*bcmd;
	t_execcmd	*ecmd;
	t_listcmd	*lcmd;
	t_pipecmd	*pcmd;
	t_redircmd	*rcmd;

	exit(0);
}*/

void	print_exec(t_execcmd *ecmd)
{
	int	i;

	i = 0;
	printf("Exec node\n");
	while (ecmd->argv[i])
	{
		printf("Arg[%d]: %s\n", i, ecmd->argv[i]);
		i++;
	}
	printf("\n");
}

void	print_tree(t_cmd *cmd)
{
	t_execcmd	*ecmd;
	t_pipecmd	*pcmd;
	t_redircmd	*rcmd;

	if (cmd->type == 1)
	{
		ecmd = (t_execcmd *)cmd;
		print_exec(ecmd);
	}
	if (cmd->type == 2)
	{
		printf("Redir node: \n\n");
		rcmd = (t_redircmd *)cmd;
		//if (rcmd->cmd->type)
		print_tree(rcmd->cmd);
	}
	if (cmd->type == 3)
	{
		pcmd = (t_pipecmd *)cmd;
		printf("Pipe node: \n\n");
		print_tree(pcmd->left);
		print_tree(pcmd->right);
	}
}

void	free_tree(t_cmd *cmd)
{
	t_execcmd	*ecmd;
	t_pipecmd	*pcmd;
	t_redircmd	*rcmd;

	if (cmd->type == 1)
	{
		ecmd = (t_execcmd *)cmd;
		//printf("Found an exec node: \n\n");
		//print_exec(ecmd);
		//printf("Freeing exec node\n\n");
		free(cmd);
		//printf("Successfully freed the exec node \n\n");
	}
	if (cmd->type == 2)
	{
		rcmd = (t_redircmd *)cmd;
		//if (rcmd->cmd->type)
	//	printf("Found a redir node: \n\n");
		free_tree(rcmd->cmd);
	//	printf("Freeing redir node: \n\n");
		free(cmd);
	//	printf("Successfully freed the redir node \n\n");
	}
	if (cmd->type == 3)
	{
		pcmd = (t_pipecmd *)cmd;
	//	printf("Found a pipe node: \n\n");
		
		free_tree(pcmd->left);
		free_tree(pcmd->right);
	//	printf("Freeing pipe node: \n\n");
		free(cmd);
	//	printf("Successfully freed the pipe node \n\n");
	}
}
void	one_time_init(t_line_info *li, t_parsing *p, t_info *info)
{
	p->ptr_parking = p->str;
	li->info = info;
	li->whitespace = ft_strdup(" \t\r\n\v");
	li->symbols = ft_strdup("<|>");
}


