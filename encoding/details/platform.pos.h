#pragma once

#include <algorithm>
#include <locale>

#include <encoding/details/codecvt.h>

#define __NATIVE_WIDE_DISCTINCT_TYPE


namespace denc {
namespace details {
namespace platform {

    struct utf8            { };
    struct utf16           { };
    struct utf32           { };
    using  native_narrow = utf8;
    struct native_wide     { };

    using native_encoding_type = utf8;


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
        return std::copy(first, last, result);
    }

    template <typename InputIt, typename OutputIt>
    inline OutputIt to_native(utf8, InputIt first, InputIt last,
                              OutputIt result, const std::locale& loc)
    {
        return std::copy(first, last, result);
    }

    template <typename InputIt, typename OutputIt>
    inline OutputIt from_native(utf16, InputIt first, InputIt last,
                                OutputIt result, const std::locale& loc)
    {
        using Codecvt = std::codecvt<char16_t, char, std::mbstate_t>;
        return codecvt_out(first, last, result, std::use_facet<Codecvt>(loc));
    }

    template <typename InputIt, typename OutputIt>
    inline OutputIt to_native(utf16, InputIt first, InputIt last,
                              OutputIt result, const std::locale& loc)
    {
        using Codecvt = std::codecvt<char16_t, char, std::mbstate_t>;
        return codecvt_in(first, last, result, std::use_facet<Codecvt>(loc));
    }

    template <typename InputIt, typename OutputIt>
    inline OutputIt from_native(utf32, InputIt first, InputIt last,
                                OutputIt result, const std::locale& loc)
    {
        using Codecvt = std::codecvt<char32_t, char, std::mbstate_t>;
        return codecvt_out(first, last, result, std::use_facet<Codecvt>(loc));
    }

    template <typename InputIt, typename OutputIt>
    inline OutputIt to_native(utf32, InputIt first, InputIt last,
                              OutputIt result, const std::locale& loc)
    {
        using Codecvt = std::codecvt<char32_t, char, std::mbstate_t>;
        return codecvt_in(first, last, result, std::use_facet<Codecvt>(loc));
    }

    template <typename InputIt, typename OutputIt>
    inline OutputIt from_native(native_wide, InputIt first, InputIt last,
                                OutputIt result, const std::locale& loc)
    {
        using Codecvt = std::codecvt<wchar_t, char, std::mbstate_t>;
        return codecvt_out(first, last, result, std::use_facet<Codecvt>(loc));
    }

    template <typename InputIt, typename OutputIt>
    inline OutputIt to_native(native_wide, InputIt first, InputIt last,
                              OutputIt result, const std::locale& loc)
    {
        using Codecvt = std::codecvt<wchar_t, char, std::mbstate_t>;
        return codecvt_in(first, last, result, std::use_facet<Codecvt>(loc));
    }

}}} // namespace denc::details::platform
