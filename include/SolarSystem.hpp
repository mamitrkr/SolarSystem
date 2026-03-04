#ifndef SOLARSYSTEM_HPP
#define SOLARSYSTEM_HPP

#include "Planet.hpp"

/**
 * @brief Encapsulated solar system manager using polymorphic planet ownership.
 *
 * All data members are private — no external access to internal state.
 * All operations are public, const-correct methods providing a controlled
 * interface. No global state — instances are created locally where needed.
 *
 * RAII Guarantee: All planets are owned via unique_ptr. When a planet is
 * removed (blackHole/removePlanet) or the entire system is reset
 * (supernova/resetSystem), unique_ptr destructors automatically deallocate all
 * associated memory. No manual delete is ever required.
 *
 * Template class implementations are defined inline in this header file,
 * as required by the C++ standard for template instantiation.
 *
 * @tparam T The data type that planets in this system will store.
 */
template <typename T> class SolarSystem {
private:
  /**
   * @brief Central controller structure holding system-wide counters.
   */
  struct Star {
    int id = 0;            ///< Identifier of the most recently added planet.
    int planet_count = 0;  ///< Total number of planets in the system.
    int element_count = 0; ///< Total number of elements across all planets.
  };

  Star star; ///< System-wide state counters.
  vector<unique_ptr<BasePlanet<T>>>
      planets; ///< Polymorphic ownership via unique_ptr (RAII).

public:
  // ─────────────────────────────────────────────
  //  Query Methods (const-correct, read-only)
  // ─────────────────────────────────────────────

  /**
   * @brief Displays the total element count in the system to stdout.
   * @complexity O(1) — Direct access to counter variable.
   */
  void elementCount() const {
    cout << " Element count :" << star.element_count << endl;
  }

  /**
   * @brief Displays the total planet count in the system to stdout.
   * @complexity O(1) — Direct access to counter variable.
   */
  void planetCount() const {
    cout << " Planet count :" << star.planet_count << endl;
  }

  /**
   * @brief Returns the tracked element count from the Star controller.
   *
   * Used in unit tests to assert consistency with getActualElementCount().
   *
   * @return The star.element_count value.
   * @complexity O(1)
   */
  int getElementCount() const { return star.element_count; }

  /**
   * @brief Returns the tracked planet count from the Star controller.
   * @return The star.planet_count value.
   * @complexity O(1)
   */
  int getPlanetCount() const { return star.planet_count; }

  /**
   * @brief Computes the actual total element count by summing all planets.
   *
   * Iterates every planet and calls size() polymorphically. The result
   * should always equal getElementCount() — any discrepancy indicates a bug.
   *
   * @return Sum of elements across all planets.
   * @complexity O(p) where p = number of planets.
   */
  int getActualElementCount() const {
    int total = 0;
    for (size_t i = 0; i < planets.size(); i++)
      total += planets[i]->size();
    return total;
  }

  // ─────────────────────────────────────────────
  //  Mutation Methods
  // ─────────────────────────────────────────────

  /**
   * @brief Inserts an element into a specific planet.
   *
   * Uses const T& to avoid unnecessary copies of the value.
   *
   * @param index Zero-based index of the target planet.
   * @param value The value to insert (passed by const reference).
   * @throws std::out_of_range If index is out of bounds.
   * @complexity O(1) amortized — polymorphic dispatch to the correct push().
   */
  void pushToPlanet(int index, const T &value) {
    if (index < 0 || index >= (int)planets.size())
      throw out_of_range("Planet index " + to_string(index) +
                         " is out of range");
    planets[index]->push(value);
    star.element_count++;
  }

  /**
   * @brief Creates a new planet interactively via stdin (Factory Pattern).
   *
   * Prompts the user for a planet type character, validates through
   * parsePlanetType(), and delegates creation to PlanetFactory<T>.
   * Ownership is transferred to the system via move semantics.
   *
   * @throws std::invalid_argument If the user enters an unrecognized type.
   * @complexity O(1) amortized.
   */
  void addPlanet() {
    char input;
    cout << "Enter the type of Planet" << endl
         << "V For Vector- S For Stack- Q For Queue : ";
    cin >> input;

    PlanetType type = parsePlanetType(input);
    addPlanet(type);
  }

  /**
   * @brief Creates a new planet programmatically (Factory Pattern).
   *
   * Overload that accepts a PlanetType directly, enabling automated
   * testing without stdin dependency. Delegates creation to PlanetFactory<T>.
   *
   * @param type The PlanetType enum specifying which derived class to create.
   * @throws std::invalid_argument If the PlanetType is not recognized by the
   * factory.
   * @complexity O(1) amortized.
   */
  void addPlanet(PlanetType type) {
    unique_ptr<BasePlanet<T>> p = PlanetFactory<T>::create(type);

    p->name = p->typeName() + "_Planet";
    p->metadata = planets.size();

    cout << "Planet Id: " << p->metadata << " (" << p->name << ")" << endl;

    planets.push_back(move(p)); // Move semantics: transfers ownership
    star.planet_count++;
    star.id = star.planet_count - 1;
  }

  /**
   * @brief Destroys a single planet and reclaims all its memory (Black Hole).
   *
   * RAII Guarantee: When the unique_ptr is erased from the vector, the
   * derived planet object (and all its internal data) is automatically
   * deallocated. No manual delete is required. Zero memory remains allocated
   * for the destroyed planet after this call.
   *
   * @param planetIndex Zero-based index of the planet to destroy.
   * @throws std::out_of_range If planetIndex is out of bounds.
   * @complexity O(p) where p = number of planets (due to vector::erase shift).
   */
  void removePlanet(int planetIndex) {
    if (planetIndex < 0 || planetIndex >= (int)planets.size())
      throw out_of_range("Planet index " + to_string(planetIndex) +
                         " is out of range");

    BasePlanet<T> &p = *planets[planetIndex];
    int elementsLost = p.size();

    cout << "BLACK HOLE: Planet [" << p.name << "] absorbed! " << elementsLost
         << " elements lost." << endl;

    // unique_ptr automatically deletes the derived object when erased
    planets.erase(planets.begin() + planetIndex);
    star.planet_count--;
    star.element_count -= elementsLost;
  }

  /**
   * @brief Alias for removePlanet (backward compatibility).
   * @param planetIndex Zero-based index of the planet to destroy.
   * @throws std::out_of_range If planetIndex is out of bounds.
   * @complexity O(p) where p = number of planets.
   */
  void blackHole(int planetIndex) { removePlanet(planetIndex); }

  /**
   * @brief Destroys the entire solar system and resets all state (Supernova).
   *
   * RAII Guarantee: planets.clear() destroys every unique_ptr in the vector.
   * Each unique_ptr destructor deletes its owned BasePlanet-derived object,
   * which in turn destroys the internal vector/stack/queue via their own
   * destructors. After this call, zero heap memory remains allocated for
   * any planet or element. All Star counters are reset to zero.
   *
   * @complexity O(p) where p = number of planets (each destructor runs).
   */
  void resetSystem() {
    planets.clear(); // RAII: all unique_ptrs destroyed, all memory freed
    star.planet_count = 0;
    star.element_count = 0;
    star.id = 0;
  }

  /**
   * @brief Alias for resetSystem (backward compatibility).
   * @complexity O(p) where p = number of planets.
   */
  void supernova() { resetSystem(); }

  /**
   * @brief Removes all elements below a threshold from every planet.
   *
   * Uses const T& for the threshold to avoid unnecessary copy.
   * Each planet's removeBelow() is called polymorphically.
   *
   * @param threshold Elements with value < threshold are removed.
   * @complexity O(p * m) where p = planets, m = elements per planet.
   */
  void gravityPull(const T &threshold) {
    cout << "GRAVITY PULL: Absorbing elements < " << threshold << endl;
    int totalPulled = 0;

    for (size_t i = 0; i < planets.size(); i++)
      planets[i]->removeBelow(threshold, totalPulled);

    star.element_count -= totalPulled;
    cout << totalPulled << " elements absorbed by gravity." << endl;
  }

  /**
   * @brief Removes one element from a specific planet.
   *
   * Delegates to the planet's pop() via polymorphic dispatch.
   *
   * @param index Zero-based index of the target planet.
   * @throws std::out_of_range If index is out of bounds.
   * @throws std::underflow_error If the target planet is empty.
   * @complexity O(1) — polymorphic dispatch to the correct pop().
   */
  void deleteElement(int index) {
    if (index < 0 || index >= (int)planets.size())
      throw out_of_range("Planet index " + to_string(index) +
                         " is out of range");
    planets[index]->pop();
    star.element_count--;
  }

  // ─────────────────────────────────────────────
  //  Traversal
  // ─────────────────────────────────────────────

  /**
   * @brief Traverses all planets and prints their elements to stdout.
   *
   * Const-correct: does not modify any state.
   *
   * @complexity O(p * m) where p = number of planets, m = elements per planet.
   */
  void travelPlanet() const {
    for (size_t i = 0; i < planets.size(); i++) {
      BasePlanet<T> &p = *planets[i];
      cout << i + 1 << ". Planet [" << p.name << "] (ID: " << p.metadata
           << ") Elements ";
      p.display();
    }
  }
};

#endif // SOLARSYSTEM_HPP
