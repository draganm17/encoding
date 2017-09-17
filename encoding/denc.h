// Deductive Encoding Library

#pragma once

#include <locale>
#include <type_traits>

#include <encoding/details/deduction_policy.h>
#include <encoding/details/encode.h>
#include <encoding/details/traits.h>


namespace denc {

    // TODO: define the 'DeductionPolicy' concept ???


    // Defines supported encodings.
    // Each encoding is represented by an empty distinct type, except 'denc::native_narrow'
    // and 'denc::native_wide', which are allowed to be aliases to any other encoding type.
    // The user can provide additional encoding types. For each encoding type 'E' an
    // 'denc::encoding_traits<E>' specialization should be provided.
    using utf8          = details::platform::utf8;
    using utf16         = details::platform::utf16;
    using utf32         = details::platform::utf32;
    using native_narrow = details::platform::native_narrow;
    using native_wide   = details::platform::native_wide;

    // Defines the platform-specific native encoding type.
    using native_encoding_type = details::platform::native_encoding_type;

    // The default policy used to deduce the encoding type from a given character type.
    // User can provide additional deduction policies. Each policy must define a template
    // member typedef 'encoding_type', like the default policy does.
    //
    // MEMBER TYPES
    // ============================================================================================
    // encoding_type<CharT>  |  The encoding type deduced for the given character type.
    //                       |  Only the following character types are supported by the default 
    //                       |  deduction policy:
    //                       |  - 'char'     - the deduced encoding type is 'denc::native_narrow'.
    //                       |  - 'wchar_t'  - the deduced encoding type is 'denc::native_wide'.
    //                       |  - 'char16_t' - the deduced encoding type is 'denc::utf16'.
    //                       |  - 'char32_t' - the deduced encoding type is 'denc::utf32'.
    // ============================================================================================
    using default_deduction_policy = details::default_deduction_policy;

    // An empty tag type used to cpecify the deduction policy that should be used by the deductive
    // overloads of 'denc::encode'.
    template <typename DP = default_deduction_policy>
    using deduce = details::deduce<DP>;

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

    // The 'denc::encoding_traits' class is a traits class template that abstracts basic operations
    // for a given encoding type.
    // The 'denc::encoding_traits' class template serves as a basis for explicit instantiations. The 
    // user can provide a specialization for any custom encoding types. The following specializations
    // are already provided by the library:
    // - 'denc::encoding_traits<utf8>';
    // - 'denc::encoding_traits<utf16>';
    // - 'denc::encoding_traits<utf32>';
    // - 'denc::encoding_traits<native_narrow>' (only if 'denc::native_narrow' is a distinct type);
    // - 'denc::encoding_traits<native_wide>' (only if 'denc::native_wide' is a distinct type).
    //
    // The values of the member typedefs are as follows.
    // ======================================================================
    //                                       |  encoding_type  |  char_type
    // ======================================================================
    // denc::encoding_traits<utf8>           |  utf8           |  char
    // ----------------------------------------------------------------------
    // denc::encoding_traits<utf16>          |  utf16          |  char16_t
    // ----------------------------------------------------------------------
    // denc::encoding_traits<utf32>          |  utf32          |  char32_t
    // ----------------------------------------------------------------------
    // denc::encoding_traits<native_narrow>  |  native_narrow  |  char
    // ----------------------------------------------------------------------
    // denc::encoding_traits<native_wide>    |  native_wide    |  wchar_t
    //=======================================================================
    //
    //
    // MEMBER TYPES
    // ============================================================================================
    //  encoding_type   |  'Encoding'
    // --------------------------------------------------------------------------------------------
    //  char_type       |  An character type that can hold all encoding code points.
    // ============================================================================================
    //
    //
    // MEMBER FUNCTIONS
    // ============================================================================================
    // template <typename InputIt, typename OutputIt>
    // static OutputIt from_native(InputIt first, InputIt last,
    //                             OutputIt result, const std::locale& loc = std::locale());
    //
    // Converts natively-encoded characters from the source range ['first', 'last') to
    // the 'Encoding' encoding, placing the results in the subsequent locations starting
    // at 'result'.
    //
    // ---- Parameters ----
    // first, last - the range of characters to convert.
    //
    // result      - the beginning of the destination range.
    //
    // loc         - TODO: ...
    //
    //
    // ---- Type requirements ----
    //  InputIt   - must meet the requirements of 'InputIterator'. 'denc::char_type_t<InputIt>' must be
    //              implicitly convertible to 'denc::encoding_traits<native_encoding_type>::char_type'.
    //
    //  OutputIt  - must meet the requirements of 'OutputIterator'. The expression
    //              '*result = char_type()' shall be valid.
    //
    //
    // ---- Return value ----
    // Output iterator to the element past the last element converted.
    //
    //
    // ---- Exceptions ----
    // Throws 'std::range_error' on conversion failure.
    //
    // --------------------------------------------------------------------------------------------
    // template <typename InputIt, typename OutputIt>
    // static OutputIt to_native(InputIt first, InputIt last,
    //                           OutputIt result, const std::locale& loc = std::locale());
    //
    // Converts 'Encoding'-encoded characters from the source range ['first', 'last') to
    // the system native encoding, placing the results in the subsequent locations starting
    // at 'result'.
    //
    // ---- Parameters ----
    // first, last - the range of characters to convert.
    //
    // result      - the beginning of the destination range.
    //
    // loc         - TODO: ...
    //
    //
    // ---- Type requirements ----
    //  InputIt   - must meet the requirements of 'InputIterator'. 'denc::char_type_t<InputIt>'
    //              must be implicitly convertible to 'char_type'.
    //
    //  OutputIt  - must meet the requirements of 'OutputIterator'. The expression 
    //              '*result = denc::encoding_traits<denc::native_encoding_type>::char_type()' 
    //              shall be valid.
    //
    //
    // ---- Return value ----
    // Output iterator to the element past the last element converted.
    //
    //
    // ---- Exceptions ----
    // Throws 'std::range_error' on conversion failure.
    // ============================================================================================
    template <typename Encoding>
    using encoding_traits = details::encoding_traits<Encoding>;

