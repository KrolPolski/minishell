/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_in.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 10:09:19 by rboudwin          #+#    #+#             */
/*   Updated: 2024/03/19 11:32:13 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_cd(t_execcmd *ecmd, t_info *info)
{
	char	buf[100];

	if (chdir(ecmd->argv[1]) < 0)
		printf("cannot cd %s\n", ecmd->argv[1]);
	else
	{
		info->curr_dir = getcwd(buf, sizeof(buf));
		free(info->prompt);
		info->prompt = ft_prompt(info->username, "AR-Shell", info->curr_dir);
	}
}

void	ft_pwd(t_execcmd *ecmd, t_info *info)
{
	char	buf[100];

	if (!(getcwd(buf, sizeof(buf))))
		printf("pwd error\n");
	else
		(printf("%s%s", buf, "\n"));
}

void	ft_env(t_execcmd *ecmd, t_info *info)
{
	int	i;

	if (info->curr_env == NULL)
		panic("env not set");
	i = 0;
	while (info->curr_env[i])
	{
		ft_putstr_fd(info->curr_env[i], 1);
		printf("\n");
		i++;
	}
}

void	ft_echo(t_execcmd *ecmd, t_info *info)
{
	int	nl;
	int	i;

	nl = 1;
	i = 1;
	if (ecmd->argv[1] && !ft_strncmp(ecmd->argv[1], "-n", 3))
	{
		nl = 0;
		i++;
	}
	while (ecmd->argv[i])
	{
		ft_putstr_fd(ecmd->argv[i], 1);
		if (ecmd->argv[i + 1])
			ft_putchar_fd(' ', 1);
		i++;
	}
	if (nl == 1)
		ft_putchar_fd('\n', 1);
}

void	handle_builtins(t_execcmd *ecmd, char **env,
	char *builtin_command, t_info *info)
{
	if (!ft_strncmp(builtin_command, "pwd", ft_strlen(builtin_command)))
		ft_pwd(ecmd, info);
	if (!ft_strncmp(builtin_command, "cd", ft_strlen(builtin_command)))
		return ;
	if (!ft_strncmp(builtin_command, "echo", ft_strlen(builtin_command)))
		ft_echo(ecmd, info);
	if (!ft_strncmp(builtin_command, "env", ft_strlen(builtin_command)))
		ft_env(ecmd, info);
	if (!ft_strncmp(builtin_command, "export", ft_strlen(builtin_command)))
		ft_export(ecmd, info);
}