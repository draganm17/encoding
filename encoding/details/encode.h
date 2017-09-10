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
        if constexpr(is_range<std::remove_reference_t<Source>>::value) 
        // 'Source' is a range.
        {
            if constexpr(is_contiguous_range<std::remove_reference_t<Source>>::value)
            // 'Source' is a contiguous range.
            {
                using std::data; using std::size;
                return do_encode<SrcEnc, DstEnc>(data(src), data(src) + size(src), result, loc);
            } else
            // 'Source' is not a contiguous range.
            {
                using std::begin; using std::end;
                return do_encode<SrcEnc, DstEnc>(begin(src), end(src), result, loc);
            }
        } else
        // 'Source' is not a range. Assumed to be an iterator.
        {
            if constexpr(std::is_pointer_v<std::remove_reference_t<Source>>)
            // 'Source' is a pointer.
            {
                using CharT = char_type_t<std::remove_reference_t<Source>>;
                const size_t src_size = std::char_traits<CharT>::length(src);
                return do_encode<SrcEnc, DstEnc>(src, src + src_size, result, loc);
            } else
            // 'Source' is not a pointer.
            {
                using NTSCIt = ntcs_iterator<std::decay_t<Source>>;
                return do_encode<SrcEnc, DstEnc>(NTSCIt(src), NTSCIt(), result, loc);
            }
        }
    }

    template <typename SrcEnc, typename DstEnc, typename InputIt, typename OutputIt>
    inline OutputIt do_encode(InputIt first, InputIt last, OutputIt result, const std::locale& loc)
    {
        using SrcTraits = encoding_traits<SrcEnc>;
        using DstTraits = encoding_traits<DstEnc>;
        using NtTraits  = encoding_traits<native_encoding_type>;

        if constexpr(std::is_same_v<SrcEnc, DstEnc>)
        // The source and destination encodings are the same.
        // No conversion perfomed.
        {
            return std::copy(first, last, result);
        } else 
        if constexpr(std::is_same_v<DstEnc, native_encoding_type>)
        // The destination encoding is the system native encoding.
        // One-step conversion performed.
        {
            return SrcTraits::to_native(first, last, result, loc);
        } else
        if constexpr(std::is_same_v<SrcEnc, native_encoding_type>)
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