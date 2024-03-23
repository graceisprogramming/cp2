#include "mychain.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// void simplify_poly(sPoly *poly) 
// {
//     if (poly->size <= 1) return; // No simplification needed for 0 or 1 term.
//     int maxPower = 0;
//     for (uint32_t i = 0; i < poly->size; ++i) 
//     {
//         if (poly->pPowers[i] > maxPower) maxPower = poly->pPowers[i];
//     }
//     int32_t *combinedCoefficients = calloc(maxPower + 1, sizeof(int32_t));
//     if (!combinedCoefficients) 
//     {
//         return;
//         // TODO: handle error
//     }
//     for (uint32_t i = 0; i < poly->size; ++i) 
//     {
//         combinedCoefficients[poly->pPowers[i]] += poly->pCoefficients[i];
//     }
//     uint32_t newSize = 0;
//     for (uint32_t i = 0; i <= maxPower; ++i)
//     {
//         if (combinedCoefficients[i]!=0)
//         {
//             newSize++;
//         }
//     }
//     uint32_t *newPowers = malloc(newSize * sizeof(uint32_t));
//     int32_t *newCoefficients = malloc(newSize * sizeof(int32_t));
//     if (!newPowers || !newCoefficients) {
//         // Handle allocation failure
//         free(combinedCoefficients);
//         return;
//     }
//     // Fill in the powers and coefficients of the simplified polynomial.
//     for (int i = maxPower, j = 0; i >= 0; --i) {
//         if (combinedCoefficients[i] != 0) {
//             newPowers[j] = i;
//             newCoefficients[j] = combinedCoefficients[i];
//             j++;
//         }
//     }
//     free (poly->pPowers);
//     free (poly->pCoefficients);
//     poly->pPowers = newPowers;
//     poly->pCoefficients = newCoefficients;
//     poly->size = newSize;
//     free(combinedCoefficients);
// }

// void calculate_derivative(const sPoly *poly, sPoly *derivative)
// {
//     derivative->size = poly->size - 1; // Degree of result polynomial might increase
//     derivative->pPowers = (uint32_t*)malloc(derivative->size * sizeof(uint32_t));
//     derivative->pCoefficients = (int32_t*)malloc(derivative->size * sizeof(int32_t));
//     for (uint32_t i = 0; i < poly->size; ++i) 
//     {
//         if (poly->pPowers[i] == 0) 
//         continue; // Constant term, derivative is 0 and skipped.
//         derivative->pPowers[i] = poly->pPowers[i] - 1;
//         derivative->pCoefficients[i] = poly->pCoefficients[i] * poly->pPowers[i];
//     }

// }
void calculate_derivative(const sPoly *poly, sPoly *derivative) {
    uint32_t nonZeroCount = 0;
    for (uint32_t i = 0; i < poly->size; ++i) {
        if (poly->pPowers[i] != 0) nonZeroCount++;
    }

    derivative->size = nonZeroCount;
    derivative->pPowers = (uint32_t*)calloc(nonZeroCount, sizeof(uint32_t));
    derivative->pCoefficients = (int32_t*)calloc(nonZeroCount, sizeof(int32_t));
    
    for (uint32_t i = 0, j = 0; i < poly->size; ++i) {
        if (poly->pPowers[i] != 0) {
            derivative->pPowers[j] = poly->pPowers[i] - 1;
            derivative->pCoefficients[j] = poly->pCoefficients[i] * poly->pPowers[i];
            j++;
        }
    }
}

