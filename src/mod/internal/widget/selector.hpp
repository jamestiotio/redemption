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
 *   Copyright (C) Wallix 2010-2013
 *   Author(s): Christophe Grosjean, Dominique Lafages, Jonathan Poelen,
 *              Meng Tan, Jennifer Inthavong
 *
 */

#pragma once

#include "mod/internal/widget/composite.hpp"
#include "mod/internal/widget/label.hpp"
#include "mod/internal/widget/edit.hpp"
#include "mod/internal/widget/number_edit.hpp"
#include "mod/internal/widget/button.hpp"
#include "mod/internal/widget/labelgrid.hpp"
#include "utils/translation.hpp"

class Theme;

struct WidgetSelectorParams
{
    static constexpr uint16_t nb_max_columns = 3;

    uint16_t nb_columns = 0;
    chars_view label[nb_max_columns] = {};
    uint32_t weight[nb_max_columns] = {};
};


class WidgetSelector : public WidgetComposite
{
public:
    class temporary_number_of_page
    {
        char buffer[32];
        std::size_t len;

    public:
        explicit temporary_number_of_page(chars_view s);

        operator chars_view () const
        {
            return {buffer, len};
        }
    };

    struct Events
    {
        WidgetEventNotifier onconnect;
        WidgetEventNotifier oncancel;

        WidgetEventNotifier onfilter;

        WidgetEventNotifier onfirst_page;
        WidgetEventNotifier onprev_page;
        WidgetEventNotifier oncurrent_page;
        WidgetEventNotifier onnext_page;
        WidgetEventNotifier onlast_page;

        WidgetEventNotifier onctrl_shift;
    };

    WidgetSelector(gdi::GraphicApi & drawable, CopyPaste & copy_paste,
                   WidgetTooltipShower & tooltip_shower,
                   chars_view device_name,
                   int16_t left, int16_t top, uint16_t width, uint16_t height,
                   Events events,
                   chars_view current_page,
                   chars_view number_of_page,
                   WidgetButton * extra_button,
                   WidgetSelectorParams const & selector_params,
                   Font const & font, Theme const & theme, Language lang,
                   bool has_target_helpicon = false);

    void move_size_widget(int16_t left, int16_t top, uint16_t width, uint16_t height);

    void ask_for_connection();

    void add_device(array_view<chars_view> entries);

    void rdp_input_scancode(KbdFlags flags, Scancode scancode, uint32_t event_time, Keymap const& keymap) override;

    void rdp_input_mouse(uint16_t device_flags, uint16_t x, uint16_t y) override;

private:
    void rearrange();

    struct TooltipShower final : WidgetTooltipShower
    {
        TooltipShower(WidgetSelector & selector)
        : selector(selector)
        {}

        void show_tooltip(
            chars_view text, int x, int y,
            Rect const preferred_display_rect,
            Rect const mouse_area) override;

    private:
        WidgetSelector & selector;
    };

    TooltipShower tooltip_shower;
    WidgetTooltipShower & tooltip_shower_parent;

    WidgetEventNotifier onconnect;
    WidgetEventNotifier oncancel;
    WidgetEventNotifier onctrl_shift;

    bool less_than_800;
    const uint16_t nb_columns;

    WidgetLabel device_label;

    std::array<WidgetLabel, WidgetSelectorParams::nb_max_columns> header_labels;

    std::array<WidgetButton, WidgetSelectorParams::nb_max_columns> column_expansion_buttons;

    uint16_t current_columns_width[WidgetSelectorParams::nb_max_columns] = { 0 };

    int priority_column_index = -1;

public:
    std::array<WidgetEdit, WidgetSelectorParams::nb_max_columns>  edit_filters;

    WidgetLabelGrid selector_lines;

    WidgetButton first_page;
private:
    WidgetButton prev_page;
public:
    WidgetNumberEdit current_page;
    WidgetLabel number_page;
private:
    WidgetButton next_page;
    WidgetButton last_page;
    WidgetButton logout;
    WidgetButton apply;
    WidgetButton connect;

    WidgetButton target_helpicon;

    Translator tr;

    Font const & font;

    int16_t left;
    int16_t top;

public:
    enum {
        IDX_TARGETGROUP,
        IDX_TARGET,
        IDX_PROTOCOL,
        IDX_CLOSETIME
    };

private:
    enum {
        HORIZONTAL_MARGIN = 15,
        VERTICAL_MARGIN = 10,
        TEXT_MARGIN = 20,
        FILTER_SEPARATOR = 5,
        NAV_SEPARATOR = 15
    };

    WidgetButton * extra_button;

    uint32_t weight[WidgetSelectorParams::nb_max_columns] = {0};

    chars_view label[WidgetSelectorParams::nb_max_columns] = {nullptr};
};
