/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_in2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 15:54:15 by rboudwin          #+#    #+#             */
/*   Updated: 2024/04/15 14:40:53 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* if exit code is provided as an argument, exits the shell with it.
Otherwise exits with the last exit code provided by $? */
void	ft_exit(t_execcmd *ecmd, t_info *info)
{
	char	*last_exit_code;
	int		exit_code;
	int		i;

	i = 0;
	if (ecmd->argv[1])
	{
		while (ecmd->argv[1][i])
		{
			if (!ft_isdigit(ecmd->argv[1][i]))
			{
				ft_putstr_fd("AR-Shell: ", 2);
				ft_putstr_fd(ecmd->argv[1], 2);
				ft_putstr_fd(": numeric argument required\n", 2);
				return ;
			}
			i++;
		}
		exit_code = ft_atoi(ecmd->argv[1]);
	}
	else
		exit_code = info->exit_code;
	free_2d(info->curr_env);
	ft_printf("exit\n");
	exit(exit_code);
}
