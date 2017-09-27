/** @file */
#pragma once

#include <locale>
#include <type_traits>

#include <encoding/deduction_policy.h>
#include <encoding/details/encode.h>
#include <encoding/details/traits.h>
#include <encoding/traits.h>


namespace denc {

    //! Defines UTF-8 encoding type.
    /*! `denc::utf8` is always a distinct type. */
    using utf8          = details::platform::utf8;

    //! Defines UTF-16 encoding type.
    /*! `denc::utf16` is always a distinct type. */
    using utf16         = details::platform::utf16;

    //! Defines UTF-32 encoding type.
    /*! `denc::utf32` is always a distinct type. */
    using utf32         = details::platform::utf32;

    //! Defines system narrow encoding type.
    /*! `denc::native_wide` is allowed to be an alias to any other encoding type. */
    using native_narrow = details::platform::native_narrow;

    //! Defines system wide encoding type.
    /*! `denc::native_wide` is allowed to be an alias to any other encoding type. */
    using native_wide   = details::platform::native_wide;

    //! Defines platform-specific native encoding type.
    /*! `denc::native_encoding_type` is always an alias to another encoding type. */
    using native_encoding_type = details::platform::native_encoding_type;

    //! Encoding traits.
    /*! The `denc::encoding_traits` class is a traits class template that abstracts basic
    //  operations for a given encoding type. \n
    //  The `denc::encoding_traits` class template serves as a basis for explicit instantiations.
    //  The user can provide a specialization for any custom encoding types. The following
    //  specializations are already provided by the library:
    //  - `denc::encoding_traits<denc::utf8>`;
    //  - `denc::encoding_traits<denc::utf16>`;
    //  - `denc::encoding_traits<denc::utf32>`;
    //  - `denc::encoding_traits<denc::native_narrow>`;
    //  - `denc::encoding_traits<denc::native_wide>`.
    //
    // The values of the member typedefs are as follows.
    // Specialization                               | `encoding_type`       | `char_type`
    // -------------------------------------------- | --------------------- | ------------
    // `denc::encoding_traits<denc::utf8>`          | `denc::utf8`          | `char`
    // `denc::encoding_traits<denc::utf16>`         | `denc::utf16`         | `char16_t`
    // `denc::encoding_traits<denc::utf32>`         | `denc::utf32`         | `char32_t`
    // `denc::encoding_traits<denc::native_narrow>` | `denc::native_narrow` | `char`
    // `denc::encoding_traits<denc::native_wide>`   | `denc::native_wide`   | `wchar_t`
    */
    template <typename Encoding>
    struct encoding_traits
    {
        //! `Encoding`.
        using encoding_type = typename details::encoding_traits<Encoding>::encoding_type;

        //! An character type that can hold all encoding code points.
        using char_type     = typename details::encoding_traits<Encoding>::char_type;

        /*! Converts natively-encoded characters from the source range [`first`, `last`) to
        //  the `Encoding` encoding, placing the results in the subsequent locations starting
        //  at `result`.
        //
        //  @tparam InputIt - must meet the requirements of `InputIterator`.
        //                    `denc::char_type_t<InputIt>` must be implicitly convertible 
        //                    to `denc::encoding_traits<native_encoding_type>::char_type`.
        //
        //  @tparam OutputIt - must meet the requirements of `OutputIterator`.
        //                     The expression `*result = char_type()` shall be valid.
        //
        //  @param[in] first, last - the range of characters to convert.
        //
        //  @param[in] result      - the beginning of the destination range.
        //
        //  @param[in] loc         - TODO: ...
        //
        //  @return Output iterator to the element past the last element converted.
        //
        //  @throw std::range_error on conversion failure.
        */
        template <typename InputIt, typename OutputIt>
        static OutputIt to_native(InputIt first, InputIt last,
                                  OutputIt result, const std::locale& loc = std::locale());

        /*! Converts `Encoding`-encoded characters from the source range [`first`, `last`) to
        //  the system native encoding, placing the results in the subsequent locations starting
        //  at `result`.
        //
        //  @tparam InputIt - must meet the requirements of `InputIterator`.
        //                    `denc::char_type_t<InputIt>` must be implicitly convertible to
        //                    `char_type`.
        //
        //  @tparam OutputIt - must meet the requirements of `OutputIterator`. The expression
        //                     `*result = denc::encoding_traits<denc::native_encoding_type>::char_type()`
        //                     shall be valid.
        //
        //  @param[in] first, last - the range of characters to convert.
        //
        //  @param[in] result      - the beginning of the destination range.
        //
        //  @param[in] loc         - TODO: ...
        //
        //  @return Output iterator to the element past the last element converted.
        //
        //  @throw std::range_error on conversion failure.
        */
        template <typename InputIt, typename OutputIt>
        static OutputIt from_native(InputIt first, InputIt last,
                                    OutputIt result, const std::locale& loc = std::locale());
    };


    //-------------------------------------------------------------------------------------------//
    //                                       class codec                                         //
    //-------------------------------------------------------------------------------------------//

    //! TODO: ...
    template <typename SrcEnc, typename DstEnc>
    class codec
    {
    public:
        using source_encoding_type = SrcEnc;

