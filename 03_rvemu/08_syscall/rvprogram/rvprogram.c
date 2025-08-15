#include <assert.h>
#include <stdint.h>
#include <stdio.h>

// 矩阵大小
#define N 4

// ===================== 整型矩阵乘法 =====================
void matrix_mult_int(const int A[N][N], const int B[N][N], int C[N][N])
{
       for (int i = 0; i < N; i++)
       {
              for (int j = 0; j < N; j++)
              {
                     C[i][j] = 0;
                     for (int k = 0; k < N; k++)
                     {
                            C[i][j] += A[i][k] * B[k][j];
                     }
              }
       }
}

void test_int_matrix()
{
       int A[N][N] = {
           {1, 2, 3, 4},
           {5, 6, 7, 8},
           {9, 10, 11, 12},
           {13, 14, 15, 16}};

       int B[N][N] = {
           {16, 15, 14, 13},
           {12, 11, 10, 9},
           {8, 7, 6, 5},
           {4, 3, 2, 1}};

       int C[N][N];
       int expected[N][N] = {
           {80, 70, 60, 50},
           {240, 214, 188, 162},
           {400, 358, 316, 274},
           {560, 502, 444, 386}};

       matrix_mult_int(A, B, C);

       // 验证结果
       for (int i = 0; i < N; i++)
       {
              for (int j = 0; j < N; j++)
              {
                     assert(C[i][j] == expected[i][j]);
              }
       }
}

// ===================== 单精度浮点矩阵乘法 =====================
void matrix_mult_float(const float A[N][N], const float B[N][N], float C[N][N])
{
       for (int i = 0; i < N; i++)
       {
              for (int j = 0; j < N; j++)
              {
                     C[i][j] = 0.0f;
                     for (int k = 0; k < N; k++)
                     {
                            C[i][j] += A[i][k] * B[k][j];
                     }
              }
       }
}

void test_float_matrix()
{
       float A[N][N] = {
           {1.1f, 2.2f, 3.3f, 4.4f},
           {5.5f, 6.6f, 7.7f, 8.8f},
           {9.9f, 10.1f, 11.11f, 12.12f},
           {13.13f, 14.14f, 15.15f, 16.16f}};

       float B[N][N] = {
           {0.1f, 0.2f, 0.3f, 0.4f},
           {0.5f, 0.6f, 0.7f, 0.8f},
           {0.9f, 1.0f, 1.1f, 1.2f},
           {1.3f, 1.4f, 1.5f, 1.6f}};

       float C[N][N];
       float expected[N][N] = {
           {9.9f, 11.0f, 12.1f, 13.2f},
           {22.2f, 25.0f, 27.9f, 30.8f},
           {31.8f, 36.1f, 40.4f, 44.7f},
           {43.0f, 48.9f, 54.7f, 60.6f}};

       matrix_mult_float(A, B, C);

       // 浮点数比较需要考虑精度误差
       for (int i = 0; i < N; i++)
       {
              for (int j = 0; j < N; j++)
              {
                     assert((C[i][j] - expected[i][j]) < 0.1f &&
                            (expected[i][j] - C[i][j]) < 0.1f);
              }
       }
}

// ===================== 双精度浮点矩阵乘法 =====================
void matrix_mult_double(const double A[N][N], const double B[N][N], double C[N][N])
{
       for (int i = 0; i < N; i++)
       {
              for (int j = 0; j < N; j++)
              {
                     C[i][j] = 0.0;
                     for (int k = 0; k < N; k++)
                     {
                            C[i][j] += A[i][k] * B[k][j];
                     }
              }
       }
}

void test_double_matrix()
{
       double A[N][N] = {
           {0.01, 0.02, 0.03, 0.04},
           {0.05, 0.06, 0.07, 0.08},
           {0.09, 0.10, 0.11, 0.12},
           {0.13, 0.14, 0.15, 0.16}};

       double B[N][N] = {
           {1.0, 2.0, 3.0, 4.0},
           {5.0, 6.0, 7.0, 8.0},
           {9.0, 10.0, 11.0, 12.0},
           {13.0, 14.0, 15.0, 16.0}};

       double C[N][N];
       double expected[N][N] = {
           {0.9, 1.0, 1.1, 1.2},
           {2.0, 2.3, 2.5, 2.8},
           {3.1, 3.6, 4.0, 4.4},
           {4.3, 4.8, 5.4, 6.0}};

       matrix_mult_double(A, B, C);

       // 双精度浮点数比较
       for (int i = 0; i < N; i++)
       {
              for (int j = 0; j < N; j++)
              {
                     assert((C[i][j] - expected[i][j]) < 0.1 &&
                            (expected[i][j] - C[i][j]) < 0.1);
              }
       }
}

// ===================== 主函数 =====================
int main()
{
       // 测试整型矩阵乘法
       test_int_matrix();

       // 测试单精度浮点矩阵乘法
       test_float_matrix();

       // 测试双精度浮点矩阵乘法
       test_double_matrix();

       printf("PASS\n");
       return 0;
}