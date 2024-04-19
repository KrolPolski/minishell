/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_and_quote_handler.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 16:49:02 by akovalev          #+#    #+#             */
/*   Updated: 2024/04/19 16:20:56 by akovalev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*fetch_env_var(char *var, char **env, t_line_info *li)
{
	int		i;
	char	*eq_ptr;

	if (!ft_strncmp(var, "$?", 3))
	{
		li->info->exit_code_ptr = ft_itoa(li->info->exit_code);
		li->info->ecp_flag = 1;
		return (li->info->exit_code_ptr);
	}
	i = 0;
	while (env[i])
	{
		eq_ptr = ft_strchr(env[i], '=');
		if ((long)ft_strlen(var) - 1 == eq_ptr - env[i])
			if (ft_strnstr(env[i], var + 1, ft_strlen(var) - 1))
				return (env[i] + ft_strlen(var));
		i++;
	}
	return (NULL);
}

char	*replace_name(t_line_info *li, char *var, char *exp_var, char **str)
{
	size_t	old_vlen;
	size_t	new_vlen;
	size_t	old_strlen;
	size_t	new_strlen;
	char	*upd_str;

	if (exp_var == NULL)
		exp_var = ft_strdup("");
	old_strlen = ft_strlen(li->beg_str);
	old_vlen = ft_strlen(var);
	new_vlen = ft_strlen(exp_var);
	new_strlen = old_strlen + new_vlen - old_vlen;
	upd_str = ft_calloc(new_strlen + 1, 1);
	if (upd_str == NULL)
		return (NULL);
	ft_memcpy(upd_str, li->beg_str, li->beg_var - li->beg_str);
	ft_memcpy(upd_str + (li->beg_var - li->beg_str), exp_var, new_vlen);
	ft_memcpy(upd_str + (li->beg_var - li->beg_str) + new_vlen, li->beg_var + \
		old_vlen, old_strlen - (li->beg_var - li->beg_str) - old_vlen + 1);
	*str = upd_str + (li->beg_var - li->beg_str) + new_vlen - 1;
	if (li->free_flag == 1)
		free_and_null(exp_var);
	li->free_flag = 0;
	return (upd_str);
}

char	*expand_env(t_line_info *li, char *str, char **env)
{
	char	*var;
	char	*exp_var;
	int		diff;
	char	*ptr_parking;

	ptr_parking = NULL;

	diff = li->begdq - li->beg_str;
	li->end_str = li->beg_str + ft_strlen(li->beg_str);
	li->beg_var = str;
	if (*(str + 1) == ' ' || !*(str + 1))
		return (str);
	if (*(str + 1) == '?' || (*(str + 1) == '_'))
	{
		str++;
		str++;
	}
	else
	{
		while (str++ < li->end_str && !ft_strchr(" \t\r\n\v\'\"|<>", *str))
			if (*str == '$')
				break ;
	}
	var = ft_calloc(str - li->beg_var + 1, 1);
	ft_strlcpy(var, li->beg_var, str - li->beg_var + 1);
	exp_var = fetch_env_var(var, env, li);
	if (exp_var == NULL)
		li->free_flag = 1;
	if (li->beg_str_first_time == FALSE)
		ptr_parking = li->beg_str;
	li->beg_str = replace_name(li, var, exp_var, &str);
	li->beg_str_first_time = FALSE;
	if (ptr_parking)
		free_and_null(ptr_parking);
	if (li->begdq && li->enddq)
	{
		li->begdq = li->beg_str + diff;
		li->enddq = ft_strchr(li->begdq + 1, '\"');
	}
	free_and_null(var);
	if (li->info->ecp_flag == 1)
	{
		free (li->info->exit_code_ptr);
		li->info->ecp_flag = 0;
	}
	return (str);
}

void	remove_quotes(char *begq, char *endq)
{
	ft_memmove(begq, begq + 1, ft_strlen(begq));
	ft_memmove(endq - 1, endq, ft_strlen(endq - 1));
}

void	quote_handler(t_line_info *li, char **str, char **env)
{
	if (**str == '\'' && li->dfl != 1 && li->sfl == 0)
	{
		li->endsq = ft_strchr(*str + 1, '\'');
		if (li->endsq)
		{
			li->begsq = *str;
			li->sfl = 1;
		}
	}
	else if (**str == '\"' && li->sfl != 1 && li->dfl == 0)
	{
		li->enddq = ft_strchr(*str + 1, '\"');
		if (li->enddq)
		{
			li->begdq = *str;
			li->dfl = 1;
		}
	}
	else if (**str == '$' && li->sfl != 1)
	{
		*str = expand_env(li, *str, env);
	}
}

void	init_line_info(t_line_info *li, char **str)
{
	li->sfl = 0;
	li->dfl = 0;
	li->hdfl = 0;
	li->endsq = NULL;
	li->enddq = NULL;
	li->begsq = NULL;
	li->begdq = NULL;
	li->beg_var = NULL;
	li->beg_str = *str;
	li->beg_str_first_time = TRUE;
	li->end_str = *str + ft_strlen(*str);
	li->free_flag = 0;
	li->flag_changed = 0;
	li->in_quotes = 0;
}

char	*expand_main(char *str, char **env, t_line_info *li)
{
	init_line_info(li, &str);
	while (*str)
	{
		quote_handler(li, &str, env);
		if (li->endsq != NULL && (str == li->endsq))
		{
			li->sfl = 0;
			li->endsq = NULL;
		}
		if (li->enddq != NULL && (str == li->enddq))
		{
			li->dfl = 0;
			li->enddq = NULL;
		}
		str++;
	}
	return (li->beg_str);
}
