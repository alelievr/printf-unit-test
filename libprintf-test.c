#include <stdarg.h>
#include <stdio.h>

int		ft_printf(const char *format, ...)
{
	va_list		ap;
	int			ret;
	char		buff[0xF0000];

	va_start(ap, format);
	ret = vprintf(format, ap);
	return ret;
}