void substitute_poly(const sPoly *original, const sPoly *substitute, sPoly *result) {
    // 初始化结果多项式的大小和存储空间
    result->size = 0;
    result->pPowers = calloc(1000, sizeof(uint32_t)); // 假定最多有1000个不同的次方项
    result->pCoefficients = calloc(1000, sizeof(int32_t)); // 分配对应的系数数组空间

    int tempResult[2000] = {0}; // 用于暂存计算结果的数组，大小根据需求调整
    int maxPower = 0;

    for (uint32_t i = 0; i < original->size; ++i) {
        sPoly tempPoly = {0}; // 存储临时的多项式结果
        tempPoly.pPowers = calloc(substitute->size, sizeof(uint32_t));
        tempPoly.pCoefficients = calloc(substitute->size, sizeof(int32_t));
        tempPoly.size = substitute->size;

        // 将 original 中的每一项与 substitute 相乘
        for (uint32_t j = 0; j < substitute->size; ++j) {
            tempPoly.pPowers[j] = substitute->pPowers[j] * original->pPowers[i];
            tempPoly.pCoefficients[j] = substitute->pCoefficients[j] * original->pCoefficients[i];
            if (tempPoly.pPowers[j] > maxPower) {
                maxPower = tempPoly.pPowers[j];
            }
        }

        // 将临时多项式的结果加到总结果中
        for (uint32_t k = 0; k < tempPoly.size; ++k) {
            tempResult[tempPoly.pPowers[k]] += tempPoly.pCoefficients[k];
        }

        free(tempPoly.pPowers);
        free(tempPoly.pCoefficients);
    }

    // 将 tempResult 中的非零项复制到 result 中
    for (int i = maxPower, idx = 0; i >= 0; --i) {
        if (tempResult[i] != 0) {
            result->pPowers[idx] = i;
            result->pCoefficients[idx] = tempResult[i];
            idx++;
            result->size++;
        }
    }
}
// void multiply_polynomials(const sPoly *poly1, const sPoly *poly2, sPoly *result)
// {
//     int maxPower = poly1->pPowers[0] + poly2->pPowers[0]; // Assuming powers are sorted in descending order
//     int *tempCoefficients = calloc(maxPower + 1, sizeof(int));

//     // result->size = poly1->size + poly2->size - 1; // Degree of result polynomial might increase
//     // result->pPowers = (uint32_t*)malloc(result->size * sizeof(uint32_t));
//     // result->pCoefficients = (int32_t*)malloc(result->size * sizeof(int32_t));
//     for (uint32_t i = 0; i < poly1->size; ++i) {
//         for (uint32_t j = 0; j < poly2->size; ++j) {
//             int power = poly1->pPowers[i] + poly2->pPowers[j];
//             int coefficient = poly1->pCoefficients[i] * poly2->pCoefficients[j];
//             tempCoefficients[power] += coefficient;
//         }
//     }
//     // for (uint32_t i = 0; i < poly1->size; ++i) 
//     // {
//     //     for (uint32_t j = 0; j < poly2->size; ++j) 
//     //     {
//     //         result->pPowers[i + j] = poly1->pPowers[i] + poly2->pPowers[j];
//     //         result->pCoefficients[i + j] = poly1->pCoefficients[i] * poly2->pCoefficients[j];
//     //     }
//     // }
//     // Count non-zero coefficients for result size
//     uint32_t nonZeroCount = 0;
//     for (int i = 0; i <= maxPower; ++i) {
//         if (tempCoefficients[i] != 0) {
//             ++nonZeroCount;
//         }
//     }
//     result->size = nonZeroCount;
//     result->pPowers = malloc(nonZeroCount * sizeof(uint32_t));
//     result->pCoefficients = malloc(nonZeroCount * sizeof(int32_t));

//     for (int i = maxPower, idx = 0; i >= 0; --i) {
//         if (tempCoefficients[i] != 0) {
//             result->pPowers[idx] = i;
//             result->pCoefficients[idx] = tempCoefficients[i];
//             ++idx;
//         }
//     }

//     free(tempCoefficients);

// }


