#include <algorithm>
#include <forward_list>
#include <iterator>
#include <sstream>
#include <string>
#include <type_traits>
#include <typeindex>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <encoding/denc.h>

using namespace denc;

#define TEST_TEXT                            \
        "«Мой дядя самых честных правил,\n"  \
        "Когда не в шутку занемог,\n"        \
        "Он уважать себя заставил\n"         \
        "И лучше выдумать не мог.\n"         \
        "Его пример другим наука;\n"         \
        "Но, боже мой, какая скука\n"        \
        "С больным сидеть и день и ночь,\n"  \
        "Не отходя ни шагу прочь!\n"         \
        "Какое низкое коварство\n"           \
        "Полуживого забавлять,\n"            \
        "Ему подушки поправлять,\n"          \
        "Печально подносить лекарство,\n"    \
        "Вздыхать и думать про себя:\n"      \
        "Когда же черт возьмет тебя!»"

#define COUNT(arr)  sizeof((arr)) / sizeof((arr)[0])


namespace {

    const char     utf8_str[]          = u8"" TEST_TEXT;
    const char16_t utf16_str[]         = u""  TEST_TEXT;
    const char32_t utf32_str[]         = U""  TEST_TEXT;
    const char     native_narrow_str[] =      TEST_TEXT;
    const wchar_t  native_wide_str[]   = L""  TEST_TEXT;

    constexpr const char     (&encoded_string(utf8))          [COUNT(utf8_str)]          { return utf8_str; }
    constexpr const char16_t (&encoded_string(utf16))         [COUNT(utf16_str)]         { return utf16_str; }
    constexpr const char32_t (&encoded_string(utf32))         [COUNT(utf32_str)]         { return utf32_str; }
#ifdef DENC_NATIVE_NARROW_DISCTINCT_TYPE
    constexpr const char     (&encoded_string(native_narrow)) [COUNT(native_narrow_str)] { return native_narrow_str; }
#endif
#ifdef DENC_NATIVE_WIDE_DISCTINCT_TYPE
    constexpr const wchar_t  (&encoded_string(native_wide) )  [COUNT(native_wide_str)]   { return native_wide_str; }
#endif

    //template <typename Encoding>
    //const auto& encoded_input_range()
    //{
        // TODO: ...

    //    return encoded_contiguous_range<Encoding>();
    //}

    //template <typename Encoding>
    //const auto& encoded_null_terminated_input_range()
    //{
        // TODO: ...

    //    return encoded_null_terminated_contiguous_range<Encoding>();
    //}

    template <typename Encoding>
    const auto& encoded_forward_range()
    {
        using CharT = typename encoding_traits<Encoding>::char_type;
        static std::forward_list<CharT> result(std::begin(encoded_string(Encoding())),
                                               std::end(encoded_string(Encoding())) - 1);
        return result;
    }

    template <typename Encoding>
    const auto& encoded_null_terminated_forward_range()
    {
        using CharT = typename encoding_traits<Encoding>::char_type;
        static std::forward_list<CharT> result(std::begin(encoded_string(Encoding())),
                                               std::end(encoded_string(Encoding())));
        return result;
    }

    template <typename Encoding>
    const auto& encoded_contiguous_range()
    {
        using CharT = typename encoding_traits<Encoding>::char_type;
        static std::basic_string<CharT> result = encoded_string(Encoding());
        return result;
    }

    template <typename Encoding>
    const auto& encoded_null_terminated_contiguous_range()
    {
        using CharT = typename encoding_traits<Encoding>::char_type;
        static std::basic_string<CharT> result(std::begin(encoded_string(Encoding())),
                                               std::end(encoded_string(Encoding())));
        return result;
    }

    template <typename Encoding>
    const auto& encoded_input_range()
    {
        // TODO: ...

        return encoded_contiguous_range<Encoding>();
    }

