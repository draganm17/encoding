#pragma once

#include <algorithm>
#include <codecvt>
#include <cstdint>
#include <locale>

#include <encoding/details/codecvt.h>

#define DENC_NATIVE_NARROW_DISCTINCT_TYPE
#define DENC_NATIVE_WIDE_DISCTINCT_TYPE


namespace denc {
namespace details {
namespace platform {

    struct utf8          { };
    struct utf16         { };
    struct utf32         { };
    struct native_narrow { };
    struct native_wide   { };

    using native_encoding_type = native_wide;


    //-------------------------------------------------------------------------------------------//
    //                                   NON-MEMBER FUNCTIONS                                    //
    //-------------------------------------------------------------------------------------------//

    template <typename InputIt, typename OutputIt>
    OutputIt from_native(utf8, InputIt first, InputIt last, 
                         OutputIt result, const std::locale& loc);

    template <typename InputIt, typename OutputIt>
    OutputIt to_native(utf8, InputIt first, InputIt last, 
                       OutputIt result, const std::locale& loc);

    template <typename InputIt, typename OutputIt>
    OutputIt from_native(utf16, InputIt first, InputIt last, 
                         OutputIt result, const std::locale& loc);

    template <typename InputIt, typename OutputIt>
    OutputIt to_native(utf16, InputIt first, InputIt last, 
                       OutputIt result, const std::locale& loc);

    template <typename InputIt, typename OutputIt>
    OutputIt from_native(utf32, InputIt first, InputIt last, 
                         OutputIt result, const std::locale& loc);

    template <typename InputIt, typename OutputIt>
    OutputIt to_native(utf32, InputIt first, InputIt last, 
                       OutputIt result, const std::locale& loc);

    template <typename InputIt, typename OutputIt>
    OutputIt from_native(native_narrow, InputIt first, InputIt last, 
                         OutputIt result, const std::locale& loc);

    template <typename InputIt, typename OutputIt>
    OutputIt to_native(native_narrow, InputIt first, InputIt last, 
                       OutputIt result, const std::locale& loc);

    template <typename InputIt, typename OutputIt>
    OutputIt from_native(native_wide, InputIt first, InputIt last, 
                         OutputIt result, const std::locale& loc);

    template <typename InputIt, typename OutputIt>
    OutputIt to_native(native_wide, InputIt first, InputIt last, 
                       OutputIt result, const std::locale& loc);


    //-------------------------------------------------------------------------------------------//
    //                                    INLINE DEFINITIONS                                     //
    //-------------------------------------------------------------------------------------------//

    template <typename InputIt, typename OutputIt>
    inline OutputIt from_native(utf8, InputIt first, InputIt last, 
                                OutputIt result, const std::locale& loc)
    {
        auto* cvt = new std::codecvt_utf8_utf16<wchar_t>;
        std::locale loc_tmp(loc, cvt); // manages reference to codecvt facet

        std::u16string tmp;
        from_native(utf16(), first, last, std::back_inserter(tmp), loc);
        return codecvt_out(tmp.data(), tmp.data() + tmp.size(), result, *cvt);
    }

    template <typename InputIt, typename OutputIt>
    inline OutputIt to_native(utf8, InputIt first, InputIt last,
                              OutputIt result, const std::locale& loc)
    {
        auto* cvt = new std::codecvt_utf8_utf16<wchar_t>;
        std::locale loc_tmp(loc, cvt); // manages reference to codecvt facet

        std::u16string tmp;
        codecvt_in(first, last, std::back_inserter(tmp), *cvt);
        return to_native(utf16(), tmp.data(), tmp.data() + tmp.size(), result, loc);
    }

    template <typename InputIt, typename OutputIt>
    inline OutputIt from_native(utf16, InputIt first, InputIt last,
                                OutputIt result, const std::locale& loc)
    {
        return std::copy(first, last, result);
        // NOTE: BOM is alway preserved.
    }

    template <typename InputIt, typename OutputIt>
    inline OutputIt to_native(utf16, InputIt first, InputIt last,
                              OutputIt result, const std::locale& loc)
    {
        if (first == last) return result;
        const char16_t first_cp = *first++;

        if (first_cp == 0xFEFF)
        // BOM indicates little-endian.
        {
            return std::copy(first, last, result);
        } else
        if (first_cp == 0xFFFE)
        // BOM indicates big-endian.
        {
            return std::transform(first, last, result,
                                  [](char16_t c) { return ((c & 0x00FF) << 8) | ((c & 0xFF00) >> 8); });
        } else
        // BOM is not present. Little-endian assumed.
        {
            *result++ = first_cp;
            return std::copy(first, last, result);
        }

        // NOTE: BOM is alway removed.
    }

    template <typename InputIt, typename OutputIt>
    inline OutputIt from_native(utf32, InputIt first, InputIt last,
                                OutputIt result, const std::locale& loc)
    {
        auto* cvt = new std::codecvt_utf8<uint32_t>;
        std::locale loc_tmp(loc, cvt); // manages reference to codecvt facet

        std::string tmp;
        from_native(utf8(), first, last, std::back_inserter(tmp), loc);
        return codecvt_in(tmp.data(), tmp.data() + tmp.size(), result, *cvt);
    }

    template <typename InputIt, typename OutputIt>
    inline OutputIt to_native(utf32, InputIt first, InputIt last,
                              OutputIt result, const std::locale& loc)
    {
        auto* cvt = new std::codecvt_utf8<uint32_t>;
        std::locale loc_tmp(loc, cvt); // manages reference to codecvt facet

        std::string tmp;
        codecvt_out(first, last, std::back_inserter(tmp), *cvt);
        return to_native(utf8(), tmp.data(), tmp.data() + tmp.size(), result, loc);
    }

    template <typename InputIt, typename OutputIt>
    inline OutputIt from_native(native_narrow, InputIt first, InputIt last,
                                OutputIt result, const std::locale& loc)
    {
        using Codecvt = std::codecvt<wchar_t, char, std::mbstate_t>;
        return codecvt_out(first, last, result, std::use_facet<Codecvt>(loc));
    }

    template <typename InputIt, typename OutputIt>
    inline OutputIt to_native(native_narrow, InputIt first, InputIt last,
                              OutputIt result, const std::locale& loc)
    {
        using Codecvt = std::codecvt<wchar_t, char, std::mbstate_t>;
        return codecvt_in(first, last, result, std::use_facet<Codecvt>(loc));
    }

    template <typename InputIt, typename OutputIt>
    inline OutputIt from_native(native_wide, InputIt first, InputIt last,
                                OutputIt result, const std::locale& loc)
    {
        return std::copy(first, last, result);
    }

    template <typename InputIt, typename OutputIt>
    inline OutputIt to_native(native_wide, InputIt first, InputIt last,
                              OutputIt result, const std::locale& loc)
    {
        return std::copy(first, last, result);
    }

}}} // namespace denc::details::platform