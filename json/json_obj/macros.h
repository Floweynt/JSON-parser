// SPDX-License-Identifier: MIT
// Copyright 2020 Ruiqi Li

#ifndef __CFP_JSON_MACROS_H__
#define __CFP_JSON_MACROS_H__
#ifdef _MSC_VER
#pragma warning(disable: 26812)
#pragma warning(disable: 4996)
#endif


#ifdef PRINT_EXIT
constexpr bool IS_DEBUG_CONSTEXPR = true;
#else
constexpr bool IS_DEBUG_CONSTEXPR = false;
#endif

#define __RETURN__(code) {if (IS_DEBUG_CONSTEXPR) std::cout << "return line: " << __LINE__ << '\n'; return code; }
#endif