#ifndef PLANET_HPP
#define PLANET_HPP

/**
 * @file Planet.hpp
 * @brief Polymorphic planet hierarchy and factory for the SolarSystem library.
 *
 * Defines the abstract BasePlanet<T> interface, three concrete implementations
 * (VectorPlanet, StackPlanet, QueuePlanet), and the PlanetFactory<T> class.
 * Each planet wraps a different STL container while exposing a uniform API
 * through virtual dispatch.
 *
 * @author mamitrkr
 * @version 1.0.0
 * @date 2026
 */

#include <algorithm>
#include <iostream>
#include <memory>
#include <queue>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

/**
 * @brief Strict enumeration for planet data structure types.
 *
 * Provides compile-time type safety for planet creation,
 * preventing invalid states that raw char inputs would allow.
 */
enum class PlanetType { Vector, Stack, Queue };

/**
 * @brief Converts a character input to a validated PlanetType enum.
 *
 * Accepts both uppercase and lowercase input for user convenience.
 *
 * @param input The character to parse ('V'/'v', 'S'/'s', 'Q'/'q').
 * @return The corresponding PlanetType enum value.
 * @throws std::invalid_argument If the input character is not recognized.
 * @note Complexity: O(1).
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

/**
 * @brief Abstract base class template for all planet types.
 *
 * Defines the polymorphic interface that all planet data structures must
 * implement. Uses RAII principles: std::string for name management, virtual
 * destructor for safe polymorphic deletion through smart pointers.
 *
 * @tparam T The data type stored within the planet's data structure.
 */
template <typename T> class BasePlanet {
public:
  string name;      ///< Planet name, managed by std::string (RAII).
  int metadata = 0; ///< Planet identifier/index.

  /**
   * @brief Virtual destructor ensuring safe polymorphic deletion.
   * @note Invoked automatically by std::unique_ptr when a planet is destroyed.
   */
  virtual ~BasePlanet() = default;

  /**
   * @brief Inserts a value into the planet's data structure.
   * @param value The value to insert (passed by const reference to avoid
   * copies).
   * @note Complexity: O(1) amortized.
   */
  virtual void push(const T &value) = 0;

  /**
   * @brief Removes an element from the planet's data structure.
   * @throws std::underflow_error If the data structure is empty.
   * @note Complexity: O(1).
   */
  virtual void pop() = 0;

  /**
   * @brief Returns the number of elements in the planet.
   * @return Element count.
   * @note Complexity: O(1).
   */
  virtual int size() const = 0;

  /**
   * @brief Checks whether the planet's data structure is empty.
   * @return True if empty, false otherwise.
   * @note Complexity: O(1).
   */
  virtual bool empty() const = 0;

  /**
   * @brief Displays all elements in the planet to stdout.
   * @note Complexity: O(n) where n is the number of elements.
   */
  virtual void display() const = 0;

  /**
   * @brief Returns the human-readable type name of the planet.
   * @return Type name string (e.g., "Vector", "Stack", "Queue").
   * @note Complexity: O(1).
   */
  virtual string typeName() const = 0;

  /**
   * @brief Removes all elements below the given threshold.
   * @param threshold Elements with value < threshold are removed.
   * @param removedCount Reference incremented by the number of removed
   * elements.
   * @note Complexity: O(n) where n is the number of elements.
   */
  virtual void removeBelow(const T &threshold, int &removedCount) = 0;
};

/**
 * @brief Planet implementation using std::vector<T> for storage.
 *
 * Only allocates a vector — no stack or queue memory is wasted.
 * Supports random access and dynamic resizing.
 *
 * @tparam T The data type stored in the vector.
 */
