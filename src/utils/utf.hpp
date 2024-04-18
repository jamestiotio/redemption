/*
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   Product name: redemption, a FLOSS RDP proxy
   Copyright (C) Wallix 2012
   Author(s): Christophe Grosjean, Javier Caverni, Raphael Zhou, Meng Tan,
              Jennifer Inthavong
   Based on xrdp Copyright (C) Jay Sorg 2004-2010

   stream object, used for input / output communication between
   entities
*/

#pragma once

#include "utils/sugar/bytes_view.hpp"
#include "utils/only_type.hpp"
#include "cxx/cxx.hpp"

#include <type_traits>
#include <cstdint>

enum {
      maximum_length_of_utf8_character_in_bytes = 4
};


// UTF8Len assumes input is valid utf8, zero terminated, that has been checked before
std::size_t UTF8Len(byte_ptr source) noexcept;
std::size_t UTF16ByteLen(bytes_view source) noexcept;

void UTF16Lower(uint8_t * source, std::size_t max_len) noexcept;
void UTF16Upper(uint8_t * source, std::size_t max_len) noexcept;


// UTF8GetLen find the number of bytes of the len first characters of input.
// It assumes input is valid utf8, zero terminated (that has been checked before).
std::size_t UTF8GetPos(uint8_t const * source, std::size_t len) noexcept;

// UTF8Len assumes input is valid utf8, zero terminated, that has been checked before
std::size_t UTF8StringAdjustedNbBytes(const uint8_t * source, std::size_t max_len) noexcept;

// UTF8Len assumes input is valid utf8, zero terminated, that has been checked before
std::size_t UTF8StringAdjustedNbBytes(bytes_view source, std::size_t max_len) noexcept;

// UTF8RemoveOne assumes input is valid utf8, zero terminated, that has been checked before
void UTF8RemoveOne(writable_bytes_view source) noexcept;

// UTF8InsertUtf16 assumes input is valid utf8, zero terminated, that has been checked before
// UTF8InsertUtf16 won't insert anything and return false if modified string buffer does not have enough space to insert
bool UTF8InsertUtf16(writable_bytes_view source, std::size_t bytes_used, uint16_t unicode_char) noexcept;

// UTF8toUTF16 never writes the trailing zero
std::size_t UTF8toUTF16(bytes_view source, uint8_t * target, size_t t_len) noexcept;
std::size_t UTF8toUTF16(bytes_view source, writable_bytes_view target) noexcept;

// UTF8toUTF16 never writes the trailing zero (with Lf to CrLf conversion).
std::size_t UTF8toUTF16_CrLf(bytes_view source, uint8_t * target, std::size_t t_len) noexcept;

template<class ResizableArray>
void UTF8toResizableUTF16(bytes_view utf8_source, ResizableArray& utf16_target)
{
    utf16_target.resize(utf8_source.size() * 4);
    auto len = UTF8toUTF16(utf8_source, make_writable_array_view(utf16_target));
    utf16_target.resize(len);
}

template<class ResizableArray>
ResizableArray UTF8toResizableUTF16(bytes_view utf8_source)
{
    ResizableArray utf16_target;
    UTF8toResizableUTF16(utf8_source, utf16_target);
    return utf16_target;
}

template<class ResizableArray>
void UTF8toResizableUTF16_zstring(bytes_view utf8_source, ResizableArray& utf16_target)
{
    utf16_target.resize(utf8_source.size() * 4);
    auto len = UTF8toUTF16(utf8_source, make_writable_array_view(utf16_target));
    utf16_target[len    ] = '\0';
    utf16_target[len + 1] = '\0';
    utf16_target.resize(len + 2);
}

template<class ResizableArray>
ResizableArray UTF8toResizableUTF16_zstring(bytes_view utf8_source)
{
    ResizableArray utf16_target;
    UTF8toResizableUTF16_zstring(utf8_source, utf16_target);
    return utf16_target;
}


class UTF8toUnicodeIterator
{
public:
    explicit UTF8toUnicodeIterator(byte_ptr str) noexcept;

    UTF8toUnicodeIterator & operator++() noexcept;

    uint32_t operator*() const noexcept
    { return this->ucode; }

    [[nodiscard]] uint32_t code() const noexcept
    { return this->ucode; }

