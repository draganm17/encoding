#pragma once

#ifdef _WIN32
#include <encoding/details/platform.win.h>
#else
#include <encoding/details/platform.pos.h>
#endif


namespace denc {
namespace details {

    //-------------------------------------------------------------------------------------------//
    //                              class default_deduction_policy                               //
    //-------------------------------------------------------------------------------------------//

    template <typename T> struct default_encoding { };
    template <> struct default_encoding<char>     { using type = platform::native_narrow; };
    template <> struct default_encoding<wchar_t>  { using type = platform::native_wide; };
    template <> struct default_encoding<char16_t> { using type = platform::utf16; };
    template <> struct default_encoding<char32_t> { using type = platform::utf32; };

    struct default_deduction_policy
    {
        template <typename CharT>
        using encoding_type = typename default_encoding<CharT>::type;
    };


    //-------------------------------------------------------------------------------------------//
    //                                       class deduce                                        //
    //-------------------------------------------------------------------------------------------//

    template <typename DP>
    struct deduce { };

}} // namespace denc::details
