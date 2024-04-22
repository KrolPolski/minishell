/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 18:01:56 by rboudwin          #+#    #+#             */
/*   Updated: 2024/04/22 15:38:12 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H
# include "Libft/libft.h"
# include <signal.h>
# include <stdio.h>
# include <fcntl.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <termios.h>
# include <curses.h>

# define EXEC  1
# define REDIR 2
# define PIPE  3
# define MAXARGS 100

typedef struct s_cmd
{
	int	type;
}	t_cmd;

typedef struct s_execcmd
{
	int		type;
	char	*argv[MAXARGS];
	char	*eargv[MAXARGS];
}	t_execcmd;

typedef struct s_redircmd
{
	int			type;
	t_cmd		*cmd;
	char		*file;
	char		*efile;
	int			mode;
	int			fd;
}	t_redircmd;

typedef struct s_pipecmd
{
	int			type;
	t_cmd		*left;
	t_cmd		*right;
}	t_pipecmd;

typedef struct s_info
{
	char	*username;
	char	*init_dir;
	char	*curr_dir;
	char	*input;
	char	*prompt;
	int		argc;
	char	**argv;
	char	**init_env;
	int		exit_code;
	char	*exit_code_ptr;
	bool	ecp_flag;
	char	**curr_env;

}	t_info;

typedef struct s_line_info
{
	bool	sfl;
	bool	dfl;
	bool	hdfl;
	char	*endsq;
	char	*enddq;
	char	*begsq;
	char	*begdq;
	char	*beg_str;
	bool	beg_str_first_time;
	char	*end_str;
	char	*beg_var;
	bool	free_flag;
	bool	flag_changed;
	bool	in_quotes;
	char	*whitespace;
	char	*symbols;
	char	*heredoc_buff;
	int		pid;
	int		redir_tok;
	t_info	*info;
}	t_line_info;

typedef struct s_export
{
	char	**new_env;
	int		curr_len;
	int		i;
	int		k;
	int		target_len;
}	t_export;

typedef struct s_parsing
{
	int			fd;
	char		*str;
	int			status;
	char		*expanded;
	int			tree_prisoner;
	int			exp_wants_freedom;
	char		*ptr_parking;
}	t_parsing;

typedef struct s_unset
{
	int		k;
	char	*str;
	int		i;
	char	curr_len;
	char	**new_env;
}	t_unset;

typedef struct s_redir_node_info
{
	char	*q;
	char	*eq;
	bool	heredoc_flag;
}	t_redir_node_info;

//external from brew readline

char		*readline(const char *prompt);
extern void	rl_replace_line(const char *text, int clear_undo);

//built_in.c

void		ft_cd(t_execcmd *ecmd, t_info *info);
void		ft_echo(t_execcmd *ecmd, t_info *info);
void		ft_env(t_info *info);
void		ft_pwd(void);
void		handle_builtins(t_execcmd *ecmd,
				char *builtin_command, t_info *info);

//built_in2.c

void		ft_exit(t_execcmd *ecmd, t_info *info);

//constructors.c

t_cmd		*execcmd(void);
t_cmd		*redircmd(t_cmd *subcmd, t_redir_node_info *rni, int mode, int fd);
t_cmd		*pipecmd(t_cmd *left, t_cmd *right);
t_cmd		*build_heredoc_node(t_cmd *cmd, \
				t_redir_node_info *rni, t_line_info *li);

//env_and_quote_handler.c

char		*expand_main(char *str, char **env, t_line_info *li);
void		quote_handler(t_line_info *li, char **str, char **env);
void		fetch_and_replace(t_line_info *li, \
				char **str, char **env, int diff);
char		*expand_env(t_line_info *li, char *str, char **env);
char		*replace_name(t_line_info *li, \
				char *var, char *exp_var, char **str);

//execute_tools.c

int			fork_with_protection(void);
void		handle_pipe_fds(int *p, int fd);
void		wait_and_set_exit_code(t_info *info);

//execute.c

void		handle_pipe_node(t_cmd *cmd, \
				char **env, t_info *info, t_line_info *li);
void		handle_heredoc_case(t_line_info *li, t_redircmd *rcmd, int *p);
void		handle_redir_node(t_cmd *cmd, \
				char **env, t_info *info, t_line_info *li);
void		handle_exec_node(t_cmd *cmd, \
				char **env, t_info *info, char **builtins);
void		execute(t_cmd *cmd, char **env, t_info *info, t_line_info *li);

//export.c

int			check_matrix_no_needle(t_execcmd *ecmd, \
				char **new_env, int i, int k);
int			check_matrix(t_execcmd *ecmd, int k, char **new_env);
void		init_export(t_execcmd *ecmd, t_info *info, t_export *ex);
void		ft_export_handler(t_execcmd *ecmd, t_export *ex);
void		ft_export(t_execcmd *ecmd, t_info *info);

//export2.c

int			export_validator(char *str);
char		*var_to_equals(t_execcmd *ecmd, int k, int i);
void		export_empty(t_info *info);

//friends_of_gettoken.c

void		handle_quote_removal(char **str, t_line_info *li, bool qflag);
void		handle_regular_chars(char **str, t_line_info *li);
void		handle_current_char(char **str, int *ret, t_line_info *li);
void		handle_quote_flags(t_line_info *li, bool qflag);
void		check_quotes(char **ps, t_line_info *li);

//heredoc.c

char		*heredoc_builder(char *delimiter);

//main.c

void		set_username_dir(t_info *info);
int			main(int argc, char **argv, char **env);
void		populate_env_matrix(t_info *info);
void		set_shell_level(t_info *info);
char		*ft_prompt(char *username, char *hostname, char *path);

//parsing_main.c

void		null_command_handler(t_parsing *p, t_line_info *li);
void		parsing_loop(t_parsing *p, t_line_info *li, t_info *info);
int			parsing(t_info *info);

//parsing_tools_command

char		**parse_paths(char **env);
char		*check_absolute_path(char *com);
char		*check_command(char *com, char **env);

//parsing_tools.c

void		nullterminate_exec(t_cmd *cmd);
t_cmd		*nullterminate(t_cmd *cmd);
void		free_tree(t_cmd *cmd);
bool		check_pipe_syntax(char *ps, char *es, t_line_info *li);
int			peek(char **ps, char *es, char *tokens);

//parsing.c

int			gettoken(char **pstr, char **q, char **eq, t_line_info *li);
t_cmd		*parsecommand(char *str, t_line_info *li);
t_cmd		*parseline(char **ps, char *es, t_line_info *li);
t_cmd		*parseexec(char **ps, char *es, t_line_info *li);
t_cmd		*parseredirs(t_cmd *cmd, char **ps, char *es, t_line_info *li);

//parsing2.c
void		fork_and_execute(t_cmd *cmd, t_info *info,
				t_line_info *li, t_parsing *p);
void		parsing_cleanup(t_parsing *p, t_cmd *cmd, t_line_info *li);
void		parsing_signal_exit_codes(t_parsing *p, t_info *info);
void		single_command_handler(t_cmd *cmd, t_info *info,
				t_parsing *p, t_line_info *li);
//signals.c

void		set_signal_action(void);
void		sigint_handler(int signal);

// tools.c
void		free_2d(char **arr);
int			ft_matrix_len(char **str);
void		final_cleanup(t_info *info);
void		set_shell_level(t_info *info);
void		panic(char *str);

//tools1.c

void		init_line_info(t_line_info *li, char **str);
void		remove_quotes(char *begq, char *endq);
char		*fetch_env_var(char *var, char **env, t_line_info *li);
void		one_time_init(t_line_info *li, t_parsing *p, t_info *info);
void		check_malloc_failure(void *ptr);

//tools2.c
void		check_and_free(char *prompt, char *ptr_parking);

// unit_tests.c
void		test(t_info *info);
int			alt_parsing(t_info *info, char *str);

//unset.c

int			unset_validator(char *str);
char		*search_matrix(char *arg, char **matrix, int *i, int curr_len);
void		copy_unset(t_info *info, t_unset *un);
void		print_unset_error(t_execcmd *ecmd, t_unset *un);
void		ft_unset(t_execcmd *ecmd, t_info *info);

#endif