void multiply_and_simplify_polynomials(const sPoly *p1, const sPoly *p2, sPoly *result) {
    // 临时存储结果的数组，假设最大次数不超过某个值，这里简化处理
    int tempResult[2000] = {0}; // 临时数组大小根据实际情况调整
    int maxPower = 0;

    // 执行多项式乘法
    for (uint32_t i = 0; i < p1->size; ++i) {
        for (uint32_t j = 0; j < p2->size; ++j) 
        {
            int power = p1->pPowers[i] + p2->pPowers[j];
            int coefficient = p1->pCoefficients[i] * p2->pCoefficients[j];
            tempResult[power] += coefficient; // 直接合并同类项
            if (power > maxPower) maxPower = power;
        }
    }

    // 统计结果中非零项的数量
    uint32_t nonZeroCount = 0;
    for (int i = maxPower; i >= 0; --i) {
        if (tempResult[i] != 0) ++nonZeroCount;
    }

    // 为结果分配内存
    result->size = nonZeroCount;
    result->pPowers = (uint32_t*)calloc(nonZeroCount, sizeof(uint32_t));
    result->pCoefficients = (int32_t*)calloc(nonZeroCount, sizeof(int32_t));

    // 填充结果
    for (int i = maxPower, idx = 0; i >= 0; --i) {
        if (tempResult[i] != 0) {
            result->pPowers[idx] = i;
            result->pCoefficients[idx] = tempResult[i];
            ++idx;
        }
    }
}
// int32_t chain_rule(sPoly *pResult, const sPoly *pFy, const sPoly *pFx)
// {
//     if (!pResult || !pFy || !pFx || pFy->size == 0 || pFx->size == 0) 
//     {
//         return -1;
//     }
//     sPoly dy = {0}, dx = {0};
//     calculate_derivative(pFy, &dy);
//     calculate_derivative(pFx, &dx);
//     // multiply_polynomials(&dy, &dx, pResult);
//     // simplify_poly(pResult);
//     multiply_and_simplify_polynomials(&dy, &dx, pResult);
//     free(dy.pPowers);
//     free(dy.pCoefficients);
//     free(dx.pPowers);
//     free(dx.pCoefficients);
//     return 0;
// }
int32_t chain_rule(sPoly *pResult, const sPoly *pFy, const sPoly *pFx) {
    // 验证输入的有效性
    if (!pResult || !pFy || !pFx || pFy->size == 0 || pFx->size == 0) {
        return -1;
    }

    // Step 1: 计算 f'(y)
    sPoly dy = {0};
    calculate_derivative(pFy, &dy);

    // Step 2: 将 f(x) 代入到 f'(y)
    // sPoly substituted_dy = {0}; // 用于存储代入后的多项式
    // substitute_poly(&dy, pFx, &substituted_dy);
    // Check if dy is essentially a constant function (i.e., all powers are 0)
    bool isConstant = true;
    for (uint32_t i = 0; i < dy.size; i++) {
        if (dy.pPowers[i] > 0) {
            isConstant = false;
            break;
        }
    }

    // Step 2: Substitute f(x) into f'(y) if dy is not constant
    sPoly substituted_dy = {0};
    if (!isConstant) {
        substitute_poly(&dy, pFx, &substituted_dy);
    } else {
        // If dy is constant, we simply copy dy to substituted_dy
        substituted_dy.size = dy.size;
        substituted_dy.pPowers = calloc(dy.size, sizeof(uint32_t));
        substituted_dy.pCoefficients = calloc(dy.size, sizeof(int32_t));
        for (uint32_t i = 0; i < dy.size; i++) {
            substituted_dy.pPowers[i] = dy.pPowers[i];
            substituted_dy.pCoefficients[i] = dy.pCoefficients[i];
        }
    }
    // Step 3: 计算 f'(x)
    sPoly dx = {0};
    calculate_derivative(pFx, &dx);

    // Step 4: 将代入后的 f'(f(x)) 与 f'(x) 相乘
    multiply_and_simplify_polynomials(&substituted_dy, &dx, pResult);

    // 清理分配的内存
    free(dy.pPowers); free(dy.pCoefficients);
    free(substituted_dy.pPowers); free(substituted_dy.pCoefficients);
    free(dx.pPowers); free(dx.pCoefficients);

    return 0; // 成功执行
}
