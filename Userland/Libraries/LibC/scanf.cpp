/*
 * Copyright (c) 2021, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Assertions.h>
#include <AK/ByteString.h>
#include <AK/Format.h>
#include <AK/GenericLexer.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum class LengthModifier {
    None,
    Default,
    Char,
    Short,
    Long,
    LongLong,
    IntMax,
    Size,
    PtrDiff,
    LongDouble,
};

enum class ConversionSpecifier {
    Unspecified,
    Decimal,
    Integer,
    Octal,
    Unsigned,
    Hex,
    Floating,
    String,
    UseScanList,
    Character,
    Pointer,
    OutputNumberOfBytes,
    Invalid,
};

enum class ReadKind {
    Normal,
    Octal,
    Hex,
    Infer,
};

template<typename T, typename ApT, ReadKind kind = ReadKind::Normal>
struct ReadElementConcrete {
    bool operator()(GenericLexer&, va_list)
    {
        return false;
    }
};

static char const* get_remaining_substr(GenericLexer& lexer, size_t max_width, ByteString& buf)
{
    auto remaining = lexer.remaining();
    char const* nptr = remaining.characters_without_null_termination();
    if (max_width < remaining.length()) {
        // Create a null terminated substring for strto*
        buf = ByteString(nptr, max_width);
        if (buf.is_empty())
            return nullptr;
        nptr = buf.characters();
    }
    return nptr;
}

template<typename ApT, ReadKind kind>
struct ReadElementConcrete<int, ApT, kind> {
    bool operator()(GenericLexer& lexer, va_list* ap, size_t width_specifier, bool suppress_assignment)
    {
        long value = 0;
        char* endptr = nullptr;
        ByteString buf;
        auto nptr = get_remaining_substr(lexer, width_specifier, buf);
        if (!nptr)
            return false;

        if constexpr (kind == ReadKind::Normal)
            value = strtol(nptr, &endptr, 10);
        if constexpr (kind == ReadKind::Octal)
            value = strtol(nptr, &endptr, 8);
        if constexpr (kind == ReadKind::Hex)
            value = strtol(nptr, &endptr, 16);
        if constexpr (kind == ReadKind::Infer)
            value = strtol(nptr, &endptr, 0);

        if (!endptr || endptr == nptr)
            return false;

        auto diff = endptr - nptr;
        VERIFY(diff > 0);
        lexer.ignore((size_t)diff);

        if (!suppress_assignment) {
            auto* ptr = va_arg(*ap, ApT*);
            *ptr = value;
        }
        return true;
    }
};

template<typename ApT, ReadKind kind>
struct ReadElementConcrete<unsigned, ApT, kind> {
    bool operator()(GenericLexer& lexer, va_list* ap, size_t width_specifier, bool suppress_assignment)
    {
        unsigned long value = 0;
        char* endptr = nullptr;
        ByteString buf;
        auto nptr = get_remaining_substr(lexer, width_specifier, buf);
        if (!nptr)
            return false;

        if constexpr (kind == ReadKind::Normal)
            value = strtoul(nptr, &endptr, 10);
        if constexpr (kind == ReadKind::Octal)
            value = strtoul(nptr, &endptr, 8);
        if constexpr (kind == ReadKind::Hex)
            value = strtoul(nptr, &endptr, 16);
        if constexpr (kind == ReadKind::Infer)
            value = strtoul(nptr, &endptr, 0);

        if (!endptr || endptr == nptr)
            return false;

        auto diff = endptr - nptr;
        VERIFY(diff > 0);
        lexer.ignore((size_t)diff);

        if (!suppress_assignment) {
            auto* ptr = va_arg(*ap, ApT*);
            *ptr = value;
        }
        return true;
    }
};

template<typename ApT, ReadKind kind>
struct ReadElementConcrete<long long, ApT, kind> {
    bool operator()(GenericLexer& lexer, va_list* ap, size_t width_specifier, bool suppress_assignment)
    {
        long long value = 0;
        char* endptr = nullptr;
        ByteString buf;
        auto nptr = get_remaining_substr(lexer, width_specifier, buf);
        if (!nptr)
            return false;

        if constexpr (kind == ReadKind::Normal)
            value = strtoll(nptr, &endptr, 10);
        if constexpr (kind == ReadKind::Octal)
            value = strtoll(nptr, &endptr, 8);
        if constexpr (kind == ReadKind::Hex)
            value = strtoll(nptr, &endptr, 16);
        if constexpr (kind == ReadKind::Infer)
            value = strtoll(nptr, &endptr, 0);

        if (!endptr || endptr == nptr)
            return false;

        auto diff = endptr - nptr;
        VERIFY(diff > 0);
        lexer.ignore((size_t)diff);

        if (!suppress_assignment) {
            auto* ptr = va_arg(*ap, ApT*);
            *ptr = value;
        }
        return true;
    }
};

template<typename ApT, ReadKind kind>
struct ReadElementConcrete<unsigned long long, ApT, kind> {
    bool operator()(GenericLexer& lexer, va_list* ap, size_t width_specifier, bool suppress_assignment)
    {
        unsigned long long value = 0;
        char* endptr = nullptr;
        ByteString buf;
        auto nptr = get_remaining_substr(lexer, width_specifier, buf);
        if (!nptr)
            return false;

        if constexpr (kind == ReadKind::Normal)
            value = strtoull(nptr, &endptr, 10);
        if constexpr (kind == ReadKind::Octal)
            value = strtoull(nptr, &endptr, 8);
        if constexpr (kind == ReadKind::Hex)
            value = strtoull(nptr, &endptr, 16);
        if constexpr (kind == ReadKind::Infer)
            value = strtoull(nptr, &endptr, 0);

        if (!endptr || endptr == nptr)
            return false;

        auto diff = endptr - nptr;
        VERIFY(diff > 0);
        lexer.ignore((size_t)diff);

        if (!suppress_assignment) {
            auto* ptr = va_arg(*ap, ApT*);
            *ptr = value;
        }
        return true;
    }
};

template<typename ApT, ReadKind kind>
struct ReadElementConcrete<float, ApT, kind> {
    bool operator()(GenericLexer& lexer, va_list* ap, size_t width_specifier, bool suppress_assignment)
    {
        double value = 0;
        char* endptr = nullptr;
        ByteString buf;
        auto nptr = get_remaining_substr(lexer, width_specifier, buf);
        if (!nptr)
            return false;

        // FIXME: Use strtold for %Lf
        if constexpr (kind == ReadKind::Normal)
            value = strtod(nptr, &endptr);
        else
            return false;

        if (!endptr || endptr == nptr)
            return false;

        auto diff = endptr - nptr;
        VERIFY(diff > 0);
        lexer.ignore((size_t)diff);

        if (!suppress_assignment) {
            auto* ptr = va_arg(*ap, ApT*);
            *ptr = value;
        }
        return true;
    }
};

template<typename T, ReadKind kind>
struct ReadElement {
    bool operator()(LengthModifier length_modifier, GenericLexer& input_lexer, va_list* ap, size_t width_specifier, bool suppress_assignment)
    {
        input_lexer.ignore_while([](char c) { return isspace(c); });

        switch (length_modifier) {
        default:
        case LengthModifier::None:
            VERIFY_NOT_REACHED();
        case LengthModifier::Default:
            return ReadElementConcrete<T, T, kind> {}(input_lexer, ap, width_specifier, suppress_assignment);
        case LengthModifier::Char:
            return ReadElementConcrete<T, char, kind> {}(input_lexer, ap, width_specifier, suppress_assignment);
        case LengthModifier::Short:
            return ReadElementConcrete<T, short, kind> {}(input_lexer, ap, width_specifier, suppress_assignment);
        case LengthModifier::Long:
            if constexpr (IsSame<T, int>)
                return ReadElementConcrete<T, long, kind> {}(input_lexer, ap, width_specifier, suppress_assignment);
            if constexpr (IsSame<T, unsigned>)
                return ReadElementConcrete<T, unsigned long, kind> {}(input_lexer, ap, width_specifier, suppress_assignment);
            if constexpr (IsSame<T, float>)
                return ReadElementConcrete<float, double, kind> {}(input_lexer, ap, width_specifier, suppress_assignment);
            return false;
        case LengthModifier::LongLong:
            if constexpr (IsSame<T, int>)
                return ReadElementConcrete<long long, long long, kind> {}(input_lexer, ap, width_specifier, suppress_assignment);
            if constexpr (IsSame<T, unsigned>)
                return ReadElementConcrete<unsigned long long, unsigned long long, kind> {}(input_lexer, ap, width_specifier, suppress_assignment);
            return false;
        case LengthModifier::IntMax:
            return ReadElementConcrete<T, intmax_t, kind> {}(input_lexer, ap, width_specifier, suppress_assignment);
        case LengthModifier::Size:
            return ReadElementConcrete<T, size_t, kind> {}(input_lexer, ap, width_specifier, suppress_assignment);
        case LengthModifier::PtrDiff:
            return ReadElementConcrete<T, ptrdiff_t, kind> {}(input_lexer, ap, width_specifier, suppress_assignment);
        case LengthModifier::LongDouble:
            return ReadElementConcrete<T, long double, kind> {}(input_lexer, ap, width_specifier, suppress_assignment);
        }
    }
};

template<>
struct ReadElement<char*, ReadKind::Normal> {
    ReadElement(size_t width_specifier, StringView scan_set = {}, bool invert = true)
        : m_count(0)
        , m_max_count(width_specifier)
        , m_scan_set(scan_set)
        , m_invert(invert)
    {
    }

    bool operator()(LengthModifier length_modifier, GenericLexer& input_lexer, va_list* ap, ConversionSpecifier conversion_specifier, bool suppress_assignment)
    {
        // FIXME: Implement wide strings and such.
        if (length_modifier != LengthModifier::Default)
            return false;

        if (conversion_specifier == ConversionSpecifier::String)
            input_lexer.ignore_while([](char c) { return isspace(c); });

        auto str = input_lexer.consume_while([this](auto c) { return this->matches(c); });
        if (str.is_empty())
            return false;

        if (!suppress_assignment) {
            auto* ptr = va_arg(*ap, char*);
            size_t length = str.length();
            memcpy(ptr, str.characters_without_null_termination(), length);
            if (conversion_specifier != ConversionSpecifier::Character)
                ptr[length] = 0;
        }

        return true;
    }

private:
    bool matches(char c)
    {
        if (m_max_count <= m_count)
            return false;
        ++m_count;
        return m_invert ^ m_scan_set.contains(c);
    }

    size_t m_count;
    size_t m_max_count;
    StringView const m_scan_set;
    bool m_invert;
};

template<>
struct ReadElement<void*, ReadKind::Normal> {
    bool operator()(LengthModifier length_modifier, GenericLexer& input_lexer, va_list* ap, size_t width_specifier, bool suppress_assignment)
    {
        if (length_modifier != LengthModifier::Default)
            return false;

        input_lexer.ignore_while([](char c) { return isspace(c); });

        unsigned long long value = 0;
        char* endptr = nullptr;
        ByteString buf;
        auto nptr = get_remaining_substr(input_lexer, width_specifier, buf);
        if (!nptr)
            return false;

        value = strtoull(nptr, &endptr, 16);

        if (!endptr || endptr == nptr)
            return false;

        auto diff = endptr - nptr;
        VERIFY(diff > 0);
        input_lexer.ignore((size_t)diff);

        if (!suppress_assignment) {
            void** ptr = va_arg(*ap, void**);
            *ptr = reinterpret_cast<void*>(static_cast<uintptr_t>(value));
        }
        return true;
    }
};

extern "C" int vsscanf(char const* input, char const* format, va_list ap)
{
    GenericLexer format_lexer { { format, strlen(format) } };
    GenericLexer input_lexer { { input, strlen(input) } };

    int elements_matched = 0;

    va_list copy;
    __builtin_va_copy(copy, ap);

    while (!format_lexer.is_eof()) {
        if (format_lexer.next_is(isspace)) {
            format_lexer.ignore_while(isspace);
            input_lexer.ignore_while(isspace);
        }

        if (!format_lexer.next_is('%')) {
        read_one_literal:;
            if (format_lexer.is_eof())
                break;

            auto next_char = format_lexer.consume();
            if (!input_lexer.consume_specific(next_char))
                return elements_matched;
            continue;
        }

        if (format_lexer.next_is("%%")) {
            format_lexer.ignore();
            goto read_one_literal;
        }

        format_lexer.ignore(); // '%'

        bool suppress_assignment = false;
        if (format_lexer.next_is('*')) {
            suppress_assignment = true;
            format_lexer.ignore();
        }

        // Parse width specification
        size_t width_specifier = -1;
        if (format_lexer.next_is(isdigit)) {
            auto width_digits = format_lexer.consume_while([](char c) { return isdigit(c); });
            width_specifier = width_digits.to_number<size_t>().value();
        }

        bool invert_scanlist = false;
        StringView scanlist;
        LengthModifier length_modifier { LengthModifier::None };
        ConversionSpecifier conversion_specifier { ConversionSpecifier::Unspecified };
    reread_lookahead:;
        auto format_lookahead = format_lexer.peek();
        if (length_modifier == LengthModifier::None) {
            switch (format_lookahead) {
            case 'h':
                if (format_lexer.peek(1) == 'h') {
                    format_lexer.consume(2);
                    length_modifier = LengthModifier::Char;
                } else {
                    format_lexer.consume(1);
                    length_modifier = LengthModifier::Short;
                }
                break;
            case 'l':
                if (format_lexer.peek(1) == 'l') {
                    format_lexer.consume(2);
                    length_modifier = LengthModifier::LongLong;
                } else {
                    format_lexer.consume(1);
                    length_modifier = LengthModifier::Long;
                }
                break;
            case 'j':
                format_lexer.consume();
                length_modifier = LengthModifier::IntMax;
                break;
            case 'z':
                format_lexer.consume();
                length_modifier = LengthModifier::Size;
                break;
            case 't':
                format_lexer.consume();
                length_modifier = LengthModifier::PtrDiff;
                break;
            case 'L':
                format_lexer.consume();
                length_modifier = LengthModifier::LongDouble;
                break;
            default:
                length_modifier = LengthModifier::Default;
                break;
            }
            goto reread_lookahead;
        }
        if (conversion_specifier == ConversionSpecifier::Unspecified) {
            switch (format_lookahead) {
            case 'd':
                format_lexer.consume();
                conversion_specifier = ConversionSpecifier::Decimal;
                break;
            case 'i':
                format_lexer.consume();
                conversion_specifier = ConversionSpecifier::Integer;
                break;
            case 'o':
                format_lexer.consume();
                conversion_specifier = ConversionSpecifier::Octal;
                break;
            case 'u':
                format_lexer.consume();
                conversion_specifier = ConversionSpecifier::Unsigned;
                break;
            case 'x':
            case 'X':
                format_lexer.consume();
                conversion_specifier = ConversionSpecifier::Hex;
                break;
            case 'a':
            case 'e':
            case 'f':
            case 'g':
                format_lexer.consume();
                conversion_specifier = ConversionSpecifier::Floating;
                break;
            case 's':
                format_lexer.consume();
                conversion_specifier = ConversionSpecifier::String;
                break;
            case '[':
                format_lexer.consume();
                scanlist = format_lexer.consume_until(']');
                format_lexer.ignore();
                if (scanlist.starts_with('^')) {
                    scanlist = scanlist.substring_view(1);
                    invert_scanlist = true;
                }
                conversion_specifier = ConversionSpecifier::UseScanList;
                break;
            case 'c':
                format_lexer.consume();
                conversion_specifier = ConversionSpecifier::Character;
                break;
            case 'p':
                format_lexer.consume();
                conversion_specifier = ConversionSpecifier::Pointer;
                break;
            case 'n':
                format_lexer.consume();
                conversion_specifier = ConversionSpecifier::OutputNumberOfBytes;
                break;
            case 'C':
                format_lexer.consume();
                length_modifier = LengthModifier::Long;
                conversion_specifier = ConversionSpecifier::Character;
                break;
            case 'S':
                format_lexer.consume();
                length_modifier = LengthModifier::Long;
                conversion_specifier = ConversionSpecifier::String;
                break;
            default:
                format_lexer.consume();
                conversion_specifier = ConversionSpecifier::Invalid;
                break;
            }
        }

        // Now try to read.
        switch (conversion_specifier) {
        case ConversionSpecifier::Invalid:
        case ConversionSpecifier::Unspecified:
        default:
            // "undefined behavior", let's be nice and crash.
            dbgln("Invalid conversion specifier {} in scanf!", (int)conversion_specifier);
            VERIFY_NOT_REACHED();
        case ConversionSpecifier::Decimal:
            if (!ReadElement<int, ReadKind::Normal> {}(
                    length_modifier, input_lexer, &copy, width_specifier, suppress_assignment))
                format_lexer.consume_all();
            else if (!suppress_assignment)
                ++elements_matched;
            break;
        case ConversionSpecifier::Integer:
            if (!ReadElement<int, ReadKind::Infer> {}(
                    length_modifier, input_lexer, &copy, width_specifier, suppress_assignment))
                format_lexer.consume_all();
            else if (!suppress_assignment)
                ++elements_matched;
            break;
        case ConversionSpecifier::Octal:
            if (!ReadElement<unsigned, ReadKind::Octal> {}(
                    length_modifier, input_lexer, &copy, width_specifier, suppress_assignment))
                format_lexer.consume_all();
            else if (!suppress_assignment)
                ++elements_matched;
            break;
        case ConversionSpecifier::Unsigned:
            if (!ReadElement<unsigned, ReadKind::Normal> {}(
                    length_modifier, input_lexer, &copy, width_specifier, suppress_assignment))
                format_lexer.consume_all();
            else if (!suppress_assignment)
                ++elements_matched;
            break;
        case ConversionSpecifier::Hex:
            if (!ReadElement<unsigned, ReadKind::Hex> {}(
                    length_modifier, input_lexer, &copy, width_specifier, suppress_assignment))
                format_lexer.consume_all();
            else if (!suppress_assignment)
                ++elements_matched;
            break;
        case ConversionSpecifier::Floating:
            if (!ReadElement<float, ReadKind::Normal> {}(
                    length_modifier, input_lexer, &copy, width_specifier, suppress_assignment))
                format_lexer.consume_all();
            else if (!suppress_assignment)
                ++elements_matched;
            break;
        case ConversionSpecifier::String:
            if (!ReadElement<char*, ReadKind::Normal> { width_specifier, " \t\n\f\r"sv }(
                    length_modifier, input_lexer, &copy, conversion_specifier, suppress_assignment))
                format_lexer.consume_all();
            else if (!suppress_assignment)
                ++elements_matched;
            break;
        case ConversionSpecifier::UseScanList:
            if (!ReadElement<char*, ReadKind::Normal> { width_specifier, scanlist, invert_scanlist }(
                    length_modifier, input_lexer, &copy, conversion_specifier, suppress_assignment))
                format_lexer.consume_all();
            else if (!suppress_assignment)
                ++elements_matched;
            break;
        case ConversionSpecifier::Character:
            if (width_specifier == static_cast<size_t>(-1))
                width_specifier = 1;
            if (!ReadElement<char*, ReadKind::Normal> { width_specifier }(
                    length_modifier, input_lexer, &copy, conversion_specifier, suppress_assignment))
                format_lexer.consume_all();
            else if (!suppress_assignment)
                ++elements_matched;
            break;
        case ConversionSpecifier::Pointer:
            if (!ReadElement<void*, ReadKind::Normal> {}(
                    length_modifier, input_lexer, &copy, width_specifier, suppress_assignment))
                format_lexer.consume_all();
            else if (!suppress_assignment)
                ++elements_matched;
            break;
        case ConversionSpecifier::OutputNumberOfBytes: {
            if (!suppress_assignment) {
                auto* ptr = va_arg(copy, int*);
                *ptr = input_lexer.tell();
            }
            break;
        }
        }
    }
    va_end(copy);

    return elements_matched;
}