template <typename T> class VectorPlanet : public BasePlanet<T> {
public:
  vector<T> v; ///< Internal vector storage.

  /**
   * @brief Appends a value to the end of the vector.
   * @param value The value to insert.
   * @note Complexity: O(1) amortized.
   */
  void push(const T &value) override { v.push_back(value); }

  /**
   * @brief Removes the last element from the vector.
   * @throws std::underflow_error If the vector is empty.
   * @note Complexity: O(1).
   */
  void pop() override {
    if (v.empty())
      throw underflow_error("Cannot pop from empty Vector planet");
    v.pop_back();
  }

  /**
   * @brief Returns the number of elements in the vector.
   * @return Current element count.
   * @note Complexity: O(1).
   */
  int size() const override { return v.size(); }

  /**
   * @brief Checks whether the vector is empty.
   * @return True if the vector contains no elements, false otherwise.
   * @note Complexity: O(1).
   */
  bool empty() const override { return v.empty(); }

  /**
   * @brief Returns the type identifier string.
   * @return The string literal "Vector".
   * @note Complexity: O(1).
   */
  string typeName() const override { return "Vector"; }

  /**
   * @brief Displays all vector elements to stdout.
   * @note Complexity: O(n) where n is the number of elements.
   */
  void display() const override {
    cout << "(" << this->typeName() << ") : ";
    if (v.empty())
      cout << "EMPTY";
    for (size_t i = 0; i < v.size(); i++)
      cout << v[i] << " ";
    cout << endl;
  }

  /**
   * @brief Removes all elements below threshold using the erase-remove idiom.
   *
   * Uses std::remove_if to partition elements in-place, then erases the
   * trailing invalidated range. This avoids allocating a secondary container.
   *
   * @param threshold Elements with value < threshold are removed.
   * @param removedCount Reference incremented by number of removed elements.
   * @note Complexity: O(n) where n is the number of elements.
   */
  void removeBelow(const T &threshold, int &removedCount) override {
    int beforeSize = static_cast<int>(v.size());
    v.erase(remove_if(v.begin(), v.end(),
                       [&threshold](const T &val) { return val < threshold; }),
            v.end());
    removedCount += beforeSize - static_cast<int>(v.size());
  }
};

/**
 * @brief Planet implementation using std::stack<T> for LIFO storage.
 *
 * Only allocates a stack — no vector or queue memory is wasted.
 * Follows Last-In-First-Out ordering.
 *
 * @tparam T The data type stored in the stack.
 */
template <typename T> class StackPlanet : public BasePlanet<T> {
public:
  stack<T> s; ///< Internal stack storage.

  /**
   * @brief Pushes a value onto the top of the stack.
   * @param value The value to insert.
   * @note Complexity: O(1).
   */
  void push(const T &value) override { s.push(value); }

  /**
   * @brief Removes the top element from the stack (LIFO).
   * @throws std::underflow_error If the stack is empty.
   * @note Complexity: O(1).
   */
  void pop() override {
    if (s.empty())
      throw underflow_error("Cannot pop from empty Stack planet");
    s.pop();
  }

  /**
   * @brief Returns the number of elements in the stack.
   * @return Current element count.
   * @note Complexity: O(1).
   */
  int size() const override { return s.size(); }

  /**
   * @brief Checks whether the stack is empty.
   * @return True if the stack contains no elements, false otherwise.
   * @note Complexity: O(1).
   */
  bool empty() const override { return s.empty(); }

  /**
   * @brief Returns the type identifier string.
   * @return The string literal "Stack".
   * @note Complexity: O(1).
   */
  string typeName() const override { return "Stack"; }

  /**
   * @brief Displays all stack elements to stdout (top to bottom).
   * @note Complexity: O(n) where n is the number of elements.
   */
  void display() const override {
    cout << "(" << this->typeName() << ") : ";
    stack<T> temp = s; // Copy to preserve original
    if (temp.empty())
      cout << "EMPTY";
    while (!temp.empty()) {
      cout << temp.top() << " ";
      temp.pop();
    }
    cout << endl;
  }

  /**
   * @brief Removes all elements below threshold from the stack.
   * @param threshold Elements with value < threshold are removed.
   * @param removedCount Reference incremented by number of removed elements.
   * @note Complexity: O(n) where n is the number of elements.
   */
  void removeBelow(const T &threshold, int &removedCount) override {
    int beforeSize = static_cast<int>(s.size());
    stack<T> survivors;
    stack<T> temp;

    // Reverse into temp to preserve original insertion order
    while (!s.empty()) {
      temp.push(move(s.top()));
      s.pop();
    }
    // Filter: keep only elements >= threshold
    while (!temp.empty()) {
      T value = move(temp.top());
      temp.pop();
      if (value >= threshold)
        survivors.push(move(value));
    }
    s = move(survivors);
    removedCount += beforeSize - static_cast<int>(s.size());
  }
};

