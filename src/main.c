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
#include <stdarg.h>

char *				g_current_format;
static int			g_current_index = 0;
static int			g_failed_tests = 0;
static int			g_passed_tests = 0;

static void	usage() __attribute__((noreturn));
static void	usage()
{
	printf("usage: ./run_test < converters >\n");
	printf("supported converters: \"douXcsp\"");
	exit(-1);
}

static void	cout(const char *f, ...)
{
	va_list		ap;
	static int	new_stdout = 0;

	if (new_stdout == 0)
		new_stdout = open("/dev/tty", O_RDWR);

	va_start(ap, f);
	vdprintf(new_stdout, f, ap);
	va_end(ap);
}

static void	sigh(int s)
{
	cout(C_CRASH"catched signal: %s at test \"%i\", current format: \"%s\"\n", strsignal(s), g_current_index, g_current_format);
	(void)s;
}

static void run_test(void (*testf)(char *b, int (*)(), int *, int *), int (*ft_printf)(), int fd[2])
{
	char		buff[0xF00];
	char		printf_buff[0xF00];
	char		ftprintf_buff[0xF00];
	long		r;
	int			d1, d2; //d1 is the printf return and d2 ft_printf return.

	testf(g_current_format, ft_printf, &d1, &d2);
	if (d1 != d2)
	{
		cout("bad return value for format: \"%s\": %i vs %i", g_current_format, d1, d2);
	}
	r = read(fd[READ], buff, sizeof(buff));
	if (r > 0)
	{
		if (!strchr(buff, '\x99'))
		{
			printf("error while getting result on test: %s\n", g_current_format);
			return ;
		}
		strlcpy(printf_buff, buff, strchr(buff, '\x99') - buff);
		strlcpy(ftprintf_buff, strchr(buff, '\x99') + 1, sizeof(buff));
		buff[r] = 0;
		if (strcmp(printf_buff, ftprintf_buff))
		{
			cout(C_ERROR"[ERROR] diff on output for format \"%s\" -> got: [%s], expected: [%s]\n", g_current_format, ftprintf_buff, printf_buff);
			g_failed_tests++;
		}
		else
			g_passed_tests++;
		//split and diff the results
	}
	(void)index;
}

static void	run_tests(void *tests_h, int (*ft_printf)(), char *convs)
{
	char		fun_name[0xF00];
	int			fd[2];
	void		(*test)();
	int			index;
	int			test_id = 0;
	int			old_failed_tests;

	if (*convs)
		printf("Starting tests ...\n");
	if (pipe(fd) != 0)
		perror("pipe"), exit(-1);
	dup2(fd[WRITE], STDOUT_FILENO);
	close(fd[WRITE]);
	while (*convs)
	{
		old_failed_tests = g_failed_tests;
		cout("testing %%%c ...\n", *convs);
		index = 0;
		while (42)
		{
			g_current_index = index;
			sprintf(fun_name, "printf_unit_test_%c_%.9i", *convs, index);
			if (!(test = (void(*)())dlsym(tests_h, fun_name)))
				break ;
			run_test(test, ft_printf, fd);
			index++;
			test_id++;
		}
		if (g_failed_tests == old_failed_tests)
			cout(C_PASS"Passed all %i tests for convertion %c\n", index, *convs);
		else
			cout(C_ERROR"Failed %i tests for convertion %c\n", g_failed_tests - old_failed_tests, *convs);
		convs++;
	}
	cout("total format tested: %i\n", test_id);
}

static void	ask_download_tests(void)
{
	char	c;

	printf("main test library was not found, do you want to download it ? (y/n)");
	if ((c = (char)getchar()) == 'y' || c == 'Y' || c == '\n')
		;
}

int			main(int ac, char **av)
{
	static char		buff[0xF00];
	void			*tests_handler;
	void			*ftprintf_handler;
	char			*testflags = SUPPORTED_CONVERTERS;
	int				(*ft_printf)();

	if (ac > 2)
		usage();
	if (ac == 2)
		testflags = av[1];
	signal(SIGSEGV, sigh);
	signal(SIGBUS, sigh);
	signal(SIGPIPE, sigh);
	if (access(TEST_LIB_SO, F_OK))
		perror(TEST_LIB_SO), exit(-1);
	if (access(TEST_LIB_SO, F_OK | X_OK))
		ask_download_tests();
	if (!(tests_handler = dlopen(TEST_LIB_SO, RTLD_LAZY)))
		perror("dlopen"), exit(-1);
	if (!(ftprintf_handler = dlopen(FTPRINTF_LIB_SO, RTLD_LAZY)))
		perror("dlopen"), exit(-1);
	if (!(ft_printf = (int (*)())dlsym(ftprintf_handler, "ft_printf")))
		perror("dlsym"), exit(-1);
	g_current_format = buff;
	run_tests(tests_handler, ft_printf, testflags);
	return (0);
}
