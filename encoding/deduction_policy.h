#pragma once

#include <encoding/details/deduction_policy.h>


namespace denc {

    // TODO: define the 'DeductionPolicy' concept ???


    // The default policy used to deduce the encoding type from a given character type.
    // User can provide additional deduction policies. Each policy must define a template
    // member typedef 'encoding_type', like the default policy does.
    //
    // MEMBER TYPES
    // ============================================================================================
    // encoding_type<CharT>  |  The encoding type deduced for the given character type.
    //                       |  Only the following character types are supported by the default 
    //                       |  deduction policy:
    //                       |  - 'char'     - the deduced encoding type is 'denc::native_narrow'.
    //                       |  - 'wchar_t'  - the deduced encoding type is 'denc::native_wide'.
    //                       |  - 'char16_t' - the deduced encoding type is 'denc::utf16'.
    //                       |  - 'char32_t' - the deduced encoding type is 'denc::utf32'.
    // ============================================================================================
    using default_deduction_policy = details::default_deduction_policy;

    // An empty tag type used to cpecify the deduction policy that should be used by the deductive
    // overloads of 'denc::encode'.
    template <typename DP = default_deduction_policy>
    using deduce = details::deduce<DP>;

} // namespace denc