#ifndef DataTypes_hh
#define DataTypes_hh

template <typename T, size_t N1, size_t N2, size_t N3> using Array3 = T[N1][N2][N3];

template <typename T, size_t N1, size_t N2> using Array2 = T[N1][N2];

template <typename T, size_t N1> using Array1 = T[N1];

// the Map versions below seems very less performant, at least once streamed on file: factor 4 in size...

template <typename T, size_t N1, size_t N2, size_t N3>
// using Map3 = std::map<size_t, std::map<size_t, T[N3]>>;
// using Map3 = std::map<size_t, std::map<size_t, std::array<T, N3>>>;
using Map3 = std::map<size_t, std::map<size_t, std::map<size_t, T>>>;

template <typename T, size_t N1, size_t N2> using Map2 = std::map<size_t, std::map<size_t, T>>;

template <typename T, size_t N1> using Map1 = std::map<size_t, T>;

#endif
