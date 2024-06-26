/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_tools.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 19:35:02 by akovalev          #+#    #+#             */
/*   Updated: 2024/04/22 15:18:25 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	fork_with_protection(void)
{
	int	pid;

	pid = fork();
	if (pid == -1)
		panic("fork");
	return (pid);
}

void	handle_pipe_fds(int *p, int fd)
{
	close(fd);
	dup(p[fd]);
	close(p[0]);
	close(p[1]);
}

void	wait_and_set_exit_code(t_info *info)
{
	int	status;

	wait(&status);
	if (WIFEXITED(status))
		info->exit_code = WEXITSTATUS(status);
	wait(&status);
	if (WIFEXITED(status))
		info->exit_code = WEXITSTATUS(status);
}
