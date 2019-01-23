#ifndef _FTK_LINEAR_SOLVE3_H
#define _FTK_LINEAR_SOLVE3_H

#include <ftk/numeric/det.hh>
#include <ftk/numeric/cond.hh>

namespace ftk {

template <typename T>
inline T linear_solver2_cond(const T A[2][2], const T b[2], T x[2])
{
  const T det = linear_solver2(A, b, x);
  const T cond = cond_real2x2(A);
  return cond;
}

template <typename T>
inline T linear_solver3(const T A[3][3], const T b[3], T x[3])
{
  T invA[3][3];
  const T det = matrix_inverse3(A, invA);
  matrix_vector_multiplication_3x3(invA, b, x);
  return det;
}

template <typename T> // returns determinant
// inline T linear_solver2(const T A[2][2], const T b[2], T x[2])
inline T solve_linear_real2x2(const T A[2][2], const T b[2], T x[2], const T epsilon=1e-6)
{
  const T D  = A[0][0]*A[1][1] - A[0][1]*A[1][0],
          Dx = b[0]   *A[1][1] - A[0][1]*b[1],
          Dy = A[0][0]*b[1]    - b[0]   *A[1][0];

  x[0] = Dx / D; 
  x[1] = Dy / D;
  // x[0] = (Dx + epsilon) / (D + epsilon);
  // x[1] = (Dy + epsilon) / (D + epsilon);
  
  return D;
}

template <typename T>
inline T solve_linear_real1(const T P[2], T x[1], const T epsilon = 1e-6)
{
  if (std::abs(P[1]) < epsilon || std::isinf(P[1]) || std::isnan(P[1])) return 0;
  else {
    if (std::isinf(P[0]) || std::isnan(P[0])) return 0; 
    else {
      x[0] = - P[0] / P[1];
      return 1;
    }
  }
}

template <typename T>
inline T solve_least_square3x2(const T A[3][2], const T b[3], T x[2], const T epsilon = 1e-9)
{
  T AT[2][3];
  transpose3x2(A, AT);

  T ATA[2][2];
  matrix_matrix_multiplication_2x3_3x2(AT, A, ATA);

  T invATA[2][2];
  const T det = matrix_inverse2(ATA, invATA);
  const T cond = cond_real2x2(ATA);

  T invATAAT[2][3];
  matrix_matrix_multiplication_2x2_2x3(invATA, AT, invATAAT);

  matrix_vector_multiplication_2x3(invATAAT, b, x);
  return cond;
}

}

#endif
