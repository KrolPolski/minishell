/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_in2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 11:30:34 by rboudwin          #+#    #+#             */
/*   Updated: 2024/04/15 12:36:35 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*var_to_equals(t_execcmd *ecmd, int k, int i)
{
	char	*needle;
	char	*equal_pos;

	equal_pos = ft_strchr(ecmd->argv[k], '=');
	if (!equal_pos)
		return (NULL);
	else
	{
		needle = ft_calloc(equal_pos - ecmd->argv[k] + 2, 1);
		if (!needle)
			return (NULL);
		while (ecmd->argv[k][i] && ecmd->argv[k][i] != '=')
		{
			needle[i] = ecmd->argv[k][i];
			i++;
		}
		if (ecmd->argv[k][i] == '=')
		{
			needle[i] = ecmd->argv[k][i];
			i++;
		}
		needle[i] = '\0';
		return (needle);
	}
}

int	check_matrix(t_execcmd *ecmd, t_info *info, int k, char **new_env)
{
	int		i;
	char	*needle;
	char	*alt_needle;

	i = 0;
	needle = var_to_equals(ecmd, k, i);
	if (!needle)
	{
		while (new_env[i] && ft_strncmp(new_env[i],
				ecmd->argv[k], ft_strlen(ecmd->argv[k])))
			i++;
		return (i);
	}
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

void	export_empty(t_info *info)
{
	int		i;
	char	**split_env;

	i = 0;
	while (info->curr_env[i])
	{
		split_env = ft_split(info->curr_env[i], '=');
		if (split_env[1])
			ft_printf("declare -x %s=\"%s\"\n", split_env[0], split_env[1]);
		else
			ft_printf("declare -x %s\n", split_env[0]);
		i++;
		free_2d(split_env);
	}
}

void	init_export(t_execcmd *ecmd, t_info *info, t_export *ex)
{
	ex->k = 1;
	ex->curr_len = ft_matrix_len(info->curr_env);
	//we need to somehow make this dependent 
	//on whether it actually exists already or not
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
int		export_validator(char *str)
{
	int i;

	i = 0;
	if (!ft_isalpha(str[0]))
		return (0);
	while (str[i])
	{
		if (str[i] == '=')
			return (1);
		if (!ft_isalnum(str[i]))
			return (0);
		i++;
	}
	return (1);
}

void	ft_export(t_execcmd *ecmd, t_info *info)
{
	t_export	ex;

	if (ecmd->argv[1] == NULL)
	{
		export_empty(info);
		return ;
	}
	init_export(ecmd, info, &ex);
	while (ecmd->argv[ex.k])
	{
		if (!export_validator(ecmd->argv[ex.k]))
		{
			ft_putstr_fd("export: ", 2);
			ft_putstr_fd(ecmd->argv[ex.k], 2);
			ft_putstr_fd(": not a a valid identifier\n", 2);
			ex.k++;
			continue;
		}
		ex.i = check_matrix(ecmd, info, ex.k, ex.new_env);
		if (ft_strchr(ecmd->argv[ex.k], '=') || !ex.new_env[ex.i])
		{
			if (ex.new_env[ex.i])
				free_and_null(ex.new_env[ex.i]);
			ex.new_env[ex.i] = ft_strdup(ecmd->argv[ex.k]);
		}
		ex.i++;
		ex.k++;
	}
	ex.new_env[ex.target_len] = NULL;
	free(info->curr_env);
	info->curr_env = ex.new_env;
}
