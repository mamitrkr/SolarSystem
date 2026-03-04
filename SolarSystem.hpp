#ifndef SOLARSYSTEM_HPP
#define SOLARSYSTEM_HPP

#include "Planet.hpp"

/*
 * SolarSystem<T> (Encapsulated Class Template)
 * Generics: template<typename T> allows the system to manage planets
 * of any data type, not just int.
 *
 * All data members are private - no external access to internal state.
 * All operations are public const-correct methods - controlled interface only.
 * No global state - instances are created where needed.
 *
 * Note: Template class implementations must be in the header file
 * so the compiler can instantiate them for any type T.
 */
template <typename T> class SolarSystem {
private:
  struct Star {
    int id = 0;            // Star's identifier - to access the core
    int planet_count = 0;  // Number of planets in the system
    int element_count = 0; // Total element count in the system
  };

  Star star;
  vector<unique_ptr<BasePlanet<T>>>
      planets; // Polymorphic ownership via unique_ptr (RAII)

public:
  /*
   * elementCount: Displays total element count in the system
   * Const-Correctness: marked const - only reads data
   * Time Complexity: O(1)
   */
  void elementCount() const {
    cout << " Element count :" << star.element_count << endl;
  }

  /*
   * planetCount: Displays total planet count in the system
   * Const-Correctness: marked const - only reads data
   * Time Complexity: O(1)
   */
  void planetCount() const {
    cout << " Planet count :" << star.planet_count << endl;
  }

  /*
   * pushToPlanet: Inserts an element into a specific planet
   * Uses const T& to avoid unnecessary copy of the value
   * Time Complexity: O(1) amortized
   */
  void pushToPlanet(int index, const T &value) {
    if (index < 0 || index >= (int)planets.size())
      throw out_of_range("Planet index " + to_string(index) +
                         " is out of range");
    planets[index]->push(value); // Polymorphic call
    star.element_count++;
  }

  /*
   * addPlanet: Creates a new planet using PlanetFactory (Factory Pattern)
   * The factory centralizes creation logic - SolarSystem doesn't need to
   * know about derived classes. Adding new planet types requires no changes
   * here. Move Semantics: unique_ptr is moved into the planets vector. Time
   * Complexity: O(1) amortized
   */
  void addPlanet() {
    char input;
    cout << "Enter the type of Planet" << endl
         << "V For Vector- S For Stack- Q For Queue : ";
    cin >> input;

    // Validate input through enum class - throws if invalid
    PlanetType type = parsePlanetType(input);

    // Factory Pattern: creation logic is delegated to PlanetFactory
    unique_ptr<BasePlanet<T>> p = PlanetFactory<T>::create(type);

    p->name = p->typeName() + "_Planet";
    p->metadata = planets.size();

    cout << "Planet Id: " << p->metadata << " (" << p->name << ")" << endl;

    planets.push_back(move(p)); // Move semantics: transfers ownership
    star.planet_count = planets.size();
    star.id = planets.size() - 1;
  }

  /*
   * blackHole: Completely destroys a planet and frees its memory
   * unique_ptr ensures automatic deallocation when erased (RAII)
   * Time Complexity: O(n) where n is the number of planets
   */
  void blackHole(int planetIndex) {
    if (planetIndex < 0 || planetIndex >= (int)planets.size())
      throw out_of_range("Planet index " + to_string(planetIndex) +
                         " is out of range");

    BasePlanet<T> &p = *planets[planetIndex];
    int elementsLost = p.size(); // Polymorphic call

    cout << "BLACK HOLE: Planet [" << p.name << "] absorbed! " << elementsLost
         << " elements lost." << endl;

    // Erase planet - unique_ptr automatically deletes the derived object
    planets.erase(planets.begin() + planetIndex);
    star.planet_count--;
    star.element_count -= elementsLost;
  }

  /*
   * supernova: Destroys entire solar system
   * planets.clear() destroys all unique_ptrs, triggering automatic cleanup
   * Time Complexity: O(n) where n is the number of planets
   */
  void supernova() {
    planets.clear();
    star.planet_count = 0;
    star.element_count = 0;
    star.id = 0;
  }

  /*
   * gravityPull: Removes elements below threshold from all planets
   * Uses const T& for the threshold to avoid unnecessary copy
   * Time Complexity: O(n*m) where n=planets, m=elements per planet
   */
  void gravityPull(const T &threshold) {
    cout << "GRAVITY PULL: Absorbing elements < " << threshold << endl;
    int totalPulled = 0;

    for (size_t i = 0; i < planets.size(); i++) {
      planets[i]->removeBelow(threshold, totalPulled); // Polymorphic call
    }

    star.element_count -= totalPulled;
    cout << totalPulled << " elements absorbed by gravity." << endl;
  }

  /*
   * deleteElement: Removes one element from a specific planet
   * Time Complexity: O(1)
   */
  void deleteElement(int index) {
    if (index < 0 || index >= (int)planets.size())
      throw out_of_range("Planet index " + to_string(index) +
                         " is out of range");
    planets[index]
        ->pop(); // Polymorphic call (may throw underflow_error if empty)
    star.element_count--;
  }

  /*
   * travelPlanet: Traverses all planets and prints their elements
   * Const-Correctness: marked const - only reads and displays data
   * Time Complexity: O(n * m) where n = planets, m = elements per planet
   */
  void travelPlanet() const {
    for (size_t i = 0; i < planets.size(); i++) {
      BasePlanet<T> &p = *planets[i];
      cout << i + 1 << ". Planet [" << p.name << "] (ID: " << p.metadata
           << ") Elements ";
      p.display(); // Polymorphic call
    }
  }
};

#endif // SOLARSYSTEM_HPP
