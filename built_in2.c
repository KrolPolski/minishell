/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_in2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 11:30:34 by rboudwin          #+#    #+#             */
/*   Updated: 2024/03/26 11:34:25 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_export(t_execcmd *ecmd, t_info *info)
{
	char	**new_env;
	int		curr_len;
	int		i;

	curr_len = ft_matrix_len(info->curr_env);
	ft_printf("current length of curr_env is %d\n", curr_len);
	new_env = malloc(sizeof(char *) * (curr_len + 2)); // adding one for NULL and one for the new value
	i = 0;
	while (info->curr_env[i])
	{
		new_env[i] = info->curr_env[i];
		i++;
	}
	new_env[i] = ecmd->argv[1];
	ft_printf("new_env[i] should now have a new value: %s\n", new_env[i]);
	i++;
	new_env[i] = NULL;
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
		//free(info->curr_env[i]); why are we getting free errors here? it should be leaking without it
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