/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   source-generator.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/12/22 16:20:34 by alelievr          #+#    #+#             */
/*   Updated: 2016/12/22 21:07:51 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "source-generator.h"

static int							g_index = 0;

static std::list< intmax_t >		generateRandomNumbers(size_t num, intmax_t mask, bool sup = false)
{
	std::list< intmax_t >	ints;

	if (sup)
		ints.push_back(NOALIGN);

	ints.push_back(0);

	for (size_t i = 0; i < num; i++)
		ints.push_back(((intmax_t)rand() * (intmax_t)rand()) & mask);
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

static void							generateBasicTests(char convertion, std::string arg)
{
	char				buff[0xF00];
	std::ofstream		ofs;
	std::string			fmt;
	const char			*format;
	const char			*sarg;
	std::ostringstream	ss;

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

	//for all
	for (const char & c1 : PRINTF_FLAGS_BASIC) //first flag
	for (const char & c2 : PRINTF_FLAGS_BASIC) //second flag
	for (const char & c3 : PRINTF_FLAGS_BASIC) //...
	for (const char & c4 : PRINTF_FLAGS_BASIC)
	for (const char & c5 : PRINTF_FLAGS_BASIC)
	for (const char & c6 : PRINTF_FLAGS_BASIC)
	for (const intmax_t & align : generateRandomNumbers(1, CHAR_MASK, true)) //align attribute
	for (const intmax_t & padd : generateRandomNumbers(1, CHAR_MASK, true)) //padding attribute
	for (const std::string & prefix : generateRandomStrings(1)) //additional string to format
	for (const std::string & sufix : generateRandomStrings(1)) //additional string to format
	{
		fmt = prefix + "%";
		for (const char & c : {c1, c2, c3, c4, c5, c6})
			if (c != NO_FLAG[0])
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
		if (padd != NOPADD)
			fmt += "." + std::to_string(padd);
		fmt += convertion + sufix;
		format = fmt.c_str();
		sarg = arg.c_str();
		ss.str("");
		ss << OUT_FOLDER << "printf_unit_test_" << std::setfill('0') << std::setw(7) << g_index << ".c";
		ofs.open(ss.str(), std::ofstream::out | std::ofstream::trunc | std::ofstream::in);
		sprintf(buff, FILE_TEMPLATE, g_index, format, format, sarg, format, sarg);
		ofs << buff;
		ofs.close();
		g_index++;
		std::cout << "created test file with format: " << fmt << std::endl;
		abortCurrentFormat:
		;
	}
}

static void							generateTestFiles(void)
{
	std::string		randint;

	randint = std::to_string(rand());
	generateBasicTests('d', randint);
}

int									main(void)
{
	system("rm -rf "OUT_FOLDER);
	mkdir(OUT_FOLDER, 0755);

	srand((unsigned)time(NULL) + (unsigned)clock());

	//create all test files
	generateTestFiles();

	//compile these files
	
	//move all compiled sources to the true test source folder

	//compile all in a big library
	return (0);
}
