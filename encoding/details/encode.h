#pragma once

#include <algorithm>
#include <locale>
#include <type_traits>

#include <encoding/details/traits.h>
#include <encoding/details/ntcs_iterator.h>
#ifdef _WIN32
#include <encoding/details/platform.win.h>
#else
#include <encoding/details/platform.pos.h>
#endif


namespace denc {
namespace details {

    struct it_tag { };
    struct seq_tag { };


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
    //                                   NON-MEMBER FUNCTIONS                                    //
    //-------------------------------------------------------------------------------------------//

    template <typename SrcEnc, typename DstEnc,
              typename Source, typename ResultToken
    >
    typename encode_result<std::decay_t<ResultToken>>::type
    encode(Source&& src, ResultToken&& token, const std::locale& loc);

    template <typename SrcEnc, typename DstEnc,
              typename InputIt, typename ResultToken
    >
    typename encode_result<std::decay_t<ResultToken>>::type
    encode(InputIt first, InputIt last, ResultToken&& token, const std::locale& loc);

    template <typename SrcEnc, typename DstEnc,
              typename Source, typename OutputIt
    >
    OutputIt do_encode(Source&& src, it_tag, OutputIt result, const std::locale& loc);

    template <typename SrcEnc, typename DstEnc,
              typename Source, typename Sequence
    >
    Sequence do_encode(Source&& src, seq_tag, Sequence result, const std::locale& loc);

    template <typename SrcEnc, typename DstEnc,
              typename InputIt, typename OutputIt
    >
    OutputIt do_encode(InputIt first, InputIt last,
                       it_tag, OutputIt result, const std::locale& loc);

    template <typename SrcEnc, typename DstEnc,
              typename InputIt, typename Sequence
    >
    Sequence do_encode(InputIt first, InputIt last,
                       seq_tag, Sequence result, const std::locale& loc);


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

    template <typename SrcEnc, typename DstEnc,
              typename Source, typename ResultToken
    >
    inline typename encode_result<std::decay_t<ResultToken>>::type
    encode(Source&& src, ResultToken&& token, const std::locale& loc)
    {
        using Token = std::decay_t<ResultToken>;
        using Result  = typename encode_result<Token>::type;
        using ImplTag = std::conditional_t<std::is_same_v<Result, Token>, it_tag, seq_tag>;

        return do_encode<SrcEnc, DstEnc>(
               std::forward<Source>(src), ImplTag(),
               encode_result<Token>(std::forward<ResultToken>(token)).get(), loc);
    }

    template <typename SrcEnc, typename DstEnc,
              typename InputIt, typename ResultToken
    >
    inline typename encode_result<std::decay_t<ResultToken>>::type
    encode(InputIt first, InputIt last, ResultToken&& token, const std::locale& loc)
    {
        using Token   = std::decay_t<ResultToken>;
        using Result  = typename encode_result<Token>::type;
        using ImplTag = std::conditional_t<std::is_same_v<Result, Token>, it_tag, seq_tag>;

        encode_result<Token> xx(std::forward<ResultToken>(token));

        return do_encode<SrcEnc, DstEnc>(
               first, last, ImplTag(),
               encode_result<Token>(std::forward<ResultToken>(token)).get(), loc);
    }

    template <typename SrcEnc, typename DstEnc, typename Source, typename OutputIt>
    inline OutputIt do_encode(Source&& src, it_tag, OutputIt result, const std::locale& loc)
    {
        using SourceT = std::decay_t<Source>;
        if constexpr(is_range<SourceT>::value)
        // 'Source' is a range.
        {
            if constexpr(is_contiguous_range<SourceT>::value)
            // 'Source' is an contiguous range.
            {
                using std::data; using std::size;
                return do_encode<SrcEnc, DstEnc>(data(src), 
                                                 data(src) + size(src), it_tag(), result, loc);
            } else
            // 'Source' is at least an input range.
            {
                using std::begin; using std::end;
                return do_encode<SrcEnc, DstEnc>(begin(src), end(src), it_tag(), result, loc);
            }
        } else
        // 'Source' is an iterator.
        {
            if constexpr(std::is_base_of_v<std::forward_iterator_tag,
                                           typename std::iterator_traits<SourceT>::iterator_category>)
            // 'Source' is at least an forward iterator.
            {
                auto first = src, last = src;
                const auto null_char = typename std::iterator_traits<SourceT>::value_type();

                for (; *last != null_char; ++last); // calculate length
                return do_encode<SrcEnc, DstEnc>(first, last, it_tag(), result, loc);
            } else
            // 'Source' is at least an input iterator.
            {
                using NTSCIt = ntcs_iterator<SourceT>;
                return do_encode<SrcEnc, DstEnc>(NTSCIt(src), NTSCIt(), it_tag(), result, loc);
            }
        }
    }

    template <typename SrcEnc, typename DstEnc, typename Source, typename Sequence>
    inline Sequence do_encode(Source&& src, seq_tag, Sequence result, const std::locale& loc)
    {
        do_encode<SrcEnc, DstEnc>(std::forward<Source>(src), it_tag(),
                                  std::inserter(result, result.end()), loc);
        return std::move(result);
    }

    template <typename SrcEnc, typename DstEnc, typename InputIt, typename OutputIt>
    inline OutputIt do_encode(InputIt first, InputIt last,
                              it_tag, OutputIt result, const std::locale& loc)
    {
        using SrcTraits = encoding_traits<SrcEnc>;
        using DstTraits = encoding_traits<DstEnc>;
        using NtTraits  = encoding_traits<platform::native_encoding_type>;

        if constexpr(std::is_same_v<SrcEnc, DstEnc>)
        // The source and destination encodings are the same.
        // No conversion perfomed.
        {
            return std::copy(first, last, result);
        } else 
        if constexpr(std::is_same_v<DstEnc, platform::native_encoding_type>)
        // The destination encoding is the system native encoding.
        // One-step conversion performed.
        {
            return SrcTraits::to_native(first, last, result, loc);
        } else
        if constexpr(std::is_same_v<SrcEnc, platform::native_encoding_type>)
        // The source encoding is the system native encoding.
        // One-step conversion performed.
        {
            return DstTraits::from_native(first, last, result, loc);
        } else
        // Neather the source or the destination encoding match the system native encoding.
        // Two-step conversion performed.
        {
            std::basic_string<typename NtTraits::char_type> tmp;
            SrcTraits::to_native(first, last, std::back_inserter(tmp), loc);
            return DstTraits::from_native(tmp.data(), tmp.data() + tmp.size(), result, loc);
        }
    }

    template <typename SrcEnc, typename DstEnc, typename InputIt, typename Sequence>
    inline Sequence do_encode(InputIt first, InputIt last,
                              seq_tag, Sequence result, const std::locale& loc)
    {
        do_encode<SrcEnc, DstEnc>(first, last, it_tag(),
                                  std::inserter(result, result.end()), loc);
        return std::move(result);
    }

}} // namespace denc::details
