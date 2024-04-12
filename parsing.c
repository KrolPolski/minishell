/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/05 14:36:57 by akovalev          #+#    #+#             */
/*   Updated: 2024/04/11 16:45:12 by akovalev         ###   ########.fr       */
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
	if (ft_strchr(com, '/'))
	{
		if (access(command, X_OK != -1))
			return (ft_strdup(com));
		else
			return (NULL);
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
	int fd;

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
		if (fork1() == 0)
		{
			close(1);
			dup(p[1]);
			close(p[0]);
			close(p[1]);
			execute(pcmd->left, env, info, li);
		}
		if (fork1() == 0)
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
				handle_builtins(ecmd, env, builtins[i], info);
				exit(1);
			}
			i++;
		}
		command = check_command(ecmd->argv[0], env);
		execve(command, ecmd->argv, env);
		printf("execve failed\n");
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
			if (write(p[1], li->heredoc_buff, strlen(li->heredoc_buff)) < 0)
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

void	check_quotes(char **ps, t_line_info *li);

void	handle_quote_removal(char **str, int *ret, t_line_info *li, bool qflag)
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

void	handle_regular_chars(char **str, int *ret, t_line_info *li)
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
					handle_quote_removal(str, ret, li, 0);
				(*str)++;
			}
		}
		else if (li->dfl == 1 && li->in_quotes)
		{
			while (*str <= li->enddq)
			{
				if (li->dfl == 1 && *str == li->enddq)
					handle_quote_removal(str, ret, li, 1);
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
		handle_regular_chars(str, ret, li);
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
t_cmd *parseline(char**, char*, t_line_info *li);
t_cmd *parsepipe(char**, char*);
t_cmd *parseexec(char**, char*, t_line_info *li);
t_cmd *nullterminate(t_cmd *);
t_cmd*	parseredirs(t_cmd *cmd, char **ps, char *es, t_line_info *li);

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
	/*if (li->heredoc_buff)
	{
		free(li->heredoc_buff);
		li->heredoc_buff = NULL;
	}*/
	init_line_info(li, ps);
	cmd = parseexec(ps, es, li);
	if (peek(ps, es, "|"))
	{
		tok = gettoken(ps, 0, 0, li);
		if (li->symbols)
	{
		//ft_printf("li->symbols must not be null so freedom time\n");
		free(li->symbols);
		li->symbols = NULL;
	}
	if (li->whitespace)
	{
		//ft_printf("li->whitespace must not be null so freedom time\n");
		free(li->whitespace);
		li->whitespace = NULL;
	}
		cmd = pipecmd(cmd, parseline(ps, es, li));
	}
	if (li->symbols)
	{
		//ft_printf("li->symbols must not be null so freedom time\n");
		free(li->symbols);
		li->symbols = NULL;
	}
	if (li->whitespace)
	{
		//ft_printf("li->whitespace must not be null so freedom time\n");
		free(li->whitespace);
		li->whitespace = NULL;
	}
	//ft_printf("we are about to exit parseline\n");
	return (cmd);
}
void	handle_quote_flags(t_line_info *li, bool	qflag)
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
	int fd;

	//printf("String is now at %s\n, and begq is %s\n and end is %s\n and the flags are: %d, %d\n", *ps, li->begdq, li->enddq, li->sfl, li->dfl);
	if ((!li->in_quotes)) // && ((*ps > li->endsq && *ps < li->begsq) || (*ps > li->enddq && *ps < li->begdq))
	{
		while (peek(ps, es, "<>") && !li->in_quotes)
		{
			tok = gettoken(ps, 0, 0, li);
			if (gettoken(ps, &q, &eq, li) != 'a')
				if (li->pid == 0)
					panic("missing file for redirection");
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

int	parsing(t_info *info)
{
	int			fd;
	char		*str;
	t_cmd		*cmd;
	int			status;
	t_execcmd	*ecmd;
	char		*expanded;
	t_line_info	li;

	int			tree_prisoner;
	char		exp_wants_freedom;
	char		*ptr_parking;

	while ((fd = open("console", O_RDWR)) >= 0)
	{
		if (fd >= 3)
		{
			close(fd);
			break ;
		}
	}
	//save_curs_pos(); //currently unnecessary
	str = readline(info->prompt);
	//ft_printf("received first readline input of\n %s\n", str);
	ptr_parking = str;
	li.info = info;
	while (str != NULL)
	{
		add_history(str);
		li.heredoc_buff = NULL;
	//	ft_printf("Before quote expansion, checking leaks:\n");
	//	system("leaks -q minishell");
	//	ft_printf("End of pre-quote check\n");
		expanded = expand_env_remove_quotes(str, info->curr_env, &li);
		if (expanded == ptr_parking)
			exp_wants_freedom = 0;
		else
			exp_wants_freedom = 1;
	//	ft_printf("Now after expansion expanded is '%s' and str is '%s'\n", expanded, str);
		//system("leaks -q minishell");
		cmd = parsecommand(expanded, &li);
		//print_tree(cmd);
		//ft_printf("Now after parsecommand\n");
		//system("leaks -q minishell");
		if (cmd->type == 1)
		{
			ecmd = (t_execcmd *)cmd;
			if (ecmd->argv[0] && ft_strncmp(ecmd->argv[0], "cd", 3) == 0)
				ft_cd(ecmd, info);
			else if (ecmd->argv[0] && ft_strncmp(ecmd->argv[0], "export", 7) == 0) // should it not have the last condition removed? otherwise it does not run in parent without args
				ft_export(ecmd, info);
			else if (ecmd->argv[0] && ft_strncmp(ecmd->argv[0], "exit", 5) == 0)
				ft_exit(ecmd, info);
			else if (ecmd->argv[0] && ft_strncmp(ecmd->argv[0], "unset", 6) == 0)
				ft_unset(ecmd, info);
			else if (ecmd->argv[0] && ft_strncmp(ecmd->argv[0], "pwd", 4) == 0)
				ft_pwd(ecmd, info);
			else
			{
				li.pid = fork1();
				if (li.pid == 0)
				{
					signal(SIGQUIT, SIG_DFL);
					execute(cmd, info->curr_env, info, &li);
				}
				tree_prisoner = 0;
				ft_printf("we must be a single command\n");}
		}
		else
		{
			li.pid = fork1();
			if (li.pid == 0)
			{
				signal(SIGQUIT, SIG_DFL);
				execute(cmd, info->curr_env, info, &li);	
			}
			tree_prisoner = 1;

		}
		//ft_printf("after fork, but in parent\n");
		//system("leaks -q minishell");
		signal(SIGINT, SIG_IGN);
		wait(&status);
		set_signal_action();
		if (WIFEXITED(status))
		{
			info->exit_code = WEXITSTATUS(status);
			//ft_printf("EXECUTE HANDLER EXIT CODE IS %d\n", info->exit_code);
		}
		//ft_printf("after setting exit code, but before frees\n");
		//system("leaks -q minishell");
		//print_tree(cmd);
		if (tree_prisoner)
		{
			//ft_printf("Our tree is oppressed\n");
			free_tree(cmd);
		}
		else
			free(cmd);
		//we need a free_tree(cmd) function written and placed here.
		free(str);
		if (li.heredoc_buff)
			free(li.heredoc_buff);
		free(li.whitespace);
		free(li.symbols);
		//ft_printf("after freeing stuff\n");
		//ft_printf("before freedom rings str is '%s'\n", str);
		//free(ptr_parking);
		if (exp_wants_freedom)
			free(expanded);
		//ft_printf("after freeing stuff\n");
		system("leaks -q minishell");
		str = readline(info->prompt);
		ptr_parking = str;
	}
	ft_printf("We bid you all a very fond farewell.\n");
	return (0);
}
