#include "Wire.hpp"

void Wire::update()
{
	if (input != nullptr && output != nullptr)
		*output = *input;
}

bool Wire::getState() const
{
	if (input != nullptr && output != nullptr)
		return *input;

	return false;
}
