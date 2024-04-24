# Minishell

This is a basic bash-like unix shell, written in C, by Ryan Boudwin and Andrey Kovalevskiy as part of the 42 curriculum offered by Hive Helsinki, a peer-to-peer coding school.

## Requirements
* This was written for Mac OS X.
* It requires the homebrew version of readline (brew install readline). You will need to update the paths in the makefile to reflect the actual location of your readline installation (since in the Hive school environment we have to use a non-default path).

## Features
* Custom prompt with username and current working path
* Support for pipes (|)
* Output redirection with (> or >>)
* Input redirection with < and heredoc with <<
* Single and double quote parsing
* environmental variable management and expansion
* Command history that persists between sessions
* Manages posix signals to ensure child processes can be interrupted as expected but won't quit the shell itself
* Replicates the following builtin commands: cd, pwd, export, env, unset, echo, exit

## Things we learned
* We learned how to build and manage abstract syntax trees to be able to parse complex command chains
* How to manage handling of posix signals
* How to create and manage child processes to be able to execute multiple commands in sequence
