#include <bits/stdc++.h> //Std Libraries
#include <immintrin.h> //AVX Intrinsics

const int BLOCK = 16;
const int N = 6400100;
const int V = N/BLOCK;

__attribute__((optimize("no-tree-vectorize"))) //disable auto-vectorization
//Normal version of add/round function
inline float *normal_add_round(float *A,float *B)
{
	float *C = (float *)malloc(N*sizeof(float)); 
	for (int i = 0; i < N; ++i) C[i] = round(A[i] + B[i]);
	return C;
}

//AVX version of add/round function
inline void avx_add_round(float *A,float *B,float *R,size_t size)
{
	size_t i = 0;
	
	//For AVX-512
	for(; i < (size & ~0x7); i += 16 )
	{
		//load values into avx variables
		const __m512 avx_a8 = _mm512_loadu_ps(&A[i]);	
		const __m512 avx_b8 = _mm512_loadu_ps(&B[i]);	

		//perform add/round operations
		const __m512 avx_add8 = _mm512_add_ps(avx_a8,avx_b8);
		const __m512 avx_res8 = _mm512_roundscale_ps(avx_add8,_MM_FROUND_TO_NEAREST_INT);

		//store in result
		_mm512_storeu_ps(&R[i],avx_res8);
	}

	//Serial case handles leftovers
	for(; i < size; i++)
	{
		R[i] = round(A[i] + B[i]);
	}
}

using namespace std;
using namespace std::chrono;

high_resolution_clock::time_point now = high_resolution_clock::now();
#define TIME duration_cast<duration<double>>(high_resolution_clock::now() - now).count()

int main()
{
	float *A = (float *)malloc(N*sizeof(float));
	float *B = (float *)malloc(N*sizeof(float)); 
	float *avx_result = (float *)malloc(N*sizeof(float)); 
	float *norm_result = NULL; 
        
	//init data
	for (int i = 0; i < N; ++i) 
        { 
		A[i] = ((float)i) + 0.1335f; 
		B[i] = ((float)i) / 133.5f; 
        }
	
	//start timing and calculate normal
    now = high_resolution_clock::now();
    for (int i = 0; i < 20; ++i) { norm_result = normal_add_round(A,B); }
 	double linear_time = TIME;
   	cerr << "Normal round : "<< linear_time << endl;

	//start timing and calculate AVX
	now = high_resolution_clock::now();
	for (int i = 0; i < 20; ++i){ avx_add_round(A,B,avx_result,N); }
	double avx_time = TIME;
 	cerr << "AVX round : "<<avx_time << endl;

	//Verify Correctness
	for (int i = 0; i < N; ++i) {
  	       if (abs(norm_result[i] - avx_result[i]) > 1.0f)
		   {
	         cerr << "ERROR: AVX round is not the same as linear!!!";
	         cerr << norm_result[i]<<" <-> "<<avx_result[i]<<endl;
			 return -1;
		   }
	}
	cout << "CG> message -channel \"exercise results\" Linear to AVX improvement : "<< (linear_time/avx_time*100)<<"%" << endl;
 

	free(A);
	free(B);
	free(norm_result);
	free(avx_result);

	return 0;
}
