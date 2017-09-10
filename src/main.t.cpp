#include <array>
#include <string>
#include <type_traits>
#include <typeindex>
#include <vector>

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

namespace {

    const char     utf8_str[]          = u8"" TEST_TEXT;
    const char16_t utf16_str[]         = u""  TEST_TEXT;
    const char32_t utf32_str[]         = U""  TEST_TEXT;
    const char     native_narrow_str[] =      TEST_TEXT;
    const wchar_t  native_wide_str[]   = L""  TEST_TEXT;


    template <typename DP, typename CharT, typename = void>
    struct can_deduce_impl : std::false_type { };
    
    template <typename DP, typename CharT>
    struct can_deduce_impl<DP, CharT, std::void_t<typename DP::template encoding_type<CharT>>>
    : std::true_type { };
    
    template <typename DP, typename CharT>
    struct can_deduce : can_deduce_impl<DP, CharT> { };

    template <typename DP>
    DP test_deduce_dp_type(deduce<DP>);

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
              typename It = typename std::vector<CharT>::iterator
    >
    struct test_char_type_container_iterator_base
    : std::conjunction<std::is_same<char_type_t<               It>,  std::remove_cv_t<CharT>>,
                       std::is_same<char_type_t<const          It>,  std::remove_cv_t<CharT>>,
                       std::is_same<char_type_t<      volatile It>,  std::remove_cv_t<CharT>>,
                       std::is_same<char_type_t<const volatile It>,  std::remove_cv_t<CharT>>>
    { };

    template <typename CharT>
    struct test_char_type_container_iterator
    : std::conjunction<test_char_type_container_iterator_base<         std::remove_cv_t<CharT>>,
                       test_char_type_container_iterator_base<volatile std::remove_cv_t<CharT>>>
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

    template <typename CharT,
              typename Range = std::vector<CharT>
    >
    struct test_char_type_container_range_base
    : std::conjunction<std::is_same<char_type_t<      Range>,  std::remove_cv_t<CharT>>,
                       std::is_same<char_type_t<const Range>,  std::remove_cv_t<CharT>>>
    { };

    template <typename CharT>
    struct test_char_type_container_range
    : std::conjunction<test_char_type_container_range_base<         std::remove_cv_t<CharT>>,
                       test_char_type_container_range_base<volatile std::remove_cv_t<CharT>>>
    { };

    template <typename CharT>
    struct test_char_type
    : std::conjunction<test_char_type_pointer_iterator<CharT>,
                       test_char_type_container_iterator<CharT>,
                       test_char_type_array_range_base<CharT>,
                       test_char_type_container_range<CharT>>
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

    template <typename CharT, 
              typename DP, 
              typename It       = typename std::vector<CharT>::iterator,
              typename Encoding = typename DP::template encoding_type<std::remove_cv_t<CharT>>
    >
    struct test_encoding_type_container_iterator_base
    : std::conjunction<std::is_same<encoding_type_t<               It, DP>, Encoding>,
                       std::is_same<encoding_type_t<const          It, DP>, Encoding>,
                       std::is_same<encoding_type_t<      volatile It, DP>, Encoding>,
                       std::is_same<encoding_type_t<const volatile It, DP>, Encoding>>
    { };

    template <typename CharT, typename DP>
    struct test_encoding_type_container_iterator
    : std::conjunction<test_encoding_type_container_iterator_base<         std::remove_cv_t<CharT>, DP>,
                       test_encoding_type_container_iterator_base<volatile std::remove_cv_t<CharT>, DP>>
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

    template <typename CharT, typename DP,
              typename Range    = std::vector<CharT>,
              typename Encoding = typename DP::template encoding_type<std::remove_cv_t<CharT>>
    >
    struct test_encoding_type_container_range_base
    : std::conjunction<std::is_same<encoding_type_t<      Range>, Encoding>,
                       std::is_same<encoding_type_t<const Range>, Encoding>>
    { };

