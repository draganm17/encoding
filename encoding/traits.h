#pragma once

#include <encoding/deduction_policy.h>
#include <encoding/details/traits.h>


namespace denc {

    // Deduces the character type of 'T' at compile time. 'T' must be eather an iterator type and meat
    // the requirements of 'InputIterator', or an range type and meat the requirements of 'InputRange'.
    // Otherwise, the behavior is undefined.
    //
    // MEMBER TYPES
    // ============================================================================================
    //  type   |  the character type of 'T', defined as follows.
    //         |  
    //         |  a) if 'T' is an iterator type, 'type' is defined as
    //         |     std::remove_cv_t<std::iterator_traits<std::remove_cv_t<T>>::value_type>.
    //         |
    //         |  b) if 'T' is an range type, 'type' is defined as
    //         |     std::remove_cv_t<std::iterator_traits<RangeIt>::value_type>.
    //         |     Where 'RangeIt' is the type of 'T's iterator, which is defined as
    //         |     'decltype(BEGIN(std::declval<T&>()))', where 'BEGIN' is an unqualified
    //         |     function call 'begin()' in the context where both 'std::begin' and the
    //         |     user-defined 'begin()'s are visible.
    // ============================================================================================
    //
    // NOTES
    // TODO: ...
    template <typename T>
    using char_type = details::char_type<T>;

    // Helper type
    template <typename T>
    using char_type_t = details::char_type_t<T>;

    // Deduces the encoding type of 'T' at compile time using the 'DP' deduction policy. 'T' must
    // be eather an iterator type and meat the requirements of 'InputIterator', or an range type 
    // and meat the requirements of 'InputRange'. Otherwise, the behavior is undefined.
    //
    // MEMBER TYPES
    // ============================================================================================
    //  type   |  the deduced encoding type of 'T' defined as 
    //         |  'DP::encoding_type<denc::char_type_t<T>>'.
    //         |  Only defined if the encoding type can be deduced by 'DP'.
    // ============================================================================================
    template <typename T,
              typename DP = default_deduction_policy
    >
    using encoding_type = details::encoding_type<T, DP>;

    // Helper type
    template <typename T, 
              typename DP = default_deduction_policy
    >
    using encoding_type_t = details::encoding_type_t<T, DP>;

    // TODO: description ...
    // TODO: move to encoding.h ???
    template <typename ResultToken>
    using encode_result = details::encode_result<ResultToken>;

} // namespace denc