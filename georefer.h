#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>
#include <vector>
#include <cassert>
#include <iostream>

// =================================================================================
// 向量模板类 (Vector Template)
// =================================================================================

template <size_t N, typename T>
struct Vec {
    T data[N];

    Vec() { for (size_t i = 0; i < N; i++) data[i] = T(); }
    T& operator[](const size_t i) { assert(i < N); return data[i]; }
    const T& operator[](const size_t i) const { assert(i < N); return data[i]; }

    // 求向量的模长 (Magnitude/Norm)
    float norm() const {
        float result = 0;
        for (size_t i = 0; i < N; i++) result += data[i] * data[i];
        return std::sqrt(result);
    }

    // 单位化向量 (Normalize)
    Vec<N, T>& normalize() {
        *this = (*this) / norm();
        return *this;
    }
};

// --- 特化的 2D 向量 ---
template <typename T>
struct Vec<2, T> {
    union {
        struct { T x, y; };
        T raw[2];
    };

    Vec() : x(0), y(0) {}
    Vec(T _x, T _y) : x(_x), y(_y) {}
    T& operator[](const size_t i) { assert(i < 2); return raw[i]; }
    const T& operator[](const size_t i) const { assert(i < 2); return raw[i]; }

    float norm() const { return std::sqrt(x*x + y*y); }
    Vec<2, T>& normalize() { *this = (*this) / norm(); return *this; }
};

// --- 特化的 3D 向量 ---
template <typename T>
struct Vec<3, T> {
    union {
        struct { T x, y, z; };
        T raw[3];
    };

    Vec() : x(0), y(0), z(0) {}
    Vec(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
    T& operator[](const size_t i) { assert(i < 3); return raw[i]; }
    const T& operator[](const size_t i) const { assert(i < 3); return raw[i]; }

    float norm() const { return std::sqrt(x*x + y*y + z*z); }
    Vec<3, T>& normalize() { *this = (*this) / norm(); return *this; }
};

// --- 特化的 4D 向量 (用于齐次坐标) ---
template <typename T>
struct Vec<4, T> {
    union {
        struct { T x, y, z, w; };
        T raw[4];
    };
    
