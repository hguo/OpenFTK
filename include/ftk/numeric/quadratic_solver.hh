#ifndef _FTK_QUADRATIC_SOLVER_HH
#define _FTK_QUADRATIC_SOLVER_HH

#include <cmath>
#include <complex>
#include <ftk/numeric/sqrt.hh>
#include <ftk/numeric/linear_solver.hh>

namespace ftk {

template <typename T>
inline void solve_quadratic(const T P[3], std::complex<T> x[2])
{
  const T delta = P[1]*P[1] - 4*P[2]*P[0];
  if (delta >= 0) {
    x[0] = (-P[1] + sqrt(delta)) / (2 * P[2]);
    x[1] = (-P[1] - sqrt(delta)) / (2 * P[2]);
  } else {
    x[0] = (-P[1] + complex_sqrt<T>(delta)) / (2 * P[2]);
    x[1] = (-P[1] - complex_sqrt<T>(delta)) / (2 * P[2]);
  }
}

template <typename T>
inline int solve_quadratic_real(const T P[3], T x[2], const T epsilon = 1e-8)
{
  if (std::abs(P[2]) < epsilon || std::isinf(P[2]) || std::isnan(P[2])) 
    return solve_linear_real1(P, x);
  else if (std::abs(P[0]) < epsilon || std::isinf(P[0]) || std::isnan(P[0])) {
    const int n = solve_linear_real1(P+1, x);
    x[n+1] = T(0);
    return n+1;
  }
  else {
    const T delta = P[1]*P[1] - 4*P[2]*P[0];
    if (delta > 0) {
      x[0] = (-P[1] + sqrt(delta)) / (2 * P[2]);
      x[1] = (-P[1] - sqrt(delta)) / (2 * P[2]);
      return 2;
    } else if (delta == T(0)) {
      x[0] = -P[1] / (2 * P[2]);
      return 1;
    } else 
      return 0;
  }
}

}

#endif