    template <typename CharT, typename DP>
    struct test_encoding_type_container_range
    : std::conjunction<test_encoding_type_container_range_base<         std::remove_cv_t<CharT>, DP>,
                       test_encoding_type_container_range_base<volatile std::remove_cv_t<CharT>, DP>>
    { };

    template <typename CharT, typename DP>
    struct test_encoding_type
    : std::conjunction<test_encoding_type_pointer_iterator<CharT, DP>,
                       test_encoding_type_container_iterator<CharT, DP>,
                       test_encoding_type_array_range_base<CharT, DP>,
                       test_encoding_type_container_range<CharT, DP>>
    { };

    constexpr const char     (&get_encoded_string(utf8))          [sizeof(utf8_str)          / sizeof(char)]     { return utf8_str; }
    constexpr const char16_t (&get_encoded_string(utf16))         [sizeof(utf16_str)         / sizeof(char16_t)] { return utf16_str; }
    constexpr const char32_t (&get_encoded_string(utf32))         [sizeof(utf32_str)         / sizeof(char32_t)] { return utf32_str; }
    constexpr const char     (&get_encoded_string(native_narrow)) [sizeof(native_narrow_str) / sizeof(char)]     { return native_narrow_str; }
    constexpr const wchar_t  (&get_encoded_string(native_wide) )  [sizeof(native_wide_str)   / sizeof(wchar_t)]  { return native_wide_str; }

    template <typename Encoding, typename CharT>
    struct test_encoding_traits_typedefs
    : std::conjunction<std::is_same<typename encoding_traits<Encoding>::encoding_type, Encoding>,
                       std::is_same<typename encoding_traits<Encoding>::char_type,     CharT>>
    { };

    template <typename Encoding>
    bool test_encoding_traits_conversions()
    {
        using Traits = encoding_traits<Encoding>;

    #ifdef _WIN32
        std::wstring   to_native_result;
        std::locale loc = std::locale("ru-RU");
    #else
        std::string    to_native_result;
        std::locale loc = std::locale();
    #endif
        std::basic_string<Traits::char_type> from_native_result;

        Traits::to_native(std::begin(get_encoded_string(Encoding())),
                          std::end(get_encoded_string(Encoding())) - 1,
                          std::back_inserter(to_native_result), loc);

        Traits::from_native(std::begin(get_encoded_string(native_encoding_type())),
                            std::end(get_encoded_string(native_encoding_type())) - 1,
                            std::back_inserter(from_native_result), loc);

        return to_native_result   == get_encoded_string(native_encoding_type()) &&
               from_native_result == get_encoded_string(Encoding());
    }

    template <typename Encoding, typename CharT>
    bool test_encoding_traits()
    {
        return test_encoding_traits_typedefs<Encoding, CharT>() &&
               test_encoding_traits_conversions<Encoding>();
    }

    template <typename SrcEncoding, typename DstEncoding>
    bool test_encode_1_pointer_iterator()
    {
    #ifdef _WIN32
        std::locale loc = std::locale("ru-RU");
    #else
        std::locale loc = std::locale();
    #endif
        std::basic_string<encoding_traits<DstEncoding>::char_type> result;

        auto *src = get_encoded_string(SrcEncoding());
        auto it = encode<SrcEncoding, DstEncoding>(src, std::back_inserter(result), loc);

        return //it     == result.end() &&
               result == get_encoded_string(DstEncoding());
    }

    template <typename SrcEncoding, typename DstEncoding>
    bool test_encode_1_container_iterator()
    {
    #ifdef _WIN32
        std::locale loc = std::locale("ru-RU");
    #else
        std::locale loc = std::locale();
    #endif
        std::basic_string<encoding_traits<SrcEncoding>::char_type> src(
            std::begin(get_encoded_string(SrcEncoding())),
            std::end(get_encoded_string(SrcEncoding())));

        std::basic_string<encoding_traits<DstEncoding>::char_type> result;
        auto it = encode<SrcEncoding, DstEncoding>(src.begin(), std::back_inserter(result), loc);

        return //it     == result.end() &&
               result == get_encoded_string(DstEncoding());
    }