/**
 * @brief Planet implementation using std::queue<T> for FIFO storage.
 *
 * Only allocates a queue — no vector or stack memory is wasted.
 * Follows First-In-First-Out ordering.
 *
 * @tparam T The data type stored in the queue.
 */
template <typename T> class QueuePlanet : public BasePlanet<T> {
public:
  queue<T> q; ///< Internal queue storage.

  /**
   * @brief Enqueues a value at the back of the queue.
   * @param value The value to insert.
   * @note Complexity: O(1).
   */
  void push(const T &value) override { q.push(value); }

  /**
   * @brief Dequeues the front element from the queue (FIFO).
   * @throws std::underflow_error If the queue is empty.
   * @note Complexity: O(1).
   */
  void pop() override {
    if (q.empty())
      throw underflow_error("Cannot pop from empty Queue planet");
    q.pop();
  }

  /**
   * @brief Returns the number of elements in the queue.
   * @return Current element count.
   * @note Complexity: O(1).
   */
  int size() const override { return q.size(); }

  /**
   * @brief Checks whether the queue is empty.
   * @return True if the queue contains no elements, false otherwise.
   * @note Complexity: O(1).
   */
  bool empty() const override { return q.empty(); }

  /**
   * @brief Returns the type identifier string.
   * @return The string literal "Queue".
   * @note Complexity: O(1).
   */
  string typeName() const override { return "Queue"; }

  /**
   * @brief Displays all queue elements to stdout (front to back).
   * @note Complexity: O(n) where n is the number of elements.
   */
  void display() const override {
    cout << "(" << this->typeName() << ") : ";
    queue<T> temp = q; // Copy to preserve original
    if (temp.empty())
      cout << "EMPTY";
    while (!temp.empty()) {
      cout << temp.front() << " ";
      temp.pop();
    }
    cout << endl;
  }

  /**
   * @brief Removes all elements below threshold from the queue.
   * @param threshold Elements with value < threshold are removed.
   * @param removedCount Reference incremented by number of removed elements.
   * @note Complexity: O(n) where n is the number of elements.
   */
  void removeBelow(const T &threshold, int &removedCount) override {
    int beforeSize = static_cast<int>(q.size());
    queue<T> survivors;

    // Drain queue, keeping only elements >= threshold
    while (!q.empty()) {
      T value = move(q.front());
      q.pop();
      if (value >= threshold)
        survivors.push(move(value));
    }
    q = move(survivors);
    removedCount += beforeSize - static_cast<int>(q.size());
  }
};

/**
 * @brief Factory class for creating planet instances (Factory Design Pattern).
 *
 * Centralizes planet creation logic, adhering to the Open/Closed Principle.
 * To add a new planet type, only this factory needs modification —
 * the SolarSystem class remains untouched.
 *
 * @tparam T The data type that the created planet will store.
 */
template <typename T> class PlanetFactory {
public:
  /**
   * @brief Creates a new planet of the specified type.
   *
   * Returns a unique_ptr for RAII ownership transfer via move semantics.
   *
   * @param type The PlanetType enum specifying which derived class to
   * instantiate.
   * @return A unique_ptr<BasePlanet<T>> owning the newly created planet.
   * @throws std::invalid_argument If the PlanetType is not recognized.
   * @note Complexity: O(1).
   */
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

/**
 * @brief Smart Pointer Variety — Future Extension Guide.
 *
 * Current architecture uses unique_ptr<BasePlanet<T>> for exclusive ownership.
 *
 * For inter-planet interactions (e.g., moons orbiting planets):
 * - shared_ptr<BasePlanet<T>>  — multiple owners share a planet
 * - weak_ptr<BasePlanet<T>>    — non-owning observer (breaks circular
 * references)
 *
 * @code
 *   shared_ptr<BasePlanet<int>> planet = make_shared<VectorPlanet<int>>();
 *   weak_ptr<BasePlanet<int>>   moon_ref = planet;
 *   if (auto p = moon_ref.lock()) { p->display(); }
 * @endcode
 */

#endif // PLANET_HPP
