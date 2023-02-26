#ifndef __VECTOR_2D_H__
#define __VECTOR_2D_H__
#include<math.h>

class Vector2D {
public:
  Vector2D() {}

  Vector2D(float x, float y) : m_x(x), m_y(y) {}
  float getX() { return m_x; }
  float getY() { return m_y; }

  void setX(float x) { m_x = x; }
  void setY(float y) { m_y = y; }

  float length() { return sqrt(m_x*m_x+m_y*m_y); }
  float lengthSqr() { return m_x*m_x+m_y*m_y; }

  Vector2D operator+(const Vector2D &v2) const {
    return Vector2D(m_x+v2.m_x, m_y+v2.m_y);
  }

  friend Vector2D& operator+=(Vector2D &v1, const Vector2D &v2) {
    v1.m_x += v2.m_x;
    v1.m_y += v2.m_y;
    return v1;
  }

  Vector2D operator*(float scalar) {
    return Vector2D(m_x*scalar, m_y*scalar);
  }

  Vector2D& operator*=(float scalar) {
    m_x *= scalar;
    m_y *= scalar;
    return *this;
  }

  Vector2D operator-(const Vector2D &v2) const {
    return Vector2D(m_x - v2.m_x, m_y - v2.m_y);
  }
  friend Vector2D& operator-=(Vector2D &v1, const Vector2D &v2) {
    v1.m_x -= v2.m_x;
    v1.m_y -= v2.m_y;
    return v1;
  }

  Vector2D operator/(float scalar) {
    return Vector2D(m_x / scalar, m_y / scalar);
  }
  Vector2D &operator/=(float scalar) {
    m_x /= scalar;
    m_y /= scalar;
    return *this;
  }

  void normalize() {
    float l=length();
    if (l>0) {
      (*this) *= 1/l;
    }
  }

private:

  float m_x;
  float m_y;
};

#endif

// float b2_epsilon = 0.0004;

//  struct b2Vec2
//  {
//      b2Vec2() {}
  
//      b2Vec2(float xIn, float yIn) : x(xIn), y(yIn) {}
  
//      void SetZero() { x = 0.0f; y = 0.0f; }
  
//      void Set(float x_, float y_) { x = x_; y = y_; }
  
//      b2Vec2 operator -() const { b2Vec2 v; v.Set(-x, -y); return v; }
  
//      float operator () (int32 i) const
//      {
//          return (&x)[i];
//      }
  
//      float& operator () (int32 i)
//      {
//          return (&x)[i];
//      }
  
//      void operator += (const b2Vec2& v)
//      {
//          x += v.x; y += v.y;
//      }
  
//      void operator -= (const b2Vec2& v)
//      {
//          x -= v.x; y -= v.y;
//      }
  
//      void operator *= (float a)
//      {
//          x *= a; y *= a;
//      }
  
//      float Length() const
//      {
//          return b2Sqrt(x * x + y * y);
//      }
  
//      float LengthSquared() const
//      {
//          return x * x + y * y;
//      }
  
//      float Normalize()
//      {
//          float length = Length();
//          if (length = b2_epsilon)
//          {
//              return 0.0f;
//          }
//          float invLength = 1.0f / length;
//          x *= invLength;
//          y *= invLength;
  
//          return length;
//      }
  
//      b2Vec2 Skew() const
//      {
//          return b2Vec2(-y, x);
//      }
  
//      float x, y;
//  };