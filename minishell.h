/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 18:01:56 by rboudwin          #+#    #+#             */
/*   Updated: 2024/04/05 10:20:40 by rboudwin         ###   ########.fr       */
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
	char	**curr_env;
}	t_info;

typedef struct s_line_info
{
	bool	sfl;
	bool	dfl;
	char	*endsq;
	char	*enddq;
	char	*begsq;
	char	*begdq;
	char	*beg_str;
	char	*end_str;
	char	*beg_var;
	bool	free_flag;
	bool	flag_changed;
	bool	in_quotes;
	char	*whitespace;
	char	*symbols;
}	t_line_info;

typedef struct s_export
{
	char	**new_env;
	int		curr_len;
	int		i;
	int		k;
	int		target_len;
}	t_export;

void		free_2d(char **arr);
void		set_signal_action(void);
char		*readline(const char *prompt);
extern void	rl_replace_line(const char *text, int clear_undo);
int			parsing(t_info *info);
char		*ft_prompt(char *username, char *hostname, char *path);
t_cmd		*parsecommand(char *str);
void		restore_curs_pos(void);
void		save_curs_pos(void);
char		*expand_env_var(char *var, char **env);
char		*expand_string(char *str, char **env);
int			peek(char **ps, char *es, char *tokens);
void		handle_builtins(t_execcmd *ecmd, char **env,
				char *builtin_command, t_info *info);
void		ft_cd(t_execcmd *ecmd, t_info *info);
void		ft_echo(t_execcmd *ecmd, t_info *info);
void		ft_env(t_execcmd *ecmd, t_info *info);
void		ft_pwd(t_execcmd *ecmd, t_info *info);
void		panic(char *s);
int			ft_matrix_len(char **str);
void		ft_export(t_execcmd *ecmd, t_info *info);
char		*expand_env_remove_quotes(char *str, char **env);
char		*replace_name(t_line_info *li, char *var,
				char *exp_var, char **str);
char		*heredoc_builder(char *delimiter);
void		init_line_info(t_line_info *li, char **str);
void		quote_handler(t_line_info *li, char **str, char **env);
void		remove_quotes(char *begq, char *endq);
void		ft_exit(t_execcmd *ecmd, t_info *info);
void		ft_unset(t_execcmd *ecmd, t_info *info);
#endif