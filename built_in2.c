/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_in2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 11:30:34 by rboudwin          #+#    #+#             */
/*   Updated: 2024/03/19 16:46:45 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int		check_valid_env_var(t_execcmd *ecmd)
{
	//how to check for valid var name? no spaces, = present somewhere, no spaces after.
	//looks like bash actually ignores trailing whitespace.
	if (ft_strchr(ecmd->argv[1], '='))
	{
		return (1);
	}
	return (0);
}
void	ft_export(t_execcmd *ecmd, t_info *info)
{
	//on invalid input, we should just do nothing and return to the prompt. no errors are given
	//by bash in this circumstance.
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

void	ft_unset(t_execcmd *ecmd, t_info *info)
{
	char	**new_env;
	int		curr_len;
	int		i;

	//psuedo code
	//Check to see if ecmd is in the appropriate format (VAR=value) - make it a function we can use in both export and here
	//check to see if it exists in curr_env
	//if so, remove it and remalloc as required.
	//otherwise do nothing.
	curr_len = ft_matrix_len(info->curr_env);
	ft_printf("current length of curr_env is %d\n", curr_len);
	i = 0;
	while (info->curr_env[i])
	{
		if (ft_strncmp)
		i++;
	}
	new_env = malloc(sizeof(char *) * (curr_len)); // don't need a spot for null termination anymore because we are removing something
	new_env[i] = ecmd->argv[1];
	ft_printf("new_env[i] should now have a new value: %s\n", new_env[i]);
	i++;
	new_env[i] = NULL;
	free(info->curr_env);
	info->curr_env = new_env;
}