    template <typename Encoding>
    const auto& encoded_null_terminated_input_range()
    {
        // TODO: ...

        return encoded_null_terminated_contiguous_range<Encoding>();
    }

    template <typename DP, typename CharT, typename = void>
    struct can_deduce_impl : std::false_type { };
    
    template <typename DP, typename CharT>
    struct can_deduce_impl<DP, CharT, std::void_t<typename DP::template encoding_type<CharT>>>
    : std::true_type { };
    
    template <typename DP, typename CharT>
    struct can_deduce : can_deduce_impl<DP, CharT> { };

    template <typename CharT,
              typename It = typename std::basic_string<CharT>::iterator
    >
    struct test_char_type_iterator_base
    : std::conjunction<std::is_same<char_type_t<               It>,  std::remove_cv_t<CharT>>,
                       std::is_same<char_type_t<const          It>,  std::remove_cv_t<CharT>>,
                       std::is_same<char_type_t<      volatile It>,  std::remove_cv_t<CharT>>,
                       std::is_same<char_type_t<const volatile It>,  std::remove_cv_t<CharT>>>
    { };

    template <typename CharT>
    struct test_char_type_iterator
    : std::conjunction<test_char_type_iterator_base<         std::remove_cv_t<CharT>>,
                       test_char_type_iterator_base<volatile std::remove_cv_t<CharT>>>
    { };

    template <typename CharT>
    struct test_char_type_pointer_iterator_base
    : std::conjunction<std::is_same<char_type_t<CharT*               >,  std::remove_cv_t<CharT>>,
                       std::is_same<char_type_t<CharT* const         >,  std::remove_cv_t<CharT>>,
                       std::is_same<char_type_t<CharT*       volatile>,  std::remove_cv_t<CharT>>,
                       std::is_same<char_type_t<CharT* const volatile>,  std::remove_cv_t<CharT>>>
    { };

    template <typename CharT>
    struct test_char_type_pointer_iterator
    : std::conjunction<test_char_type_pointer_iterator_base<               std::remove_cv_t<CharT>>,
                       test_char_type_pointer_iterator_base<const          std::remove_cv_t<CharT>>,
                       test_char_type_pointer_iterator_base<      volatile std::remove_cv_t<CharT>>,
                       test_char_type_pointer_iterator_base<const volatile std::remove_cv_t<CharT>>>
    { };

    template <typename CharT,
              typename RangeT = std::basic_string<CharT>
    >
    struct test_char_type_range_base
    : std::conjunction<std::is_same<char_type_t<      RangeT>,  std::remove_cv_t<CharT>>,
                       std::is_same<char_type_t<const RangeT>,  std::remove_cv_t<CharT>>>
    { };

    template <typename CharT>
    struct test_char_type_range
    : std::conjunction<test_char_type_range_base<         std::remove_cv_t<CharT>>,
                       test_char_type_range_base<volatile std::remove_cv_t<CharT>>>
    { };

    template <typename CharT>
    struct test_char_type_array_range_base
    : std::conjunction<std::is_same<char_type_t<               CharT[8]>,  std::remove_cv_t<CharT>>,
                       std::is_same<char_type_t<const          CharT[8]>,  std::remove_cv_t<CharT>>,
                       std::is_same<char_type_t<      volatile CharT[8]>,  std::remove_cv_t<CharT>>,
                       std::is_same<char_type_t<const volatile CharT[8]>,  std::remove_cv_t<CharT>>>
    { };

    template <typename CharT>
    struct test_char_type_array_range
    : std::conjunction<test_char_type_array_range_base<std::remove_cv_t<CharT>>>
    { };

    template <typename CharT>
    struct test_char_type
    : std::conjunction<test_char_type_iterator<CharT>,
                       test_char_type_pointer_iterator<CharT>,
                       test_char_type_range<CharT>,
                       test_char_type_array_range<CharT>>
    { };

