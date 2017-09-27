#pragma once

#include <algorithm>
#include <type_traits>

#ifdef _WIN32
#include <encoding/details/encoding.win.h>
#else
#include <encoding/details/encoding.pos.h>
#endif


namespace denc {
namespace details {

    template <typename T> struct default_encoding { };
    template <> struct default_encoding<char>     { using type = platform::native_narrow; };
    template <> struct default_encoding<wchar_t>  { using type = platform::native_wide; };
    template <> struct default_encoding<char16_t> { using type = platform::utf16; };
    template <> struct default_encoding<char32_t> { using type = platform::utf32; };

inline namespace range_traits
{
    using std::begin; using std::end;
    using std::data;  using std::size;

    template <class T>
    using iterator_t = decltype(begin(std::declval<T&>()));


    //-------------------------------------------------------------------------------------------//
    //                                      class is_range                                       //
    //-------------------------------------------------------------------------------------------//

    template <typename T, typename = void>
    struct is_range_impl : std::false_type { };

    template <typename T>
    struct is_range_impl<T, std::void_t<decltype(begin(std::declval<T&>())),
                                        decltype(end(std::declval<T&>()))>>
    : std::true_type { };

    template <typename T>
    struct is_range : is_range_impl<T> { };


    //-------------------------------------------------------------------------------------------//
    //                                class is_contiguous_range                                  //
    //-------------------------------------------------------------------------------------------//

    template <typename T, typename = void>
    struct is_contiguous_range_impl : std::false_type { };

    template <typename T>
    struct is_contiguous_range_impl<T, std::void_t<std::enable_if_t<is_range<T>::value>,
                                                   decltype(data(std::declval<T&>())),
                                                   decltype(size(std::declval<T&>()))>>
    : std::true_type { };

    template <typename T>
    struct is_contiguous_range : is_contiguous_range_impl<T> { };
}

    template <typename T, typename = void>
    struct char_type
    {
        using type = std::remove_cv_t<
                     typename std::iterator_traits<std::remove_cv_t<T>>::value_type>;
    };

    template <typename T>
    struct char_type<T, std::enable_if_t<is_range<T>::value>>
    {
        using type = std::remove_cv_t<
                     typename std::iterator_traits<iterator_t<T>>::value_type>;
    };

    template <typename T, typename DP, typename = void>
    struct encoding_type
    { };

    template <typename T, typename DP>
    struct encoding_type<T, DP, 
                         std::void_t<typename DP::template encoding_type<typename char_type<T>::type>>>
    {
        using type = typename DP::template encoding_type<typename char_type<T>::type>;
    };

}} // namespace denc::details