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
#include <fcntl.h>

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
	void		(*test)(char *);
	int			index;
	long		r;
	int			test_id = 0;
	int			new_stdout;

	if (*convs)
		printf("Starting tests ...\n");
	if (pipe(fd) != 0)
		perror("pipe"), exit(-1);
	dup2(fd[WRITE], STDOUT_FILENO);
	close(fd[WRITE]);
	new_stdout = open("/dev/tty", O_RDWR);
	while (*convs)
	{
		printf("testing %%%c ...\n", *convs);
		index = 0;
		while (42)
		{
			g_current_index = index;
			sprintf(fun_name, "printf_unit_test_%c_%.7i", *convs, index);
			if (!(test = (void(*)())dlsym(handler, fun_name)))
				break ;
			test(g_current_format);
			fflush(stdout);
			r = read(fd[READ], buff, sizeof(buff));
			if (r > 0)
			{
				//split and  diff the results
			}
			index++;
			test_id++;
		}
		convs++;
	}
	dprintf(new_stdout, "total format tested: %i\n", test_id);
}

//TODO: turn libftprintf.a into .so, load it and send ft_printf's symbol to the tests. (+ a parameter to return the two printf's returns)
int			main(int ac, char **av)
{
	static char		buff[0xF00];
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
	g_current_format = buff;
	run_tests(handler, testflags);
	return (0);
}
