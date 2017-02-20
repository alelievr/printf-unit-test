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
#include <setjmp.h>
#include <unistd.h>
#include <locale.h>

static char *		current_format;
static int			current_index = 0;
static char			current_conv;
static int			failed_tests = 0;
static int			passed_tests = 0;
static long long	current_arg;
static long			last_time_update;
static jmp_buf		jmp_next_test;
static int			sig_counter = 0;
static int			g_current_test_index = 0;
static bool			stop_to_first_error = false;
static double		current_speed_percent = 1.;
static bool			quiet = false;
static bool			no_speed = false;

static void	usage() __attribute__((noreturn));
static void	usage()
{
	printf("usage: ./run_test < options > < converters >\n"
			"options:\n"
			"  -e: stop to the first error / segfault\n"
			"  -q: disable errer/segv/timeout output\n"
			"  -r: disable speed test\n"
			"  -h: display help\n");
	printf("supported converters: \""SUPPORTED_CONVERTERS"\"");
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

static char	*arg_to_string(long long int arg)
{
	static char		buff[0xF000];

	switch (current_conv)
	{
		case 'd': case 'D': case 'i': case 'o': case 'O': case 'u': case 'U': case 'x': case 'X':
			sprintf(buff, "%lli", arg);
			break ;
		case 'p':
			sprintf(buff, "%p", (void *)arg);
			break ;
		case 's':
			sprintf(buff, "\"%s\"", (char *)arg);
			break ;
		case 'S':
			sprintf(buff, "L\"%S\"", (wchar_t *)arg);
			break ;
		case 'c':
			sprintf(buff, "\'%c\'", (char)arg);
			break ;
		case 'C':
			sprintf(buff, "L\'%c\'", (wchar_t)arg);
			break ;
	}
	return buff;
}

static void	sigh(int s) __attribute__((noreturn));
static void	sigh(int s)
{
	if (!quiet)
		cout(C_CRASH"catched signal: %s when testing format: \"%s\" with arg: %s\n", strsignal(s), current_format, arg_to_string(current_arg));
	if (stop_to_first_error)
		exit(0);
	if (sig_counter >= 500 && !quiet)
		cout(C_CRASH"received too many crash signals, aborting test ...\n"), exit(-1);
	sig_counter++;
	g_current_test_index++;
	longjmp(jmp_next_test, 1);
}

static void run_test(void (*testf)(char *b, int (*)(), int *, long long, int), int (*ft_printf)(), int fd[2], long long arg)
{
	char		buff[0xF00];
	char		printf_buff[0xF00];
	char		ftprintf_buff[0xF00];
	long		r;
	int			d1, d2; //d1 is the printf return and d2 ft_printf return.
	clock_t		b, m, e;
	bool		failed;

	current_arg = arg;
	b = clock();
	//true printf
	testf(current_format, ft_printf, &d1, arg, 0);
	m = clock();
	last_time_update = time(NULL); //for timeout
	//ft_printf
	testf(current_format, ft_printf, &d2, arg, 1);
	e = clock();
	if (current_index == 0)
		current_speed_percent = (double)(e - m) / (double)(m - b);
	else if (m - b != 0)
	{
		double h = 1. / ((double)current_index + 1.);
		double w = h * (current_index);
		current_speed_percent = current_speed_percent * w + ((e - m) / (m - b) * h);
	}
	fflush(stdout);
	failed = false;
	if (d1 != d2)
	{
		if (!quiet)
			cout(C_ERROR"bad return value for format \"%s\" and arg: %s -> got: %i expected %i\n"C_CLEAR, current_format, arg_to_string(arg), d2, d1);
		if (stop_to_first_error)
			exit(0);
		failed_tests++;
		failed = true;
	}
	r = read(fd[READ], buff, sizeof(buff));
	if (r > 0)
	{
		buff[r] = 0;
		if (!memchr(buff, '\x99', (size_t)r))
		{
			printf(C_ERROR"error while getting result on test: %s\n"C_CLEAR, current_format);
			return ;
		}
		size_t off = (size_t)((char *)memchr(buff, '\x99', (size_t)r) - buff);
		memcpy(printf_buff, buff, off);
		printf_buff[off] = 0;
		memcpy(ftprintf_buff, buff + off + 1, r + 1);
		if (strcmp(printf_buff, ftprintf_buff))
		{
			if (!quiet)
				cout(C_ERROR"[ERROR] diff on output for format \"%s\" and arg: %s -> got: [%s], expected: [%s]\n"C_CLEAR, current_format, arg_to_string(arg), ftprintf_buff, printf_buff);
			if (stop_to_first_error)
				exit(0);
			if (!failed)
				failed_tests++;
		}
		else
			passed_tests++;
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
	int			total_test_count = 0;
	int			test_count = 0;
	int			old_failed_tests;
	long long	args[0xF0];
	int			argc;

	if (*convs)
		printf("Starting tests ...\n");
	if (pipe(fd) != 0)
		perror("pipe"), exit(-1);
	dup2(fd[WRITE], STDOUT_FILENO);
	close(fd[WRITE]);
	setlocale(LC_ALL, "");
	while (*convs)
	{
		current_conv = *convs;
		old_failed_tests = failed_tests;
		cout(C_TITLE"testing %%%c ...\n"C_CLEAR, *convs);
		index = -1;
		test_count = 0;
		while (42)
		{
			index++;
			current_index = index;
			sprintf(fun_name, "printf_unit_test_%c_%.9i", *convs, index);
			if (!(test = (void(*)())dlsym(tests_h, fun_name)))
				break ;
			argc = generate_rand_args(*convs, args);
			g_current_test_index = -1;
			setjmp(jmp_next_test);
			while (++g_current_test_index < argc)
			{
				run_test(test, ft_printf, fd, args[g_current_test_index]);
				total_test_count++;
				test_count++;
			}
		}
		if (failed_tests == old_failed_tests)
			cout(C_PASS"Passed all %'i tests for convertion %c\n"C_CLEAR, test_count, *convs);
		else
			cout(C_ERROR"Failed %'i of %'i tests for convertion %c\n"C_CLEAR, failed_tests - old_failed_tests, test_count, *convs);
		if (!no_speed)
			cout(C_PASS"On %c convertion, your printf is %.2f times slower than system's\n"C_CLEAR, *convs, current_speed_percent);
		convs++;
	}
	cout("tested format count: %i\n", total_test_count);
}

static void	ask_download_tests(void)
{
	char	buff[0xF0];
	char	c;

	__attribute__((unused)) const char * const files[] = {
		"printf-tests.so",
		"printf-tests-floats.so"
	};

	printf("main test library was not found, do you want to download it ? (y/n) ");
	fflush(stdout);
	if ((c = (read(STDIN_FILENO, buff, sizeof(buff)), buff[0])) == 'y' || c == 'Y' || c == '\n')
		system("curl -o printf-tests.so https://raw.githubusercontent.com/alelievr/printf-unit-test-libs/master/printf-tests.so");
	else
		exit(0);
	printf("do you want to download float test library too ? (y/n)");
	fflush(stdout);
	if ((c = (read(STDIN_FILENO, buff, sizeof(buff)), buff[0])) == 'y' || c == 'Y' || c == '\n')
		system("curl -o printf-tests-floats.so https://raw.githubusercontent.com/alelievr/printf-unit-test-libs/master/printf-tests-floats.so");
}

static void	*timeout_thread(void *t)
{
	(void)t;
	while (42)
	{
		sleep(1);
		if (time(NULL) - last_time_update > 3) //3sec passed on ftprintf function
		{
			cout(C_ERROR"Timeout on format: \"%s\" with argument: %lli\n", current_format, current_arg);
			exit(0);
		}
	}
}

static void	options(int ac, char **av)
{
	int		opt;

	while ((opt = getopt(ac, av, "heq")) != -1)
		switch (opt)
		{
			case 'h':
				usage();
			case 'e':
				stop_to_first_error = true;
				break ;
			case 'q':
				quiet = true;
				break ;
			case 'r':
				no_speed = true;
				break ;
		}
}

int			main(int ac, char **av)
{
	static char		buff[0xF00];
	void			*tests_handler;
	void			*ftprintf_handler;
	char			*testflags = SUPPORTED_CONVERTERS;
	int				(*ft_printf)();
	pthread_t		p;

	options(ac, av);
	ac -= optind;
	av += optind;
	if (ac == 1)
		testflags = av[0];

	signal(SIGSEGV, sigh);
	signal(SIGBUS, sigh);
	signal(SIGPIPE, sigh);
	if (access(TEST_LIB_SO, F_OK))
		ask_download_tests();
	if (!(tests_handler = dlopen(TEST_LIB_SO, RTLD_LAZY)))
		perror("dlopen"), exit(-1);
	if (!(ftprintf_handler = dlopen(FTPRINTF_LIB_SO, RTLD_LAZY)))
		perror("dlopen"), exit(-1);
	if (!(ft_printf = (int (*)())dlsym(ftprintf_handler, "ft_printf")))
		perror("dlsym"), exit(-1);
	current_format = buff;
	if ((pthread_create(&p, NULL, timeout_thread, NULL)) == -1)
		puts(C_ERROR"thread init failed"), exit(-1);
	run_tests(tests_handler, ft_printf, testflags);
	return (0);
}