    template <typename SrcEncoding, typename DstEncoding>
    bool test_encode_1_array_range()
    {
    #ifdef _WIN32
        std::locale loc = std::locale("ru-RU");
    #else
        std::locale loc = std::locale();
    #endif
        std::basic_string<encoding_traits<DstEncoding>::char_type> result;

        auto it = encode<SrcEncoding, DstEncoding>(get_encoded_string(SrcEncoding()),
                                                   std::back_inserter(result), loc);

        return //it     == result.end() &&
               result == get_encoded_string(DstEncoding());
    }

    template <typename SrcEncoding, typename DstEncoding>
    bool test_encode_1_container_range()
    {
    #ifdef _WIN32
        std::locale loc = std::locale("ru-RU");
    #else
        std::locale loc = std::locale();
    #endif
        std::basic_string<encoding_traits<SrcEncoding>::char_type> src;
        std::basic_string<encoding_traits<DstEncoding>::char_type> result;

        src = get_encoded_string(SrcEncoding());
        auto it = encode<SrcEncoding, DstEncoding>(src, std::back_inserter(result), loc);

        return //it     == result.end() &&
               result == get_encoded_string(DstEncoding());
    }

    template <typename SrcEncoding, typename DstEncoding>
    bool test_encode_1()
    {
        return test_encode_1_pointer_iterator<SrcEncoding, DstEncoding>()   &&
               test_encode_1_container_iterator<SrcEncoding, DstEncoding>() &&
               //test_encode_1_array_range<SrcEncoding, DstEncoding>()        &&
               test_encode_1_container_range<SrcEncoding, DstEncoding>();
    }

