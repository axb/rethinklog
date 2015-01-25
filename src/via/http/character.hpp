#ifndef CHARACTER_HPP_VIA_HTTPLIB_
#define CHARACTER_HPP_VIA_HTTPLIB_

#pragma once

//////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 Ken Barker
// (ken dot barker at via-technology dot co dot uk)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////
/// @file character.hpp
/// @brief Low level functions to classify characters and manipulate strings.
//////////////////////////////////////////////////////////////////////////////
#include <cctype>
#include <string>
#include <climits>

namespace via
{
  namespace http
  {
    /// The standard HTTP line terminator.
    extern const std::string CRLF;

    /// Test whether a character is an end of line character,
    /// i.e. CR or LF.
    /// @param c the character
    /// @return true if character is CR or LF, false otherwise.
    inline bool is_end_of_line(char c)
    { return ('\r' == c) || ('\n' == c); }

    /// Test whether a character is a space or tab character.
    /// Note: equivalent to C++11 std::isblank in <cctype>
    /// @param c the character
    /// @return true if character is space or tab, false otherwise.
    inline bool is_space_or_tab(char c)
    { return (' ' == c) || ('\t' == c); }

    /// Test whether a character is a control character.
    /// @param c the character
    /// @return true if character is control character, false otherwise.
    inline bool is_ctl(char c)
    { return ((0 <= c) && (31 >= c)) || (127 ==c); }

    /// Test whether a character is a separator character.
    /// @param c the character
    /// @return true if character is a separator character, false otherwise.
    bool is_separator(char c);

    /// Test whether a character is a token character.
    /// i.e. not a control or separator character.
    /// @param c the character
    /// @return true if character is a token character, false otherwise.
    inline bool is_token(char c)
    { return !is_ctl(c) && !is_separator(c); }

    /// Convert a digit charcter to an integer.
    /// @pre the character must be a valid digit character.
    /// @param c the character
    /// @return the integer equivalent of the character
    inline int read_digit(char c)
    { return (c -'0');}

    /// The http version string, i.e. HTTP/1.1
    /// @param major_version the http major version number.
    /// @param minor_version the http minor version number.
    /// @return the http string for the given version.
    std::string http_version(int major_version, int minor_version);

    /// Convert a string representing a hexadecimal number to an unsigned int.
    /// @param hex_string the string containing a vald hexadecimal number
    /// @return the number represented by the string, ULONG_MAX if invalid.
    size_t from_hex_string(const std::string& hex_string);

    /// Convert an unsigned int into a hexadecimal string.
    /// @param number to be represented
    /// @return the string containing the number in hexadecimal.
    std::string to_hex_string(size_t number);

    /// Convert a string representing a decimal number to an unsigned int.
    /// @param dec_string the string containing a vald decimal number
    /// @return the number represented by the string, ULONG_MAX if invalid.
    size_t from_dec_string(const std::string& dec_string);

    /// Convert an int into a decimal string.
    /// @param number to be represented
    /// @return the string containing the number in decimal.
    inline std::string to_dec_string(int number)
    { return std::to_string(number); }
  }
}

#endif

