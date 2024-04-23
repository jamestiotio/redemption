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

#include "mod/internal/widget/group_box.hpp"
#include "core/RDP/orders/RDPOrdersPrimaryOpaqueRect.hpp"
#include "core/font.hpp"
#include "gdi/graphic_api.hpp"
#include "gdi/text_metrics.hpp"
#include "utils/sugar/cast.hpp"
#include "utils/utf.hpp"


WidgetGroupBox::WidgetGroupBox(
    gdi::GraphicApi & drawable, chars_view text,
    Color fgcolor, Color bgcolor, Font const & font
)
  : WidgetComposite(drawable, Focusable::Yes)
  , caption(truncatable_bounded_array_view(text))
  , fg_color(fgcolor)
  , font(font)
{
    this->set_bg_color(bgcolor);
}

void WidgetGroupBox::rdp_input_invalidate(Rect clip)
{
    Rect rect_intersect = clip.intersect(this->get_rect());

    if (!rect_intersect.isempty()) {
        this->draw_inner_free(rect_intersect, this->get_bg_color());

        // Box.
        const uint16_t border           = 6;
        const uint16_t text_margin      = 6;
        const uint16_t text_indentation = border + text_margin + 4;

        gdi::TextMetrics tm(this->font, this->caption);

        auto gcy = this->cy() - tm.height / 2 - border;
        auto gcx = this->cx() - border * 2 + 1;
        auto px = this->x() + border - 1;

        auto wlabel = text_margin * 2 + tm.width;
        auto y = this->y() + tm.height / 2;

        auto const color_ctx = gdi::ColorCtx::depth24();

        // Top Line and Label
        auto rect1 = Rect(px, y, text_indentation - text_margin - border + 2, 1);
        this->drawable.draw(RDPOpaqueRect(rect1, this->fg_color), rect_intersect, color_ctx);
        gdi::server_draw_text(this->drawable, this->font
                            , this->x() + text_indentation
                            , this->y()
                            , this->caption
                            , this->fg_color
                            , this->get_bg_color()
                            , color_ctx
                            , rect_intersect
                            );
        auto rect2 = Rect(px + wlabel + 4, y, gcx + 1 - wlabel - 4, 1);
        this->drawable.draw(RDPOpaqueRect(rect2, this->fg_color), rect_intersect, color_ctx);
        // Bottom line
        auto rect3 = Rect(px, y + gcy, gcx + 1, 1);
        this->drawable.draw(RDPOpaqueRect(rect3, this->fg_color), rect_intersect, color_ctx);

        // Left border
        auto rect4 = Rect(px, y + 1, 1, gcy - 1);
        this->drawable.draw(RDPOpaqueRect(rect4, this->fg_color), rect_intersect, color_ctx);

        // Right Border
        auto rect5 = Rect(px + gcx, y, 1, gcy);
        this->drawable.draw(RDPOpaqueRect(rect5, this->fg_color), rect_intersect, color_ctx);


        this->invalidate_children(rect_intersect);
    }
}
