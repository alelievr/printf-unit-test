/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   source-generator.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created  2016/12/22 16:20:34 by alelievr          #+#    #+#             */
/*   Updated  2016/12/23 17:41:50 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "source-generator.h"

static std::list< intmax_t >		generateRandomNumbers(size_t num, intmax_t mask, bool sup = false)
{
	std::list< intmax_t >	ints;
	intmax_t				r;

	if (sup)
		ints.push_back(NOALIGN);

	for (size_t i = 0; i < num; i++)
	{
		r = ((intmax_t)rand() * (intmax_t)rand()) & mask;
		if (r == 0)
			r = 42;
		ints.push_back(r);
	}
	return ints;
}

static std::list< std::string >		generateRandomStrings(size_t num)
{
	std::list< std::string >	strs;

	strs.push_back("");

	for (size_t i = 0; i < num; i++)
		strs.push_back(rand_strings[(int)(rand() % (int)(sizeof(rand_strings) / sizeof(char *)))]);
	return strs;
}

static std::list< std::string >		generateModifiers(char convertion)
{
	std::list< std::string >		mods;

	mods.push_back("");
	if (strchr("aAeEfFgGcs", convertion))
		mods.push_back("l");
	if (strchr("idouxXDOU", convertion))
	{
		mods.push_back("ll");
		mods.push_back("h");
		mods.push_back("hh");
		mods.push_back("j");
		mods.push_back("z");
	}
	return mods;
}

static std::map< std::string, std::string > initModMap()
{
	return std::map< std::string, std::string > {
		{"", ""},
		{"l", "(long)"},
		{"ll", "(long long)"},
		{"h", "(short)"},
		{"hh", "(char)"},
		{"j", "(intmax_t)"},
		{"z", "(size_t)"},
		{"lc", "(wint_t)"},
		{"ls", "(wchar_t *)"},
		{"lf", "(double)"},
	};
}

static const char					*getCastFromModifier(const std::string & mod, const char convertion)
{
	const static std::map< std::string, std::string > & modMap = initModMap();
	std::string											m = const_cast< std::string & >(mod);

	if (strchr("csf", convertion))
		m += convertion;
	return (modMap.find(m)->second.c_str());
}

static void							generateBasicTests(char convertion, std::string arg)
{
	char				buff[0xF00];
	std::ofstream		ofs;
	std::string			fmt;
	const char			*format;
	const char			*sarg;
	std::ostringstream	ss;
	std::string			file(FILE_HEADER_TEMPLATE);
	int					g_index = 0;

	//with padd
	//without padd
	//with align
	//without align
	//with both
	//with string after
	//without string after
	//with string before
	//without string before
	//with both

	for (const char & c1 : PRINTF_FLAGS_BASIC) //first flag
	for (const char & c2 : PRINTF_FLAGS_BASIC) //second flag
	for (const char & c3 : PRINTF_FLAGS_BASIC) //...
	for (const char & c4 : PRINTF_FLAGS_BASIC)
	for (const char & c5 : PRINTF_FLAGS_BASIC)
	for (const intmax_t & align : generateRandomNumbers(2, CHAR_MASK, true)) //align attribute
	for (const intmax_t & padd : generateRandomNumbers(2, CHAR_MASK, true)) //padding attribute
	for (const std::string & modifier : generateModifiers(convertion))
	for (const std::string & prefix : generateRandomStrings(1)) //additional string to format
	for (const std::string & sufix : generateRandomStrings(1)) //additional string to format
	{
		fmt = prefix + "%";
		for (const char & c : {c1, c2, c3, c4, c5})
			if (c && c != NO_FLAG[0])
			{
				if (fmt.find(c) != std::string::npos) //flag duplication, aborting ...
					goto abortCurrentFormat;
				fmt += c;
			}
		//check converter to flag incompat
		for (size_t i = 0; i < sizeof(convertion_to_flag_incompatibilities) / sizeof(char[2]); i++)
			if (convertion == convertion_to_flag_incompatibilities[i][0] && fmt.find(convertion_to_flag_incompatibilities[i][1]) != std::string::npos)
				goto abortCurrentFormat;
		//check flag to flag incompat
		for (size_t i = 0; i < sizeof(flag_to_flag_incompatibilities) / sizeof(char[2]); i++)
			if (fmt.find(flag_to_flag_incompatibilities[i][0]) != std::string::npos && fmt.find(flag_to_flag_incompatibilities[i][1]) != std::string::npos)
				goto abortCurrentFormat;
		if (align != NOALIGN)
			fmt += std::to_string(align);
		if (!strchr("pc", convertion) && padd != NOPADD)
			fmt += "." + std::to_string(padd);
		fmt += modifier + convertion + sufix;
		format = fmt.c_str();
		sarg = arg.c_str();
//		ss.str("");
		sprintf(buff, FILE_CONTENT_TEMPLATE,
				convertion,
				g_index,
				format,
				format, getCastFromModifier(modifier, convertion), sarg,
				format, getCastFromModifier(modifier, convertion), sarg);
		file += buff;
		g_index++;
		std::cout << "created test file with format: \"" << fmt << "\"" << std::endl;
		abortCurrentFormat:
		;
	}
	ss << OUT_FOLDER << "printf_unit_test_" << convertion << ".c";
	ofs.open(ss.str(), std::ofstream::out | std::ofstream::trunc | std::ofstream::in);
	ofs << file;
	ofs.close();
}

static void							generateTestFiles(void)
{
	std::string		randint;
	std::string		randptr;

	randint = std::to_string(rand());
	generateBasicTests('d', randint);
	randint = std::to_string(rand());
	generateBasicTests('o', randint);
	randint = std::to_string(rand());
	generateBasicTests('u', randint);
	randint = std::to_string(rand());
	generateBasicTests('X', randint);
	randptr = "(void *)0x42";
	generateBasicTests('p', randptr);
	generateBasicTests('s', "\"yolo\"");
	generateBasicTests('c', "'*'");
//	generateBasicTests('S', "こんにちは");
}

int									main(void)
{
	mkdir(OUT_FOLDER, 0755);

	srand((unsigned)time(NULL) + (unsigned)clock());

	//create all test files
	generateTestFiles();

	//compile these files
	
	//move all compiled sources to the true test source folder

	//compile all in a big library
	return (0);
}
