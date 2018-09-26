#include "random.h"

float U() {
    return ((float)rand()/(float)(RAND_MAX));
}

float expf(const float L) {
    float res;
    res = (float)((-1.0 / L) * log(1.0 - U()));
    return res;
}

void Question1(int iterations, float L) {
    float e;
    float sum = 0;
    float mean, variance;
    float A[iterations];
    double expVariance, diffVariance;        

    for (int i = 0; i < iterations; i++) {
        e = expf(L);
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

    printf("<< Question 1 >>\n");
    printf(" Raw Mean: %f \n Expected Mean: %f \
    	\n Mean Difference: %f %% \n Raw Variance: %f \
    	\n Expected Variance: %f \n Variance Difference: %f %%\n", \
    	mean, (1.0)/L, fabs((mean-(1.0)/L)/(((1.0)/L)))*100, sum / (iterations - 1), expVariance, diffVariance);
    return;
}