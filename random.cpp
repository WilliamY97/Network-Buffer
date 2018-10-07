#include "random.h"

float U() {
    return ((float)rand()/(float)(RAND_MAX));
}

float expo(const float lambda)
{
    float exp_value;
    exp_value = (float)((-1.0 / lambda) * log(1.0 - U()));
    return exp_value;
}

void Question1(int iterations, float L) {
    float e;
    float sum = 0;
    float mean, variance;
    float A[iterations];
    double expVariance, diffVariance;

    for (int i = 0; i < iterations; i++) {
        e = expo(L);
        sum += e;
        A[i] = e;
    }

    int size_of_A = sizeof(A)/sizeof(A[0]);
    mean = sum / iterations;

    sum = 0;
    for (int i = 0; i < size_of_A; i++) {
        sum += pow(A[i]-mean, 2);
    }
    expVariance = (1.0)/pow(L, 2);
    diffVariance = fabs(((sum / (iterations - 1))-expVariance)/((expVariance)))*100;

    printf("\n<< QUESTION 1 START>>\n");
    printf("\nRaw Mean: %f \nExpected Mean: %f \
        \nMean Difference: %f %% \nRaw Variance: %f \
        \nExpected Variance: %f \nVariance Difference: %f %%\n", \
        mean, (1.0)/L, fabs((mean-(1.0)/L)/(((1.0)/L)))*100, sum / (iterations - 1), expVariance, diffVariance);
    printf("\n<< QUESTION 1 END>>\n");
    return;
}