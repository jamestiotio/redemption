/*
    This program is free software; you can redistribute it and/or modify it
     under the terms of the GNU General Public License as published by the
     Free Software Foundation; either version 2 of the License, or (at your
     option) any later version.

    This program is distributed in the hope that it will be useful, but
     WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
     Public License for more details.

    You should have received a copy of the GNU General Public License along
     with this program; if not, write to the Free Software Foundation, Inc.,
     675 Mass Ave, Cambridge, MA 02139, USA.

    Product name: redemption, a FLOSS RDP proxy
    Copyright (C) Wallix 2013
    Author(s): Christophe Grosjean, Meng Tan, Raphael Zhou
*/

#pragma once

#include "mod/internal/widget/composite.hpp"
#include "utils/sugar/array_view.hpp"
#include "utils/static_string.hpp"

class Font;

class WidgetGroupBox : public WidgetComposite
{
public:
    WidgetGroupBox( gdi::GraphicApi & drawable, chars_view text
                  , Color fgcolor, Color bgcolor, Font const & font);

    void rdp_input_invalidate(Rect clip) override;

private:
    static_string<255> caption;

    Color fg_color;

    Font const & font;
};
