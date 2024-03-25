#include "mychain.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
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

void copy_poly(sPoly *destination, const sPoly *source) {
    // 首先释放目标多项式的现有内存（如果有的话）
    free(destination->pPowers);
    free(destination->pCoefficients);

    // 为目标多项式的幂次和系数数组分配新的内存
    destination->size = source->size;
    destination->pPowers = (uint32_t*)calloc(source->size, sizeof(uint32_t));
    destination->pCoefficients = (int32_t*)calloc(source->size, sizeof(int32_t));

    // 复制幂次和系数
    for (uint32_t i = 0; i < source->size; ++i) {
        destination->pPowers[i] = source->pPowers[i];
        destination->pCoefficients[i] = source->pCoefficients[i];
    }
}

void free_poly(sPoly *poly) {
    // 释放幂次和系数数组的内存
    free(poly->pPowers);
    free(poly->pCoefficients);

    // 将指针设为NULL，避免野指针问题
    poly->pPowers = NULL;
    poly->pCoefficients = NULL;
    poly->size = 0;
}
void sort_poly(sPoly *poly) {
    // 使用简单的冒泡排序算法进行排序，对于大规模数据不推荐使用此方法
    // 推荐改用快速排序或归并排序等更高效的算法
    for (uint32_t i = 0; i < poly->size - 1; ++i) {
        for (uint32_t j = 0; j < poly->size - i - 1; ++j) {
            if (poly->pPowers[j] < poly->pPowers[j + 1]) {
                // 交换幂次
                uint32_t tempPower = poly->pPowers[j];
                poly->pPowers[j] = poly->pPowers[j + 1];
                poly->pPowers[j + 1] = tempPower;
                // 交换系数
                int32_t tempCoefficient = poly->pCoefficients[j];
                poly->pCoefficients[j] = poly->pCoefficients[j + 1];
                poly->pCoefficients[j + 1] = tempCoefficient;
            }
        }
    }
}

void multiply_and_simplify_polynomials(const sPoly *p1, const sPoly *p2, sPoly *result) {
    // 初始化结果多项式
    result->size = 0;
    result->pPowers = NULL;
    result->pCoefficients = NULL;

    // 临时多项式用于存储乘法结果
    sPoly tempResult;
    tempResult.size = 0;
    tempResult.pPowers = (uint32_t*)calloc((p1->size * p2->size), sizeof(uint32_t));
    tempResult.pCoefficients = (int32_t*)calloc((p1->size * p2->size), sizeof(int32_t));

    // 执行多项式乘法
    for (uint32_t i = 0; i < p1->size; ++i) {
        for (uint32_t j = 0; j < p2->size; ++j) {
            // 计算新的幂次和系数
            uint32_t newPower = p1->pPowers[i] + p2->pPowers[j];
            int32_t newCoefficient = p1->pCoefficients[i] * p2->pCoefficients[j];
            
            // 查找是否已存在此幂次
            bool found = false;
            for (uint32_t k = 0; k < tempResult.size && !found; ++k) {
                if (tempResult.pPowers[k] == newPower) {
                    tempResult.pCoefficients[k] += newCoefficient;
                    found = true;
                }
            }
            // 如果不存在此幂次，添加新项
            if (!found) {
                tempResult.pPowers[tempResult.size] = newPower;
                tempResult.pCoefficients[tempResult.size] = newCoefficient;
                tempResult.size++;
            }
        }
    }

    copy_poly(result, &tempResult);
    free_poly(&tempResult);
    sort_poly(result);
}
void substitute_poly(const sPoly *original, const sPoly *substitute, sPoly *result) {
    // 清空result
    result->size = 0;

    sPoly tempResult = {0}; // 用作临时结果
    sPoly tempPoly = {0};   // 用于存储每次乘方的结果
    sPoly multipliedResult = {0}; // 用于累计乘法结果

    // 初始化临时结果为常数1，方便开始乘法过程
    tempResult.size = 1;
    tempResult.pPowers = (uint32_t*)calloc(1, sizeof(uint32_t));
    tempResult.pCoefficients = (int32_t*)calloc(1, sizeof(int32_t));
    tempResult.pPowers[0] = 0;
    tempResult.pCoefficients[0] = 1;

    for (uint32_t i = 0; i < original->size; ++i) {
        // 每一项先复制substitute
        copy_poly(&tempPoly, substitute);
        // 然后进行乘方运算，次数为original的幂次减1
        for (uint32_t pow = 1; pow < original->pPowers[i]; ++pow) {
            multiply_and_simplify_polynomials(&tempPoly, substitute, &multipliedResult);
            // 清理tempPoly并更新为最新结果
            free_poly(&tempPoly);
            copy_poly(&tempPoly, &multipliedResult);
            free_poly(&multipliedResult);
        }
        // 乘以系数
        for (uint32_t j = 0; j < tempPoly.size; ++j) {
            tempPoly.pCoefficients[j] *= original->pCoefficients[i];
        }
        // 将乘方后的结果乘到tempResult上
        multiply_and_simplify_polynomials(&tempResult, &tempPoly, &multipliedResult);
        free_poly(&tempResult);
        copy_poly(&tempResult, &multipliedResult);
        free_poly(&multipliedResult);
        free_poly(&tempPoly);
    }

    // 将最终结果复制到result
    sort_poly(&tempResult);
    copy_poly(result, &tempResult);
    free_poly(&tempResult);
}

int32_t chain_rule(sPoly *pResult, const sPoly *pFy, const sPoly *pFx) {
    // 验证输入的有效性
    if (!pResult || !pFy || !pFx || pFy->size == 0 || pFx->size == 0) {
        return -1;
    }

    sPoly dy = {0}, dx = {0}, substituted_dy = {0};

    // Step 1: 计算 f'(y)
    calculate_derivative(pFy, &dy);

    // Step 2: 如果 dy 非常数，将 f(x) 代入 f'(y)
    bool isConstant = dy.size == 1 && dy.pPowers[0] == 0;
    if (!isConstant) {
        substitute_poly(&dy, pFx, &substituted_dy);
    } else {
        // 如果 dy 是常数，直接复制
        copy_poly(&substituted_dy, &dy);
    }

    // Step 3: 计算 f'(x)
    calculate_derivative(pFx, &dx);

    // Step 4: 将代入后的 f'(f(x)) 与 f'(x) 相乘
    multiply_and_simplify_polynomials(&substituted_dy, &dx, pResult);

    // 清理分配的内存
    free_poly(&dy);
    free_poly(&dx);
    free_poly(&substituted_dy);

    return 0; // 成功执行
}
