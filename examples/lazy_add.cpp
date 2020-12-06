#include <iostream>

template<typename Left, typename Right>
struct AddOp {
  const Left& left;
  const Right& right;

  AddOp(const Left& left, const Right& right) : left(left), right(right) {}

  int getX() const { return left.getX() + right.getX(); }

  int getY() const { return left.getY() + right.getY(); }
};

struct Point {
private:
  int x;
  int y;

public:
  Point(int x, int y) : x(x), y(y) {}

  template<typename Left, typename Right>
  Point(const AddOp<Left, Right>& op) {
    x = op.getX();
    y = op.getY();
  }

  template<typename Left, typename Right>
  Point& operator=(const AddOp<Left, Right>& op) {
    x = op.getX();
    y = op.getY();
    return *this;
  }

  AddOp<Point, Point> operator+(const Point & right) {
    return AddOp<Point, Point>(*this, right);
  }

  int getX() const { return x; }

  int getY() const { return y; }
};

template<typename Left, typename Right>
AddOp<AddOp<Left, Right>, Point> operator+(const AddOp<Left, Right>& left, const Point& p) {
  return AddOp<AddOp<Left, Right>, Point>(left, p);
}

template<typename Left, typename Right>
AddOp<Point, AddOp<Left, Right>> operator+(const Point& p, const AddOp<Left, Right>& rhs) {
  return AddOp<Point, AddOp<Left, Right>>(p, rhs);
}

int main() {
  Point p1{1, 2};
  Point p2{3, 4};
  Point p3{5, 6};

  Point result = p1 + p2 + p3 ;

  std::cout << result.getX() << std::endl;
  std::cout << result.getY() << std::endl;

  std::cout << std::endl;

  AddOp<Point, Point> temp = p1 + p2;
  Point r = temp;
  std::cout << r.getX() << std::endl;
  std::cout << r.getY() << std::endl;

  return 0;
}