    template <typename CharT, 
              typename DP, 
              typename It       = typename std::basic_string<CharT>::iterator,
              typename Encoding = typename DP::template encoding_type<std::remove_cv_t<CharT>>
    >
    struct test_encoding_type_iterator_base
    : std::conjunction<std::is_same<encoding_type_t<               It, DP>, Encoding>,
                       std::is_same<encoding_type_t<const          It, DP>, Encoding>,
                       std::is_same<encoding_type_t<      volatile It, DP>, Encoding>,
                       std::is_same<encoding_type_t<const volatile It, DP>, Encoding>>
    { };

    template <typename CharT, typename DP>
    struct test_encoding_type_iterator
    : std::conjunction<test_encoding_type_iterator_base<         std::remove_cv_t<CharT>, DP>,
                       test_encoding_type_iterator_base<volatile std::remove_cv_t<CharT>, DP>>
    { };

    template <typename CharT, typename DP,
              typename Encoding = typename DP::template encoding_type<std::remove_cv_t<CharT>>
    >
    struct test_encoding_type_pointer_iterator_base
    : std::conjunction<std::is_same<encoding_type_t<CharT*               , DP>, Encoding>,
                       std::is_same<encoding_type_t<CharT* const         , DP>, Encoding>,
                       std::is_same<encoding_type_t<CharT*       volatile, DP>, Encoding>,
                       std::is_same<encoding_type_t<CharT* const volatile, DP>, Encoding>>
    { };

    template <typename CharT, typename DP>
    struct test_encoding_type_pointer_iterator
    : std::conjunction<test_encoding_type_pointer_iterator_base<               std::remove_cv_t<CharT>, DP>,
                       test_encoding_type_pointer_iterator_base<const          std::remove_cv_t<CharT>, DP>,
                       test_encoding_type_pointer_iterator_base<      volatile std::remove_cv_t<CharT>, DP>,
                       test_encoding_type_pointer_iterator_base<const volatile std::remove_cv_t<CharT>, DP>>
    { };

    template <typename CharT, typename DP,
              typename Range    = std::basic_string<CharT>,
              typename Encoding = typename DP::template encoding_type<std::remove_cv_t<CharT>>
    >
    struct test_encoding_type_range_base
    : std::conjunction<std::is_same<encoding_type_t<      Range>, Encoding>,
                       std::is_same<encoding_type_t<const Range>, Encoding>>
    { };

    template <typename CharT, typename DP>
    struct test_encoding_type_range
    : std::conjunction<test_encoding_type_range_base<         std::remove_cv_t<CharT>, DP>,
                       test_encoding_type_range_base<volatile std::remove_cv_t<CharT>, DP>>
    { };

    template <typename CharT, typename DP,
              typename Encoding = typename DP::template encoding_type<std::remove_cv_t<CharT>>>
    struct test_encoding_type_array_range_base
    : std::conjunction<std::is_same<encoding_type_t<               CharT[8]>, Encoding>,
                       std::is_same<encoding_type_t<const          CharT[8]>, Encoding>,
                       std::is_same<encoding_type_t<      volatile CharT[8]>, Encoding>,
                       std::is_same<encoding_type_t<const volatile CharT[8]>, Encoding>>
    { };

    template <typename CharT, typename DP>
    struct test_encoding_type_array_range
    : std::conjunction<test_encoding_type_array_range_base<std::remove_cv_t<CharT>, DP>>
    { };

    template <typename CharT, typename DP>
    struct test_encoding_type
    : std::conjunction<test_encoding_type_iterator<CharT, DP>,
                       test_encoding_type_pointer_iterator<CharT, DP>,
                       test_encoding_type_range<CharT, DP>,
                       test_encoding_type_array_range_base<CharT, DP>>
    { };

    template <typename Encoding, typename CharT>
    struct test_encoding_traits_typedefs
    : std::conjunction<std::is_same<typename encoding_traits<Encoding>::encoding_type, Encoding>,
                       std::is_same<typename encoding_traits<Encoding>::char_type,     CharT>>
    { };

