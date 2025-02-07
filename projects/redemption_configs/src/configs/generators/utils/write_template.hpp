/*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*   Product name: redemption, a FLOSS RDP proxy
*   Copyright (C) Wallix 2010-2016
*   Author(s): Jonathan Poelen
*/

#pragma once

#include <iostream>
#include <iomanip>
#include <string_view>

#include <cstring>


namespace cfg_generators
{

struct io_prefix_lines
{
    std::string_view s;
    std::string_view prefix;
    std::string_view suffix;
    unsigned space;

    friend std::ostream & operator<<(std::ostream & out, io_prefix_lines const & comment)
    {
        auto first = comment.s.begin();
        auto last = comment.s.end();
        auto const prefix_size = comment.prefix.size();
        while (first != last) {
            auto s_point = first;
            for (; first != last && '\n' != *first; ++first) {
            }
            out << std::setw(int(comment.space + prefix_size)) << comment.prefix;
            out.write(s_point, first - s_point);
            if (first != last && *first == '\n') {
                ++first;
            }
            out << comment.suffix << '\n';

        }
        return out;
    }
};

inline io_prefix_lines cpp_comment(std::string_view s, unsigned space)
{
    return io_prefix_lines{s, "// ", "", space};
}

inline io_prefix_lines cpp_doxygen_comment(std::string_view s, unsigned space)
{
    return io_prefix_lines{s, "/// ", " <br/>", space};
}

inline io_prefix_lines python_comment(std::string_view s, unsigned space)
{
    return io_prefix_lines{s, "# ", "", space};
}

struct io_upper
{
    std::string_view s;

    friend std::ostream& operator<<(std::ostream& out, io_upper const& u)
    {
        for (char c : u.s) {
            out << char(('a' <= c && c <= 'z') ? c - 'a' + 'A' : c);
        }
        return out;
    }
};

constexpr std::string_view do_not_edit =
    "DO NOT EDIT THIS FILE BY HAND -- YOUR CHANGES WILL BE OVERWRITTEN\n"
;

}
