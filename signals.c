/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 11:55:49 by rboudwin          #+#    #+#             */
/*   Updated: 2024/03/13 10:38:08 by rboudwin         ###   ########.fr       */
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
	
	/* at present ctrl-C will just quit our minishell entirely, not the 
	program running in the minishell.
	we need to figure out a way to make it target the child processes 
	instead of the main minishell process
	unless we are just waiting for a command with readline. if we are 
	waiting for a command it should just reprint the prompt */
	if (signal == SIGINT)
	{
		
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