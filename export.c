/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 11:30:34 by rboudwin          #+#    #+#             */
/*   Updated: 2024/04/15 16:52:07 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	check_matrix_no_needle(t_execcmd *ecmd, char **new_env, int i, int k)
{
	while (new_env[i] && ft_strncmp(new_env[i],
			ecmd->argv[k], ft_strlen(ecmd->argv[k])))
		i++;
	return (i);
}

/* Returns the position within the matrix of existing 
environment variables, or a spot at the end if it doesn't yet
exist*/
int	check_matrix(t_execcmd *ecmd, int k, char **new_env)
{
	int		i;
	char	*needle;
	char	*alt_needle;

	i = 0;
	needle = var_to_equals(ecmd, k, i);
	if (!needle)
		return (check_matrix_no_needle(ecmd, new_env, i, k));
	alt_needle = ft_strdup(needle);
	alt_needle[ft_strlen(needle) - 1] = '\0';
	while (new_env[i] && ft_strncmp(new_env[i], needle, ft_strlen(needle)))
	{
		if (!ft_strncmp(new_env[i], needle, ft_strlen(needle) - 1)
			&& new_env[i][ft_strlen(needle) - 1] == '\0')
		{
			free_and_null(needle);
			free_and_null(alt_needle);
			return (i);
		}
		i++;
	}
	free_and_null(needle);
	free_and_null(alt_needle);
	return (i);
}
/* Initializes various values in the struct to ensure we have
sufficient memory for the array copy, then does a shallow copy
from curr_env to new_env */
void	init_export(t_execcmd *ecmd, t_info *info, t_export *ex)
{
	ex->k = 1;
	ex->curr_len = ft_matrix_len(info->curr_env);
	ex->target_len = ex->curr_len + ft_matrix_len(ecmd->argv);
	ex->new_env = ft_calloc(sizeof(char *), ex->target_len + 1);
	if (!ex->new_env)
	{
		ft_printf("malloc failure\n");
		exit(1);
	}
	ex->i = 0;
	while (info->curr_env[ex->i])
	{
		ex->new_env[ex->i] = info->curr_env[ex->i];
		ex->i++;
	}
}
/* Ensures arguments are in appropriate format and finds the correct 
position inside the array to place the new value */
void	ft_export_handler(t_execcmd *ecmd, t_export *ex)
{
	while (ecmd->argv[ex->k])
	{
		if (!export_validator(ecmd->argv[ex->k]))
		{
			ft_putstr_fd("export: ", 2);
			ft_putstr_fd(ecmd->argv[ex->k], 2);
			ft_putstr_fd(": not a a valid identifier\n", 2);
			ex->k++;
			continue ;
		}
		ex->i = check_matrix(ecmd, ex->k, ex->new_env);
		if (ft_strchr(ecmd->argv[ex->k], '=') || !ex->new_env[ex->i])
		{
			if (ex->new_env[ex->i])
				free_and_null(ex->new_env[ex->i]);
			ex->new_env[ex->i] = ft_strdup(ecmd->argv[ex->k]);
		}
		ex->i++;
		ex->k++;
	}
}
/* Built-in export command */
void	ft_export(t_execcmd *ecmd, t_info *info)
{
	t_export	ex;

	if (ecmd->argv[1] == NULL)
	{
		export_empty(info);
		return ;
	}
	init_export(ecmd, info, &ex);
	ft_export_handler(ecmd, &ex);
	ex.new_env[ex.target_len] = NULL;
	free(info->curr_env);
	info->curr_env = ex.new_env;
}
