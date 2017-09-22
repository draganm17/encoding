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

    //-------------------------------------------------------------------------------------------//
    //                                   NON-MEMBER FUNCTIONS                                    //
    //-------------------------------------------------------------------------------------------//

    template <typename SrcEnc, typename DstEnc, typename Source, typename OutputIt>
    OutputIt do_encode(Source&& src, OutputIt result, const std::locale& loc);

    template <typename SrcEnc, typename DstEnc, typename InputIt, typename OutputIt>
    OutputIt do_encode(InputIt first, InputIt last, OutputIt result, const std::locale& loc);


    //-------------------------------------------------------------------------------------------//
    //                                    INLINE DEFINITIONS                                     //
    //-------------------------------------------------------------------------------------------//

    template <typename SrcEnc, typename DstEnc, typename Source, typename OutputIt>
    inline OutputIt do_encode(Source&& src, OutputIt result, const std::locale& loc)
    {
        using SourceT = std::decay_t<Source>;
        if constexpr(is_range<SourceT>::value)
        // 'Source' is a range.
        {
            if constexpr(is_contiguous_range<SourceT>::value)
            // 'Source' is an contiguous range.
            {
                using std::data; using std::size;
                return do_encode<SrcEnc, DstEnc>(data(src), data(src) + size(src), result, loc);
            } else
            // 'Source' is at least an input range.
            {
                using std::begin; using std::end;
                return do_encode<SrcEnc, DstEnc>(begin(src), end(src), result, loc);
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
                return do_encode<SrcEnc, DstEnc>(first, last, result, loc);
            } else
            // 'Source' is at least an input iterator.
            {
                using NTSCIt = ntcs_iterator<SourceT>;
                return do_encode<SrcEnc, DstEnc>(NTSCIt(src), NTSCIt(), result, loc);
            }
        }
    }

    template <typename SrcEnc, typename DstEnc, typename InputIt, typename OutputIt>
    inline OutputIt do_encode(InputIt first, InputIt last, OutputIt result, const std::locale& loc)
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

}} // namespace denc::details
