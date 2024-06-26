/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 13:18:47 by rboudwin          #+#    #+#             */
/*   Updated: 2024/04/21 15:05:16 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* rejects env variable names with special characters
or that don't start with a letter */
int	export_validator(char *str)
{
	int	i;

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

/* isolates the variable name + equals sign to search the
existing matrix; e.g. export VAR=somevalue becomes VAR=
to ensure an exact match when we search */
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
		check_malloc_failure(needle);
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

/* prints out all existing environment variables
in the same format as bash when export is run with 
no arguments*/
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
