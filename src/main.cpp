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
#include <ctype.h>

static const char *	current_format;
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
static bool			debug = false;
static bool			disable_timeout = false;

static void	*		runTestFuncs[128];

static void	usage() __attribute__((noreturn));
static void	usage()
{
	printf("usage: ./run_test < options > < converters >\n"
			"options:\n"
			"  -e: stop to the first error / segfault\n"
			"  -q: disable errer/segv/timeout output\n"
			"  -r: disable speed test\n"
			"  -d: debug mode\n"
			"  -h: display help\n");
	printf("supported converters: \"" SUPPORTED_CONVERTERS "\"");
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
			if (isprint((char)arg))
				sprintf(buff, "\'%c\'", (char)arg);
			else
				sprintf(buff, "(char)%i", (int)arg);
			break ;
		case 'C':
			if (arg != 0)
				sprintf(buff, "L\'%C\'(%i)", (wchar_t)arg, (int)arg);
			else
				sprintf(buff, "(wchar_t)%i", (int)arg);
			break ;
		case 'a': case 'A': case 'e': case 'E': case 'f': case 'F': case 'g': case 'G':
			sprintf(buff, "%f", (float)arg);
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

void print_mem(char *mem, size_t size);
void print_mem(char *mem, size_t size)
{
	size_t	i = 0;

	while (i < size)
	{
		if (isprint(mem[i]))
			cout("%c", mem[i]);
		else
			cout("\\x%hhx", mem[i]);
		i++;
	}
}

static char *escapeBuff(char *str, size_t len)
{
	static char		tmp[0xF000];

	for (size_t i = 0; i < len; i++)
	{
		if (!str[i])
		{
			tmp[i] = '\\';
			tmp[i] = '0';
		}
		else
			tmp[i] = str[i];
	}
	return tmp;
}

template< typename T >
static void runTestSpec(const char *fmt, int (*ft_printf)(const char *f, ...), int fd[2], T arg)
{
	char		printf_buff[0xF00];
	char		ftprintf_buff[0xF00];
	long		r;
	int			d1, d2; //d1 is the printf return and d2 ft_printf return.
	clock_t		b, m, e;
	bool		failed;

	current_arg = LONGIFY(arg);
	current_format = fmt;
	b = clock();

	if (debug)
	{
		cout("format: [%s], arg: %s\n", fmt, arg_to_string((long long)arg));
	}

	//true printf
	d1 = printf(fmt, arg);
	write(1, "\0\0\0\0", 4);

	last_time_update = time(NULL); //for timeout
	if ((r = read(fd[READ], printf_buff, sizeof(printf_buff) - 1)) < 0)
		perror("read"), exit(-1);
	printf_buff[r] = 0;
	m = clock();

	d2 = ft_printf(fmt, arg);
	write(1, "\0\0\0\0", 4);
	if ((r = read(fd[READ], ftprintf_buff, sizeof(ftprintf_buff) - 1)) < 0)
		perror("read"), exit(-1);
	ftprintf_buff[r] = 0;

	e = clock();

	if (debug)
	{
		cout("   printf: [");
		print_mem(printf_buff, r);
		cout("]\nft_printf: [");
		print_mem(ftprintf_buff, r);
		cout("]\n\n");
	}

	if (current_index == 0)
		current_speed_percent = (double)(e - m) / (double)(m - b);
	else if (m - b != 0)
	{
		double h = 1. / ((double)current_index + 1.);
		double w = h * (current_index);
		current_speed_percent = current_speed_percent * w + ((e - m) / (m - b) * h);
	}

	failed = false;
	if (d1 != d2)
	{
		if (!quiet)
		{
			cout(C_ERROR "bad return value for format \"%s\" and arg: %s -> got: %i expected %i\n" C_CLEAR, current_format, arg_to_string((long long)arg), d2, d1);
		}
		if (stop_to_first_error)
			exit(0);
		failed_tests++;
		failed = true;
	}
	if (memcmp(printf_buff, ftprintf_buff, r))
	{
		if (!quiet)
			cout(C_ERROR "[ERROR] diff on output for format \"%s\" and arg: %s -> got: [%s], expected: [%s]\n" C_CLEAR, current_format, arg_to_string((long long)arg), escapeBuff(ftprintf_buff, r), escapeBuff(printf_buff, r));
		if (stop_to_first_error)
			exit(0);
		if (!failed)
			failed_tests++;
	}
	else
		passed_tests++;
	//split and diff the results
	(void)index;
}

#define generateRunTest(type) static void runTest_##type(const char *fmt, int (*ft_printf)(const char *f, ...), int fd[2], type c) \
{ \
	runTestSpec< type >(fmt, ft_printf, fd, c); \
} 

generateRunTest(int)
generateRunTest(long)
generateRunTest(string)
generateRunTest(wstring)
generateRunTest(wchar_t)
generateRunTest(char)
generateRunTest(double)
generateRunTest(ptr)

static void	run_tests(int (*ft_printf)(const char *, ...), const char *convs, const char *allowed_convs)
{
	int			fd[2];
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
	setbuf(stdout, NULL);
	setlocale(LC_ALL, "");
	while (*convs)
	{
		if (!strchr(allowed_convs, *convs) && convs++)
			continue ;
		current_conv = *convs;
		old_failed_tests = failed_tests;
		cout(C_TITLE "testing %%%c ...\n" C_CLEAR, *convs);
		index = -1;
		test_count = 0;
		disable_timeout = true;
		auto formats = generateTestFormats(*convs);
		disable_timeout = false;
		for (auto fmt : formats)
		{
			index++;
			current_index = index;
			argc = generateRandArgs(*convs, args);
			g_current_test_index = -1;
			setjmp(jmp_next_test);
			while (++g_current_test_index < argc)
			{
				((void (*)(const char *, int (*)(const char *, ...), int[2], ...))runTestFuncs[(int)*convs])(fmt.c_str(), ft_printf, fd, args[g_current_test_index]);
				total_test_count++;
				test_count++;
			}
		}
		if (failed_tests == old_failed_tests)
			cout(C_PASS "Passed all %'i tests for convertion %c\n" C_CLEAR, test_count, *convs);
		else
			cout(C_ERROR "Failed %'i of %'i tests for convertion %c\n" C_CLEAR, failed_tests - old_failed_tests, test_count, *convs);
		if (!no_speed)
		{
			cout(C_PASS "On %c convertion, your printf is %.2f times slower than system's\n" C_CLEAR, *convs, current_speed_percent);
		}
		convs++;
	}
	cout("tested format count: %i\n", total_test_count);
}

static void	*timeout_thread(void *t)
{
	(void)t;
	while (42)
	{
		sleep(1);
		if (disable_timeout)
			last_time_update = time(NULL);
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

	while ((opt = getopt(ac, av, "heqdr")) != -1)
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
			case 'd':
				debug = true;
				break ;
			case 'r':
				no_speed = true;
				break ;
		}
}

static void InitRunTest()
{
	runTestFuncs[(int)'d'] = (void *)runTest_int;
	runTestFuncs[(int)'i'] = (void *)runTest_int;
	runTestFuncs[(int)'o'] = (void *)runTest_int;
	runTestFuncs[(int)'u'] = (void *)runTest_int;
	runTestFuncs[(int)'x'] = (void *)runTest_int;
	runTestFuncs[(int)'X'] = (void *)runTest_int;

	runTestFuncs[(int)'O'] = (void *)runTest_long;
	runTestFuncs[(int)'D'] = (void *)runTest_long;
	runTestFuncs[(int)'U'] = (void *)runTest_long;

	runTestFuncs[(int)'p'] = (void *)runTest_ptr;

	runTestFuncs[(int)'e'] = (void *)runTest_double;
	runTestFuncs[(int)'E'] = (void *)runTest_double;
	runTestFuncs[(int)'f'] = (void *)runTest_double;
	runTestFuncs[(int)'F'] = (void *)runTest_double;
	runTestFuncs[(int)'g'] = (void *)runTest_double;
	runTestFuncs[(int)'G'] = (void *)runTest_double;
	runTestFuncs[(int)'a'] = (void *)runTest_double;
	runTestFuncs[(int)'A'] = (void *)runTest_double;

	runTestFuncs[(int)'C'] = (void *)runTest_wchar_t;
	runTestFuncs[(int)'c'] = (void *)runTest_char;

	runTestFuncs[(int)'S'] = (void *)runTest_wstring;
	runTestFuncs[(int)'s'] = (void *)runTest_string;
}

int			main(int ac, char **av)
{
	void			*ftprintf_handler;
	const char		*testflags = SUPPORTED_CONVERTERS;
	int				(*ft_printf)(const char *, ...);
	pthread_t		p;

	options(ac, av);
	ac -= optind;
	av += optind;
	if (ac == 1)
		testflags = av[0];

	InitRunTest();
	signal(SIGSEGV, sigh);
	signal(SIGBUS, sigh);
	signal(SIGPIPE, sigh);
	if (!(ftprintf_handler = dlopen(FTPRINTF_LIB_SO, RTLD_LAZY)))
		perror("dlopen"), exit(-1);
	if (!(ft_printf = (int (*)(const char *, ...))dlsym(ftprintf_handler, "ft_printf")))
		perror("dlsym"), exit(-1);
	if ((pthread_create(&p, NULL, timeout_thread, NULL)) == -1)
		puts(C_ERROR"thread init failed"), exit(-1);
	run_tests(ft_printf, testflags, SUPPORTED_CONVERTERS);
	return (0);
}