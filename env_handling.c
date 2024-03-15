/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_handling.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/15 10:13:35 by rboudwin          #+#    #+#             */
/*   Updated: 2024/03/15 10:23:41 by rboudwin         ###   ########.fr       */
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
			return(env[i] + ft_strlen(var) + 1); //to get past VAR=
		i++;
	}
	return (NULL);
}