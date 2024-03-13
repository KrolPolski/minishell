/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 11:55:49 by rboudwin          #+#    #+#             */
/*   Updated: 2024/03/13 14:39:57 by rboudwin         ###   ########.fr       */
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
	/*need to make the below logic smart enough to not redraw the prompt twice
	when we use ctrl-c to end a child process*/
	if (signal == SIGINT)
	{
		write(2, "\n", 1);
		rl_replace_line("", 0);
   		rl_on_new_line();
    	rl_redisplay();
		//still need to hide the ^C output, probably using stty.
	}
	if (signal == SIGQUIT) //CTRL-backslash
	{
		return ;
	}
}

void	set_signal_action(void)
{
	struct sigaction	act;
	sigset_t 			set;

	ft_bzero(&act, sizeof(act));
	act.sa_handler = &sigint_handler;
	sigaction(SIGINT, &act, NULL);
	sigaction(SIGQUIT, &act, NULL);
}