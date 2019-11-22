/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printf_unit_test.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/12/23 17:46:11 by alelievr          #+#    #+#             */
/*   Updated: 2019/11/19 14:50:57 by juligonz         ###   ########.fr       */
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
#include <iostream>
#include <list>

typedef char *		string;
typedef wchar_t *	wstring;
typedef void *		ptr;

#define LONGIFY(x) *(long long *)(void *)(&x)

//#define SUPPORTED_CONVERTERS	"idDoOuUxXcCsSpaAeEfFgG"
//#define DEFAULT_CONVERTERS		"idDoOuUxXcCsSp"

#define SUPPORTED_CONVERTERS	"cspdiuxX"
#define DEFAULT_CONVERTERS		"cspdiuxXfge"
#define TEST_LIB_SO				"./printf-tests.so"
#define TEST_LIB_FLOATS_SO		"./printf-tests-floats.so"
#define FTPRINTF_LIB_SO			"./libftprintf.so"
#define LOG_FILE				"./result.log"

#define READ		0
#define WRITE		1

int							generateRandArgs(char conv, const char *mods, long long *args);
std::list< std::string >	generateTestFormats(char conv);
