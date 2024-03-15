/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 18:01:56 by rboudwin          #+#    #+#             */
/*   Updated: 2024/03/14 19:05:03 by akovalev         ###   ########.fr       */
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
# define MAXARGS 10

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
	char	**env;
}	t_info;

void	set_signal_action(void);
char	*readline(const char *prompt);
extern void rl_replace_line (const char *, int);
int		parsing(t_info *info);
char	*ft_prompt(char *username, char *hostname, char *path);
t_cmd	*parsecommand(char *str);
void	restore_curs_pos(void);
void	save_curs_pos(void);
#endif