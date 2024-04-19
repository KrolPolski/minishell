/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools1.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 17:48:28 by akovalev          #+#    #+#             */
/*   Updated: 2024/04/19 17:55:04 by akovalev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*a function that initializes the starting values in the line info struct*/
void	init_line_info(t_line_info *li, char **str)
{
	li->sfl = 0;
	li->dfl = 0;
	li->hdfl = 0;
	li->endsq = NULL;
	li->enddq = NULL;
	li->begsq = NULL;
	li->begdq = NULL;
	li->beg_var = NULL;
	li->beg_str = *str;
	li->beg_str_first_time = TRUE;
	li->end_str = *str + ft_strlen(*str);
	li->free_flag = 0;
	li->flag_changed = 0;
	li->in_quotes = 0;
}

/*a function that removes the quotes by overwriting them with memmove*/
void	remove_quotes(char *begq, char *endq)
{
	ft_memmove(begq, begq + 1, ft_strlen(begq));
	ft_memmove(endq - 1, endq, ft_strlen(endq - 1));
}

/*a function that fetches the env variable from the env array or
the exit code for the special ? case*/
char	*fetch_env_var(char *var, char **env, t_line_info *li)
{
	int		i;
	char	*eq_ptr;

	if (!ft_strncmp(var, "$?", 3))
	{
		li->info->exit_code_ptr = ft_itoa(li->info->exit_code);
		li->info->ecp_flag = 1;
		return (li->info->exit_code_ptr);
	}
	i = 0;
	while (env[i])
	{
		eq_ptr = ft_strchr(env[i], '=');
		if ((long)ft_strlen(var) - 1 == eq_ptr - env[i])
			if (ft_strnstr(env[i], var + 1, ft_strlen(var) - 1))
				return (env[i] + ft_strlen(var));
		i++;
	}
	return (NULL);
}
