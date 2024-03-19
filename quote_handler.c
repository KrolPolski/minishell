/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote_handler.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 16:49:02 by akovalev          #+#    #+#             */
/*   Updated: 2024/03/19 18:55:22 by akovalev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*fetch_env_var(char *var, char **env)
{
	int		i;
	char	*eq_ptr;

	i = 0;

	while (env[i])
	{
		eq_ptr = ft_strchr(env[i], '=');
		if (ft_strlen(var) - 1 == eq_ptr - env[i])
			if (ft_strnstr(env[i], var + 1, ft_strlen(var) - 1)) //offsets are to account for $
				return (env[i] + ft_strlen(var));//to get past VAR=
		i++;
	}
	return (NULL);
}

char	*replace_name(char *ptr, char *var, char *exp_var, char *beg_str, char **str)
{
	size_t	old_vlen;
	size_t	new_vlen;
	size_t	old_strlen;
	size_t	new_strlen;
	char	*upd_str;

	old_strlen = ft_strlen(beg_str);
	old_vlen = ft_strlen(var);
	new_vlen = ft_strlen(exp_var);
	new_strlen = old_strlen + new_vlen - old_vlen;
	upd_str = malloc(new_strlen + 1);
	if (upd_str == NULL)
		return (NULL);
	memcpy(upd_str, beg_str, ptr - beg_str);
	memcpy(upd_str + (ptr - beg_str), exp_var, new_vlen);
	memcpy(upd_str + (ptr - beg_str) + new_vlen, ptr + old_vlen, \
		old_strlen - (ptr - beg_str) - old_vlen + 1);
	*str = upd_str + (ptr - beg_str) + new_vlen - 1;
	//printf("str is now at: %s\n", *str);
	free(beg_str);
	return (upd_str);
}

char	*expand_env(char **beg_str, char *str, char **env)
{
	char	*end_str;
	char	*ptr;
	char	*var;
	char	*exp_var;
	char	whitespace[] = " \t\r\n\v\'\"";

	end_str = *beg_str + ft_strlen(*beg_str);
	ptr = str;
	while (str < end_str && !ft_strchr(whitespace, *str))
		(str)++;
	var = malloc(str - ptr + 1);
	ft_strlcpy(var, ptr, str - ptr + 1);
	exp_var = fetch_env_var(var, env);
	if (exp_var != NULL)
		*beg_str = replace_name(ptr, var, exp_var, *beg_str, &str);
	free(var);
	return (str);
}

char	*quote_handler(char *str, char **env)
{
	bool	sfl;
	bool	dfl;
	char	*endq;
	char	*beg_str;

	beg_str = str;
	sfl = 0;
	dfl = 0;

	while (*str)
	{
		if (*str == '\'' && dfl != 1)
		{
			if (sfl == 0 && ft_strchr(str + 1, '\''))
			{
				sfl = 1;
				//printf("Opening single\n");
			}
			else if (sfl == 1)
			{
				sfl = 0;
				//printf("Closing single\n");
			}
		}
		else if (*str == '\"' && sfl != 1)
		{
			endq = ft_strchr(str + 1, '\"');
			if (dfl == 0 && endq)
			{
				dfl = 1;
				//printf("Opening double\n");
			}
			else if (dfl == 1)
			{
				dfl = 0;
				//printf("Closing double\n");
			}
		}
		else if (*str == '$' && sfl != 1)
		{
			str = expand_env(&beg_str, str, env);
		}
		str++;
	}
	return (beg_str);
}

// int main(int argc, char **argv, char **env)
// {
// 	char *str;

// 	while ((str = get_next_line(1)) != NULL)
// 		quote_handler(str, env);
// 	return (0);
// }
