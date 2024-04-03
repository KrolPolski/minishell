/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_in2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 11:30:34 by rboudwin          #+#    #+#             */
/*   Updated: 2024/04/03 17:17:40 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*var_to_equals(t_execcmd *ecmd, int k)
{
	int		i;
	char	*needle;
	char	*equal_pos;

	i = 0;
	equal_pos = ft_strchr(ecmd->argv[k], '=');
	if (!equal_pos)
		return (NULL);
	else
	{
		needle = ft_calloc(equal_pos - ecmd->argv[k] + 2, 1); //this needed plus 2 because it might need an equals sign and it needs null termination
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
	needle = var_to_equals(ecmd, k);
	
	//needs to address the case where an env variable
	// already exists, but has no value
	//and we want to set a value now.
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
			&& new_env[i][ft_strlen(needle)-1] == '\0')
		{
			return (i);
		}
		i++;
	}
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

void	ft_export(t_execcmd *ecmd, t_info *info)
{
	char	**new_env;
	int		curr_len;
	int		i;
	int		k;
	int		target_len;

	if (ecmd->argv[1] == NULL)
	{
		export_empty(info);
		return ;
	}
	k = 1;
	curr_len = ft_matrix_len(info->curr_env);
	//we need to somehow make this dependent 
	//on whether it actually exists already or not
	target_len = curr_len + ft_matrix_len(ecmd->argv);
	new_env = ft_calloc(sizeof(char *), target_len + 1);
	if (!new_env)
	{
		ft_printf("malloc failure\n");
		exit(1);
	}
	i = 0;
	while (info->curr_env[i])
	{
		new_env[i] = info->curr_env[i];
		i++;
	}
	while (ecmd->argv[k])
	{
		i = check_matrix(ecmd, info, k, new_env);
		if (ft_strchr(ecmd->argv[k], '=') || !new_env[i])
		{
			new_env[i] = ft_strdup(ecmd->argv[k]);
		}
		i++;
		k++;
	}
	new_env[target_len] = NULL;
	free(info->curr_env);
	info->curr_env = new_env;
}

/* if exit code is provided as an argument, exits the shell with it.
Otherwise exits with the last exit code provided by $? */
void	ft_exit(t_execcmd *ecmd, t_info *info)
{
	char	*last_exit_code;
	int		exit_code;
	int		i;

	i = 0;
	//consider what happens if a non-int value is provided
	if (ecmd->argv[1])
		exit_code = ft_atoi(ecmd->argv[1]);
	else
	{
		exit_code = info->exit_code;
	}
	free_2d(info->curr_env);
	ft_printf("exit\n");
	//consider freeing other stuff if required
	exit(exit_code);
}