    template <typename Encoding,
              typename InputIt1,
              typename InputIt2>
    bool do_test_encoding_traits_conversions(InputIt1 first1, InputIt1 last1,
                                             InputIt2 first2, InputIt2 last2)
    {
        using Traits = encoding_traits<Encoding>;

    #ifdef _WIN32
        std::wstring   to_native_result;
        std::locale loc = std::locale("ru-RU");
    #else
        std::string    to_native_result;
        std::locale loc = std::locale("en_US.UTF-8");
    #endif

        std::basic_string<typename Traits::char_type> from_native_result;
        Traits::to_native(first1, last1, std::back_inserter(to_native_result), loc);
        Traits::from_native(first2, last2,std::back_inserter(from_native_result), loc);

        return to_native_result   == encoded_string(native_encoding_type()) &&
               from_native_result == encoded_string(Encoding());
    }

    template <typename Encoding>
    bool test_encoding_traits_conversions_input_iterator()
    {
        auto& rng1 = encoded_input_range<Encoding>();
        auto& rng2 = encoded_input_range<native_encoding_type>();
        return do_test_encoding_traits_conversions<Encoding>(std::begin(rng1), std::end(rng1),
                                                             std::begin(rng2), std::end(rng2));
    }

    template <typename Encoding>
    bool test_encoding_traits_conversions_forward_iterator()
    {
        auto& rng1 = encoded_forward_range<Encoding>();
        auto& rng2 = encoded_forward_range<native_encoding_type>();
        return do_test_encoding_traits_conversions<Encoding>(std::begin(rng1), std::end(rng1),
                                                             std::begin(rng2), std::end(rng2));
    }

    template <typename Encoding>
    bool test_encoding_traits_conversions_contiguous_iterator()
    {
        auto& rng1 = encoded_contiguous_range<Encoding>();
        auto& rng2 = encoded_contiguous_range<native_encoding_type>();
        return do_test_encoding_traits_conversions<Encoding>(std::data(rng1), std::data(rng1) + std::size(rng1),
                                                             std::data(rng2), std::data(rng2) + std::size(rng2));
    }

    template <typename Encoding>
    bool test_encoding_traits_conversions()
    {
        return test_encoding_traits_conversions_input_iterator<Encoding>()   &&
               test_encoding_traits_conversions_forward_iterator<Encoding>() &&
               test_encoding_traits_conversions_contiguous_iterator<Encoding>();
    }

    template <typename Encoding, typename CharT>
    bool test_encoding_traits()
    {
        return test_encoding_traits_typedefs<Encoding, CharT>() &&
               test_encoding_traits_conversions<Encoding>();
    }

    template <typename SrcEnc, 
              typename DstEnc,
              typename... Args
    >
    bool do_test_encode(Args&&... args)
    {
    #ifdef _WIN32
        std::locale loc = std::locale("ru-RU");
    #else
        std::locale loc = std::locale("en_US.UTF-8");
    #endif
        using DstCharT = typename encoding_traits<DstEnc>::char_type;

        codec<SrcEnc, DstEnc> codec(loc);
        std::basic_string<DstCharT> result1, result2;
        auto it = codec(std::forward<Args>(args)..., std::back_inserter(result1));
        result2 = codec(std::forward<Args>(args)..., use_basic_string<DstCharT>());

        return //it     == result.end() &&
               result1 == encoded_string(DstEnc()) &&
               result2 == encoded_string(DstEnc());
    }

