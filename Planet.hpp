#ifndef PLANET_HPP
#define PLANET_HPP

#include <iostream>
#include <memory>
#include <queue>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>


using namespace std;

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
inline PlanetType parsePlanetType(char input) {
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
 * BasePlanet<T> (Abstract Base Class Template)
 * Generics: template<typename T> allows planets to store ANY data type,
 * not just int. This follows the Open/Closed Principle.
 *
 * Const-Correctness: All non-modifying methods are marked const.
 * Virtual destructor ensures safe polymorphic deletion (RAII).
 */
template <typename T> class BasePlanet {
public:
  string name;      // RAII: std::string manages its own memory
  int metadata = 0; // RAII: plain int, no dynamic allocation needed

  virtual ~BasePlanet() =
      default; // Virtual destructor for safe polymorphic deletion

  // Pure virtual functions - each derived class implements its own behavior
  virtual void push(const T &value) = 0; // const ref: avoids unnecessary copy
  virtual void pop() = 0;
  virtual int size() const = 0;        // const: does not modify state
  virtual bool empty() const = 0;      // const: does not modify state
  virtual void display() const = 0;    // const: does not modify state
  virtual string typeName() const = 0; // const: does not modify state
  virtual void removeBelow(const T &threshold, int &removedCount) = 0;
};

/*
 * VectorPlanet<T> - Planet that uses std::vector<T> for storage
 * Only allocates a vector, no stack or queue memory wasted.
 */
template <typename T> class VectorPlanet : public BasePlanet<T> {
public:
  vector<T> v;

  void push(const T &value) override { v.push_back(value); }

  void pop() override {
    if (v.empty())
      throw underflow_error("Cannot pop from empty Vector planet");
    v.pop_back();
  }

  int size() const override { return v.size(); }
  bool empty() const override { return v.empty(); }
  string typeName() const override { return "Vector"; }

  void display() const override {
    cout << "(" << this->typeName() << ") : ";
    if (v.empty())
      cout << "EMPTY";
    for (size_t i = 0; i < v.size(); i++)
      cout << v[i] << " ";
    cout << endl;
  }

  void removeBelow(const T &threshold, int &removedCount) override {
    int beforeSize = v.size();
    vector<T> survivors;
    size_t j = 0;
    while (j < v.size()) {
      const T &deger = v[j];
      if (deger >= threshold) {
        survivors.push_back(deger);
      }
      j++;
    }
    v = move(survivors); // Move semantics: avoids copying the entire vector
    removedCount += (beforeSize - (int)v.size());
  }
};

/*
 * StackPlanet<T> - Planet that uses std::stack<T> for storage
 * Only allocates a stack, no vector or queue memory wasted.
 */
template <typename T> class StackPlanet : public BasePlanet<T> {
public:
  stack<T> s;

  void push(const T &value) override { s.push(value); }

  void pop() override {
    if (s.empty())
      throw underflow_error("Cannot pop from empty Stack planet");
    s.pop();
  }

  int size() const override { return s.size(); }
  bool empty() const override { return s.empty(); }
  string typeName() const override { return "Stack"; }

  void display() const override {
    cout << "(" << this->typeName() << ") : ";
    stack<T> temp = s;
    if (temp.empty())
      cout << "EMPTY";
    while (!temp.empty()) {
      cout << temp.top() << " ";
      temp.pop();
    }
    cout << endl;
  }

  void removeBelow(const T &threshold, int &removedCount) override {
    int beforeSize = s.size();
    stack<T> survivors;
    stack<T> temp;

    while (!s.empty()) {
      temp.push(move(s.top())); // Move semantics
      s.pop();
    }

    while (!temp.empty()) {
      T deger = move(temp.top()); // Move semantics
      temp.pop();
      if (deger >= threshold) {
        survivors.push(move(deger));
      }
    }

    s = move(survivors); // Move semantics: avoids copying the entire stack
    removedCount += (beforeSize - (int)s.size());
  }
};

/*
 * QueuePlanet<T> - Planet that uses std::queue<T> for storage
 * Only allocates a queue, no vector or stack memory wasted.
 */
template <typename T> class QueuePlanet : public BasePlanet<T> {
public:
  queue<T> q;

  void push(const T &value) override { q.push(value); }

  void pop() override {
    if (q.empty())
      throw underflow_error("Cannot pop from empty Queue planet");
    q.pop();
  }

  int size() const override { return q.size(); }
  bool empty() const override { return q.empty(); }
  string typeName() const override { return "Queue"; }

  void display() const override {
    cout << "(" << this->typeName() << ") : ";
    queue<T> temp = q;
    if (temp.empty())
      cout << "EMPTY";
    while (!temp.empty()) {
      cout << temp.front() << " ";
      temp.pop();
    }
    cout << endl;
  }

  void removeBelow(const T &threshold, int &removedCount) override {
    int beforeSize = q.size();
    queue<T> survivors;

    while (!q.empty()) {
      T deger = move(q.front()); // Move semantics
      q.pop();
      if (deger >= threshold) {
        survivors.push(move(deger));
      }
    }

    q = move(survivors); // Move semantics: avoids copying the entire queue
    removedCount += (beforeSize - (int)q.size());
  }
};

/*
 * PlanetFactory<T> (Factory Design Pattern)
 * Centralizes planet creation logic in one place.
 * Open/Closed Principle: To add a new planet type (e.g., DequePlanet),
 * you only add a new case here - no changes to SolarSystem needed.
 *
 * Returns unique_ptr for RAII ownership transfer via move semantics.
 */
template <typename T> class PlanetFactory {
public:
  static unique_ptr<BasePlanet<T>> create(PlanetType type) {
    switch (type) {
    case PlanetType::Vector:
      return make_unique<VectorPlanet<T>>();
    case PlanetType::Stack:
      return make_unique<StackPlanet<T>>();
    case PlanetType::Queue:
      return make_unique<QueuePlanet<T>>();
    }
    throw invalid_argument("Unknown PlanetType in factory");
  }
};

/*
 * SMART POINTER VARIETY - Future Extension Guide
 *
 * Current: unique_ptr<BasePlanet<T>> — exclusive ownership (one owner per
 * planet)
 *
 * For inter-planet interactions (e.g., moons orbiting planets):
 *   - shared_ptr<BasePlanet<T>>  — multiple owners share a planet
 *   - weak_ptr<BasePlanet<T>>    — non-owning observer (breaks circular
 * references)
 *
 * Example:
 *   shared_ptr<BasePlanet<int>> planet = make_shared<VectorPlanet<int>>();
 *   weak_ptr<BasePlanet<int>>   moon_ref = planet; // Moon observes, doesn't
 * own if (auto p = moon_ref.lock()) { p->display(); } // Safe access check
 */

#endif // PLANET_HPP
