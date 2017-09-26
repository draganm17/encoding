/** @file */
#pragma once

#include <memory>
#include <string>


namespace denc {

    template <typename ResultToken>
    struct encode_result;


    //-------------------------------------------------------------------------------------------//
    //                                 class use_basic_string                                    //
    //-------------------------------------------------------------------------------------------//

    /*! The class template `use_basic_string` defines a set of types that, when passed as a result
    //  token to `denc::encode`, cause the result of the encoding operation to be delivered via a
    //  string. Specifically an instance of `std::basic_string<CharT, Traits, Allocator>`.
    */
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
        //! Constructs a `use_basic_string` with a default-constructed allocator.
        constexpr use_basic_string() noexcept(noexcept(allocator_type()));

        //! Constructs a `use_basic_string` with a copy of `a`.
        /*! @post `get_allocator() == a`.
        */
        explicit use_basic_string(const allocator_type& a) noexcept;

    public:
        //! Returns the associated allocator object.
        allocator_type get_allocator() const noexcept;

    private:
        allocator_type m_allocator;
    };


    //-------------------------------------------------------------------------------------------//
    //                       class encode_result<use_basic_string<Ts...>>                        //
    //-------------------------------------------------------------------------------------------//

    //! Specialization of `encode_result` for `use_basic_string`.
    template <typename... Ts>
    class encode_result<use_basic_string<Ts...>>
    {
    public:
        //! Result type.
        using type = std::basic_string<Ts...>;

    public:
        //! Initializes `m_value` with `token.get_allocator()`.
        explicit encode_result(use_basic_string<Ts...>& token);

        encode_result(const encode_result&) = delete;

        encode_result& operator=(const encode_result&) = delete;

    public:
        //! Returns `std::move(m_value)`.
        type get();

    private:
        type m_value;
    };


    //-------------------------------------------------------------------------------------------//
    //                                    INLINE DEFINITIONS                                     //
    //-------------------------------------------------------------------------------------------//

    template <typename CharT, typename Traits, typename Allocator>
    inline constexpr use_basic_string<CharT, Traits, Allocator>
    ::use_basic_string() noexcept(noexcept(allocator_type()))
    { }

    template <typename CharT, typename Traits, typename Allocator>
    inline use_basic_string<CharT, Traits, Allocator>
    ::use_basic_string(const allocator_type& a) noexcept
    : m_allocator(a)
    { }

    template <typename CharT, typename Traits, typename Allocator>
    inline Allocator use_basic_string<CharT, Traits, Allocator>
    ::get_allocator() const noexcept
    {
        return m_allocator;
    }

    template <typename... Ts>
    inline encode_result<use_basic_string<Ts...>>::encode_result(use_basic_string<Ts...>& token)
    : m_value(token.get_allocator())
    { }

    template <typename... Ts>
    inline typename encode_result<use_basic_string<Ts...>>::type
    encode_result<use_basic_string<Ts...>>::get()
    {
        return std::move(m_value);
    }

} // namespace denc