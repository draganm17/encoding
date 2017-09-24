#pragma once

#include <algorithm>
#include <type_traits>


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
    //                                   class encode_result                                     //
    //-------------------------------------------------------------------------------------------//

    template <typename ResultToken>
    class encode_result
    {
    public:
        using type = ResultToken;

    public:
        template <typename T>
        explicit encode_result(T&& token);

        encode_result(const encode_result&) = delete;

        encode_result& operator=(const encode_result&) = delete;

    public:
        type get();

    private:
        ResultToken m_token;
    };


    //-------------------------------------------------------------------------------------------//
    //                                    INLINE DEFINITIONS                                     //
    //-------------------------------------------------------------------------------------------//

    template <typename ResultToken>
    template <typename T>
    encode_result<ResultToken>::encode_result(T&& token)
    : m_token(std::forward<T>(token))
    { }

    template <typename ResultToken>
    typename encode_result<ResultToken>::type encode_result<ResultToken>::get()
    {
        return std::move(m_token);
    }

}} // namespace denc::details