    template <typename SrcEnc, typename DstEnc>
    bool test_encode_1_input_iterator()
    {
        auto& rng = encoded_null_terminated_input_range<SrcEnc>();
        return do_test_encode<SrcEnc, DstEnc>(std::begin(rng));

        //using InCharT = encoding_traits<SrcEnc>::char_type;
        //std::basic_stringstream<InCharT> src;

        //std::noskipws(src);
        //src.write(encoded_string(SrcEnc()), sizeof(encoded_string(SrcEnc())));
        //return do_test_encode<SrcEnc, DstEnc>(std::istream_iterator<InCharT, InCharT>(src));
    }

    template <typename SrcEnc, typename DstEnc>
    bool test_encode_1_forward_iterator()
    {
        auto& rng = encoded_null_terminated_forward_range<SrcEnc>();
        return do_test_encode<SrcEnc, DstEnc>(std::begin(rng));
    }

    template <typename SrcEnc, typename DstEnc>
    bool test_encode_1_contiguous_iterator()
    {
        auto& rng = encoded_null_terminated_contiguous_range<SrcEnc>();
        return do_test_encode<SrcEnc, DstEnc>(std::data(rng));
    }

    template <typename SrcEnc, typename DstEnc>
    bool test_encode_1_input_range()
    {
        auto& rng = encoded_input_range<SrcEnc>();
        return do_test_encode<SrcEnc, DstEnc>(rng);
    }

    template <typename SrcEnc, typename DstEnc>
    bool test_encode_1_forward_range()
    {
        auto& rng = encoded_forward_range<SrcEnc>();
        return do_test_encode<SrcEnc, DstEnc>(rng);
    }

    template <typename SrcEnc, typename DstEnc>
    bool test_encode_1_contiguous_range()
    {
        auto& rng = encoded_contiguous_range<SrcEnc>();
        return do_test_encode<SrcEnc, DstEnc>(rng);
    }

    template <typename SrcEnc, typename DstEnc>
    bool test_encode_1_array()
    {
        auto& rng = encoded_string(SrcEnc());
        return do_test_encode<SrcEnc, DstEnc>(rng);
    }

    template <typename SrcEnc, typename DstEnc>
    bool test_encode_1()
    {
        return test_encode_1_input_iterator<SrcEnc, DstEnc>()      &&
               test_encode_1_forward_iterator<SrcEnc, DstEnc>()    &&
               test_encode_1_contiguous_iterator<SrcEnc, DstEnc>() &&
               test_encode_1_input_range<SrcEnc, DstEnc>()         &&
               test_encode_1_forward_range<SrcEnc, DstEnc>()       &&
               test_encode_1_contiguous_range<SrcEnc, DstEnc>()    &&
               test_encode_1_array<SrcEnc, DstEnc>();
    }

    template <typename SrcEnc, typename DstEnc>
    bool test_encode_3_input_iterator()
    {
        auto& rng = encoded_input_range<SrcEnc>();
        return do_test_encode<SrcEnc, DstEnc>(std::begin(rng), std::end(rng));

        //using InCharT = encoding_traits<SrcEnc>::char_type;
        //std::basic_stringstream<InCharT> src;

        //std::noskipws(src);
        //src << encoded_string(SrcEnc());
        //return do_test_encode<SrcEnc, DstEnc>(std::istream_iterator<InCharT, InCharT>(src),
        //                                                std::istream_iterator<InCharT, InCharT>());
    }

    template <typename SrcEnc, typename DstEnc>
    bool test_encode_3_forward_iterator()
    {
        auto& rng = encoded_forward_range<SrcEnc>();
        return do_test_encode<SrcEnc, DstEnc>(std::begin(rng), std::end(rng));
    }

    template <typename SrcEnc, typename DstEnc>
    bool test_encode_3_contiguous_iterator()
    {
        auto& rng = encoded_contiguous_range<SrcEnc>();
        return do_test_encode<SrcEnc, DstEnc>(std::data(rng), std::data(rng) + std::size(rng));
    }

    template <typename SrcEnc, typename DstEnc>
    bool test_encode_3()
    {
        return test_encode_3_input_iterator<SrcEnc, DstEnc>()   &&
               test_encode_3_forward_iterator<SrcEnc, DstEnc>() &&
               test_encode_3_contiguous_iterator<SrcEnc, DstEnc>();
    }
}


