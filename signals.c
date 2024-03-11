/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 11:55:49 by rboudwin          #+#    #+#             */
/*   Updated: 2024/03/11 13:22:25 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	sigint_handler(int signal)
{
	if (signal == SIGINT)
	{
		ft_printf("\nI am not so easily murdered. But I will take my leave\n");
		sleep(1);
		ft_printf("...\n");
		sleep(1);
		exit(1);
	}
}
void	set_signal_action(void)
{
	struct sigaction	act;
	
	ft_bzero(&act, sizeof(act));
	act.sa_handler = &sigint_handler;
	sigaction(SIGINT, &act, NULL);
}