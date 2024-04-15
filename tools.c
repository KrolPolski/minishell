/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 09:19:48 by rboudwin          #+#    #+#             */
/*   Updated: 2024/04/15 15:18:46 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
	free(info->username);
	free(info->init_dir);
	free(info->prompt);
	free_2d(info->curr_env);
}

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
			free(info->curr_env[i]);
			lvlstr = ft_itoa(lvl);
			info->curr_env[i] = ft_strjoin("SHLVL=", lvlstr);
			free(lvlstr);
			return ;
		}
		i++;
	}
}
