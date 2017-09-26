/** @file */
#pragma once

#include <encoding/deduction_policy.h>
#include <encoding/details/traits.h>


namespace denc {

    //-------------------------------------------------------------------------------------------//
    //                                     class char_type                                       //
    //-------------------------------------------------------------------------------------------//

    //! Deduces the character type of `T` at compile time.
    /*!
    //  @tparam T must be eather an iterator type and meat the requirements of `InputIterator` or
    //          an range type and meat the requirements of `InputRange`. Otherwise, the behavior is
    //          undefined.
    //
    // NOTES
    // TODO: ...
    */
    template <typename T>
    struct char_type
    {
        //! The character type of `T`.
        /*! Defined as follows.
        //  - If `T` is an iterator type, `type` is defined as
        //    `std::remove_cv_t<std::iterator_traits<std::remove_cv_t<T>>::value_type>`.
        //
        //  - If `T` is an range type, `type` is defined as
        //    `std::remove_cv_t<std::iterator_traits<RangeIt>::value_type>`.
        //    Where `RangeIt` is the type of `T`'s iterator, which is defined as
        //    `decltype(BEGIN(std::declval<T&>()))`, where `BEGIN` is an unqualified
        //    function call `begin()` in the context where both `std::begin` and the
        //    user-defined `begin()`'s are visible.
        */
        using type = typename details::char_type<T>::type;
    };

    //! Helper type
    template <typename T>
    using char_type_t = typename char_type<T>::type;


    //-------------------------------------------------------------------------------------------//
    //                                   class encoding_type                                     //
    //-------------------------------------------------------------------------------------------//

    //! Deduces the encoding type of `T` at compile time using the `DP` deduction policy.
    /*!
    //  @tparam T must be eather an iterator type and meat the requirements of `InputIterator`, 
    //          or an range type and meat the requirements of `InputRange`. Otherwise, the behavior
    //          is undefined.
    */
    template <typename T,
              typename DP = default_deduction_policy
    >
    struct encoding_type
    {
        //! The deduced encoding type of `T`.
        /*! Defined as `DP::encoding_type<denc::char_type_t<T>>`.
        //  Only defined if the encoding type can be deduced by `DP`.
        */
        using type = typename details::encoding_type<T, DP>::type;
    };

    //! Helper type
    template <typename T, 
              typename DP = default_deduction_policy
    >
    using encoding_type_t = typename encoding_type<T, DP>::type;


    //-------------------------------------------------------------------------------------------//
    //                                   class encode_result                                     //
    //-------------------------------------------------------------------------------------------//

    // TODO: description ...
    // TODO: move to encoding.h ???
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
    inline encode_result<ResultToken>::encode_result(T&& token)
    : m_token(std::forward<T>(token))
    { }

    template <typename ResultToken>
    inline typename encode_result<ResultToken>::type encode_result<ResultToken>::get()
    {
        return std::move(m_token);
    }

} // namespace denc