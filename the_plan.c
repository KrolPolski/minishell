/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   the_plan.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/17 15:45:06 by rboudwin          #+#    #+#             */
/*   Updated: 2024/03/14 12:51:34 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Current Bugs
	1. when we ctrl-c a process, we end up with two printings of the prompt.
	2. when we ctrl-c an idle prompt, the input isn't actually used for anything.
/* Current focuses:
	1. Add execution logic that uses the tree created in parsing.c. This should
		largely follow the pipex fork and exec logic. The tree needs to determine
		order of operations.
		a) first make it work with a single command with args.
		b) then make it work with a single pipe
		c) make it work with redirects
		d) make it work with multiple pipes and redirects
	2. Dig deeply into readline and figure out:
		a) how to implement ctrl-D detection properly. Particularly need to figure
			out what are all the things that can cause readline to return a NULL
			pointer. 
			Except CTRL-D seems to be working already to log us out without us needing
			to do anything.
	3. implement all the required built in functions. Make sure that they modify the
		prompt when the path changes, etc.
	4. get caught up on termcaps and figure out how to move the cursor.
	5. implement history.
	6. Start writing unit tests for the functionality above.		
*/
/* Todos before we write any code:
 * 
 * Figure out how branches work in git and set things up so we have a main branch, 
 * and a separate branch for each of us to be working with ourselves. And then we can 
 * do pull requests to put stuff into the main branch after we have both reviewed 
 * all the changes. Andrey
 
 * Figure out what syntax trees are and why they would be useful for this project: Andrey
 
 * read up on signals processing, cover whatever we missed by not doing minitalk as a project - Ryan

 * read up on unit testing, make a plan for automated unit tests to grow as the project grows - Ryan
 
 * make a plan on how to divide the work to start with

 Review in detail this post:
 https://dashboard.hive.fi/topics/101/messages?cursus_id=21 
 * 
* Familiarize ourselves with all the permitted functions:
	readline, rl_clear_history, rl_on_new_line,
	rl_replace_line, rl_redisplay, add_history,
	printf, malloc, free, write, access, open, read,
	close, fork, wait, waitpid, wait3, wait4, signal,
	sigaction, sigemptyset, sigaddset, kill, exit,
	getcwd, chdir, stat, lstat, fstat, unlink, execve,
	dup, dup2, pipe, opendir, readdir, closedir,
	strerror, perror, isatty, ttyname, ttyslot, ioctl,
	getenv, tcsetattr, tcgetattr, tgetent, tgetflag,
	tgetnum, tgetstr, tgoto, tputs
 */

/*readline is keg-only, which means it was not symlinked into /Users/rboudwin/.brew,
because macOS provides BSD libedit.

For compilers to find readline you may need to set:
  export LDFLAGS="-L/Users/rboudwin/.brew/opt/readline/lib"
  export CPPFLAGS="-I/Users/rboudwin/.brew/opt/readline/include"*/

/* Your shell should:
• Display a prompt when waiting for a new command.
• Have a working history.
• Search and launch the right executable (based on the PATH variable or using a
relative or an absolute path).
• Avoid using more than one global variable to indicate a received signal. Consider
the implications: this approach ensures that your signal handler will not access your
main data structures.
Be careful. This global variable cannot provide any other
information or data access than the number of a received signal.
Therefore, using "norm" type structures in the global scope is
forbidden.
5
Minishell As beautiful as a shell
• Not interpret unclosed quotes or special characters which are not required by the
subject such as \ (backslash) or ; (semicolon).
• Handle ’ (single quote) which should prevent the shell from interpreting the metacharacters in the quoted sequence.
• Handle " (double quote) which should prevent the shell from interpreting the metacharacters in the quoted sequence except for $ (dollar sign).
• Implement redirections:
◦ < should redirect input.
◦ > should redirect output.
◦ << should be given a delimiter, then read the input until a line containing the
delimiter is seen. However, it doesn’t have to update the history!
◦ >> should redirect output in append mode.
• Implement pipes (| character). The output of each command in the pipeline is
connected to the input of the next command via a pipe.
• Handle environment variables ($ followed by a sequence of characters) which
should expand to their values.
• Handle $? which should expand to the exit status of the most recently executed
foreground pipeline.
• Handle ctrl-C, ctrl-D and ctrl-\ which should behave like in bash.
• In interactive mode:
◦ ctrl-C displays a new prompt on a new line.
◦ ctrl-D exits the shell.
◦ ctrl-\ does nothing.
• Your shell must implement the following builtins:
◦ echo with option -n
◦ cd with only a relative or absolute path
◦ pwd with no options
◦ export with no options
◦ unset with no options
◦ env with no options or arguments
◦ exit with no options
6
Minishell As beautiful as a shell
The readline() function can cause memory leaks. You don’t have to fix them. But
that doesn’t mean your own code, yes the code you wrote, can have memory
leaks.
*/
