#Makefile for Round with AVX

avx: avxround.cc
	nvcc -Xcompiler -O3,-g,-march=native,-mtune=native,-Wall,-mavx512f,-funroll-loops,-finline,-fomit-frame-pointer -o avxround avxround.cc

clean:
	rm -f avxround

