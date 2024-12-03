#include "sanity.hpp"
#include "state.hpp"

//
// Render State
//
auto RenderState::tie() const
{
	return std::tie();
}

bool RenderState::operator==(const RenderState& right_state) const
{
	return tie() == right_state.tie();
}