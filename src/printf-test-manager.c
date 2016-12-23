/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printf-test-manager.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/12/23 18:58:25 by alelievr          #+#    #+#             */
/*   Updated: 2016/12/23 19:06:01 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

extern char		*g_current_format;
char		*g_current_format;

void		printf_diff_error(int d1, int d2);
void		printf_diff_error(int d1, int d2)
{
	printf("diff on format: %s\n", g_current_format);
	(void)d1;
	(void)d2;
}
