#pragma once
//========================================================================
// String.h : Defines some useful string utility functions
//
// Part of the GameCode4 Application
//
// GameCode4 is the sample application that encapsulates much of the source code
// discussed in "Game Coding Complete - 4th Edition" by Mike McShaffry and David
// "Rez" Graham, published by Charles River Media. 
// ISBN-10: 1133776574 | ISBN-13: 978-1133776574
//
// If this source code has found it's way to you, and you think it has helped you
// in any way, do the authors a favor and buy a new copy of the book - there are 
// detailed explanations in it that compliment this code well. Buy a copy at Amazon.com
// by clicking here: 
//    http://www.amazon.com/gp/product/1133776574/ref=olp_product_details?ie=UTF8&me=&seller=
//
// There's a companion web site at http://www.mcshaffry.com/GameCode/
// 
// The source code is managed and maintained through Google Code: 
//    http://code.google.com/p/gamecode4/
//
// (c) Copyright 2012 Michael L. McShaffry and David Graham
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser GPL v3
// as published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See 
// http://www.gnu.org/licenses/lgpl-3.0.txt for more details.
//
// You should have received a copy of the GNU Lesser GPL v3
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
//========================================================================

#define MAX_DIGITS_IN_INT 12  //max num of digits in an int (-2147483647 = 11 digits, +1 for the '\0')
typedef std::vector<std::string> StringVec;

//removes characters up to the first \n
extern void RemoveFirstLine(std::wstring& src, std::wstring& result);

//removes leading white space
extern void TrimLeft(std::wstring& s);

//counts the number of lines in a block of text
extern int CountLines(const std::wstring& s);

//does a classic * & ? pattern match on a file name - case sensitive
extern BOOL WildcardMatch(const char* pat, const char* str);

//converts a regular string to a wide string
extern void StringToWideString(const std::string& source, std::wstring& out);

extern HRESULT AnsiToWideCch(WCHAR* dest, const CHAR* src, int count);
extern HRESULT WideToAnsiCch(CHAR* dest, const WCHAR* src, int count);
extern HRESULT GenericToAnsiCch(CHAR* dest, const TCHAR* src, int count);
extern HRESULT GenericToWideCch(WCHAR* dest, const TCHAR* src, int count);
extern HRESULT AnsiToGenericCch(TCHAR* dest, const CHAR* src, int count);
extern HRESULT WideToGenericCch(TCHAR* dest, const WCHAR* src, int count);

extern std::string ToStr(int num, int base = 10);
extern std::string ToStr(unsigned int num, int base = 10);
extern std::string ToStr(unsigned long num, int base = 10);
extern std::string ToStr(float num);
extern std::string ToStr(double num);
extern std::string ToStr(bool val);
extern std::string ToStr(const Vec3& vec);

extern std::string ws2s(const std::wstring& s);
extern std::wstring s2ws(const std::string& s);

// Splits a string by the delimeter into a vector of strings.  For example, say you have the following string:
// std::string test("one,two,three");
// You could call Split() like this:
// Split(test, outVec, ',');
// outVec will have the following values:
// "one", "two", "three"
void Split(const std::string& str, StringVec& vec, char delimiter);
