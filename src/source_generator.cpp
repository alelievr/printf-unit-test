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
	{
		ints.push_back(NOALIGN);
		ints.push_back(2);
	}

	for (size_t i = 0; i < num; i++)
	{
		r = ((intmax_t)rand() * (intmax_t)rand()) & mask;
		if (r == 0 || r == NOALIGN)
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

std::list< std::string >			generateTestFormats(char convertion)
{
	std::string					fmt;
	std::list< std::string >	retFormats;

	srand((unsigned)time(NULL) + (unsigned)clock());

	for (const char & c1 : PRINTF_FLAGS_BASIC) //first flag
	for (const char & c2 : PRINTF_FLAGS_BASIC) //second flag
	for (const char & c3 : PRINTF_FLAGS_BASIC) //...
	for (const char & c4 : PRINTF_FLAGS_BASIC)
	for (const char & c5 : PRINTF_FLAGS_BASIC)
	for (const intmax_t & align : generateRandomNumbers(2, 0x3F, true)) //align attribute
	for (const intmax_t & padd : generateRandomNumbers(2, 0x3F, true)) //padding attribute
	for (const std::string & modifier : generateModifiers(convertion))
	for (const std::string & prefix : generateRandomStrings(1)) //additional string to format
	for (const std::string & sufix : generateRandomStrings(0)) //additional string to format
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
		if (!strchr("pcCS", convertion) && padd != NOPADD)
			fmt += "." + std::to_string(padd);
		fmt += modifier + convertion + sufix + prefix; //to test with string before and after
		retFormats.push_front(fmt);
		abortCurrentFormat:
		;
	}
	return retFormats;
}
