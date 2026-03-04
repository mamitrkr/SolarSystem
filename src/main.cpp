/**
 * @file main.cpp
 * @brief Entry point for the SolarSystem application.
 *
 * Demonstrates the complete SolarSystem<T> interface with exception handling.
 */
#include "SolarSystem.hpp"

int main() {
  try {
    SolarSystem<int>
        system; // Generics: int type, local instance (no global state)

    system.addPlanet();
    system.addPlanet();
    system.addPlanet();

    system.planetCount();
    system.deleteElement(0);

    system.pushToPlanet(0, 10);
    system.pushToPlanet(0, 20);
    system.pushToPlanet(0, 30);
    system.pushToPlanet(1, 10);
    system.pushToPlanet(1, 20);
    system.pushToPlanet(1, 30);
    system.pushToPlanet(2, 10);
    system.pushToPlanet(2, 20);
    system.pushToPlanet(2, 30);

    system.elementCount();
    system.travelPlanet();

    system.deleteElement(0);
    system.deleteElement(1);
    system.deleteElement(2);

    system.travelPlanet();
    system.elementCount();

    system.removePlanet(1); // Black Hole: Destroy second planet
    system.travelPlanet();
    system.planetCount();

    system.gravityPull(15); // Remove elements < 15
    system.travelPlanet();
    system.elementCount();

    system.resetSystem(); // Supernova: Destroy everything
    system.travelPlanet();
    system.planetCount();
  } catch (const out_of_range &e) {
    cerr << "[Out of Range Error] " << e.what() << endl;
    return 1;
  } catch (const underflow_error &e) {
    cerr << "[Underflow Error] " << e.what() << endl;
    return 2;
  } catch (const invalid_argument &e) {
    cerr << "[Invalid Argument] " << e.what() << endl;
    return 3;
  } catch (const exception &e) {
    cerr << "[Unexpected Error] " << e.what() << endl;
    return 4;
  }

  return 0;
}
