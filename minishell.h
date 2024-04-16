/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 18:01:56 by rboudwin          #+#    #+#             */
/*   Updated: 2024/04/16 15:52:35 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H
# include "Libft/libft.h"
# include "Libft/vec.h"
# include "Libft/ft_printf.h"
# include "Libft/get_next_line_bonus.h"
# include <signal.h>
# include <stdio.h>
# include <fcntl.h>
# include <stdlib.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <termios.h>
# include <curses.h>
# include <term.h>
# include <termcap.h>

# define EXEC  1
# define REDIR 2
# define PIPE  3
# define LIST  4
# define BACK  5
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

typedef struct s_listcmd
{
	int			type;
	t_cmd		*left;
	t_cmd		*right;
}	t_listcmd;

typedef struct s_backcmd
{
	int			type;
	t_cmd		*cmd;
}	t_backcmd;

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

//external from brew readline

char		*readline(const char *prompt);
extern void	rl_replace_line(const char *text, int clear_undo);

//built_in.c

void		ft_cd(t_execcmd *ecmd, t_info *info);
void		ft_echo(t_execcmd *ecmd);
void		ft_env(t_info *info);
void		ft_pwd(void);
void		handle_builtins(t_execcmd *ecmd,
				char *builtin_command, t_info *info);

//built_in2.c

int			check_matrix(t_execcmd *ecmd, int k, char **new_env);
void		ft_export(t_execcmd *ecmd, t_info *info);
void		export_empty(t_info *info);
void		init_export(t_execcmd *ecmd, t_info *info, t_export *ex);
char		*var_to_equals(t_execcmd *ecmd, int k, int i);

//built_in3.c

void		ft_exit(t_execcmd *ecmd, t_info *info);
void		ft_unset(t_execcmd *ecmd, t_info *info);
void		copy_unset(t_info *info, t_unset *un);
char		*search_matrix(char *arg, char **matrix, int *i, int curr_len);

//env_and_quote_handler.c

char		*expand_env_remove_quotes(char *str, char **env, t_line_info *li);
char		*expand_env(t_line_info *li, char *str, char **env);
char		*fetch_env_var(char *var, char **env, t_line_info *li);
void		init_line_info(t_line_info *li, char **str);
void		quote_handler(t_line_info *li, char **str, char **env);
void		remove_quotes(char *begq, char *endq);
char		*replace_name(t_line_info *li, char *var,
				char *exp_var, char **str);

//heredoc.c

char		*heredoc_builder(char *delimiter);

//main.c

void		final_cleanup(t_info *info);
int			main(int argc, char **argv, char **env);
void		populate_env_matrix(t_info *info);
void		set_shell_level(t_info *info);
void		set_termios_settings(void);
char		*ft_prompt(char *username, char *hostname, char *path);

//parsing.c

void		one_time_init(t_line_info *li, t_parsing *p, t_info *info);
t_cmd		*parsecommand(char *str, t_line_info *li);
char		*check_command(char *com, char **env);
void		execute(t_cmd *cmd, char **env, t_info *info, t_line_info *li);
void		free_tree(t_cmd *cmd);
int			fork1(void);
char		**parse_paths(char **env);
int			parsing(t_info *info);
int			peek(char **ps, char *es, char *tokens);
void		check_quotes(char **ps, t_line_info *li);
t_cmd		*parseline(char **ps, char *es, t_line_info *li);
t_cmd*		parseredirs(t_cmd *cmd, char **ps, char *es, t_line_info *li);
t_cmd		*nullterminate(t_cmd *cmd);
t_cmd		*parseexec(char **ps, char *es, t_line_info *li);

//parsing2.c
void		fork_and_execute(t_cmd *cmd, t_info *info,
				t_line_info *li, t_parsing *p);
void		parsing_cleanup(t_parsing *p, t_cmd *cmd, t_line_info *li);
void		parsing_signal_exit_codes(t_parsing *p, t_info *info);
void		single_command_handler(t_cmd *cmd, t_info *info,
				t_parsing *p, t_line_info *li);
				
//signals.c

void		set_signal_action(void);
void		restore_curs_pos(void);
void		save_curs_pos(void);
void		sigint_handler(int signal);

// tools.c
void		free_2d(char **arr);
int			ft_matrix_len(char **str);
int			export_validator(char *str);
void		final_cleanup(t_info *info);
void		set_shell_level(t_info *info);
void		panic(char *str);
// unit_tests.c
void		test(t_info *info);
int			alt_parsing(t_info *info, char *str);

#endif