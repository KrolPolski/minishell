/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unit_tests.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 14:53:37 by rboudwin          #+#    #+#             */
/*   Updated: 2024/04/11 14:50:05 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	test(t_info *info)
{
	int fd;
	char *str;
	char *bash_str;
	char *mini_output;
	char *bash_output;
	char *ptr_parking;
	char *str_no_nl;

	ft_printf("Wouldn't it be lovely to have some tests?\n");
	fd = open("test_input", O_RDONLY);
	if (fd > 0)
	{
		system("rm -f mini_output bash_output");
		system("touch bash_output");
		str = get_next_line(fd);
		while (str)
		{
			//bash_str = ft_strjoin("bash -c '", str);
			//ptr_parking = bash_str;
			//bash_str = ft_strjoin(bash_str, "'");
			//free(ptr_parking);
			//str_no_nl = ft_strdup(str);
			//str_no_nl[ft_strlen(str) - 1] = '\0';
			//bash_output = ft_strjoin(str_no_nl, " >> bash_output");
			///ft_printf("bash_output is '%s'\n", bash_output);
			//ptr_parking = bash_output;
			//bash_output = ft_strjoin("bash -c ", bash_output);
			//free(ptr_parking);
			mini_output = ft_strjoin(str, " >> mini_output");
			ft_printf("\ntest string:\n%s", str);
			ft_printf("\nMinishell output:\n");
			alt_parsing(info, str);
			alt_parsing(info, mini_output);
			//ft_printf("\nbash output:\n");
			//system(bash_str);
			
			
			//free(bash_str);
			//free(bash_output);
			//system("leaks -q minishell");
			str = get_next_line(fd);
		}
		system("bash -c './test_input > bash_output'");
		ft_printf("\ndiff mini_output bash_output\n");
		system("diff mini_output bash_output");
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
	li.heredoc_buff = NULL;
	expanded = expand_env_remove_quotes(str, info->curr_env, &li);
	if (expanded == ptr_parking)
		exp_wants_freedom = 0;
	else
		exp_wants_freedom = 1;
	cmd = parsecommand(expanded, &li);
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
		else if (fork1() == 0)
		{
			signal(SIGQUIT, SIG_DFL);
			execute(cmd, info->curr_env, info, &li);
		}
		tree_prisoner = 0;
		ft_printf("we must be a single command\n");
	}
	else
	{
		if (fork1() == 0)
		{
			signal(SIGQUIT, SIG_DFL);
			execute(cmd, info->curr_env, info, &li);	
		}
		tree_prisoner = 1;
	}
	signal(SIGINT, SIG_IGN);
	wait(&status);
	set_signal_action();
	if (WIFEXITED(status))
	{
		info->exit_code = WEXITSTATUS(status);
	}
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
	if (exp_wants_freedom)
		free(expanded);
		//ft_printf("after freeing stuff\n");
	//system("leaks -q minishell");
	return (0);
}
/*int	alt_parsing(t_info *info, char *str)
{
	int			fd;
	t_cmd		*cmd;
	int			status;
	t_execcmd	*ecmd;
	char		*expanded;
	int			tree_prisoner;
	char		exp_wants_freedom;
	char		*ptr_parking;
	t_line_info	li;

	ptr_parking = str;
	expanded = expand_env_remove_quotes(str, info->curr_env, &li);
	if (expanded == ptr_parking)
		exp_wants_freedom = 0;
	else
		exp_wants_freedom = 1;
	cmd = parsecommand(expanded, &li);
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
		else if (fork1() == 0)
		{
			signal(SIGQUIT, SIG_DFL);
			execute(cmd, info->curr_env, info, &li);
		}
		tree_prisoner = 0;
		}
		else
		{
		if (fork1() == 0)
		{
			signal(SIGQUIT, SIG_DFL);
			execute(cmd, info->curr_env, info, &li);	
		}
		tree_prisoner = 1;
	}
	signal(SIGINT, SIG_IGN);
	wait(&status);
	set_signal_action();
	if (WIFEXITED(status))
	{
		info->exit_code = WEXITSTATUS(status);
	}
	if (tree_prisoner)
	{
		free_tree(cmd);
	}
	else
		free(cmd);
	free(ptr_parking);
	if (exp_wants_freedom)
		free(expanded);
	return (0);
}*/