    [[nodiscard]] uint8_t const * pos() const noexcept
    { return this->source; }

private:
    const uint8_t * source;
    uint32_t ucode = 0;
};

// Return number of UTF8 bytes used to encode UTF16 input
// do not write trailing 0
std::size_t UTF16toUTF8(const uint8_t * utf16_source, std::size_t utf16_len, uint8_t * utf8_target, std::size_t target_len) noexcept;
// Return number of UTF8 bytes used to encode UTF16 input
// do not write trailing 0
writable_bytes_view UTF16toUTF8_buf(bytes_view utf16_source, writable_bytes_view utf8_target) noexcept;
writable_bytes_view UTF16toUTF8_buf(only_type<uint16_t> utf16_source, writable_bytes_view utf8_target) noexcept;

template<class ResizableArray>
void UTF16toResizableUTF8(bytes_view utf16_source, ResizableArray& utf8_target)
{
    utf8_target.resize(utf16_source.size() * 2);
    auto len = UTF16toUTF8_buf(utf16_source, make_writable_array_view(utf8_target)).size();
    utf8_target.resize(len);
}

template<class ResizableArray>
ResizableArray UTF16toResizableUTF8(bytes_view utf16_source)
{
    ResizableArray utf8_target;
    UTF16toResizableUTF8(utf16_source, utf8_target);
    return utf8_target;
}

template<class ResizableArray>
void UTF16toResizableUTF8_zstring(bytes_view utf16_source, ResizableArray& utf8_target)
{
    utf8_target.resize(utf16_source.size() * 2 + 1);
    auto len = UTF16toUTF8_buf(utf16_source, make_writable_array_view(utf8_target)).size();
    utf8_target[len] = '\0';
    utf8_target.resize(len + 1);
}

template<class ResizableArray>
ResizableArray UTF16toResizableUTF8_zstring(bytes_view utf16_source)
{
    ResizableArray utf8_target;
    UTF16toResizableUTF8_zstring(utf16_source, utf8_target);
    return utf8_target;
}

// Return number of UTF8 bytes used to encode UTF16 input
// do not write trailing 0
std::size_t UTF16toUTF8(const uint16_t * utf16_source, std::size_t utf16_len, uint8_t * utf8_target, std::size_t target_len) noexcept;

// Return number of UTF8 bytes used to encode UTF32 input
// do not write trailing 0
std::size_t UTF32toUTF8(const uint8_t * utf32_source, std::size_t utf32_len, uint8_t * utf8_target, std::size_t target_len) noexcept;

// Return number of UTF8 bytes used to encode UTF32 input
// do not write trailing 0
std::size_t UTF32toUTF8(uint32_t utf32_char, uint8_t * utf8_target, std::size_t target_len) noexcept;

size_t UTF8CharNbBytes(const uint8_t * source) noexcept;

bool is_utf8_string(uint8_t const * s, int length = -1) noexcept; /*NOLINT*/

bool is_ASCII_string(bytes_view source) noexcept;
bool is_ASCII_string(byte_ptr source) noexcept;

std::size_t UTF8StrLenInChar(byte_ptr source) noexcept;

std::size_t UTF16toLatin1(const uint8_t * utf16_source_, std::size_t utf16_len, uint8_t * latin1_target, std::size_t latin1_len) noexcept;

std::size_t Latin1toUTF16(bytes_view latin1_source,
        uint8_t * utf16_target, std::size_t utf16_len) noexcept;

std::size_t Latin1toUTF8(
    const uint8_t * latin1_source, std::size_t latin1_len,
    uint8_t * utf8_target, std::size_t utf8_len) noexcept;


constexpr uint32_t utf8_2_bytes_to_ucs(uint8_t a, uint8_t b) noexcept
{
    return ((a & 0x1Fu) << 6u) |  (b & 0x3Fu);
}

constexpr uint32_t utf8_3_bytes_to_ucs(uint8_t a, uint8_t b, uint8_t c) noexcept
{
    return ((a & 0x0Fu) << 12) | ((b & 0x3Fu) << 6) |  (c & 0x3Fu);
}

constexpr uint32_t utf8_4_bytes_to_ucs(uint8_t a, uint8_t b, uint8_t c, uint8_t d) noexcept
{
    return ((a & 0x07u) << 18) | ((b & 0x3Fu) << 12) | ((c & 0x3Fu) << 6) | (d & 0x3Fu);
}


