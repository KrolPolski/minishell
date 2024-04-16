/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unit_tests.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 14:53:37 by rboudwin          #+#    #+#             */
/*   Updated: 2024/04/15 15:52:44 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
/*
void	test(t_info *info)
{
	int fd;
	char *str;
	char *mini_output;

	ft_printf("Wouldn't it be lovely to have some automated tests?\n");
	fd = open("test_input", O_RDONLY);
	if (fd > 0)
	{
		system("rm -f mini_output bash_output");
		system("touch bash_output");
		str = get_next_line(fd);
		while (str)
		{
			mini_output = ft_strjoin(str, " >> mini_output");
			ft_printf("\ntest string:\n%s", str);
			ft_printf("\nMinishell output:\n");
			alt_parsing(info, mini_output);
			system("leaks -q minishell");
			free(mini_output);
			free(str);
			str = get_next_line(fd);
		}
		ft_printf("==== Now running test commands with bash ====\n");
		system("bash -c './test_input > bash_output'");
		ft_printf("\n====diff mini_output bash_output====\n");
		system("diff mini_output bash_output");
		ft_printf("==== THE END ====");
		ft_printf("\nif there is nothing above then that means we matched bash!\n");
	}
}

int	alt_parsing(t_info *info, char *str)
{
	int			fd;
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
	ptr_parking = str;
	li.info = info;
	one_time_init(info, &li);
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
	//free(str);
	if (li.heredoc_buff)
		free(li.heredoc_buff);
	//ft_printf("after freeing stuff\n");
	//ft_printf("before freedom rings str is '%s'\n", str);
	//free(ptr_parking);
	if (exp_wants_freedom)
		free(expanded);
	//ft_printf("after freeing stuff\n");
	system("leaks -q minishell");
	ptr_parking = str;
	free_and_null(li.whitespace);
	free_and_null(li.symbols);
	//ft_printf("We bid you all a very fond farewell.\n");
	return (0);
}*/