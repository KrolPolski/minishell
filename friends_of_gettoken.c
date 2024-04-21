/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   friends_of_gettoken.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akovalev <akovalev@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 19:40:03 by akovalev          #+#    #+#             */
/*   Updated: 2024/04/19 19:48:22 by akovalev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*a fucntion handling quote removal and setting of quote flags back to 0*/
void	handle_quote_removal(char **str, t_line_info *li, bool qflag)
{
	if (qflag == 0)
	{
		remove_quotes(li->begsq, li->endsq);
		li->sfl = 0;
		li->begsq = NULL;
		li->endsq = NULL;
		li->in_quotes = 0;
		(*str)--;
		(*str)--;
	}
	else if (qflag == 1)
	{
		remove_quotes(li->begdq, li->enddq);
		li->dfl = 0;
		li->begdq = NULL;
		li->enddq = NULL;
		li->in_quotes = 0;
		(*str)--;
		(*str)--;
	}
}

/*a function that handles the tokens that are not special characters
and makes sure that the quotes are handled and removed appropriately*/
void	handle_regular_chars(char **str, t_line_info *li)
{
	while (*str < li->end_str && (!(ft_strchr(li->whitespace, **str))) \
		&& ((!(ft_strchr(li->symbols, **str)))))
	{
		check_quotes(str, li);
		if (li->sfl == 1 && li->in_quotes)
		{
			while (*str <= li->endsq)
			{
				if (li->sfl == 1 && *str == li->endsq)
					handle_quote_removal(str, li, 0);
				(*str)++;
			}
		}
		else if (li->dfl == 1 && li->in_quotes)
		{
			while (*str <= li->enddq)
			{
				if (li->dfl == 1 && *str == li->enddq)
					handle_quote_removal(str, li, 1);
				(*str)++;
			}
		}
		else
			(*str)++;
	}
}

/*a function that sets the return value to the type of encountered token
and moves the string pointer to the character past the end of the token*/
void	handle_current_char(char **str, int *ret, t_line_info *li)
{
	if (**str == '|' && (!li->in_quotes))
		(*str)++;
	else if (**str == '>' && (!li->in_quotes))
	{
		(*str)++;
		if (**str == '>' && (!li->in_quotes))
		{
			*ret = '+';
			(*str)++;
		}
	}
	else if (**str == '<' && (!li->in_quotes))
	{
		(*str)++;
		if (**str == '<' && (!li->in_quotes))
		{
			*ret = '-';
			(*str)++;
		}
	}
	else if (**str != 0)
	{
		*ret = 'a';
		handle_regular_chars(str, li);
	}
}

/*a fucntion that sets appropriate flags based on whether
there are single or double quotes to be encountered*/
void	handle_quote_flags(t_line_info *li, bool qflag)
{
	if (qflag == 0)
	{
		li->endsq = ft_strchr(li->begsq + 1, '\'');
		if (li->endsq)
		{
			li->sfl = 1;
			li->dfl = 0;
			li->begdq = NULL;
			li->enddq = NULL;
		}
	}
	else if (qflag == 1)
	{
		li->enddq = ft_strchr(li->begdq + 1, '\"');
		if (li->enddq)
		{
			li->dfl = 1;
			li->sfl = 0;
			li->begsq = NULL;
			li->endsq = NULL;
		}
	}
}

/*a function that looks for quotes and sets flags appropriately
depending on whether the beginning or end of quotes is discovered*/
void	check_quotes(char **ps, t_line_info *li)
{
	char		*str;

	str = *ps;
	if (!li->sfl)
		li->begsq = ft_strchr(str, '\'');
	if (!li->dfl)
		li->begdq = ft_strchr(str, '\"');
	if ((!li->endsq) && (li->begsq) && ((li->begsq < li->begdq \
		|| li->begdq == NULL)) && li->dfl != 1 && li->sfl == 0)
	{
		handle_quote_flags(li, 0);
	}
	if ((!li->enddq) && (li->begdq) && ((li->begdq < li->begsq \
		|| li->begsq == NULL)) && li->sfl != 1 && li->dfl == 0)
	{
		handle_quote_flags(li, 1);
	}
	if (li->endsq && (*ps >= li->begsq && *ps <= li->endsq))
		li->in_quotes = 1;
	if (li->enddq && (*ps >= li->begdq && *ps <= li->enddq))
		li->in_quotes = 1;
}
