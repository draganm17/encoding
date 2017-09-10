#pragma once

#include <algorithm>
#include <iterator>
#include <optional>
#include <type_traits>


namespace denc {
namespace details {

    //-------------------------------------------------------------------------------------------//
    //                                   class ntcs_iterator                                     //
    //-------------------------------------------------------------------------------------------//

    template <typename InputIt>
    class ntcs_iterator {
    // An null-terminated character sequence iterator adapter.
    // Default-constructed iterator is the end iterator. Two end iterators are always equal.
    // An iterator becomes the end iterator when pointing to the null terminator character symbol.
    //
    // NOTE: 
    // The behaviour is undefined unless the sequence pointed by the adaptable iterator does not
    // contain an null-character value - 'ntcs_iterator::value_type()'.

    public:
        using value_type =        std::remove_cv_t<
                                  typename std::iterator_traits<InputIt>::value_type>;
        using difference_type =   std::ptrdiff_t;
        using pointer =           const value_type*;
        using reference =         const value_type&;
        using iterator_category = std::input_iterator_tag;

    public:
        ntcs_iterator() noexcept = default;

        ntcs_iterator(InputIt first);

        ntcs_iterator(const ntcs_iterator&) = default;

        ntcs_iterator& operator=(const ntcs_iterator&) = default;

    public:
        bool operator==(const ntcs_iterator& rhs) const noexcept;

        bool operator!=(const ntcs_iterator& rhs) const noexcept;

        reference operator*() const noexcept;

        pointer operator->() const noexcept;

    public:
        ntcs_iterator& operator++();

        ntcs_iterator operator++(int);

    public:
        void swap(ntcs_iterator& other) noexcept(std::is_nothrow_swappable_v<InputIt> &&
                                                 std::is_nothrow_swappable_v<value_type>);

    private:
        std::optional<InputIt>  m_it;
        value_type              m_val = value_type();
    };

    template <typename InputIt>
    void swap(ntcs_iterator<InputIt>& lhs, 
              ntcs_iterator<InputIt>& rhs) noexcept(noexcept(lhs.swap(rhs)));


    //-------------------------------------------------------------------------------------------//
    //                                    INLINE DEFINITIONS                                     //
    //-------------------------------------------------------------------------------------------//

    template <typename InputIt>
    inline ntcs_iterator<InputIt>::ntcs_iterator(InputIt first)
    : m_it(std::move(first)) // TODO: STD ???
    , m_val(*(*m_it)++)
    { }

    template <typename InputIt>
    inline bool ntcs_iterator<InputIt>::operator==(const ntcs_iterator& rhs) const noexcept
    {
        return m_val == value_type() && rhs.m_val == value_type();
    }

    template <typename InputIt>
    inline bool ntcs_iterator<InputIt>::operator!=(const ntcs_iterator& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    template <typename InputIt>
    inline typename ntcs_iterator<InputIt>::reference ntcs_iterator<InputIt>::operator*() const 
                                                                                       noexcept
    {
        return m_val;
    }

    template <typename InputIt>
    inline typename ntcs_iterator<InputIt>::pointer ntcs_iterator<InputIt>::operator->() const
                                                                                      noexcept
    {
        return &m_val;
    }

    template <typename InputIt>
    inline ntcs_iterator<InputIt>& ntcs_iterator<InputIt>::operator++()
    {
        m_val = *(*m_it)++; return *this;
    }

    template <typename InputIt>
    inline ntcs_iterator<InputIt> ntcs_iterator<InputIt>::operator++(int)
    {
        auto tmp = *this; ++*this; return tmp;
    }

    template <typename InputIt>
    inline void ntcs_iterator<InputIt>::swap(ntcs_iterator& other) 
                                        noexcept(std::is_nothrow_swappable_v<InputIt> && 
                                                 std::is_nothrow_swappable_v<value_type>)
    {
        using std::swap;
        swap(m_it, other.m_it);
        swap(m_val, other.m_val);
    }

    template <typename InputIt>
    inline void swap(ntcs_iterator<InputIt>& lhs, 
                     ntcs_iterator<InputIt>& rhs) noexcept(noexcept(lhs.swap(rhs)))
    { 
        lhs.swap(rhs);
    }

}} // namespace denc::details