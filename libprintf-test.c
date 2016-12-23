#include <stdarg.h>
#include <stdio.h>

int		ft_printf(const char *format, ...)
{
	va_list		ap;

	va_start(ap, format);
	return vprintf(format, ap);
}
