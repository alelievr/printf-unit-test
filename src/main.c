/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created  2016/12/23 17:42:25 by alelievr          #+#    #+#             */
/*   Updated  2016/12/23 17:42:25 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "printf_unit_test.h"

char *				g_current_format;
static int			g_current_index = 0;

static void	usage() __attribute__((noreturn));
static void	usage()
{
	printf("usage: ./run_test < converters >\n");
	printf("supported converters: \"douXcsp\"");
	exit(-1);
}

static void	sigh(int s)
{
	printf("catched signal: %s at test: \"%i\"\n", strsignal(s), g_current_index);
	(void)s;
}

static void	run_tests(void *handler, char *convs)
{
	char		fun_name[0xF00];
	char		buff[0xF00];
	int			fd[2];
	void		(*test)(void);
	int			index;
	long		r;

	if (*convs)
		printf("Starting tests ...\n");
	dup2(fd[WRITE], 1);
	close(fd[WRITE]);
	while (*convs)
	{
		index = 0;
		while (42)
		{
			g_current_index = index;
			sprintf(fun_name, "printf_unit_test_%c_%.7i", *convs, index);
//			dprintf(2, "test: %c - %04i: %s\n", *convs, index, g_current_format);
			if (!(test = (void(*)())dlsym(handler, fun_name)))
			{
				dprintf(2, "nope !\n");
				break ;
			}
			test();
			r = read(fd[READ], buff, sizeof(buff));
			if (r > 0)
			{
				//split and  diff the results
			}
			index++;
		}
		convs++;
	}
}

int			main(int ac, char **av)
{
	void	*handler;
	char	*testflags = SUPPORTED_CONVERTERS;

	if (ac > 2)
		usage();
	if (ac == 2)
		testflags = av[1];
	signal(SIGSEGV, sigh);
	signal(SIGBUS, sigh);
	signal(SIGPIPE, sigh);
	if (access(TEST_LIB_SO, F_OK))
		perror(TEST_LIB_SO), exit(-1);
	if (!(handler = dlopen(TEST_LIB_SO, RTLD_LAZY)))
		perror("dlopen"), exit(-1);
	if (!(g_current_format = dlsym(handler, "g_current_format")))
		perror("dlsym"), exit(-1);
	run_tests(handler, testflags);
	return (0);
}
