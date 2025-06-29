#pragma once

#include<iostream>
#include<cassert>
#include<cmath>

template<size_t N, typename T>
struct Vec{
    T data[N];

    Vec(): data{} {}

    T& operator[](size_t index){
        assert(index < N);
        return data[index];
    }

    const T& operator[](size_t index) const {
        assert(index < N);
        return data[index];
    }

    float norm() const{
        float result = 0;

        for(size_t i = 0;i < N;i++){
            result += data[i] * data[i];
        }

        return std::sqrt(result);
    }

    Vec<N,T>& normalize(){
        *this = (*this) / norm();

        return *this;
    }

};

template<typename T>
struct Vec<2,T>{
    union{
        struct{T x,y;};
        T raw[2];
    };

    Vec<2,T>() : x{0}, y{0} {}
    Vec<2,T>(T _x, T _y) : x(_x), y(_y) {}
    
    T& operator[](size_t index){assert(index < 2); return raw[index];}
    const T& operator[](size_t index) const { assert(index < 2); return raw[index]; }

    float norm() const{
        return std::sqrt(x*x + y*y);
    }

    Vec<2,T>& normalize(){
        *this = (*this) / norm();
        return *this;
    }
};

template<typename T>
struct Vec<3,T>{
    union{
        struct{T x,y,z;};
        T raw[3];
    };

    Vec<3,T>() : x{0}, y{0}, z{0} {}
    Vec<3,T>(T _x, T _y, T _z) : x(_x), y(_y), z{_z} {}
    
    T& operator[](size_t index){assert(index < 3); return raw[index];}
    const T& operator[](size_t index) const { assert(index < 3); return raw[index]; }

    float norm() const{
        return std::sqrt(x*x + y*y + z*z);
    }

    Vec<3,T>& normalize(){
        *this = (*this) / norm();
        return *this;
    }
};

template<typename T>
struct Vec<4,T>{
    union{
        struct{T x,y,z,w;};
        T raw[4];
    };

    Vec<4,T>() : x{0}, y{0}, z{0}, w{0} {}
    Vec<4,T>(T _x, T _y, T _z, T _w) : x(_x), y(_y), z{_z}, w{_w} {}
    
    T& operator[](size_t index){assert(index < 4); return raw[index];}
    const T& operator[](size_t index) const { assert(index < 4); return raw[index]; }
};

template<size_t N, typename T>
Vec<N,T> operator+(const Vec<N,T>& a, const Vec<N,T>& b){
    Vec<N,T> result;
    for(size_t i = 0;i < N;i++) {result[i] = a[i] + b[i];}
    return result;
}

template<size_t N, typename T>
Vec<N,T> operator-(const Vec<N,T>& a, const Vec<N,T>& b){
    Vec<N,T> result;
    for(size_t i = 0;i < N;i++) {result[i] = a[i] - b[i];}
    return result;
}

template<size_t N, typename T>
Vec<N,T> operator-(const Vec<N,T>& a){
    Vec<N,T> result;
    for(size_t i = 0;i < N;i++) {result[i] = -a[i];}
    return result;
}

template<size_t N, typename T>
Vec<N,T> operator*(const Vec<N,T>& a, T scalar){
    Vec<N,T> result;
    for(size_t i = 0;i < N;i++){result[i] = a[i]*scalar;}    
    return result;
}

// 向量点积 (Dot Product)
template <size_t N, typename T>
T operator*(const Vec<N, T>& lhs, const Vec<N, T>& rhs) {
    T result = T();
    for (size_t i = 0; i < N; i++) result += lhs[i] * rhs[i];
    return result;
}


template<size_t N, typename T>
Vec<N,T> operator/(const Vec<N,T>& a, T scalar){
    Vec<N,T> result;
    for(size_t i = 0;i < N;i++){result[i] = a[i]/scalar;}    
    return result;
}

template<size_t N, typename T>
T dot(const Vec<N,T>& a, const Vec<N,T>& b){
    T result;
    for(size_t i = 0;i < N;i++) {result[i] += a[i] * b[i];}
    return result;
}

// specific for 3-dimension vector
template<typename T>
Vec<3,T> cross(const Vec<3,T>& a, const Vec<3,T>& b){
    return Vec<3,T>{
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

template<typename T>
Vec<4,T> toVec4(const Vec<3,T>& v, T w = 1.0f) {
    return Vec<4,T>{v.x, v.y, v.z, w};
}

template<typename T>
Vec<3,T> toVec3(const Vec<4,T>& v){
    return Vec<3,T>(v.x, v.y, v.z);
}

template<size_t N, typename T>
std::ostream& operator<<(std::ostream& os, const Vec<N, T>& vec){
    os << "("; 
    for(size_t i = 0; i < N; ++i) {
        os << vec[i];
        if (i < N - 1) {
            os << ", ";
        }
    }
    os << ")";
    return os;
}

// using Vector2f = Vec<2, float>;
// using Vector3f = Vec<3, float>;
// using Vector4f = Vec<4, float>;
// using Vector3i = Vec<3, int>;



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