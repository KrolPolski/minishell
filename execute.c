/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 19:34:00 by akovalev          #+#    #+#             */
/*   Updated: 2024/04/19 19:48:24 by akovalev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*a function that handles the pipe node*/
void	handle_pipe_node(t_cmd *cmd, char **env, t_info *info, t_line_info *li)
{
	t_pipecmd	*pcmd;
	int			p[2];

	pcmd = (t_pipecmd *)cmd;
	if (pipe(p) < 0)
		panic("pipe");
	li->pid = fork1();
	if (li->pid == 0)
	{
		handle_pipe_fds(p, 1);
		execute(pcmd->left, env, info, li);
	}
	li->pid = fork1();
	if (li->pid == 0)
	{
		handle_pipe_fds(p, 0);
		execute(pcmd->right, env, info, li);
	}
	close(p[0]);
	close(p[1]);
	wait_and_set_exit_code(info);
}

/*a function that handles heredoc by piping the heredoc byffer*/
void	handle_heredoc_case(t_line_info *li, t_redircmd *rcmd, int *p)
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

void	handle_redir_node(t_cmd *cmd, char **env, t_info *info, t_line_info *li)
{
	t_redircmd	*rcmd;
	int			p[2];

	rcmd = (t_redircmd *)cmd;
	close (rcmd->fd);
	if (rcmd->file == NULL)
		handle_heredoc_case(li, rcmd, p);
	else if (open(rcmd->file, rcmd->mode, 0666) < 0)
	{
		ft_putstr_fd("AR-Shell: ", 2);
		perror(rcmd->file);
		exit(1);
	}
	execute (rcmd->cmd, env, info, li);
}

/*a function that handles exec node and builtins*/
void	handle_exec_node(t_cmd *cmd, char **env, t_info *info, char **builtins)
{
	t_execcmd	*ecmd;
	char		*command;
	int			i;

	i = 0;
	ecmd = (t_execcmd *)cmd;
	if (ecmd->argv[0] == NULL)
		exit(1);
	while (builtins[i])
	{
		if (ft_strlen(builtins[i]) == ft_strlen(ecmd->argv[0]) && \
			!ft_strncmp(ecmd->argv[0], builtins[i], ft_strlen(builtins[i])))
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

/*main execute function that calls functions handling differend nodes*/
void	execute(t_cmd *cmd, char **env, t_info *info, t_line_info *li)
{
	char		*builtins[8];

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
		handle_pipe_node(cmd, env, info, li);
	else if (cmd->type == EXEC)
		handle_exec_node(cmd, env, info, builtins);
	else if (cmd->type == REDIR)
		handle_redir_node(cmd, env, info, li);
	exit(info->exit_code);
}
