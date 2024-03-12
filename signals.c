/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 11:55:49 by rboudwin          #+#    #+#             */
/*   Updated: 2024/03/12 16:18:21 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	sigint_handler(int signal)
{
	//CTRL-C when the minishell is in interactive mode should just reprint the prompt
	//CTRL-C while something is running should interrupt it
	//CTRL-D should logout the current user, but this isn't actually a POSIX signal so
	// have to handle it a different way. probably a "key function"
	// as described in the readline docs.

	/* From the documentation:
	Variable: int rl_eof_found
	Readline will set this variable when it has read an EOF character (e.g., 
	the stty ‘EOF’ character) on an empty line or encountered a read error and 
	is about to return a NULL line to the caller.
	*/
	/* So it looks like we can create key bindings etc in readline. so anything bound there
	applies to interactive mode. and we can have keybindings outside of readline that handle
	while child processes are running etc. called a keymap in the documentation*/
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
	if (signal == SIGQUIT) //CTRL-backslash
	{
		//might need some cleanup here. or not? because 
		//exit frees memory anyway, right?
		//should still think about fd leakage at least surely
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