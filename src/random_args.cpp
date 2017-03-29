/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   random_args.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/01/08 16:04:18 by alelievr          #+#    #+#             */
/*   Updated: 2017/03/29 20:08:34 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "printf_unit_test.h"

static int		generateRandomInts(long long *args, int n)
{
	intmax_t	r;
	int			ret = 1;

	*args++ = 0;
	for (int i = 0; i < n; i++)
	{
		r = ((intmax_t)rand() * (intmax_t)rand());
		*args++ = LONGIFY(r);
		ret++;
	}
	return (ret);
}

static int		generateRandomFloats(long long *args, int n)
{
	float	r;
	int		ret = 3;

	*args++ = 0;
	r = NAN;
	*args++ = LONGIFY(r);
	r = INFINITY;
	*args++ = LONGIFY(r);
	for (int i = 0; i < n; i++)
	{
		if (rand() % 2)
			r = ((float)(int)rand() / (float)(int)rand());
		else
			r = ((float)(int)rand() * (float)(int)rand());
		*args++ = LONGIFY(r);
		ret++;
	}
	return (ret);
}

static int		generateRandomStrings(long long *args, int n)
{
	const char	*r = "";
	int			ret = 2;

	*args++ = LONGIFY(r);
	r = NULL;
	*args++ = LONGIFY(r);
	for (int i = 0; i < n; i++)
	{
		r = (const char *[]){"#nyancta inside", "yolol !", "AAAAAAAAAAAAAAAAAAAAAAAAAA", "\x7f", ""}[rand() % 4];
		*args++ = LONGIFY(r);
		ret++;
	}
	return (ret);
}

static int		generateRandomWStrings(long long *args, int n)
{
	const wchar_t	*r = L"";
	int		ret = 2;

	*args++ = LONGIFY(r);
	r = NULL;
	*args++ = LONGIFY(r);
	for (int i = 0; i < n; i++)
	{
		r = (const wchar_t *[]){L"こんにちは、私はprintf単体テストです",
			L"いいえ",
			L"最終的なフラッシュ",
			L"(╯°□°)╯︵ ┻━┻ ",
			L"(╯°□°)╯︵ ┻━┻ ︵ ╯(°□° ╯)",
			L"┬─┬﻿ ノ( ゜-゜ノ)",
			L"(╯°Д°）╯︵ /(.□ . \\)",
			L"(/ .□.)\\ ︵╰(゜Д゜)╯︵ /(.□. \\)",
			L"ʕノ•ᴥ•ʔノ ︵ ┻━┻ "
		}[rand() % 9];
		*args++ = LONGIFY(r);
		ret++;
	}
	return (ret);
}

static int		generateRandomChars(long long *args, int n)
{
	char	r = '\0';
	int		ret = 1;

	*args++ = LONGIFY(r);
	for (int i = 0; i < n; i++)
	{
		r = (char)(rand() % 256);
		if (r == '\x99')
			r = 42;
		*args++ = LONGIFY(r);
		ret++;
	}
	return (ret);
}

static int		generateRandomWChars(long long *args, int n)
{
	wchar_t	r = '\0';
	int		ret = 1;

	*args++ = LONGIFY(r);
	for (int i = 0; i < n; i++)
	{
		r = (wchar_t []){L'⭐', L'💖', L'⚔', L'☕', L'☠', L'⚡', L'Ω', L'ø', L'π'}[rand() % 9];
		*args++ = LONGIFY(r);
		ret++;
	}
	return (ret);
}

static int		generateRandomPointers(long long *args, int n)
{
	void	*r = NULL;
	int		ret = 1;

	*args++ = LONGIFY(r);
	for (int i = 0; i < n; i++)
	{
		r = (void *)(unsigned long)rand();
		*args++ = LONGIFY(r);
		ret++;
	}
	return (ret);
}

int				generateRandArgs(char conv, long long *args)
{
	int		n_rand_args = 5;

	if (strchr("idDoOuUxX", conv))
		return generateRandomInts(args, n_rand_args);
	if (strchr("aAeEfFgG", conv))
		return generateRandomFloats(args, n_rand_args);
	if (conv == 's')
		return generateRandomStrings(args, n_rand_args);
	if (conv == 'S')
		return generateRandomWStrings(args, n_rand_args);
	if (conv == 'c')
		return generateRandomChars(args, n_rand_args);
	if (conv == 'C')
		return generateRandomWChars(args, n_rand_args);
	if (conv == 'p')
		return generateRandomPointers(args, n_rand_args);
	return (0);
}

