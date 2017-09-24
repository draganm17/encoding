#pragma once

#include <memory>
#include <string>

namespace denc {
namespace details {

    template <typename ResultToken>
    struct encode_result;

    //-------------------------------------------------------------------------------------------//
    //                                 class use_basic_string                                    //
    //-------------------------------------------------------------------------------------------//

    template <typename CharT,
              typename Traits = std::char_traits<CharT>,
              typename Allocator = std::allocator<CharT>
    >
    class use_basic_string
    {
    public:
        using value_type     = CharT;

        using traits_type    = Traits;

        using allocator_type = Allocator;

    public:
        constexpr use_basic_string() noexcept(noexcept(allocator_type()));

        explicit use_basic_string(const allocator_type& allocator) noexcept;

    public:
        allocator_type get_allocator() const noexcept;

    private:
        allocator_type m_allocator;
    };


    //-------------------------------------------------------------------------------------------//
    //             class encode_result<use_basic_string<CharT, Traits, Allocator>>               //
    //-------------------------------------------------------------------------------------------//

    template <typename CharT, typename Traits, typename Allocator>
    class encode_result<use_basic_string<CharT, Traits, Allocator>>
    {
    public:
        using type = std::basic_string<CharT, Traits, Allocator>;

    public:
        explicit encode_result(use_basic_string<CharT, Traits, Allocator>& token)
        : m_value(type(token.get_allocator()))
        { }

        encode_result(const encode_result&) = delete;

        encode_result& operator=(const encode_result&) = delete;

    public:
        type get() { return std::move(m_value); }

    private:
        type m_value;
    };


    //-------------------------------------------------------------------------------------------//
    //                                    INLINE DEFINITIONS                                     //
    //-------------------------------------------------------------------------------------------//

    template <typename CharT, typename Traits, typename Allocator>
    constexpr use_basic_string<CharT, Traits, Allocator>::use_basic_string()
                                                          noexcept(noexcept(allocator_type()))
    { }

    template <typename CharT, typename Traits, typename Allocator>
    use_basic_string<CharT, Traits, Allocator>::use_basic_string(const allocator_type& allocator)
                                                                                         noexcept
    : m_allocator(allocator)
    { }

    template <typename CharT, typename Traits, typename Allocator>
    Allocator use_basic_string<CharT, Traits, Allocator>::get_allocator() const noexcept
    {
        return m_allocator;
    }

}} // namespace denc::details