struct utf8_char_1byte
{
    uint8_t const* ptr;

    constexpr operator uint32_t () const noexcept { return *ptr; }
    constexpr operator bytes_view () const noexcept { return {ptr, 1}; }

    constexpr uint32_t unicode() const noexcept { return *this; }
    constexpr bytes_view bytes() const noexcept { return *this; }
};

struct utf8_char_2bytes
{
    uint8_t const* ptr;

    constexpr operator uint32_t () const noexcept { return utf8_2_bytes_to_ucs(ptr[0], ptr[1]); }
    constexpr operator bytes_view () const noexcept { return {ptr, 2}; }

    constexpr uint32_t unicode() const noexcept { return *this; }
    constexpr bytes_view bytes() const noexcept { return *this; }
};

struct utf8_char_3bytes
{
    uint8_t const* ptr;

    constexpr operator uint32_t () const noexcept { return utf8_3_bytes_to_ucs(ptr[0], ptr[1], ptr[2]); }
    constexpr operator bytes_view () const noexcept { return {ptr, 3}; }

    constexpr uint32_t unicode() const noexcept { return *this; }
    constexpr bytes_view bytes() const noexcept { return *this; }
};

struct utf8_char_4bytes
{
    uint8_t const* ptr;

    constexpr operator uint32_t () const noexcept { return utf8_4_bytes_to_ucs(ptr[0], ptr[1], ptr[2], ptr[3]); }
    constexpr operator bytes_view () const noexcept { return {ptr, 4}; }

    constexpr uint32_t unicode() const noexcept { return *this; }
    constexpr bytes_view bytes() const noexcept { return *this; }
};

struct utf8_char_invalid
{
    bytes_view data;

    constexpr bytes_view remaining() const noexcept { return data; }

    constexpr operator uint32_t () const noexcept { return 0xFFFD; }
    constexpr operator bytes_view () const noexcept { return data.first(1); }

    constexpr uint32_t unicode() const noexcept { return *this; }
    constexpr bytes_view bytes() const noexcept { return *this; }
};

struct utf8_char_truncated
{
    bytes_view data;

    constexpr operator uint32_t () const noexcept { return 0xFFFD; }
    constexpr operator bytes_view () const noexcept { return data; }

    constexpr uint32_t unicode() const noexcept { return *this; }
    constexpr bytes_view bytes() const noexcept { return *this; }
};

struct utf8_char_generic
{
    constexpr utf8_char_generic() = default;
    constexpr utf8_char_generic(utf8_char_1byte c) : av(c), uc(c) {}
    constexpr utf8_char_generic(utf8_char_2bytes c) : av(c), uc(c) {}
    constexpr utf8_char_generic(utf8_char_3bytes c) : av(c), uc(c) {}
    constexpr utf8_char_generic(utf8_char_4bytes c) : av(c), uc(c) {}
    constexpr utf8_char_generic(utf8_char_invalid c) : av(c), uc(c) {}
    constexpr utf8_char_generic(utf8_char_truncated c) : av(c), uc(c) {}
    constexpr utf8_char_generic(bytes_view av, uint32_t uc) : av(av), uc(uc) {}

    constexpr operator uint32_t () const noexcept { return uc; }
    constexpr operator bytes_view () const noexcept { return av; }

    constexpr uint32_t unicode() const noexcept { return uc; }
    constexpr bytes_view bytes() const noexcept { return av; }

private:
    bytes_view av;
    uint32_t uc = 0;
};

struct utf8_decode_new_offset
{
    uint8_t const* p;
};


