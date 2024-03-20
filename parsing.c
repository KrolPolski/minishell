/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/05 14:36:57 by akovalev          #+#    #+#             */
/*   Updated: 2024/03/20 13:51:06 by rboudwin         ###   ########.fr       */
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

void	execute(t_cmd *cmd, char **env, t_info *info)
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
		if (fork1() == 0)
		{
			close(1);
			dup(p[1]);
			close(p[0]);
			close(p[1]);
			execute(pcmd->left, env, info);
		}
		if (fork1() == 0)
		{
			close(0);
			dup(p[0]);
			close(p[0]);
			close(p[1]);
			execute(pcmd->right, env, info);
		}
		close(p[0]);
		close(p[1]);
		wait(&status);
		if (WIFEXITED(status))
		{
			info->exit_code = WEXITSTATUS(status);
			ft_printf("Exit code of pipe fork is %d\n", info->exit_code);
		}
		wait(&status);
		if (WIFEXITED(status))
		{
			info->exit_code = WEXITSTATUS(status);
			ft_printf("Exit code of pipe fork2 is %d\n", info->exit_code);
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
		if (open(rcmd->file, rcmd->mode, 0666) < 0)
		{
			printf("open %s failed\n", rcmd->file);
			exit(1);
		}
		execute (rcmd->cmd, env, info);
	}
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


int	gettoken(char **pstr, char *end_str, char **q, char **eq)
{
	char	*str;
	int		ret;
	char	whitespace[] = " \t\r\n\v";
	char	symbols[] = "<|>";

	str = *pstr;
	while (str < end_str && ft_strchr(whitespace, *str))
		str++;
	if (q)
		*q = str;
	ret = *str;
	if (*str == '|')
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
	char	symbols[] = "<|>";

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
	// while (peek(ps, es, "&"))
	// {
	// 	gettoken(ps, es, 0, 0);
	// 	cmd = backcmd(cmd);
	// }
	// if (peek(ps, es, ";"))
	// {
	// 	gettoken(ps, es, 0, 0);
	// 	cmd = listcmd(cmd, parseline(ps, es));
	// }
	return (cmd);
}

t_cmd	*parsepipe(char **ps, char *es)
{
	t_cmd	*cmd;
	int		tok;

	cmd = parseexec(ps, es);
	if (peek(ps, es, "|"))
	{
		tok = gettoken(ps, es, 0, 0);
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
			ft_printf("missing file for redirection\n");
		if (tok == '<')
			cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
		else if (tok == '-')
		{
			heredoc_handler();
		}
		else if (tok == '>')
			cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREAT | O_TRUNC, 1);
		else if (tok == '+')
			cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREAT | O_APPEND, 1);
	}
	return (cmd);
}

// t_cmd	*parseblock(char **ps, char *es)
// {
// 	t_cmd	*cmd;

// 	if (!peek(ps, es, "("))
// 		panic("parseblock");
// 	gettoken(ps, es, 0, 0);
// 	cmd = parseline(ps, es);
// 	if (!peek(ps, es, ")"))
// 		panic("syntax - missing )");
// 	gettoken(ps, es, 0, 0);
// 	cmd = parseredirs(cmd, ps, es);
// 	return (cmd);
// }

t_cmd	*parseexec(char **ps, char *es)
{
	char		*q;
	char		*eq;
	int			tok;
	int			argc;
	t_execcmd	*cmd;
	t_cmd		*ret;

	// tok = gettoken(ps, es, 0, 0);
	// if (tok != 'a')
	// 	panic("syntax: multiple operators");
	// if (peek(ps, es, "("))
	// 	return (parseblock(ps, es));
	ret = execcmd();
	cmd = (t_execcmd *)ret;

	argc = 0;
	ret = parseredirs(ret, ps, es);
	while (!peek(ps, es, "|"))
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

int	parsing(t_info *info)
{
	int			fd;
	char		*str;
	char		*pstr;
	char		*str_check;
	char		*tmp;
	t_cmd		*cmd;
	int			status;
	t_execcmd	*ecmd;
	char		*expanded;

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
	while (str != NULL)
	{
		str_check = ft_strdup (str);
		tmp = str_check;
		while (*str_check)
		{
			if (*str_check == '|')
			{
				str_check++;
				if (*str_check == '|')
					panic("syntax: multiple operators\n");
			}
			else
				str_check++;
		}
		free (tmp);
		pstr = str;
		//printf("Command before expansion: %s\n", str);
		expanded = quote_handler(str, info->curr_env);
		//str = pstr;
		//printf("Command after expansion: %s\n", expanded);
		//free(str);
		cmd = parsecommand(expanded);
		if (cmd->type == 1)
		{
			ecmd = (t_execcmd *)cmd;
			if (ecmd->argv[0] && ft_strncmp(ecmd->argv[0], "cd", 3) == 0)
				ft_cd(ecmd, info);
			else if (ecmd->argv[0] && ft_strncmp(ecmd->argv[0], "export", 7) == 0)
				ft_export(ecmd, info);
		// 	// else if (ecmd->argv[0] && ft_strncmp(ecmd->argv[0], "pwd", 4) == 0)
		// 	// 	ft_pwd(ecmd, info);
		}
		if (fork1() == 0)
		{
			signal(SIGQUIT, SIG_DFL);
			execute(cmd, info->curr_env, info);
		}
		wait(&status);
		if (WIFEXITED(status))
		{
			info->exit_code = WEXITSTATUS(status);
			ft_printf("EXECUTE HANDLER EXIT CODE IS %d\n", info->exit_code);
		}
		//print_tree(cmd);
		//free(str);
		str = readline(info->prompt);
	}
	return (0);
}
