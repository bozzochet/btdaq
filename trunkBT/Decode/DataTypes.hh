#ifndef DataTypes_hh
#define DataTypes_hh

template <typename T, size_t N1, size_t N2, size_t N3>
using Array3 = T[N1][N2][N3];

template <typename T, size_t N1, size_t N2>
using Array2 = T[N1][N2];

template <typename T, size_t N1>
using Array1 = T[N1];

#endif