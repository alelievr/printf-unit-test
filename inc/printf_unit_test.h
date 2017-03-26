/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printf_unit_test.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/12/23 17:46:11 by alelievr          #+#    #+#             */
/*   Updated: 2017/03/26 01:35:53 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#pragma clang diagnostic ignored "-Wformat-nonliteral"
#pragma clang diagnostic ignored "-Wformat-non-iso"

#include <stdio.h>
#include <dlfcn.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define SUPPORTED_CONVERTERS	"idDoOuUxXcCsSpaAeEfFgG"
#define TEST_LIB_SO				"./printf-tests.so"
#define TEST_LIB_FLOATS_SO		"./printf-tests-floats.so"
#define FTPRINTF_LIB_SO			"./libftprintf.so"

#define	C_ERROR					"\033[38;5;196m"
#define	C_PASS					"\033[38;5;118m"
#define	C_CRASH					"\033[38;5;93m"
#define C_TITLE					"\033[38;5;70m"
#define C_CLEAR					"\033[0m"

#define READ		0
#define WRITE		1

extern char *			g_current_format;

int						generate_rand_args(char conv, long long *args);