/// Read a utf8 sequence is call function for each unicode point.
/// ChFn is function with as parameter
///     - utf8_char_1byte or
///     - utf8_char_2bytes or
///     - utf8_char_3bytes or
///     - utf8_char_4bytes
/// ChErrorFn is function with as parameter
///     - utf8_char_invalid
/// TruncatedFn is function with as parameter
///     - utf8_char_truncated
/// ChFn, ChErrorFn and TruncatedFn result type are possibly
///     - void
///     - bool with false for stop the function
///     - utf8_decode_new_offset
/// TruncatedFn stop always the function with
/// \return unread bytes
template<class ChFn, class ChErrorFn, class TruncatedFn>
bytes_view utf8_for_each(bytes_view utf8, ChFn&& ch_fn, ChErrorFn&& err_fn, TruncatedFn&& truncated_fn)
{
    auto source = utf8.begin();
    auto last = utf8.end();

#define UTF8_FOR_EACH_PROCESS(fn, ...) do {                              \
    using result_type = decltype(fn(__VA_ARGS__));                       \
    if constexpr (std::is_same_v<utf8_decode_new_offset, result_type>) { \
        auto new_offset = fn(__VA_ARGS__);                               \
        assert(source < new_offset.p && new_offset.p <= last             \
            && "infinite loop or invalid pointer");                      \
        source = new_offset.p;                                           \
        continue;                                                        \
    }                                                                    \
    else if constexpr (!std::is_void_v<result_type>) {                   \
        if (!fn(__VA_ARGS__)) {                                          \
            return {source, last};                                       \
        }                                                                \
    }                                                                    \
    else {                                                               \
        fn(__VA_ARGS__);                                                 \
    }                                                                    \
} while (0)

#define UTF8_FOR_EACH_TRUNCATED_PROCESS() do {                                     \
    auto truncated_av = bytes_view{source, last};                                  \
    using result_type = decltype(truncated_fn(utf8_char_truncated{truncated_av})); \
    if constexpr (std::is_same_v<utf8_decode_new_offset, result_type>) {           \
        auto new_offset = truncated_fn(utf8_char_truncated{truncated_av});         \
        assert(source <= new_offset.p && new_offset.p <= last                      \
            && "invalid pointer");                                                 \
        source = new_offset.p;                                                     \
    }                                                                              \
    else {                                                                         \
        truncated_fn(utf8_char_truncated{truncated_av});                           \
    }                                                                              \
} while (0)

    // fast loop with unchecked size
    if (utf8.size() >= 4) {
        while (source <= last - 4) {
            switch (*source >> 4) {
                [[likely]]
                case 0:
                case 1: case 2: case 3:
                case 4: case 5: case 6: case 7:
                    UTF8_FOR_EACH_PROCESS(ch_fn, utf8_char_1byte{source});
                    source += 1;
                    break;

                /* handle U+0080..U+07FF inline : 2 bytes sequences */
                case 0xC: case 0xD:
                    UTF8_FOR_EACH_PROCESS(ch_fn, utf8_char_2bytes{source});
                    source += 2;
                    break;

                /* handle U+8FFF..U+FFFF inline : 3 bytes sequences */
                case 0xE:
                    UTF8_FOR_EACH_PROCESS(ch_fn, utf8_char_3bytes{source});
                    source += 3;
                    break;

                case 0xF:
                    UTF8_FOR_EACH_PROCESS(ch_fn, utf8_char_4bytes{source});
                    source += 4;
                    break;

                // these should never happen on valid UTF8
                [[unlikely]]
                case 8: case 9: case 0xA: case 0xB:
                    UTF8_FOR_EACH_PROCESS(err_fn, utf8_char_invalid{{source, last}});
                    source += 1;
                    break;

                default:
                    REDEMPTION_UNREACHABLE();
            }
        }
    }

    // last loops with checked size
    while (source < last) {
        switch (*source >> 4) {
            [[likely]]
            case 0:
            case 1: case 2: case 3:
            case 4: case 5: case 6: case 7:
                UTF8_FOR_EACH_PROCESS(ch_fn, utf8_char_1byte{source});
                source += 1;
                break;

            /* handle U+0080..U+07FF inline : 2 bytes sequences */
            case 0xC: case 0xD:
                if (last - source >= 2) [[likely]] {
                    UTF8_FOR_EACH_PROCESS(ch_fn, utf8_char_2bytes{source});
                    source += 2;
                }
                else {
                    UTF8_FOR_EACH_TRUNCATED_PROCESS();
                    return {source, last};
                }
                break;
                /* handle U+8FFF..U+FFFF inline : 3 bytes sequences */

            case 0xE:
                if (last - source >= 3) [[likely]] {
                    UTF8_FOR_EACH_PROCESS(ch_fn, utf8_char_3bytes{source});
                    source += 3;
                }
                else {
                    UTF8_FOR_EACH_TRUNCATED_PROCESS();
                    return {source, last};
                }
                break;

            case 0xF:
                UTF8_FOR_EACH_TRUNCATED_PROCESS();
                return {source, last};

            // these should never happen on valid UTF8
            [[unlikely]]
            case 8: case 9: case 0xA: case 0xB:
                UTF8_FOR_EACH_PROCESS(err_fn, utf8_char_invalid{{source, last}});
                source += 1;
                break;

            default:
                REDEMPTION_UNREACHABLE();
        }
    }

    return {source, last};