TEST(DENC, Encodings)
{
    // encodings are empty class types
    EXPECT_TRUE((std::is_empty_v<utf8>          && std::is_class_v<utf8>));
    EXPECT_TRUE((std::is_empty_v<utf16>         && std::is_class_v<utf16>));
    EXPECT_TRUE((std::is_empty_v<utf32>         && std::is_class_v<utf32>));
    EXPECT_TRUE((std::is_empty_v<native_narrow> && std::is_class_v<native_narrow>));
    EXPECT_TRUE((std::is_empty_v<native_wide>   && std::is_class_v<native_wide>));

    // each encoding type is unique (except 'native_narrow' and 'native_wide')
    std::type_index encs[] = { std::type_index(typeid(utf8)),
                               std::type_index(typeid(utf16)),
                               std::type_index(typeid(utf32)) };
    EXPECT_TRUE(std::distance(std::begin(encs),
                              std::unique(std::begin(encs), std::end(encs))) == 3);

    // 'native_encoding_type' is an alias to one of the encoding types
    EXPECT_TRUE((std::is_same_v<native_encoding_type, utf8>          ||
                 std::is_same_v<native_encoding_type, utf16>         ||
                 std::is_same_v<native_encoding_type, utf32>         ||
                 std::is_same_v<native_encoding_type, native_narrow> ||
                 std::is_same_v<native_encoding_type, native_wide>));
}

TEST(DENC, DefaultDeductionPoliCy)
{
    using DDP = default_deduction_policy;

    // check deduction rules
    EXPECT_TRUE((std::is_same_v<DDP::encoding_type<char>,     native_narrow>));
    EXPECT_TRUE((std::is_same_v<DDP::encoding_type<wchar_t>,  native_wide>));
    EXPECT_TRUE((std::is_same_v<DDP::encoding_type<char16_t>, utf16>));
    EXPECT_TRUE((std::is_same_v<DDP::encoding_type<char32_t>, utf32>));

    // is SFINAE-frendly
    struct empty { };
    EXPECT_FALSE((can_deduce<DDP, empty>()));
}

TEST(DENC, CharType)
{
    // 'T' is an input iterator
    EXPECT_TRUE((test_char_type<char>()));
    EXPECT_TRUE((test_char_type<wchar_t>()));
    EXPECT_TRUE((test_char_type<char16_t>()));
    EXPECT_TRUE((test_char_type<char32_t>()));

    // TODO: test defaulted template parameter
}

TEST(DENC, EncodingType)
{
    using DDP = default_deduction_policy;

    EXPECT_TRUE((test_encoding_type<char, DDP>()));
    EXPECT_TRUE((test_encoding_type<wchar_t, DDP>()));
    EXPECT_TRUE((test_encoding_type<char16_t, DDP>()));
    EXPECT_TRUE((test_encoding_type<char32_t, DDP>()));

    // TODO: test defaulted template parameter
}

TEST(DENC, EncodingTraits)
{
    EXPECT_TRUE((test_encoding_traits<utf8, char>()));
    EXPECT_TRUE((test_encoding_traits<utf16, char16_t>()));
    EXPECT_TRUE((test_encoding_traits<utf32, char32_t>()));
    EXPECT_TRUE((test_encoding_traits<native_narrow, char>()));
    EXPECT_TRUE((test_encoding_traits<native_wide, wchar_t>()));
}

