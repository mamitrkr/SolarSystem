#include "SolarSystem.hpp"

/*
 * Explicit Template Instantiation
 *
 * Since SolarSystem<T> is a template class with implementations in the header,
 * the compiler instantiates it on demand. However, we provide explicit
 * instantiations here for commonly used types to:
 *   1. Improve compile times in large projects (avoids redundant instantiation)
 *   2. Document which types are officially supported
 *   3. Maintain the 4-file project structure (Planet.hpp, SolarSystem.hpp,
 * SolarSystem.cpp, main.cpp)
 *
 * To support a new data type (e.g., double, string), simply add:
 *   template class SolarSystem<double>;
 */
template class SolarSystem<int>;
