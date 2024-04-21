/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 09:19:48 by rboudwin          #+#    #+#             */
/*   Updated: 2024/04/21 15:32:25 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* frees all data for an array of strings */
void	free_2d(char **arr)
{
	int	i;

	i = 0;
	while (arr[i])
	{
		free(arr[i]);
		arr[i] = NULL;
		i++;
	}
	free(arr);
	arr = NULL;
}

/* returns the number of items in a matrix */
int	ft_matrix_len(char **str)
{
	int	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

void	final_cleanup(t_info *info)
{
	write_history(".shell_history");
	free_and_null(info->username);
	free_and_null(info->init_dir);
	free_and_null(info->prompt);
	free_2d(info->curr_env);
}

/* increments the SHLVL environment variable by one each time
minishell is launched */
void	set_shell_level(t_info *info)
{
	int		i;
	int		lvl;
	char	*lvlstr;

	i = 0;
	while (info->curr_env[i])
	{
		if (ft_strnstr(info->curr_env[i], "SHLVL=", 6))
		{
			lvl = ft_atoi(info->curr_env[i] + 6);
			lvl++;
			free_and_null(info->curr_env[i]);
			lvlstr = ft_itoa(lvl);
			check_malloc_failure(lvlstr);
			info->curr_env[i] = ft_strjoin("SHLVL=", lvlstr);
			free_and_null(lvlstr);
			return ;
		}
		i++;
	}
}

void	panic(char *s)
{
	ft_putstr_fd(s, 2);
	ft_putchar_fd('\n', 2);
	exit(1);
}
