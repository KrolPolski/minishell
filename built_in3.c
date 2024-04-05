/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_in3.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 15:54:15 by rboudwin          #+#    #+#             */
/*   Updated: 2024/04/05 10:46:41 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*search_matrix(char *arg, char **matrix, int *i, int curr_len)
{
	char	*arg_plus;

	*i = 0;
	arg_plus = ft_strjoin(arg, "=");
	while (*i < curr_len)
	{
		if (!matrix[*i])
			(*i)++;
		else if (!ft_strncmp(matrix[*i], arg_plus, ft_strlen(arg_plus))
			|| !ft_strncmp(matrix[*i], arg, ft_strlen(arg) + 1))
		{
			free(arg_plus);
			arg_plus = NULL;
			return (matrix[*i]);
		}
		else
			(*i)++;
	}
	free(arg_plus);
	arg_plus = NULL;
	return (NULL);
}
void	copy_unset(t_execcmd *ecmd, t_info *info, t_unset *un)
{
	un->new_env = ft_calloc(sizeof(char *), (un->curr_len + 1));
	if (!un->new_env)
		exit(1);
	un->i = 0;
	un->k = 0;
	while (un->k < un->curr_len)
	{
		if (info->curr_env[un->k])
		{
			un->new_env[un->i] = info->curr_env[un->k];
			un->i++;
		}
		un->k++;
	}
	un->new_env[un->i] = NULL;
	free(info->curr_env);
	info->curr_env = NULL;
	info->curr_env = un->new_env;
}

void	ft_unset(t_execcmd *ecmd, t_info *info)
{
	t_unset	un;

	un.k = 1;
	un.curr_len = ft_matrix_len(info->curr_env);
	while (ecmd->argv[un.k])
	{
		un.str = search_matrix(ecmd->argv[un.k],
				info->curr_env, &un.i, un.curr_len);
		if (!un.str)
		{
			un.k++;
		}
		else
		{
			free(info->curr_env[un.i]);
			info->curr_env[un.i] = NULL;
			un.i = 0;
			un.k++;
		}
	}
	copy_unset(ecmd, info, &un);
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
