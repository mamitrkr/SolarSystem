/*Central management of collections with various behaviors*/
#include <iostream>
using namespace std;
#include <memory> // For unique_ptr (RAII ownership of polymorphic planets)
#include <queue>
#include <stack>
#include <stdexcept> // For exception handling (out_of_range, underflow_error, invalid_argument)
#include <string> // RAII-based string management
#include <vector>


/*
 * PlanetType (enum class) - Strict type safety for planet creation
 * Unlike raw char inputs, enum class prevents invalid states at compile time
 * and provides a clear, self-documenting set of valid planet types.
 */
enum class PlanetType { Vector, Stack, Queue };

/*
 * parsePlanetType: Converts user input to a validated PlanetType enum
 * Accepts both uppercase and lowercase input for user convenience.
 * Throws std::invalid_argument if the input is not a recognized type.
 */
PlanetType parsePlanetType(char input) {
  switch (input) {
  case 'V':
  case 'v':
    return PlanetType::Vector;
  case 'S':
  case 's':
    return PlanetType::Stack;
  case 'Q':
  case 'q':
    return PlanetType::Queue;
  default:
    throw invalid_argument(string("Invalid planet type: '") + input +
                           "'. Use V, S, or Q.");
  }
}

/*
 * Planet (Base Class) - Abstract interface for all planet types
 * Each derived class holds ONLY the data structure it actually uses,
 * eliminating the memory waste of the old design.
 *
 * Virtual functions enable polymorphism: the correct behavior is
 * dispatched at runtime without any if/else type-checking.
 */
class Planet {
public:
  string name;      // RAII: std::string manages its own memory
  int metadata = 0; // RAII: plain int, no dynamic allocation needed

  virtual ~Planet() =
      default; // Virtual destructor for safe polymorphic deletion

  // Pure virtual functions - each derived class implements its own behavior
  virtual void push(int value) = 0;
  virtual void pop() = 0;
  virtual int size() const = 0;
  virtual bool empty() const = 0;
  virtual void display() const = 0;
  virtual string typeName() const = 0;
  virtual void removeBelow(int threshold, int &removedCount) = 0;
};

/*
 * VectorPlanet - Planet that uses std::vector for storage
 * Only allocates a vector, no stack or queue memory wasted.
 */
class VectorPlanet : public Planet {
public:
  vector<int> v;

  void push(int value) override { v.push_back(value); }

  void pop() override {
    if (v.empty())
      throw underflow_error("Cannot pop from empty Vector planet");
    v.pop_back(); // Removes the last element
  }

  int size() const override { return v.size(); }
  bool empty() const override { return v.empty(); }
  string typeName() const override { return "Vector"; }

  void display() const override {
    cout << "(" << typeName() << ") : ";
    if (v.empty())
      cout << "EMPTY";
    for (size_t i = 0; i < v.size(); i++)
      cout << v[i] << " ";
    cout << endl;
  }

  void removeBelow(int threshold, int &removedCount) override {
    int beforeSize = v.size();
    vector<int> survivors;
    size_t j = 0;
    while (j < v.size()) {
      int deger = v[j];
      if (deger >= threshold) {
        survivors.push_back(deger);
      }
      j++;
    }
    v = survivors;
    removedCount += (beforeSize - v.size());
  }
};

/*
 * StackPlanet - Planet that uses std::stack for storage
 * Only allocates a stack, no vector or queue memory wasted.
 */
class StackPlanet : public Planet {
public:
  stack<int> s;

  void push(int value) override { s.push(value); }

  void pop() override {
    if (s.empty())
      throw underflow_error("Cannot pop from empty Stack planet");
    s.pop(); // LIFO - removes the top element
  }

  int size() const override { return s.size(); }
  bool empty() const override { return s.empty(); }
  string typeName() const override { return "Stack"; }

  void display() const override {
    cout << "(" << typeName() << ") : ";
    stack<int> temp = s; // Backup to prevent data loss
    if (temp.empty())
      cout << "EMPTY";
    while (!temp.empty()) {
      cout << temp.top() << " ";
      temp.pop();
    }
    cout << endl;
  }

  void removeBelow(int threshold, int &removedCount) override {
    int beforeSize = s.size();
    stack<int> survivors;
    stack<int> temp;

    // Extract all elements to temp stack (reverses order)
    while (!s.empty()) {
      temp.push(s.top());
      s.pop();
    }

    // Check each element and keep survivors
    while (!temp.empty()) {
      int deger = temp.top();
      temp.pop();
      if (deger >= threshold) {
        survivors.push(deger);
      }
    }

    s = survivors;
    removedCount += (beforeSize - s.size());
  }
};

/*
 * QueuePlanet - Planet that uses std::queue for storage
 * Only allocates a queue, no vector or stack memory wasted.
 */
class QueuePlanet : public Planet {
public:
  queue<int> q;

  void push(int value) override { q.push(value); }

  void pop() override {
    if (q.empty())
      throw underflow_error("Cannot pop from empty Queue planet");
    q.pop(); // FIFO - removes the first element
  }

  int size() const override { return q.size(); }
  bool empty() const override { return q.empty(); }
  string typeName() const override { return "Queue"; }

