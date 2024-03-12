/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 18:01:56 by rboudwin          #+#    #+#             */
/*   Updated: 2024/03/12 19:01:35 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H
# include "Libft/libft.h"
# include "Libft/vec.h"
# include "Libft/ft_printf.h"
# include "Libft/get_next_line_bonus.h"
# include <signal.h>
/*# include <readline/readline.h>
# include <readline/history.h>*/
# include <stdio.h>
# include <fcntl.h>

void	set_signal_action(void);
char	*readline(const char *prompt);
int	parsing(int argc, char **argv, char **env);

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

#endif