    Vec() : x(0), y(0), z(0), w(0) {}
    Vec(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}
    T& operator[](const size_t i) { assert(i < 4); return raw[i]; }
    const T& operator[](const size_t i) const { assert(i < 4); return raw[i]; }
};


// =================================================================================
// 向量运算 (Vector Operations)
// =================================================================================

// 向量加法
template <size_t N, typename T>
Vec<N, T> operator+(const Vec<N, T>& lhs, const Vec<N, T>& rhs) {
    Vec<N, T> result;
    for (size_t i = 0; i < N; i++) result[i] = lhs[i] + rhs[i];
    return result;
}

// 向量减法
template <size_t N, typename T>
Vec<N, T> operator-(const Vec<N, T>& lhs, const Vec<N, T>& rhs) {
    Vec<N, T> result;
    for (size_t i = 0; i < N; i++) result[i] = lhs[i] - rhs[i];
    return result;
}

// 向量与标量乘法
template <size_t N, typename T>
Vec<N, T> operator*(const Vec<N, T>& lhs, const T& rhs) {
    Vec<N, T> result;
    for (size_t i = 0; i < N; i++) result[i] = lhs[i] * rhs;
    return result;
}

// 向量与标量除法
template <size_t N, typename T>
Vec<N, T> operator/(const Vec<N, T>& lhs, const T& rhs) {
    Vec<N, T> result;
    for (size_t i = 0; i < N; i++) result[i] = lhs[i] / rhs;
    return result;
}

// 向量点积 (Dot Product)
template <size_t N, typename T>
T operator*(const Vec<N, T>& lhs, const Vec<N, T>& rhs) {
    T result = T();
    for (size_t i = 0; i < N; i++) result += lhs[i] * rhs[i];
    return result;
}

// 3D向量叉积 (Cross Product)
template <typename T>
Vec<3, T> cross(const Vec<3, T>& v1, const Vec<3, T>& v2) {
    return Vec<3, T>(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
}

// 用于调试输出
template <size_t N, typename T>
std::ostream& operator<<(std::ostream& out, const Vec<N, T>& v) {
    out << "(";
    for (size_t i = 0; i < N; i++) {
        out << v[i] << (i == N - 1 ? "" : ", ");
    }
    out << ")";
    return out;
}


// =================================================================================
// 矩阵模板类 (Matrix Template)
// =================================================================================

template <size_t R, size_t C, typename T> struct Matrix;

// --- 类型别名 (Typedefs) ---
// 这是我们最常用的类型
typedef Vec<2, float> Vector2f;
typedef Vec<2, int>   Vector2i;
typedef Vec<3, float> Vector3f;
typedef Vec<3, int>   Vector3i;
typedef Vec<4, float> Vector4f;
typedef Matrix<4, 4, float> Matrix4f;
typedef Matrix<3, 3, float> Matrix3f;


template<size_t R, size_t C, typename T>
struct Matrix {
    Vec<C, T> rows[R];

    Matrix() = default;
    Vec<C, T>& operator[](const size_t i) { assert(i < R); return rows[i]; }
    const Vec<C, T>& operator[](const size_t i) const { assert(i < R); return rows[i]; }

    // 获取某一列
    Vec<R, T> col(const size_t idx) const {
        assert(idx < C);
        Vec<R, T> ret;
        for (size_t i = R; i--; ret[i] = rows[i][idx]);
        return ret;
    }
    
    // 设置某一列
    void set_col(size_t idx, const Vec<R, T>& v) {
        assert(idx < C);
        for (size_t i = R; i--; rows[i][idx] = v[i]);
    }
    
    // 创建单位矩阵
    static Matrix<R, C, T> identity() {
        Matrix<R, C, T> ret;
        for (size_t i = 0; i < R; i++)
            for (size_t j = 0; j < C; j++)
                ret[i][j] = (i == j);
        return ret;
    }

    // 矩阵转置
    Matrix<C, R, T> transpose() const {
        Matrix<C, R, T> ret;
        for (size_t i = C; i--; ret[i] = this->col(i));
        return ret;
    }
    
    // =================================================================================
    //  新增实现: 矩阵求逆与行列式 (New: Inverse & Determinant)
    // =================================================================================

private:
    // 辅助函数: 获取 M(row, col) 的余子式矩阵 (Helper: Get the minor matrix)
    Matrix<R - 1, C - 1, T> get_minor(const size_t row, const size_t col) const {
        Matrix<R - 1, C - 1, T> ret;
        for (size_t i = 0; i < R - 1; i++) {
            for (size_t j = 0; j < C - 1; j++) {
                ret[i][j] = rows[i < row ? i : i + 1][j < col ? j : j + 1];
            }
        }
        return ret;
    }

    // 辅助函数: 计算代数余子式 (Helper: Calculate cofactor)
    T cofactor(const size_t row, const size_t col) const {
        // Cofactor C_ij = (-1)^(i+j) * det(M_ij)
        return get_minor(row, col).determinant() * (((row + col) % 2) ? -1 : 1);
    }

public:
    // 计算行列式 (Calculate determinant)
    T determinant() const {
        static_assert(R == C, "Determinant is only defined for square matrices");

        // 使用 if constexpr 进行编译时判断，为 1x1 矩阵提供递归基例
        if constexpr (R == 1) {
            return rows[0][0];
        } else {
            T ret = 0;
            // 沿第一行展开
            for (size_t j = 0; j < C; j++) {
                ret += rows[0][j] * get_minor(0, j).determinant() * ((j % 2) ? -1 : 1);
            }
            return ret;
        }
    }
    
    // 矩阵求逆 (Matrix Inverse)
    Matrix<R, C, T> inverse() const {
        static_assert(R == C, "Inverse is only defined for square matrices");
        T det = determinant();

        // 行列式为0的矩阵(奇异矩阵)没有逆矩阵。
        // 对于浮点数，我们检查其绝对值是否接近于0。
        assert(std::abs(det) > 1e-6 && "Cannot invert a singular matrix (determinant is close to zero)");

        Matrix<R, C, T> result;
        // 计算伴随矩阵 (Adjugate Matrix)
        for (size_t i = 0; i < R; i++) {
            for (size_t j = 0; j < C; j++) {
                // 伴随矩阵的 (i,j) 元素是原矩阵代数余子式矩阵的 (j,i) 元素
                result[i][j] = cofactor(j, i);
            }
        }

        // 伴随矩阵除以行列式
        T inv_det = static_cast<T>(1.0) / det;
        for (size_t i = 0; i < R; i++) {
            for (size_t j = 0; j < C; j++) {
                result[i][j] = result[i][j] * inv_det;
            }
        }
        return result;
    }
};


// =================================================================================
// 矩阵运算 (Matrix Operations)
// =================================================================================

// 矩阵 * 矩阵
template<size_t R, size_t C, size_t K, typename T>
Matrix<R, C, T> operator*(const Matrix<R, K, T>& lhs, const Matrix<K, C, T>& rhs) {
    Matrix<R, C, T> result;
    for (size_t i = 0; i < R; i++) {
        for (size_t j = 0; j < C; j++) {
            result[i][j] = lhs[i] * rhs.col(j);
        }
    }
    return result;
}

// 矩阵 * 向量
template<size_t R, size_t C, typename T>
Vec<R, T> operator*(const Matrix<R, C, T>& lhs, const Vec<C, T>& rhs) {
    Vec<R, T> result;
    for (size_t i = 0; i < R; i++) {
        result[i] = lhs[i] * rhs;
    }
    return result;
}

// 用于调试输出
template<size_t R, size_t C, typename T>
std::ostream& operator<<(std::ostream& out, const Matrix<R,C,T>& m) {
    for (size_t i=0; i<R; i++) {
        out << m[i] << std::endl;
    }
    return out;
}

#endif //__GEOMETRY_H__