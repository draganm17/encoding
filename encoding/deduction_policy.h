/** @file */
#pragma once

#include <encoding/details/traits.h>


namespace denc {

    // TODO: define the 'DeductionPolicy' concept ???


    //-------------------------------------------------------------------------------------------//
    //                              class default_deduction_policy                               //
    //-------------------------------------------------------------------------------------------//

    //! The default policy used to deduce the encoding type from a given character type.
    /*! User can provide additional deduction policies. Each policy must define a template member
    //  typedef `encoding_type`, like the default policy does.
    //
    //  Only the following character types are supported by the default deduction policy:
    //  Character type    | Deduced encoding type
    //  ----------------- | ----------------------
    //  `char`            | `denc::native_narrow`
    //  `wchar_t`         | `denc::native_wide`
    //  `char16_t`        | `denc::utf16`
    //  `char32_t`        | `denc::utf32`
    */
    struct default_deduction_policy
    {
        //! The encoding type deduced from `CharT`.
        template <typename CharT>
        using encoding_type = typename details::default_encoding<CharT>::type;
    };

} // namespace denc