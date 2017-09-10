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

    struct default_deduction_policy
    {
    private:
        template <typename T> struct get { };
        template <> struct get<char>     { using type = platform::native_narrow; };
        template <> struct get<wchar_t>  { using type = platform::native_wide; };
        template <> struct get<char16_t> { using type = platform::utf16; };
        template <> struct get<char32_t> { using type = platform::utf32; };

    public:
        template <typename CharT>
        using encoding_type = typename get<CharT>::type;
    };


    //-------------------------------------------------------------------------------------------//
    //                                       class deduce                                        //
    //-------------------------------------------------------------------------------------------//

    template <typename DP>
    struct deduce { };

}} // namespace denc::details