    template <typename SrcEncoding, typename DstEncoding>
    bool test_encode_3()
    {
    #ifdef _WIN32
        std::locale loc = std::locale("ru-RU");
    #elif
        std::locale loc = std::locale();
    #endif
        std::basic_string<encoding_traits<DstEncoding>::char_type> result;

        auto it = encode<SrcEncoding, DstEncoding>(std::begin(get_encoded_string(SrcEncoding())),
                                                   std::end(get_encoded_string(SrcEncoding())) - 1,
                                                   std::back_inserter(result), loc);

        return //it     == result.end() &&
               result == get_encoded_string(DstEncoding());
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

    // each encoding type should be unique (except 'native_narrow' and 'native_wide')
    std::array<std::type_index, 3> encs{ std::type_index(typeid(utf8)),
                                         std::type_index(typeid(utf16)),
                                         std::type_index(typeid(utf32)) };
    EXPECT_TRUE(std::distance(encs.begin(), std::unique(encs.begin(), encs.end())) == encs.size());

    // 'native_encoding_type' should be an alias to one of the encoding types
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

    // should be SFINAE-frendly
    EXPECT_FALSE((can_deduce<DDP, float>::value));
}

TEST(DENC, Deduce)
{
    // should be an empty class type and take one template parameter
    EXPECT_TRUE((std::is_empty_v<deduce<void>> && std::is_class_v<deduce<void>>));

    // the template parameter default value is 'default_deduction_policy'
    EXPECT_TRUE((std::is_same_v<decltype(test_deduce_dp_type(deduce<>())), default_deduction_policy>));
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
    //EXPECT_TRUE((test_encoding_traits<utf8, char>()));
    EXPECT_TRUE((test_encoding_traits<utf16, char16_t>()));
    //EXPECT_TRUE((test_encoding_traits<utf32, char32_t>()));
    EXPECT_TRUE((test_encoding_traits<native_narrow, char>()));
    EXPECT_TRUE((test_encoding_traits<native_wide, wchar_t>()));
}

TEST(DENC, Encode_1)
{
    EXPECT_TRUE((test_encode_1<utf8, utf8>()));
    //EXPECT_TRUE((test_encode_1<utf8, utf16>()));
    //EXPECT_TRUE((test_encode_1<utf8, utf32>()));
    //EXPECT_TRUE((test_encode_1<utf8, native_narrow>()));
    //EXPECT_TRUE((test_encode_1<utf8, native_wide>()));

    //EXPECT_TRUE((test_encode_1<utf16, utf8>()));
    EXPECT_TRUE((test_encode_1<utf16, utf16>()));
    //EXPECT_TRUE((test_encode_1<utf16, utf32>()));
    EXPECT_TRUE((test_encode_1<utf16, native_narrow>()));
    EXPECT_TRUE((test_encode_1<utf16, native_wide>()));

    //EXPECT_TRUE((test_encode_1<utf32, utf8>()));
    //EXPECT_TRUE((test_encode_1<utf32, utf16>()));
    EXPECT_TRUE((test_encode_1<utf32, utf32>()));
    //EXPECT_TRUE((test_encode_1<utf32, native_narrow>()));
    //EXPECT_TRUE((test_encode_1<utf32, native_wide>()));

    //EXPECT_TRUE((test_encode_1<native_narrow, utf8>()));
    EXPECT_TRUE((test_encode_1<native_narrow, utf16>()));
    //EXPECT_TRUE((test_encode_1<native_narrow, utf32>()));
    EXPECT_TRUE((test_encode_1<native_narrow, native_narrow>()));
    EXPECT_TRUE((test_encode_1<native_narrow, native_wide>()));

    //EXPECT_TRUE((test_encode_1<native_wide, utf8>()));
    EXPECT_TRUE((test_encode_1<native_wide, utf16>()));
    //EXPECT_TRUE((test_encode_1<native_wide, utf32>()));
    EXPECT_TRUE((test_encode_1<native_wide, native_narrow>()));
    EXPECT_TRUE((test_encode_1<native_wide, native_wide>()));
}

TEST(DENC, Encode_3)
{
    EXPECT_TRUE((test_encode_3<utf8, utf8>()));
    //EXPECT_TRUE((test_encode_3<utf8, utf16>()));
    //EXPECT_TRUE((test_encode_3<utf8, utf32>()));
    //EXPECT_TRUE((test_encode_3<utf8, native_narrow>()));
    //EXPECT_TRUE((test_encode_3<utf8, native_wide>()));

    //EXPECT_TRUE((test_encode_3<utf16, utf8>()));
    EXPECT_TRUE((test_encode_3<utf16, utf16>()));
    //EXPECT_TRUE((test_encode_3<utf16, utf32>()));
    EXPECT_TRUE((test_encode_3<utf16, native_narrow>()));
    EXPECT_TRUE((test_encode_3<utf16, native_wide>()));

    //EXPECT_TRUE((test_encode_3<utf32, utf8>()));
    //EXPECT_TRUE((test_encode_3<utf32, utf16>()));
    EXPECT_TRUE((test_encode_3<utf32, utf32>()));
    //EXPECT_TRUE((test_encode_3<utf32, native_narrow>()));
    //EXPECT_TRUE((test_encode_3<utf32, native_wide>()));

    //EXPECT_TRUE((test_encode_3<native_narrow, utf8>()));
    EXPECT_TRUE((test_encode_3<native_narrow, utf16>()));
    //EXPECT_TRUE((test_encode_3<native_narrow, utf32>()));
    EXPECT_TRUE((test_encode_3<native_narrow, native_narrow>()));
    EXPECT_TRUE((test_encode_3<native_narrow, native_wide>()));

    //EXPECT_TRUE((test_encode_3<native_wide, utf8>()));
    EXPECT_TRUE((test_encode_3<native_wide, utf16>()));
    //EXPECT_TRUE((test_encode_3<native_wide, utf32>()));
    EXPECT_TRUE((test_encode_3<native_wide, native_narrow>()));
    EXPECT_TRUE((test_encode_3<native_wide, native_wide>()));
}


int main(int argc, char** argv)
{
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}