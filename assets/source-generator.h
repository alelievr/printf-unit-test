/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   source-generator.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created  2016/12/22 19:07:07 by alelievr          #+#    #+#             */
/*   Updated  2016/12/23 17:41:49 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma clang diagnostic ignored "-Wc++98-compat"
#pragma clang diagnostic ignored "-Wreserved-user-defined-literal"
#pragma clang diagnostic ignored "-Wold-style-cast"

#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <list>
#include <string>
#include <array>
#include <map>

#define	OUT_FOLDER				"/tmp/print-unit-test/"
#define NO_FLAG					"\x98"
#define PRINTF_FLAGS_BASIC		NO_FLAG"#0-+ "
#define NOALIGN					INT_MIN
#define NOPADD					NOALIGN
#define PRINTF_FLAGS_ADVANCED	"'L"

#define CHAR_MASK	0x0000000000000FF
#define HOST_MASK	0x00000000000FFFF
#define INT_MASK	0x00000000FFFFFFFF
#define LONG_MASK	0xFFFFFFFFFFFFFFFF

#define FILE_HEADER_TEMPLATE	"#include <string.h>\n#include <unistd.h>\n#include <stdint.h>\n#include <stddef.h>\n#include <wchar.h>\n\nextern int\t\tft_printf(const char *, ...);\nextern int\t\tprintf(const char *, ...);\nextern void\t\tprintf_diff_error(int, int);\n\n"
#define FILE_CONTENT_TEMPLATE	"void printf_unit_test_%c_%.7i(char *b)\n{\n\tint d1, d2;\n\tstrcpy(b, \"%s\");\n\td1 = ft_printf(\"%s\", %s%s);\n\twrite(1, \"\\x99\", 1);\n\td2 = printf(\"%s\", %s%s);\n\tif (d1 != d2)\n\t\tprintf_diff_error(d1, d2);\n}\n"

#define FILE_TEMPLATE			FILE_HEADER_TEMPLATE FILE_CONTENT_TEMPLATE

static const char *		rand_strings[] = {"olol", "#42#", "\\\\!/", "^.^/"};

static const char		flag_to_flag_incompatibilities[][2] = {
	{' ', '+'},
	{'0', '-'},
};

static const char		convertion_to_flag_incompatibilities[][2] = {
	{'i', '#'},

	{'d', '#'},

	{'o', '+'},
	{'o', ' '},

	{'u', '+'},
	{'u', ' '},
	{'u', '#'},

	{'x', '+'},
	{'x', ' '},

	{'X', '+'},
	{'X', ' '},

	{'D', '#'},

	{'O', '+'},
	{'O', ' '},

	{'U', '+'},
	{'U', ' '},
	{'U', '#'},

	{'c', '0'},
	{'c', '+'},
	{'c', ' '},
	{'c', '#'},

	{'C', '0'},
	{'C', '+'},
	{'C', ' '},
	{'C', '#'},

	{'s', '0'},
	{'s', '+'},
	{'s', ' '},
	{'s', '#'},

	{'S', '0'},
	{'S', '+'},
	{'S', ' '},
	{'S', '#'},

	{'p', '0'},
	{'p', '+'},
	{'p', ' '},
	{'p', '#'},
};
