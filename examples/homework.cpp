#include <iostream>
#include <memory>
#include <cassert>

using namespace std;

/**
 * Zadanie 1.
 *
 * Przerób poniższą implementację klasy Vector w taki sposób, aby korzystała z smart pointerów.
 * Następnie zaimplementuj metodę `getPtr`, która zwaraca std::shared_ptr zarządzający "this".
 *
 */
template<typename T>
class Vector {
public:
  Vector() {
    cout << "Default constr" << endl;

    arr = new T[1];
    capacity = 1;
    size = 0;
  }

  ~Vector() {
    delete[] arr;
  }

  Vector(const Vector& m) {
    cout << "Copy constr" << endl;

    arr = new T[m.capacity];
    size = m.size;
    capacity = m.capacity;
    std::copy(m.arr, m.arr + m.getSize(), arr);
  }

  Vector(Vector&& m) {
    arr = m.arr;
    capacity = m.capacity;
    size = m.size;

    m.arr = nullptr;
    m.size = 0;
    m.capacity = 0;
  }

  Vector(std::initializer_list<T> list) {
    size = list.size();
    arr = new T[size];
    capacity = size;
    auto it = list.begin();
    for (int i = 0; i < list.size(); i++) {
      arr[i] = *it++;
    }
  }

  Vector<T>& operator=(const Vector<T>& other) {
    cout << "Assign operator" << endl;

    if (this == &other) {
      return *this;
    }

    delete[] arr;

    arr = new T[other.capacity];
    size = other.size;
    capacity = other.capacity;
    std::copy(other.arr, other.arr + other.getSize(), arr);
  }

  Vector<T>& operator=(Vector<T>&& other) {
    cout << "Move operator" << endl;

    if (this == &other) {
      return *this;
    }

    delete[] arr;

    arr = other.arr;
    size = other.size;
    capacity = other.capacity;

    other.arr = nullptr;
    other.size = 0;
    other.capacity = 0;
  }

  void push(T data) {
    if (size == capacity) {
      T* temp = new T[2 * capacity];

      for (int i = 0; i < capacity; i++) {
        temp[i] = arr[i];
      }

      delete[] arr;
      capacity *= 2;
      arr = temp;
    }

    arr[size] = data;
    size++;
  }

  T operator[](size_t index) const {
    return arr[index];
  }

  T& operator[](size_t index) {
    return arr[index];
  }

  size_t getSize() const { return size; }

  void reset() {
    delete[] arr;

    size = 0;
    capacity = 1;
    arr = new T[1];
  }

  std::shared_ptr<Vector<T>> getPtr() {
    throw "Not implemented"; // TODO
  }

  friend ostream& operator<<(ostream& out, const Vector& m) {
    for (int i = 0; i < m.size; ++i) {
      std::cout << m.arr[i] << " ";
    }
    return out;
  }

private:
  T* arr;
  int capacity;
  int size;
};

class AddOperation {
public:
  [[nodiscard]] static int operation(int a, int b) {
    return a + b;
  }
};

class SubOperation {
public:
  [[nodiscard]] static int operation(int a, int b) {
    return a - b;
  }
};

class MultOperation {
public:
  [[nodiscard]] static int operation(int a, int b) {
    return a * b;
  }
};

class DivOperation {
public:
  [[nodiscard]] static int operation(int a, int b) {
    return a / b;
  }
};

/**
 * Zadanie 2.
 *
 * Rozszerz poniższą implementację tak umożliwała tworzenie lazy wyrażeń zawierających liczby (typu int).
 *
 * Przydład:
 * Vector<int> v = Vector<int>{1,2,3,4} * 2 + Vector<int>{4,5,6} - 12;
 *
 */
template<typename A, typename B, typename Operation>
class LazyVectorNode {
public:
  LazyVectorNode(A&& a, B&& b) : a(std::forward<A>(a)), b(std::forward<B>(b)) {}

  int operator[](size_t index) {
    if (a.getSize() <= index) {
      return b[index];
    }
    if (b.getSize() <= index) {
      return a[index];
    }
    return Operation::operation(a[index], b[index]);
  }

  [[nodiscard]] size_t getSize() const {
    return std::max(a.getSize(), b.getSize());
  }

  template<typename T>
  operator Vector<T>() {
    int minSize = std::min(a.getSize(), b.getSize());
    Vector<T> result;
    for (size_t i = 0; i < minSize; i++) {
      result.push(Operation::operation(a[i], b[i]));
    }

    for (size_t i = minSize; i < a.getSize(); i++) {
      result.push(a[i]);
    }

    for (size_t i = minSize; i < b.getSize(); i++) {
      result.push(b[i]);
    }

    return result;
  }

private:
  A a;
  B b;
};