  void display() const override {
    cout << "(" << typeName() << ") : ";
    queue<int> temp = q; // Backup to prevent data loss
    if (temp.empty())
      cout << "EMPTY";
    while (!temp.empty()) {
      cout << temp.front() << " ";
      temp.pop();
    }
    cout << endl;
  }

  void removeBelow(int threshold, int &removedCount) override {
    int beforeSize = q.size();
    queue<int> survivors;

    // Check each element in queue
    while (!q.empty()) {
      int deger = q.front();
      q.pop();
      if (deger >= threshold) {
        survivors.push(deger);
      }
    }

    q = survivors;
    removedCount += (beforeSize - q.size());
  }
};

/*
 * SolarSystem (Encapsulated Class)
 * All data members are private - no external access to internal state.
 * All operations are public methods - controlled interface only.
 * No global state - instances are created where needed.
 */
class SolarSystem {
private:
  struct Star {
    int id = 0;            // Star's identifier - to access the core
    int planet_count = 0;  // Number of planets in the system
    int element_count = 0; // Total element count in the system
  };

  Star star;
  vector<unique_ptr<Planet>>
      planets; // Polymorphic ownership via unique_ptr (RAII)

public:
  /*
   * elementCount: Displays total element count in the system
   * Time Complexity: O(1) - Direct access to counter variable
   * Space Complexity: O(1) - No additional memory used
   */
  void elementCount() const {
    cout << " Element count :" << star.element_count << endl;
  }

  /*
   * planetCount: Displays total planet count in the system
   * Time Complexity: O(1) - Direct access to counter variable
   * Space Complexity: O(1) - No additional memory used
   */
  void planetCount() const {
    cout << " Planet count :" << star.planet_count << endl;
  }

  /*
   * pushToPlanet: Inserts an element into a specific planet
   * Time Complexity: O(1) amortized - virtual dispatch to the correct push()
   * Space Complexity: O(1) - Only stores one element
   */
  void pushToPlanet(int index, int value) {
    if (index < 0 || index >= (int)planets.size())
      throw out_of_range("Planet index " + to_string(index) +
                         " is out of range");
    planets[index]->push(value); // Polymorphic call
    star.element_count++;
  }

  /*
   * addPlanet: Creates a new planet using polymorphism
   * Time Complexity: O(1) amortized
   * Space Complexity: O(1) - Only the needed data structure is allocated
   */
  void addPlanet() {
    char input;
    cout << "Enter the type of Planet" << endl
         << "V For Vector- S For Stack- Q For Queue : ";
    cin >> input;

    // Validate input through enum class - throws if invalid
    PlanetType type = parsePlanetType(input);

    unique_ptr<Planet> p;

    switch (type) {
    case PlanetType::Vector:
      p = make_unique<VectorPlanet>();
      break;
    case PlanetType::Stack:
      p = make_unique<StackPlanet>();
      break;
    case PlanetType::Queue:
      p = make_unique<QueuePlanet>();
      break;
    }

    // Set planet name based on its polymorphic type
    p->name = p->typeName() + "_Planet";
    p->metadata = planets.size();

    cout << "Planet Id: " << p->metadata << " (" << p->name << ")" << endl;

    planets.push_back(move(p)); // Transfer ownership to SolarSystem
    star.planet_count = planets.size();
    star.id = planets.size() - 1;
  }

  /*
   * blackHole: Completely destroys a planet and frees its memory
   * Time Complexity: O(n) where n is the number of planets
   */
  void blackHole(int planetIndex) {
    if (planetIndex < 0 || planetIndex >= (int)planets.size())
      throw out_of_range("Planet index " + to_string(planetIndex) +
                         " is out of range");

    Planet &p = *planets[planetIndex];
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
   * Time Complexity: O(n) where n is the number of planets
   */
  void supernova() {
    // Clear all planets - unique_ptr automatically deletes each derived object
    planets.clear();
    star.planet_count = 0;
    star.element_count = 0;
    star.id = 0;
  }

  /*
   * gravityPull: Removes elements below threshold from all planets
   * Time Complexity: O(n*m) where n=planets, m=elements per planet
   */
  void gravityPull(int threshold) {
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
   * Time Complexity: O(1) - Polymorphic dispatch to the correct pop()
   * Space Complexity: O(1) - No additional memory used
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
   * Time Complexity: O(n * m) where n = number of planets, m = elements per
   * planet Space Complexity: O(m) - Temporary copy for stack/queue traversal
   */
  void travelPlanet() const {
    for (size_t i = 0; i < planets.size(); i++) {
      Planet &p = *planets[i];
      cout << i + 1 << ". Planet [" << p.name << "] (ID: " << p.metadata
           << ") Elements ";
      p.display(); // Polymorphic call
    }
  }
};

int main() {
  try {
    SolarSystem system; // Local instance, no global state

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

    system.blackHole(1); // Destroy second planet
    system.travelPlanet();
    system.planetCount();

    system.gravityPull(15); // Remove elements < 15
    system.travelPlanet();
    system.elementCount();

    system.supernova(); // Destroy everything
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