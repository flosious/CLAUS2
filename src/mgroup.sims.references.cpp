#include "mgroup.hpp"

mgroups_::sims_t::references_t::references_t(mgroups_::sims_t& MG_s) :
    log_c, MG(MG_s)
{

}

std::vector<measurements_::sims_t*> mgroups_::sims_t::references_t::implants() const
{
    std::vector<measurements_::sims_t*> Ms;
    for (auto& M : MG.measurements())
    {
        if (M->has_implanted_isotopes())
            Ms.push_back(M);
    }
    return Ms;
}
