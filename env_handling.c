/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_handling.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/15 10:13:35 by rboudwin          #+#    #+#             */
/*   Updated: 2024/04/05 10:37:54 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// char	*expand_env_var(char *var, char **env)
// {
// 	int		i;
// 	char	*eq_ptr;

// 	i = 0;

// 	while (env[i])
// 	{
// 		eq_ptr = ft_strchr(env[i], '=');
// 		if (ft_strlen(var) - 1 == eq_ptr - env[i])
// 			if (ft_strnstr(env[i], var + 1, ft_strlen(var) - 1)) 
				//offsets are to account for $
// 				return (env[i] + ft_strlen(var));//to get past VAR=
// 		i++;
// 	}
// 	return (NULL);
// }

// void	speek(char **ps, char *es, char *tokens)
// {
// 	char	*s;
// 	char	whitespace[] = " \t\r\n\v";

// 	s = *ps;
// 	while (s < es && ft_strchr(whitespace, *s))
// 		s++;
// 	while (s < es && !ft_strchr(tokens, *s))
// 		s++;
// 	*ps = s;
// }

// char	*replace_name(char *ptr, char *var, char *exp_var, char *beg_str)
// {
// 	size_t	old_vlen;
// 	size_t	new_vlen;
// 	size_t	old_strlen;
// 	size_t	new_strlen;
// 	char	*upd_str;

// 	old_strlen = ft_strlen(beg_str);
// 	old_vlen = ft_strlen(var);
// 	new_vlen = ft_strlen(exp_var);
// 	new_strlen = old_strlen + new_vlen - old_vlen;
// 	upd_str = malloc(new_strlen + 1);
// 	if (upd_str == NULL)
// 		return (NULL);
// 	memcpy(upd_str, beg_str, ptr - beg_str);
// 	memcpy(upd_str + (ptr - beg_str), exp_var, new_vlen);
// 	memcpy(upd_str + (ptr - beg_str) + new_vlen, ptr + old_vlen, \
// 		old_strlen - (ptr - beg_str) - old_vlen + 1);
// 	free(beg_str);
// 	return (upd_str);
// }

// char	*expand_string(char *str, char **env)
// {
// 	char	*ptr;
// 	char	*end_str;
// 	char	*var;
// 	char	*exp_var;
// 	char	whitespace[] = " \t\r\n\v";
// 	char	*beg_str;

// 	beg_str = str;
// 	end_str = str + ft_strlen(str);
// 	var = NULL;
// 	if (str == end_str)
// 		return (NULL);
// 	speek(&str, end_str, "$");
// 	if (ft_strlen(str) > 0)
// 	{
// 		ptr = str;
// 		while (str < end_str && !ft_strchr(whitespace, *str))
// 			(str)++;
// 		var = malloc(str - ptr + 1);
// 		ft_strlcpy(var, ptr, str - ptr + 1);
// 		exp_var = expand_env_var(var, env);
// 		if (exp_var != NULL)
// 			beg_str = replace_name(ptr, var, exp_var, beg_str);
// 		else
// 		{
// 			exp_var = ft_strdup("");
// 			beg_str = replace_name(ptr, var, exp_var, beg_str);
// 			free(exp_var);
// 		}
// 		free(var);
// 		beg_str = expand_string(beg_str, env);
// 		 // this free was moved because var only gets 
//		allocated if we go inside this if block
// 	}
// 	// else
// 	// {
// 	// 	printf("did not find any dollaridoos\n");
// 	// }
// 	return (beg_str);
// }
