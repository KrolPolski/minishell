/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 14:24:52 by rboudwin          #+#    #+#             */
/*   Updated: 2024/04/16 14:26:52 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
	char		exp_wants_freedom; // why is this a char and not an int?
	char		*ptr_parking;

	while ((fd = open("console", O_RDWR)) >= 0)
	{
		if (fd >= 3)
		{
			close(fd);
			break ;
		}
	}
	str = readline(info->prompt);
	//ft_printf("received first readline input of\n %s\n", str);
	ptr_parking = str;
	li.info = info;
	one_time_init(&li);
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
		if (cmd->type == EXEC) 
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
				ft_pwd();
			else
			{
				li.pid = fork1();
				if (li.pid == 0)
				{
					signal(SIGQUIT, SIG_DFL);
					execute(cmd, info->curr_env, info, &li);
				}
				tree_prisoner = 0;
				//ft_printf("we must be a single command\n");
			}
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
	free_and_null(li.whitespace);
	free_and_null(li.symbols);
	ft_printf("We bid you all a very fond farewell.\n");
	return (0);
}