/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 11:55:49 by rboudwin          #+#    #+#             */
/*   Updated: 2024/03/12 10:54:34 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	sigint_handler(int signal)
{
	//CTRL-C when the minishell is in interactive mode should just reprint the prompt
	//CTRL-C while something is running should interrupt it
	//CTRL-D should logout the current user
	//CTRL-\ should exit the shell or the program running in the shell as appropriate
	if (signal == SIGINT)
	{
		ft_printf("\nI am not so easily murdered. But I will take my leave\n");
		//forbidden
		sleep(1);
		ft_printf("...\n");
		//forbidden
		sleep(1);
		exit(1);
	}
	/*if (signal == SIGQUIT) //CTRL-D
	{

	}
	if (signal == SIG)*/
}
void	set_signal_action(void)
{
	struct sigaction	act;
	
	ft_bzero(&act, sizeof(act));
	act.sa_handler = &sigint_handler;
	sigaction(SIGINT, &act, NULL);
}