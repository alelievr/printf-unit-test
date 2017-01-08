/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printf_unit_test.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/12/23 17:46:11 by alelievr          #+#    #+#             */
/*   Updated: 2017/01/07 17:10:41 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#pragma clang diagnostic ignored "-Wformat-nonliteral"

#include <stdio.h>
#include <dlfcn.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define SUPPORTED_CONVERTERS	"douxcsp"
#define TEST_LIB_SO				"./printf-tests.so"
#define FTPRINTF_LIB_SO			"./libftprintf.so"

#define	C_ERROR					"\033[38;5;196m"
#define	C_PASS					"\033[38;5;118m"
#define	C_CRASH					"\033[48;5;159m\033[38;5;93m"

#define READ		0
#define WRITE		1

extern char *			g_current_format;

extern void				printf_diff_error(int d1, int d2);
