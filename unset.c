/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 14:39:49 by rboudwin          #+#    #+#             */
/*   Updated: 2024/04/21 15:33:36 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* enforces proper formatting of unset arguments */
int	unset_validator(char *str)
{
	int	i;

	i = 0;
	if (!ft_isalpha(str[0]))
		return (0);
	while (str[i])
	{
		if (str[i] == '=')
			return (0);
		if (!ft_isalnum(str[i]))
			return (0);
		i++;
	}
	return (1);
}

/* searches through the existing matrix for a match, either of the arg 
directly or arg=somevalue to ensure exact matches */
char	*search_matrix(char *arg, char **matrix, int *i, int curr_len)
{
	char	*arg_plus;

	*i = 0;
	arg_plus = ft_strjoin(arg, "=");
	check_malloc_failure(arg_plus);
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

/* copies over all env variables that are still valid after 
unset to the new_env matrix */
void	copy_unset(t_info *info, t_unset *un)
{
	un->new_env = ft_calloc(sizeof(char *), (un->curr_len + 1));
	check_malloc_failure(un->new_env);
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

void	print_unset_error(t_execcmd *ecmd, t_unset *un)
{
	ft_putstr_fd("unset: ", 2);
	ft_putstr_fd(ecmd->argv[un->k], 2);
	ft_putstr_fd(": not a a valid identifier\n", 2);
}

void	ft_unset(t_execcmd *ecmd, t_info *info)
{
	t_unset	un;

	un.k = 1;
	un.curr_len = ft_matrix_len(info->curr_env);
	while (ecmd->argv[un.k])
	{
		if (!unset_validator(ecmd->argv[un.k]))
		{
			print_unset_error(ecmd, &un);
			un.k++;
			continue ;
		}
		un.str = search_matrix(ecmd->argv[un.k],
				info->curr_env, &un.i, un.curr_len);
		if (!un.str)
			un.k++;
		else
		{
			free(info->curr_env[un.i]);
			info->curr_env[un.i] = NULL;
			un.i = 0;
			un.k++;
		}
	}
	copy_unset(info, &un);
}