#undef UTF8_FOR_EACH_PROCESS
}

template<class ChFn>
bytes_view utf8_for_each(bytes_view utf8, ChFn&& ch_fn)
{
    return utf8_for_each(utf8, ch_fn, ch_fn, ch_fn);
}

/// Read a utf8 sequence is call function for each unicode point.
/// NoChFn is function without parameter
/// ChFn is function with as parameter
///     - utf8_char_1byte or
///     - utf8_char_2bytes or
///     - utf8_char_3bytes or
///     - utf8_char_4bytes
/// ChErrorFn is function with as parameter
///     - utf8_char_invalid
/// TruncatedFn is function with as parameter
///     - utf8_char_truncated
/// TruncatedFn: void(bytes_view{current_pos, utf8_end}) then stop the function
template<class NoChFn, class ChFn, class ChErrorFn, class TruncatedFn>
decltype(auto) utf8_read_one_char(
    bytes_view utf8,
    NoChFn&& no_ch_fn,
    ChFn&& ch_fn,
    ChErrorFn&& err_fn,
    TruncatedFn&& truncated_fn)
{
    if (utf8.size() == 0) [[unlikely]] {
        return no_ch_fn();
    }

    auto source = utf8.begin();

    switch (*source >> 4) {
        [[likely]]
        case 0:
        case 1: case 2: case 3:
        case 4: case 5: case 6: case 7:
            return ch_fn(utf8_char_1byte{source});

        /* handle U+0080..U+07FF inline : 2 bytes sequences */
        case 0xC: case 0xD:
            if (utf8.size() >= 2) [[likely]] {
                return ch_fn(utf8_char_2bytes{source});
            }
            return truncated_fn(utf8_char_truncated{utf8});
            /* handle U+8FFF..U+FFFF inline : 3 bytes sequences */

        case 0xE:
            if (utf8.size() >= 3) [[likely]] {
                return ch_fn(utf8_char_3bytes{source});
            }
            return truncated_fn(utf8_char_truncated{utf8});

        case 0xF:
            if (utf8.size() >= 4) [[likely]] {
                return ch_fn(utf8_char_4bytes{source});
            }
            return truncated_fn(utf8_char_truncated{utf8});

        // these should never happen on valid UTF8
        [[unlikely]]
        case 8: case 9: case 0xA: case 0xB:
            return err_fn(utf8_char_invalid{utf8});

        default:
            REDEMPTION_UNREACHABLE();
    }
}

template<class NoChFn, class ChFn>
decltype(auto) utf8_read_one_char(bytes_view utf8, NoChFn&& no_ch_fn, ChFn&& ch_fn)
{
    return utf8_read_one_char(utf8, no_ch_fn, ch_fn, ch_fn, ch_fn);
}

struct UTF8Reader
{
    utf8_char_generic ch;
    uint8_t const* end_ptr;

    uint32_t unicode() const
    {
        return ch.unicode();
    }

    bytes_view bytes() const
    {
        return ch.bytes();
    }

    bool next(bytes_view utf8)
    {
        return utf8_read_one_char(utf8,
            []{ return false; },
            [this](utf8_char_generic c){
                ch = c;
                end_ptr = c.bytes().end();
                return true;
            }
        );
    }
};

struct UTF8TextReader
{
    explicit UTF8TextReader(bytes_view utf8_text)
        : p(utf8_text.begin())
        , end_ptr(utf8_text.end())
    {}

    bool has_value() const noexcept
    {
        return p != end_ptr;
    }

    bytes_view remaining() const noexcept
    {
        return {p, end_ptr};
    }

    utf8_char_generic next()
    {
        assert(has_value());
        auto ch = utf8_read_one_char(remaining(),
            []{ return utf8_char_generic(); },
            [](utf8_char_generic c){ return c; }
        );
        p = ch.bytes().end();
        return ch;
    }

private:
    uint8_t const* p;
    uint8_t const* end_ptr;
};
