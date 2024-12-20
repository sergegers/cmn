#pragma once

#include <array>
#include <ios>
#include <type_traits>

#include <cmn/meta/concepts.h>
#include <cmn/io/manip/slot/decoder.h>   // TODO: investigate why decoder.h inclusion is required?

namespace cmn::io::manip
{

template <typename Int, c::slot_manipulator... Manips>
struct basic_saver
{
private:
    std::ios_base                           &m_ios;
    std::array<Int, sizeof... (Manips)>    m_saved;

    template <typename Manip>
    static auto save_manip(std::ios_base &ios, std::integral_constant<long, 0>) -> long
    {
        return ios.iword(Manip::index(ios));
    }

    template <typename Manip>
    static auto save_manip(std::ios_base &ios, std::integral_constant<void *, nullptr>) -> void *
    {
        return ios.pword(Manip::index(ios));
    }

    template <typename Manip, std::size_t Idx_>
    auto restore_manip(std::integral_constant<long, 0>) -> void
    {
        using storage_type = typename Manip::storage_type;
        if constexpr (c::restore_storage<storage_type>)
            storage_type::restore(m_ios);

        m_ios.iword(Manip::index(m_ios)) = m_saved[Idx_];
    }

    template <typename Manip, std::size_t Idx_>
    auto restore_manip(std::integral_constant<void *, nullptr>) -> void
    {
        using storage_type = typename Manip::storage_type;
        if constexpr (c::restore_storage<storage_type>)
            storage_type::restore(m_ios);

        m_ios.pword(Manip::index(m_ios)) = m_saved[Idx_];
    }
public:
    [[nodiscard]] explicit basic_saver(std::ios_base &ios):
          m_ios { ios }
        , m_saved { save_manip<Manips>(ios, std::integral_constant<Int, 0>{})... }
    {}

    auto restore() -> void
    {
        [this] <std::size_t... Indices_>(std::index_sequence<Indices_...>)
        {
            (restore_manip<Manips, Indices_>(std::integral_constant<Int, 0>{}), ...);
        }
        (std::make_index_sequence<sizeof... (Manips)>{});
    }

    ~basic_saver()
    {
        restore();
    }
};

template <c::slot_manipulator... Manips> using iword_saver = basic_saver<long, Manips...>;
template <c::slot_manipulator... Manips> using pword_saver = basic_saver<void *, Manips...>;

}