TEST(DENC, Encode_1)
{
    EXPECT_TRUE((test_encode_1<utf8, utf8>()));
    EXPECT_TRUE((test_encode_1<utf8, utf16>()));
    EXPECT_TRUE((test_encode_1<utf8, utf32>()));
    EXPECT_TRUE((test_encode_1<utf8, native_narrow>()));
    EXPECT_TRUE((test_encode_1<utf8, native_wide>()));

    EXPECT_TRUE((test_encode_1<utf16, utf8>()));
    EXPECT_TRUE((test_encode_1<utf16, utf16>()));
    EXPECT_TRUE((test_encode_1<utf16, utf32>()));
    EXPECT_TRUE((test_encode_1<utf16, native_narrow>()));
    EXPECT_TRUE((test_encode_1<utf16, native_wide>()));

    EXPECT_TRUE((test_encode_1<utf32, utf8>()));
    EXPECT_TRUE((test_encode_1<utf32, utf16>()));
    EXPECT_TRUE((test_encode_1<utf32, utf32>()));
    EXPECT_TRUE((test_encode_1<utf32, native_narrow>()));
    EXPECT_TRUE((test_encode_1<utf32, native_wide>()));

    EXPECT_TRUE((test_encode_1<native_narrow, utf8>()));
    EXPECT_TRUE((test_encode_1<native_narrow, utf16>()));
    EXPECT_TRUE((test_encode_1<native_narrow, utf32>()));
    EXPECT_TRUE((test_encode_1<native_narrow, native_narrow>()));
    EXPECT_TRUE((test_encode_1<native_narrow, native_wide>()));

    EXPECT_TRUE((test_encode_1<native_wide, utf8>()));
    EXPECT_TRUE((test_encode_1<native_wide, utf16>()));
    EXPECT_TRUE((test_encode_1<native_wide, utf32>()));
    EXPECT_TRUE((test_encode_1<native_wide, native_narrow>()));
    EXPECT_TRUE((test_encode_1<native_wide, native_wide>()));
}

TEST(DENC, Encode_3)
{
    EXPECT_TRUE((test_encode_3<utf8, utf8>()));
    EXPECT_TRUE((test_encode_3<utf8, utf16>()));
    EXPECT_TRUE((test_encode_3<utf8, utf32>()));
    EXPECT_TRUE((test_encode_3<utf8, native_narrow>()));
    EXPECT_TRUE((test_encode_3<utf8, native_wide>()));

    EXPECT_TRUE((test_encode_3<utf16, utf8>()));
    EXPECT_TRUE((test_encode_3<utf16, utf16>()));
    EXPECT_TRUE((test_encode_3<utf16, utf32>()));
    EXPECT_TRUE((test_encode_3<utf16, native_narrow>()));
    EXPECT_TRUE((test_encode_3<utf16, native_wide>()));

    EXPECT_TRUE((test_encode_3<utf32, utf8>()));
    EXPECT_TRUE((test_encode_3<utf32, utf16>()));
    EXPECT_TRUE((test_encode_3<utf32, utf32>()));
    EXPECT_TRUE((test_encode_3<utf32, native_narrow>()));
    EXPECT_TRUE((test_encode_3<utf32, native_wide>()));

    EXPECT_TRUE((test_encode_3<native_narrow, utf8>()));
    EXPECT_TRUE((test_encode_3<native_narrow, utf16>()));
    EXPECT_TRUE((test_encode_3<native_narrow, utf32>()));
    EXPECT_TRUE((test_encode_3<native_narrow, native_narrow>()));
    EXPECT_TRUE((test_encode_3<native_narrow, native_wide>()));

    EXPECT_TRUE((test_encode_3<native_wide, utf8>()));
    EXPECT_TRUE((test_encode_3<native_wide, utf16>()));
    EXPECT_TRUE((test_encode_3<native_wide, utf32>()));
    EXPECT_TRUE((test_encode_3<native_wide, native_narrow>()));
    EXPECT_TRUE((test_encode_3<native_wide, native_wide>()));
}


int main(int argc, char** argv)
{
    codec<native_narrow, native_wide> cd(std::locale(""));
    auto str = cd("ТЕСТ", use_basic_string<wchar_t>());


    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