template<typename A, typename B>
LazyVectorNode<A, B, AddOperation> operator+(A&& a, B&& b) {
  return LazyVectorNode<A, B, AddOperation>(std::forward<A>(a), std::forward<B>(b));
}

template<typename A, typename B>
LazyVectorNode<A, B, SubOperation> operator-(A&& a, B&& b) {
  return LazyVectorNode<A, B, SubOperation>(std::forward<A>(a), std::forward<B>(b));
}

template<typename A, typename B>
LazyVectorNode<A, B, MultOperation> operator*(A&& a, B&& b) {
  return LazyVectorNode<A, B, MultOperation>(std::forward<A>(a), std::forward<B>(b));
}

template<typename A, typename B>
LazyVectorNode<A, B, DivOperation> operator/(A&& a, B&& b) {
  return LazyVectorNode<A, B, DivOperation>(std::forward<A>(a), std::forward<B>(b));
}

int main() {
  Vector<int> v1{1, 2, 3, 4, 5, 6};
  std::cout << v1 << std::endl; // 1, 2, 3, 4, 5 ,6
  Vector<int> v2 = v1;          // Copy constr

  assert(v2[2] == 3);

  v2[2] = 10;

  assert(v1[2] == 3);

  Vector<int> v3(std::move(v2));

  assert(v3[2] == 10);

  v3 = std::move(v3);
  assert(v3.getSize() == 6); // Move operator

  for (int i = 0; i < 50; i++) {
    v3.push(i);
  }

  v3.reset();
  assert(v3.getSize() == 0);

  for (int i = 0; i < 15; i++) {
    v3.push(i);
  }

  assert(v3.getSize() == 15); // Move operator

  for (int i = 0; i < 15; i++) {
    assert(v3[i] == i);
  }

  std::shared_ptr<Vector<int>> sharedVector = std::make_shared<Vector<int>>(); // Default constr
  assert(sharedVector.use_count() == 1);
  std::shared_ptr<Vector<int>> secondSharedVector = sharedVector->getPtr();

  assert(sharedVector.use_count() == 2);
  try {
    Vector<int> vec; // Default constr
    std::shared_ptr<Vector<int>> sp = vec.getPtr();
    assert(false);
  } catch(std::bad_weak_ptr& e) {
    std::cout << e.what() << std::endl; // bad_weak_ptr
  }

  Vector<int> v4 = (v3 + Vector{9, 8, 7}) * v1; // Default constr
  Vector<int> result4{9, 18, 27, 12, 20, 30, 6, 7, 8, 9, 10, 11, 12, 13, 14};
  cout << v4 << std::endl; // 9 18 27 12 20 30 6 7 8 9 10 11 12 13 14
  assert(v4.getSize() == result4.getSize());
  for (int i = 0; i < result4.getSize(); i++) {
    assert(v4[i] == result4[i]);
  }

  Vector<int> v5 = Vector{1, 2, 3, 4} * 2 + Vector{-2, -4, -6, -8} / 2 + Vector{1, 1}; // Default constr
  Vector<int> result5{2, 3, 3, 4};
  cout << v5 << std::endl; // 2 3 3 4
  assert(v5.getSize() == result5.getSize());
  for (int i = 0; i < result5.getSize(); i++) {
    assert(v5[i] == result5[i]);
  }

  Vector<int> v6 = 5 + (Vector{1, 2, 3, 4} + 2) + (2 + Vector{1, 2, 3, 4}) * Vector{1, 1}; // Default constr
  Vector<int> result6{11, 13, 15, 17};
  cout << v6 << std::endl; // 11 13 15 17
  assert(v6.getSize() == result6.getSize());
  for (int i = 0; i < result6.getSize(); i++) {
    assert(v6[i] == result6[i]);
  }

  auto temp7 = v1 + v1;
  v1[0] = 10;
  Vector<int> v7 = temp7;       // Default constr
  std::cout << v7 << std::endl; // 20 4 6 8 10 12
  assert(v7[0] == 20);
}

/*
1 2 3 4 5 6
Copy constr
Move operator
Default constr
Default constr
bad_weak_ptr
Default constr
9 18 27 12 20 30 6 7 8 9 10 11 12 13 14
Default constr
2 3 3 4
Default constr
11 13 15 17
Default constr
20 4 6 8 10 12
 */