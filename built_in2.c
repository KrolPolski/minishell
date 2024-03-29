/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_in2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 11:30:34 by rboudwin          #+#    #+#             */
/*   Updated: 2024/03/29 15:22:24 by rboudwin         ###   ########.fr       */
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
		if (ft_strchr(ecmd->argv[k], '=') || !info->curr_env[i])
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

char	*search_matrix(char *arg, char **matrix, int *i, int curr_len)
{
	char *arg_plus;
	*i = 0;

	
	arg_plus = ft_strjoin(arg, "=");
	ft_printf("about to search for '%s' inside '%s'\n", arg_plus, matrix[*i]);
	while (*i < curr_len)
	{
		ft_printf("about to search for '%s' inside '%s'\n", arg_plus, matrix[*i]);
		if (!matrix[*i])
			(*i)++;
		else if (!ft_strncmp(matrix[*i], arg_plus, ft_strlen(arg_plus)))
			{
				ft_printf("we found it baby\n");
				free(arg_plus);
				return (matrix[*i]);
			}
		else
			(*i)++;
	}
	free(arg_plus);
	ft_printf("about to return NULL");
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
		ft_printf("about to search the matrix for an unset\n");
		str = search_matrix(ecmd->argv[k], info->curr_env, &i, curr_len);
		ft_printf("we exited search_matrix\n");
		// if we find a bad one we just wanna move on
		if (!str)
		{
			ft_printf("we concluded that %s isn't actually in the matrix so moving on\n", ecmd->argv[k]);
			k++;
		}
		else
		{
			ft_printf("we found %s in the matrix so we are freeing it\n", ecmd->argv[k]);
			free(info->curr_env[i]);
			info->curr_env[i] = NULL;
			i = 0;
			k++;
		}
	}
	new_env = malloc(sizeof(char *) * curr_len + 1);
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
	new_env[++a] = NULL;
	free(info->curr_env);
	info->curr_env = new_env;
}