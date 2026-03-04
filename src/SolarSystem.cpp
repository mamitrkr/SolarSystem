#include "SolarSystem.hpp"

/**
 * @brief Explicit template instantiation for commonly used types.
 *
 * Provides pre-compiled instantiations to improve compile times in
 * large projects and documents officially supported data types.
 *
 * To support additional types (e.g., double, string), add:
 *   template class SolarSystem<double>;
 */
template class SolarSystem<int>;
