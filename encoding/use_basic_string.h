#pragma once

#include <encoding/details/use_basic_string.h>


namespace denc {

    // TODO: ...
    //
    //
    // MEMBER TYPES
    // ============================================================================================
    //  value_type      |  'CharT'
    // --------------------------------------------------------------------------------------------
    //  traits_type     |  'Traits'
    // --------------------------------------------------------------------------------------------
    //  allocator_type  |  'Allocator'
    // ============================================================================================
    //
    //
    // MEMBER FUNCTIONS
    // ============================================================================================
    // constexpr use_basic_string() noexcept(noexcept(allocator_type()));
    //
    // TODO: ...
    //
    // --------------------------------------------------------------------------------------------
    // explicit use_basic_string(const allocator_type& allocator) noexcept;
    //
    // TODO: ...
    //
    // --------------------------------------------------------------------------------------------
    // allocator_type get_allocator() const noexcept;
    //
    // TODO: ...
    // ============================================================================================
    template <typename CharT,
              typename Traits = std::char_traits<CharT>,
              typename Allocator = std::allocator<CharT>
    >
    using use_basic_string = details::use_basic_string<CharT, Traits, Allocator>;

} // namespace denc