    // Converts 'SrcEnc'-encoded characters from the source range 'src' to the 'DstEnc'
    // encoding, placing the results in the subsequent locations starting at 'result'.
    //
    // ---- Parameters ----
    // src    - an iterator to a null-terminated character sequence, or an range of characters.
    //
    // result - the beginning of the destination range.
    //
    // loc    - TODO: ...
    //
    //
    // ---- Type requirements ----
    //  Source    - should be eather an iterator type and meat the requirements of
    //              'InputIterator', or an range type and meat the requirements of 'InputRange'.
    //              In both cases 'denc::char_type_t<std::decay_t<Source>>' must be implicitly
    //              convertible to 'denc::encoding_traits<SrcEnc>::char_type'.
    //
    //  OutputIt  - must meet the requirements of 'OutputIterator'. The expression 
    //              '*result = denc::encoding_traits<DstEnc>::char_type()' shall be valid.
    //
    //
    // ---- Return value ----
    // Output iterator to the element past the last element converted.
    //
    //
    // ---- Exceptions ----
    // Throws 'std::range_error' on conversion failure.
    //
    //
    // ---- Notes ----
    // TODO: ...
    //
    template <typename SrcEnc,
              typename DstEnc,
              typename Source, 
              typename OutputIt
    >
    OutputIt encode(Source&& src, OutputIt result,
                    const std::locale& loc = std::locale());

    // Converts characters from the source range 'src' to the 'DstEnc'
    // encoding, placing the results in the subsequent locations starting at 'result'.
    // The source characters encoding is deduced using the 'DP' deduction policy.
    //
    // Effectivelly calls 
    // 'denc::encode<denc::encoding_type_t<std::decay_t<Source>, DP>, DstEnc>
    //      (std::forward<Source>(src), result, loc)'.
    template <typename DstEnc,
              typename DP,
              typename Source, 
              typename OutputIt
    >
    OutputIt encode(deduce<DP>, Source&& src,
                    OutputIt result, const std::locale& loc = std::locale());

    // Converts 'SrcEnc'-encoded characters from the source range ['first', 'last') to
    // the 'DstEnc' encoding, placing the results in the subsequent locations starting
    // at 'result'.
    //
    // ---- Parameters ----
    // first, last - the range of characters to convert.
    //
    // result      - the beginning of the destination range.
    //
    // loc         - TODO: ...
    //
    //
    // ---- Type requirements ----
    //  InputIt   - must meet the requirements of 'InputIterator'. 'denc::char_type_t<InputIt>'
    //              must be implicitly convertible to 'denc::encoding_traits<SrcEnc>::char_type'.
    //
    //  OutputIt  - must meet the requirements of 'OutputIterator'. The expression 
    //              '*result = denc::encoding_traits<DstEnc>::char_type()' shall be valid.
    //
    //
    // ---- Return value ----
    // Output iterator to the element past the last element converted.
    //
    //
    // ---- Exceptions ----
    // Throws 'std::range_error' on conversion failure.
    //
    //
    // ---- Notes ----
    // TODO: ...
    //
    template <typename SrcEnc,
              typename DstEnc,
              typename InputIt, 
              typename OutputIt
    >
    OutputIt encode(InputIt first, InputIt last,
                    OutputIt result, const std::locale& loc = std::locale());

    // Converts characters from the source range ['first', 'last') to the 'DstEnc'
    // encoding, placing the results in the subsequent locations starting at 'result'.
    // The source characters encoding is deduced using the 'DP' deduction policy.
    //
    // Effectivelly calls 
    // 'denc::encode<denc::encoding_type_t<InputIt, DP>, DstEnc>(first, last, result, loc)'.
    template <typename DstEnc,
              typename DP,
              typename InputIt, 
              typename OutputIt
    >
    OutputIt encode(deduce<DP>, InputIt first, InputIt last,
                    OutputIt result, const std::locale& loc = std::locale());


    //-------------------------------------------------------------------------------------------//
    //                                    INLINE DEFINITIONS                                     //
    //-------------------------------------------------------------------------------------------//

    template <typename SrcEnc, typename DstEnc, typename Source, typename OutputIt>
    inline OutputIt encode(Source&& src, OutputIt result, const std::locale& loc)
    {
        return details::do_encode<SrcEnc, DstEnc>(std::forward<Source>(src), result, loc);
    }

    template <typename DstEnc, typename DP, typename Source, typename OutputIt>
    inline OutputIt encode(deduce<DP>, Source&& src, OutputIt result, const std::locale& loc)
    {
        using SrcEnc = encoding_type_t<std::decay_t<Source>, DP>;
        return encode<SrcEnc, DstEnc>(std::forward<Source>(src), result, loc);
    }

    template <typename SrcEnc, typename DstEnc, typename InputIt, typename OutputIt>
    inline OutputIt encode(InputIt first, InputIt last, OutputIt result, const std::locale& loc)
    {
        return details::do_encode<SrcEnc, DstEnc>(first, last, result, loc);
    }

    template <typename DstEnc, typename DP, typename InputIt, typename OutputIt>
    inline OutputIt encode(deduce<DP>, InputIt first, InputIt last, 
                           OutputIt result, const std::locale& loc)
    {
        using SrcEnc = encoding_type_t<InputIt, DP>;
        return encode<SrcEnc, DstEnc>(first, last, result, loc);
    }

} // namespace denc