/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_handling.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/15 10:13:35 by rboudwin          #+#    #+#             */
/*   Updated: 2024/03/15 18:14:24 by akovalev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*expand_env_var(char *var, char **env)
{
	int	i;

	i = 0;

	while (env[i])
	{
		if (ft_strnstr(env[i], var + 1, ft_strlen(var) -1)) //offsets are to account for $
			return(env[i] + ft_strlen(var)); //to get past VAR=
		i++;
	}
	return (NULL);
}

void	speek(char **ps, char *es, char *tokens)
{
	char	*s;
	char	whitespace[] = " \t\r\n\v";

	s = *ps;
	while (s < es && ft_strchr(whitespace, *s))
		s++;
	while (s < es && !ft_strchr(tokens, *s))
		s++;
	*ps = s;
}

void	replace_name(char *ptr, char *var, char *exp_var, char **str)
{
	size_t	old_vlen;
	size_t	new_vlen;
	size_t	old_strlen;
	size_t	new_strlen;

	old_strlen = ft_strlen(ptr);
	old_vlen = ft_strlen(var);
	new_vlen = ft_strlen(exp_var);
	ft_memmove(ptr + new_vlen, ptr + old_vlen, old_strlen - old_vlen + 1);
	ft_memcpy(ptr, exp_var, new_vlen);
	*str = ptr + new_vlen;
}

char	*expand_string(char **str, char **env)
{
	char	*ptr;
	char	*end_str;
	char	*var;
	char	*exp_var;
	char	whitespace[] = " \t\r\n\v";

	end_str = *str + ft_strlen(*str);
	if (*str == end_str)
		return (NULL);
	speek(str, end_str, "$");
	//printf("String: %s\n", *str);
	if (ft_strlen(*str) > 0)
	{
		while (ft_strlen(*str) > 0)
		{
			ptr = *str;
			while (*str < end_str && !ft_strchr(whitespace, **str))
				(*str)++;
			var = malloc(*str - ptr + 1);
			ft_strlcpy(var, ptr, *str - ptr + 1);
			//printf("Found variable: %s\n", var);
			exp_var = expand_env_var(var, env);
			replace_name(ptr, var, exp_var, str);
			//printf("Variable: %s\n", var);
			//printf("Expanded variable: %s\n", exp_var);
			//printf("String now is at: %s\n", *str);
			//printf("len is %d\n", (int)ft_strlen(*str));
			if (ft_strlen(*str) > 0)
			{
				expand_string(str, env);
				while (*str && !ft_strchr(whitespace, **str))
					(*str)++;
			}
		}
	}
	else
	{
		printf("did not find any dollaridoos\n");
	}
	return (NULL);
}
