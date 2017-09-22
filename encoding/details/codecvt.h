#pragma once

#include <algorithm>
#include <iterator>
#include <locale>
#include <stdexcept>
#include <type_traits>


namespace denc {
namespace details {

    //-------------------------------------------------------------------------------------------//
    //                                   NON-MEMBER FUNCTIONS                                    //
    //-------------------------------------------------------------------------------------------//

    template <typename InputIt, typename OutputIt>
    OutputIt copy_constexpr_if(InputIt first, InputIt last, OutputIt result, std::true_type);

    template <typename InputIt, typename OutputIt>
    OutputIt copy_constexpr_if(InputIt first, InputIt last, OutputIt result, std::false_type);

    template <typename InCharT, typename OutCharT,
              typename InputIt, typename OutputIt, 
              typename Codecvt, typename Fn
    >
    OutputIt do_codecvt(InputIt first, InputIt last,
                        OutputIt result, const Codecvt& cvt, Fn fn);

    template <typename InCharT, typename OutCharT, 
              typename OutputIt, typename Codecvt, typename Fn
    >
    OutputIt do_codecvt(InCharT* first, InCharT* last, 
                        OutputIt result, const Codecvt& cvt, Fn fn);

    template <typename InCharT, typename OutCharT, 
              typename OutputIt, typename Codecvt, typename Fn
    >
    OutputIt do_codecvt(const InCharT* first, const InCharT* last, 
                        OutputIt result, const Codecvt& cvt, Fn fn);

    template <typename InputIt, typename OutputIt, 
              typename InternT, typename ExternT, 
              typename State
    >
    OutputIt codecvt_in(InputIt first, InputIt last, 
                        OutputIt result, const std::codecvt<InternT, ExternT, State>& cvt);

    template<typename InputIt, typename OutputIt, 
             typename InternT, typename ExternT, 
             typename State
    >
    OutputIt codecvt_out(InputIt first, InputIt last, 
                         OutputIt result, const std::codecvt<InternT, ExternT, State>& cvt);


    //-------------------------------------------------------------------------------------------//
    //                                    INLINE DEFINITIONS                                     //
    //-------------------------------------------------------------------------------------------//

    template <typename InputIt, typename OutputIt>
    inline OutputIt copy_constexpr_if(InputIt first, InputIt last, OutputIt result, std::true_type)
    {
        return std::copy(first, last, result);
    }

    template <typename InputIt, typename OutputIt>
    inline OutputIt copy_constexpr_if(InputIt first, InputIt last, OutputIt result, std::false_type)
    {
        return result;
    }

    template <typename InCharT, typename OutCharT,
              typename InputIt, typename OutputIt, 
              typename Codecvt, typename Fn
    >
    inline OutputIt do_codecvt(InputIt first, InputIt last, 
                               OutputIt result, const Codecvt& cvt, Fn fn)
    {
        if (first == last) return result;
        if (cvt.always_noconv()) return std::copy(first, last, result);

        std::basic_string<InCharT> in;
        if constexpr(std::is_base_of_v<std::forward_iterator_tag,
                                       typename std::iterator_traits<InputIt>::iterator_category>)
        // 'InputIt' is at least an forward iterator.
        {
            in.reserve(std::distance(first, last));
        }
        std::copy(first, last, std::back_inserter(in));
        return do_codecvt<InCharT, OutCharT>(in.data(), in.data() + in.size(), result, cvt, fn);
    }

    template <typename InCharT, typename OutCharT, 
              typename OutputIt, typename Codecvt, typename Fn
    >
    inline OutputIt do_codecvt(InCharT* first, InCharT* last, 
                               OutputIt result, const Codecvt& cvt, Fn fn)
    {
        return do_codecvt<InCharT, OutCharT>((const InCharT*)first, 
                                             (const InCharT*)last, result, cvt, fn);
    }

    template <typename InCharT, typename OutCharT, 
              typename OutputIt, typename Codecvt, typename Fn
    >
    inline OutputIt do_codecvt(const InCharT* first, const InCharT* last, 
                               OutputIt result, const Codecvt& cvt, Fn fn)
    {
        if (first == last) return result;
        if (cvt.always_noconv()) return std::copy(first, last, result);

        typename Codecvt::result res;
        typename Codecvt::state_type state{};
        do {
            OutCharT  out[64];
            OutCharT* out_next = out;
            const InCharT* in_next = first;
            using DoCopy = std::is_same<InCharT, OutCharT>; // always true on 'noconv'
            res = (cvt.*fn)(state, first, last, in_next, out, std::end(out), out_next);
            result = res != Codecvt::noconv ? std::copy(out, out_next, result)
                                            : copy_constexpr_if(first, in_next, result, DoCopy());
            first = in_next;
	    } while (res != Codecvt::error && first != last);

        if (res == Codecvt::ok) return result;
        throw std::range_error("Bad conversion");
    }

    template <typename InputIt, typename OutputIt, 
              typename InternT, typename ExternT, typename State
    >
    inline OutputIt codecvt_in(InputIt first, InputIt last, OutputIt result,
                               const std::codecvt<InternT, ExternT, State>& cvt)
    {
        return do_codecvt<ExternT, InternT>(first, last, result, cvt,
                                            &std::codecvt<InternT, ExternT, State>::in);
    }

    template <typename InputIt, typename OutputIt, 
              typename InternT, typename ExternT, typename State
    >
    inline OutputIt codecvt_out(InputIt first, InputIt last, OutputIt result,
                                const std::codecvt<InternT, ExternT, State>& cvt)
    {
        return do_codecvt<InternT, ExternT>(first, last, result, cvt,
                                            &std::codecvt<InternT, ExternT, State>::out);
    }

}} // namespace denc::details