        using destination_encoding_type = DstEnc;

    public:
        explicit codec(const std::locale& loc = std::locale());

        codec(const codec&) = delete;

        codec& operator=(const codec&) = delete;

    public:
        //! Returns the current locale associated with the codec.
        std::locale getloc() const;

    public:
        /*! Converts `SrcEnc`-encoded characters from the source range `src` to the `DstEnc`
        //  encoding, placing the results in the subsequent locations starting at `result`.
        //
        //  @tparam Source - should be eather an iterator type and meat the requirements of
        //                   `InputIterator`, or an range type and meat the requirements of `InputRange`. \n
        //                   In both cases `denc::char_type_t<std::decay_t<Source>>` must be implicitly
        //                   convertible to `denc::encoding_traits<SrcEnc>::char_type`.
        //
        //  @tparam OutputIt - must meet the requirements of `OutputIterator`. The expression
        //                     `*result = denc::encoding_traits<DstEnc>::char_type()` shall be valid.
        //
        //  @param[in] src    - an iterator to a null-terminated character sequence, or an range of characters.
        //
        //  @param[in] result - the beginning of the destination range.
        //
        //  @param[in] loc    - TODO: ...
        //
        //
        //  @return Output iterator to the element past the last element converted.
        //
        //  @throw std::range_error on conversion failure.
        //
        // ---- Notes ----
        // TODO: ...
        //
        */
        template <typename Source, typename ResultToken>
        auto operator()(Source&& src, ResultToken&& token)
        -> typename encode_result<std::decay_t<ResultToken>>::type;

        /*! Converts `SrcEnc`-encoded characters from the source range [`first`, `last`) to
        //  the `DstEnc` encoding, placing the results in the subsequent locations starting
        //  at `result`.
        //
        //  @tparam InputIt - must meet the requirements of `InputIterator`. `denc::char_type_t<InputIt>`
        //                    must be implicitly convertible to `denc::encoding_traits<SrcEnc>::char_type`.
        //
        //  @tparam OutputIt - must meet the requirements of `OutputIterator`. The expression
        //                     `*result = denc::encoding_traits<DstEnc>::char_type()` shall be valid.
        //
        //  @param[in] first, last - the range of characters to convert.
        //
        //  @param[in] result      - the beginning of the destination range.
        //
        //  @param[in] loc         - TODO: ...
        //
        //  @return Output iterator to the element past the last element converted.
        //
        //  @throw std::range_error on conversion failure.
        //
        //
        // ---- Notes ----
        // TODO: ...
        //
        */
        template <typename InputIt, typename ResultToken>
        auto operator()(InputIt first, InputIt last, ResultToken&& token)
        -> typename encode_result<std::decay_t<ResultToken>>::type;

        //! Replaces the current locale with `loc`.
        /*! @param loc - new locale to use.
        //
        //  @return The locale before the call to this function.
        */
        std::locale imbue(const std::locale& loc);

    private:
        std::locale m_loc;
    };


    //-------------------------------------------------------------------------------------------//
    //                                    INLINE DEFINITIONS                                     //
    //-------------------------------------------------------------------------------------------//

    template <typename Encoding>
    template <typename InputIt, typename OutputIt>
    inline OutputIt encoding_traits<Encoding>::to_native(InputIt first, InputIt last,
                                                         OutputIt result, const std::locale& loc)
    {
        return details::encoding_traits<Encoding>::to_native(first, last, result, loc);
    }

    template <typename Encoding>
    template <typename InputIt, typename OutputIt>
    inline OutputIt encoding_traits<Encoding>::from_native(InputIt first, InputIt last,
                                                           OutputIt result, const std::locale& loc)
    {
        return details::encoding_traits<Encoding>::from_native(first, last, result, loc);
    }


    template <typename SrcEnc, typename DstEnc>
    codec<SrcEnc, DstEnc>::codec(const std::locale& loc)
    : m_loc(loc)
    { }

    template <typename SrcEnc, typename DstEnc>
    inline std::locale codec<SrcEnc, DstEnc>::getloc() const
    {
        return m_loc;
    }

    template <typename SrcEnc, typename DstEnc>
    template <typename Source, typename ResultToken>
    inline auto codec<SrcEnc, DstEnc>::operator()(Source&& src, ResultToken&& token)
    -> typename encode_result<std::decay_t<ResultToken>>::type
    {
        return details::encode<SrcEnc, DstEnc>(std::forward<Source>(src),
                                               std::forward<ResultToken>(token), m_loc);
    }

    template <typename SrcEnc, typename DstEnc>
    template <typename InputIt, typename ResultToken>
    inline auto codec<SrcEnc, DstEnc>::operator()(InputIt first, InputIt last, ResultToken&& token)
    -> typename encode_result<std::decay_t<ResultToken>>::type
    {
        return details::encode<SrcEnc, DstEnc>(first, last,
                                               std::forward<ResultToken>(token), m_loc);
    }

    template <typename SrcEnc, typename DstEnc>
    inline std::locale codec<SrcEnc, DstEnc>::imbue(const std::locale& loc)
    {
        auto old_loc = m_loc;
        return (m_loc = loc), old_loc;
    }

} // namespace denc