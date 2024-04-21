/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboudwin <rboudwin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/21 15:12:38 by rboudwin          #+#    #+#             */
/*   Updated: 2024/04/21 16:03:26 by rboudwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* utility for ft_prompt */
void	check_and_free(char *prompt, char *ptr_parking)
{
	check_malloc_failure(prompt);
	free_and_null(ptr_parking);
}
