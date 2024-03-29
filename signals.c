/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 11:55:49 by rboudwin          #+#    #+#             */
/*   Updated: 2024/03/14 19:01:38 by akovalev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//currently save and restore cursor functions don't seem to work properly, but they might not be needed
//if the signal() function does indeed take care of the CTRL-\ cursor issue.

void	save_curs_pos(void)
{
	char	buf[1024];
	char	*save_curs;

	if (tgetent(buf, getenv("TERM")) < 0)
	{
		fprintf(stderr, "Failed to get terminal capabilities\n");
		exit(1);
	}
	save_curs = tgetstr("sc", NULL);
	if (save_curs == NULL)
	{
		fprintf(stderr, "Failed to get save cursor capability\n");
		exit(1);
	}
	tputs(save_curs, 1, putchar);
}

void	restore_curs_pos(void)
{
	char	*restore_curs;

	restore_curs = tgetstr("rc", NULL);
	if (restore_curs == NULL)
	{
		fprintf(stderr, "Failed to get restore cursor capability\n");
		exit(1);
	}
	tputs(restore_curs, 1, putchar);
}

void	sigint_handler(int signal)
{
	if (signal == SIGINT)
	{
		write(2, "\n", 1);
		rl_replace_line("", 0);
   		rl_on_new_line();
    	rl_redisplay();
	}
	if (signal == SIGQUIT)
	{
		restore_curs_pos();
		//return ;
	}
}

void	set_signal_action(void)
{
	struct sigaction	act;
	sigset_t			set;

	ft_bzero(&act, sizeof(act));
	act.sa_handler = &sigint_handler;
	sigaction(SIGINT, &act, NULL);
	//sigaction(SIGQUIT, &act, NULL);
}
