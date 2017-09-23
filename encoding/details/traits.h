#pragma once

#include <algorithm>
#include <locale>
#include <type_traits>

#ifdef _WIN32
#include <encoding/details/platform.win.h>
#else
#include <encoding/details/platform.pos.h>
#endif


namespace denc {
namespace details {

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

    //-------------------------------------------------------------------------------------------//
    //                                     class char_type                                       //
    //-------------------------------------------------------------------------------------------//

    template <typename T, typename = void>
    struct char_type_impl
    {
        using type = std::remove_cv_t<
                     typename std::iterator_traits<std::remove_cv_t<T>>::value_type>;
    };

    template <typename T>
    struct char_type_impl<T, std::enable_if_t<is_range<T>::value>>
    {
        using type = std::remove_cv_t<
                     typename std::iterator_traits<iterator_t<T>>::value_type>;
    };

    template <typename T>
    struct char_type : char_type_impl<T> { };

    template <typename T>
    using char_type_t = typename char_type<T>::type;


    //-------------------------------------------------------------------------------------------//
    //                                   class encoding_type                                     //
    //-------------------------------------------------------------------------------------------//

    template <typename T, typename DP, typename = void>
    struct encoding_type_impl
    { };

    template <typename T, typename DP>
    struct encoding_type_impl<T, DP, 
                              std::void_t<typename DP::template encoding_type<char_type_t<T>>>>
    {
        using type = typename DP::template encoding_type<char_type_t<T>>;
    };

    template <typename T, typename DP>
    struct encoding_type : encoding_type_impl<T, DP>
    { };

    template <typename T, typename DP>
    using encoding_type_t = typename encoding_type<T, DP>::type;


    //-------------------------------------------------------------------------------------------//
    //                                class encoding_traits_base                                 //
    //-------------------------------------------------------------------------------------------//

    template <typename Encoding, typename CharT>
    class encoding_traits_base
    {
    public:
        using encoding_type = Encoding;

        using char_type     = CharT;

    public:
        template <typename InputIt, typename OutputIt>
        static OutputIt from_native(InputIt first, InputIt last,
                                    OutputIt result, const std::locale& loc = std::locale());

        template <typename InputIt, typename OutputIt>
        static OutputIt to_native(InputIt first, InputIt last,
                                  OutputIt result, const std::locale& loc = std::locale());
    };


    //-------------------------------------------------------------------------------------------//
    //                                   class encoding_traits                                   //
    //-------------------------------------------------------------------------------------------//

    template <typename Encoding>
    class encoding_traits;


    //-------------------------------------------------------------------------------------------//
    //                           class encoding_traits<platform::utf8>                           //
    //-------------------------------------------------------------------------------------------//

    template <>
    class encoding_traits<platform::utf8>
    : public encoding_traits_base<platform::utf8, char>
    { };


    //-------------------------------------------------------------------------------------------//
    //                          class encoding_traits<platform::utf16>                           //
    //-------------------------------------------------------------------------------------------//

    template <>
    class encoding_traits<platform::utf16>
    : public encoding_traits_base<platform::utf16, char16_t>
    { };


    //-------------------------------------------------------------------------------------------//
    //                          class encoding_traits<platform::utf32>                           //
    //-------------------------------------------------------------------------------------------//

    template <>
    class encoding_traits<platform::utf32>
    : public encoding_traits_base<platform::utf32, char32_t>
    { };


#ifdef DENC_NATIVE_NARROW_DISCTINCT_TYPE
    //-------------------------------------------------------------------------------------------//
    //                      class encoding_traits<platform::native_narrow>                       //
    //-------------------------------------------------------------------------------------------//

    template <>
    class encoding_traits<platform::native_narrow>
    : public encoding_traits_base<platform::native_narrow, char>
    { };
#endif
    

#ifdef DENC_NATIVE_WIDE_DISCTINCT_TYPE
    //-------------------------------------------------------------------------------------------//
    //                       class encoding_traits<platform::native_wide>                        //
    //-------------------------------------------------------------------------------------------//

    template <>
    class encoding_traits<platform::native_wide>
    : public encoding_traits_base<platform::native_wide, wchar_t>
    { };
#endif


    //-------------------------------------------------------------------------------------------//
    //                                    INLINE DEFINITIONS                                     //
    //-------------------------------------------------------------------------------------------//

    template <typename Encoding, typename CharT>
    template <typename InputIt, typename OutputIt>
    inline OutputIt encoding_traits_base<Encoding, CharT>
           ::from_native(InputIt first, InputIt last, OutputIt result, const std::locale& loc)
    {
        return platform::from_native(encoding_type(), first, last, result, loc);
    }

    template <typename Encoding, typename CharT>
    template <typename InputIt, typename OutputIt>
    inline OutputIt encoding_traits_base<Encoding, CharT>
           ::to_native(InputIt first, InputIt last, OutputIt result, const std::locale& loc)
    {
        return platform::to_native(encoding_type(), first, last, result, loc);
    }

}} // namespace denc::details