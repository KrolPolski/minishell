/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_in2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 11:30:34 by rboudwin          #+#    #+#             */
/*   Updated: 2024/03/29 12:40:38 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*var_to_equals(t_execcmd *ecmd, int k)
{
	int i;
	char *needle;
	char *equal_pos;
	i = 0;
	equal_pos = ft_strchr(ecmd->argv[k], '=');
	if (!equal_pos)
		return (NULL);
	else
	{
		needle = malloc(equal_pos - ecmd->argv[k] + 1);
		if (!needle)
			return (NULL);
		while (ecmd->argv[k][i] && ecmd->argv[k][i] != '=')
		{
			needle[i] = ecmd->argv[k][i];
			i++;
		}
		needle[i] = ecmd->argv[k][i];
		i++;
		needle[i] = '\0';
		ft_printf("we have decided that since ecmd->argv[k] is '%s' the needle should be '%s'\n", ecmd->argv[k], needle);
		return (needle);
	}
}

int	check_matrix(t_execcmd *ecmd, t_info *info, int k, char **new_env)
{
	int		i;
	char	*needle;

	i = 0;
	needle = var_to_equals(ecmd, k);
	if (!needle)
	{
		while (new_env[i] && ft_strncmp(new_env[i], ecmd->argv[k], ft_strlen(ecmd->argv[k])))
			i++;
		return (i);
	}
	while (new_env[i] && ft_strncmp(new_env[i], needle, ft_strlen(needle)))
	{
		i++;
	}
	ft_printf("Check matrix decided i should be %d\n", i);
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
	ft_printf("current length of curr_env is %d\n", curr_len);
	target_len = curr_len + ft_matrix_len(&ecmd->argv[k]);
	new_env = malloc(sizeof(char *) * (target_len + 1)); //this might be mallocing more than we need.
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
		new_env[i] = ft_strdup(ecmd->argv[k]);
		ft_printf("new_env[i] should now have a new value: %s\n", new_env[i]);
		i++;
		k++;
	}
	//need to put a null at the end, but i may not be at the end right now.
		new_env[target_len] = NULL;
		free(info->curr_env);
		info->curr_env = new_env;
	//ft_printf("ecmd->argv[0] is %s and ecmd->argv[1] is %s\n", ecmd->argv[0], ecmd->argv[1]);
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
	//consider freeing other stuff if required
	exit(exit_code);
}

void	ft_unset(t_execcmd *ecmd, t_info *info)
{
	char	**new_env;
	int		curr_len;
	int		i;
	int		k;

	curr_len = ft_matrix_len(info->curr_env);
	ft_printf("current length of curr_env is %d\n", curr_len);
	new_env = malloc(sizeof(char *) * (curr_len));
	i = 0;
	while (info->curr_env[i] && ft_strncmp(info->curr_env[i], ecmd->argv[1], ft_strlen(ecmd->argv[1])))
	{
		new_env[i] = info->curr_env[i];
		i++;
	}
	if (info->curr_env[i])
	{
		free(info->curr_env[i]); //why are we getting free errors here? it should be leaking without it
		info->curr_env[i] = NULL;
		k = i;
		i++;
	}
	while (info->curr_env[i])
	{
		new_env[k] = info->curr_env[i];
		k++;
		i++;
	}
	new_env[i] = NULL;
	free(info->curr_env);
	info->curr_env = new_env;
	//need to consider the case where unset is run against something that doesn't actually exist.
}