/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_in3.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 15:54:15 by rboudwin          #+#    #+#             */
/*   Updated: 2024/04/03 15:55:32 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*search_matrix(char *arg, char **matrix, int *i, int curr_len)
{
	char	*arg_plus;

	*i = 0;
	arg_plus = ft_strjoin(arg, "=");
	//ft_printf("about to search for '%s' inside '%s'\n", arg_plus, matrix[*i]);
	while (*i < curr_len)
	{
		//ft_printf("about to search for '%s' inside '%s'\n", arg_plus, matrix[*i]);
		if (!matrix[*i])
			(*i)++;
		else if (!ft_strncmp(matrix[*i], arg_plus, ft_strlen(arg_plus))
			|| !ft_strncmp(matrix[*i], arg, ft_strlen(arg) + 1))
		{
			//ft_printf("we found it baby: %s\n", matrix[*i]);
			free(arg_plus);
			arg_plus = NULL;
			return (matrix[*i]);
		}
		else
			(*i)++;
	}
	free(arg_plus);
	arg_plus = NULL;
	//ft_printf("about to return NULL");
	return (NULL);
}

void	ft_unset(t_execcmd *ecmd, t_info *info)
{
	int		k;
	char	*str;
	int		i;
	char	*targets;
	char	curr_len;
	char	**new_env;
	int		a;
	int		b;

	k = 1;
	curr_len = ft_matrix_len(info->curr_env);
	while (ecmd->argv[k])
	{
		//ft_printf("about to search the matrix for an unset\n");
		str = search_matrix(ecmd->argv[k], info->curr_env, &i, curr_len);
		//ft_printf("we exited search_matrix\n");
		// if we find a bad one we just wanna move on
		if (!str)
		{
			///ft_printf("we concluded that %s isn't actually in the matrix so moving on\n", ecmd->argv[k]);
			k++;
		}
		else
		{
			//ft_printf("we found %s in the matrix so we are freeing it\n", ecmd->argv[k]);
			//ft_printf("str is '%s' and info->curr_env[%d] is currently '%s'\n", str, i, info->curr_env[i]);
			free(info->curr_env[i]);
			info->curr_env[i] = NULL;
			i = 0;
			k++;
		}
	}
	new_env = ft_calloc(sizeof(char *), (curr_len + 1));
	if (!new_env)
		exit(1);
	a = 0;
	b = 0;
	while (b < curr_len)
	{
		if (info->curr_env[b])
		{
			new_env[a] = info->curr_env[b];
			a++;
		}
		b++;
	}
	new_env[a] = NULL;
	free(info->curr_env);
	info->curr_env = NULL;
	info->curr_env = new_env;
}