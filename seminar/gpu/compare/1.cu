#include <cuda_runtime.h>
#include <stdio.h>

#define N 256

__global__ void matrix_vector_multi_gpu_1_1(float *A_d, float *B_d, float *C_d)
{
	int i,j;

	for(j=0;j<N;j++){
		A_d[j] = 0.0F;
		for(i=0;i<N;i++) {
		 A_d[j] = A_d[j]+B_d[j*N+i]*C_d[i];
		 }
	}
}

__global__ void matrix_vector_multi_gpu_1_1_sh(float *A_d, float *B_d, float *C_d)
{
	int i;

	__shared__ float tmp_c[N];

	tmp_c[threadIdx.x] = C_d[threadIdx.x];
	__syncthreads();

	A_d[threadIdx.x] = 0.0F;

	for(i=0;i<N;i++){
		A_d[threadIdx.x] = A_d[threadIdx.x] + B_d[threadIdx.x*N+1]*tmp_c[i];
	}
}

int main(int argc, char **argv)
{
    // set up device
    int dev = 0;

    unsigned int t, travdirtime; 

    int i,j;
    float A[N], B[N*N], C[N];
    float *A_d, *B_d, *C_d;

    dim3 blocks(1,1,1);
    dim3 threads(1,1,1);

    for(j=0;j<N;j++) {
        for(i=0;i<N;i++) {
	   B[j*N+i] = ((float)j)/256.0;
	   }
    }	   

    for(j=0;j<N;j++)
	C[j] = 1.0F;
	
    cudaMalloc((void **)&A_d, N*sizeof(float));
    cudaMalloc((void **)&B_d, N*N*sizeof(float));
    cudaMalloc((void **)&C_d, N*sizeof(float));

    cudaMemcpy(A_d, A, N*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(B_d, B, N*N*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(C_d, C, N*sizeof(float), cudaMemcpyHostToDevice);

    matrix_vector_multi_gpu_1_1<<<blocks, threads>>>(A_d, B_d, C_d);
    matrix_vector_multi_gpu_1_1_sh<<<blocks, threads>>>(A_d, B_d, C_d);

    cudaFree(A_d);
    cudaFree(B_d);
    cudaFree(C_d);

    return EXIT_SUCCESS;
}
