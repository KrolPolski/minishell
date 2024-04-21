/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_and_quote_handler.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 16:49:02 by akovalev          #+#    #+#             */
/*   Updated: 2024/04/21 15:02:38 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*a function that uses memcpy to stich a new string replacing the string being
parsed by removing the variable name and adding the expanded variable in 
its place */
char	*replace_name(t_line_info *li, char *var, char *exp_var, char **str)
{
	size_t	old_vlen;
	size_t	new_vlen;
	size_t	old_strlen;
	size_t	new_strlen;
	char	*upd_str;

	if (exp_var == NULL)
		exp_var = ft_strdup("");
	check_malloc_failure(exp_var);
	old_strlen = ft_strlen(li->beg_str);
	old_vlen = ft_strlen(var);
	new_vlen = ft_strlen(exp_var);
	new_strlen = old_strlen + new_vlen - old_vlen;
	upd_str = ft_calloc(new_strlen + 1, 1);
	check_malloc_failure(upd_str);
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

/*a function that handles the fetching of the env variable from the array and
putting it into the string if found, or, replacing it with empty string*/
void	fetch_and_replace(t_line_info *li, char **str, char **env, int diff)
{
	char	*var;
	char	*exp_var;
	char	*ptr_parking;

	ptr_parking = NULL;
	var = ft_calloc(*str - li->beg_var + 1, 1);
	check_malloc_failure(var);
	ft_strlcpy(var, li->beg_var, *str - li->beg_var + 1);
	exp_var = fetch_env_var(var, env, li);
	if (exp_var == NULL)
		li->free_flag = 1;
	if (li->beg_str_first_time == FALSE)
		ptr_parking = li->beg_str;
	li->beg_str = replace_name(li, var, exp_var, str);
	li->beg_str_first_time = FALSE;
	if (ptr_parking)
		free_and_null(ptr_parking);
	if (li->begdq && li->enddq)
	{
		li->begdq = li->beg_str + diff;
		li->enddq = ft_strchr(li->begdq + 1, '\"');
	}
	free_and_null(var);
}

/*a function that determines the variable to be replaced and handles
the special ? case, returning position in the string after the replaced
variable*/
char	*expand_env(t_line_info *li, char *str, char **env)
{
	int		diff;

	diff = li->begdq - li->beg_str;
	li->end_str = li->beg_str + ft_strlen(li->beg_str);
	li->beg_var = str;
	if (*(str + 1) == ' ' || !*(str + 1))
		return (str);
	if (*(str + 1) == '?')
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
	fetch_and_replace(li, &str, env, diff);
	if (li->info->ecp_flag == 1)
	{
		free (li->info->exit_code_ptr);
		li->info->ecp_flag = 0;
	}
	return (str);
}

/*a function that looks for quotes in the line and sets the quote flags,
calling the expand_env function when appropriate*/
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

/*main expander function that goes through the line received from readline
and expands the environmental variables if these are found*/
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
