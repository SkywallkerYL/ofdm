/*************************************************************************
	> File Name: fft.h
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2024年05月20日 星期一 19时14分34秒
 ************************************************************************/
#ifndef FFT
#define FFT
#include<iostream>
#include "common.h"
#include <complex>
#include "myfixed.h"
using namespace std;
//N 点FFT 实现 输入数据的类型为自定义

/*****
 * ifft 将信号从频域转换到时域
 * fft 将信号从时域转换到频域
 * 
 * ifft 的输入就是ofdm的符号 里的每一个分量，就是调制后的信号
 * 经过ifft后，就是时域的信号
 * 
 * s(t) =sum(0,N-1) di* exp(j2pi*fi*t)  f = 1*i/T	 
 * sk = s(kT/N) = sum(0,N-1) di * exp(j2pi*k*i/N)
 * di = sum(0,N-1) sk * exp(-j2pi*k*i/N)
 * 
 * fft的一个基础是多项式的系数和点值表示 F(x) = sum(0,N) a[k] * x^k
 * 系数表示 f(x) 写作 (a0,a1,a2,...,an)
 * 
 * 点值表示 f(x) 写作 {(x0,f(x0)),(x1,f(x1)),...,(xn,f(xn))}
 * 两个多项式相乘 f(x) * g(x)  
 * 点值表示的乘法就是点值相乘
 * tao(fg) = {(x0,f(x0)*g(x0)),(x1,f(x1)*g(x1)),...,(xn,f(xn)*g(xn))}
 * 点值表示的乘法可以以O(n)的时间复杂度完成
 * 
 * 如果可以在较低的时间内将系数表示转换为点值表示，再将点值表示转换为系数表示
 * 那么就可以在较低的时间内完成多项式的乘法
 * 
 * fft的核心就是将点值表示转换为系数表示
 * ifft的核心就是将系数表示转换为点值表示
 * 
 * ofdm最先是有了每个子载波的频域信号，然后通过ifft转换到时域
 * 这里就是相当于有了fx傅里叶级数的系数 ，然后把k个点带进去，得到f(x)的点值表示 即IDFT
 * 即 sk = sum(0,N-1) di * exp(j2pi*k*i/N)   di即是已知的  即得到了时域的N个点的数值
 * 
 * ifft/fft原理
 * 设x(n)为N点序列，X(k)为其N点DFT，x(n)的N点DFT定义为
 * X(k) = sum(0,N-1) x(n) * exp(-j2pi*k*n/N) = sum(0,N-1) x(n) * Wnk  W = exp(-j2pi/N)
 * 反变换IDFT为
 * x(n) = 1/N * sum(0,N-1) X(k) * exp(j2pi*k*n/N) = 1/N * sum(0,N-1) X(k) * W(-nk)
 * 
 * idft的实现只要先X(k)的复共轭，然后做dft,最后再除以N即可，再做一次共轭。
 * 所以idft和dft的实现是一样的
 * 
 * fft的实现主要是通过蝶形运算，将N点DFT分解为N/2点DFT，然后再分解为N/4点DFT，直到分解为2点DFT
 * 可以看看https://blog.csdn.net/m0_38139533/article/details/100942095 讲得还是蛮清楚。
 * 
*/

/***
 * 通过蝶形运算法 按时间抽取的方式
 * 其实就是把输入序列按奇数和偶数拆分
 * 实现的fft
 * flag 是1的时候是fft，0的时候是ifft
 * 这里有个反序有点难理解，可以看一下https://zhuanlan.zhihu.com/p/135259438
*/
vector<complex<DataType>> fft_dit(vector<complex<DataType>> x,bool flag = 1) {
	int N = x.size();
	//做ifft时，先将X取共轭
	if(flag == 0){
		for(int i = 0; i < N; i++){
			x[i].imag(-x[i].imag());
		}
	}


	//反序代码  
	//第0个数和第N-1个数不需要倒序
	//第一个反序数是N/2
	int J = N/2; 
	for (int i = 1; i < N - 1; i++) {
		if(i < J) {
			complex<DataType> temp = x[i];
			x[i] = x[J];
			x[J] = temp;
		}
		//求取下一个反序数
		//反序数的二进制表示是正序数的二进制表示的逆序
		//反序数的进位是从左往右进位
		//因此可以J从N/2开始，每次加1，然后将J的二进制表示逆序
		//求下一个反序数的操作其实就是J+1,但是进位的理解不一样
		//所以这里的K= N/2其实就是最低位的1
		//二进制数逢1进1变0,逢0把0变1
		/**
		 * 我们可以从最高位开始对反序数J进行判断，如果是0，
		 * 则加上该位的权值，得到下一个反序数J。如果最高位为1，则将其1变成0，
		 * 然后从左到右判断下一位是0或1。
		 * 不停重复上述操作，直到遇到某一位判断为0，并把0变1，得到下一个反序数J。
		 * 例如：反序数11101110，我们从左到右给他加1，
		 * 它开始逢1向右进1变0，直到遇到0停止，并把0变1。结果为：00011110。
		*/
		int k = N/2;
		while (k <= J) {
			//这一位是1
			J = J - k;
			k = k/2;
		}
		//把0变1
		J = J + k;
	}
	//存储蝶形运算的结果
	vector<complex<DataType>> X;
	X.resize(N);
	//将x复制到X
	for(int i = 0; i < N; i++){
		X[i] = x[i];
	}
	//根据N计算出蝶形运算的级数 
	int M = log2(N); 
	//蝶形运算 从第一级到第M级 从左到右
	for(int L = 1; L <= M ; L ++ ){
		//第L级蝶形运算 
		//首先计算该级蝶形运算递增数间隔
		int B = 1 << (L-1);
		//旋转因子的种类数
		int P = 1 << (L-1);
		//同种蝶形运算的间隔
		int Q = 1 << (L);
		//依次计算同种蝶形运算
		for(int j = 0; j < P; j++){
			//计算同种蝶形运算的第j个蝶形运算
			//计算旋转因子 Wpn = exp(-j2pi p/n)
			// p = 0,1,2,...,P-1
			// n = 2^L
			// 所以声明一个复数类型的旋转因子
			int p = j;
			int n = 1 << L;	
			//实数部分是cos(2pi p/n) 虚数部分是-sin(2pi p/n)
			complex<DataType> W = complex<DataType>(cos(2*PI*p/n),-sin(2*PI*p/n));
			//计算该种蝶形运算
			for(int k = 0; k < N; k += Q){
				//数组下标
				int index = k + j;
				complex<DataType> temp = X[index];
				X[index] = X[index] + X[index + B]*W;
				X[index + B] = temp - X[index + B]*W;
			}
		}

	}
	//做ifft时，最后的结果需要再除以N 并且再取共轭
	if(flag == 0){
		for(int i = 0; i < N; i++){
			DataType n = N;
			X[i].real(X[i].real()/n);
			X[i].imag(-X[i].imag()/n);
//			X[i].imag(-X[i].imag());
		}
	}
	return X;
}


/*****
 * 按频率抽取的方式 就是把输入按前后拆分
 * 可以参考https://blog.csdn.net/qq_42604176/article/details/105567915
 * 硬件实现可以参考https://zhuanlan.zhihu.com/p/471436911
 * dif的蝶形更适合给硬件实现
*/
vector<complex<DataType>> fft_dif(vector<complex<DataType>> x,bool flag = 1) {
	int N = x.size();
	//做ifft时，先将X取共轭
	if(flag == 0){
		for(int i = 0; i < N; i++){
			x[i].imag(-x[i].imag());
		}
	}
	//存储蝶形运算的结果
	vector<complex<DataType>> X;
	X.resize(N);
	//将x复制到X
	for(int i = 0; i < N; i++){
		X[i] = x[i];
	}
	//根据N计算出蝶形运算的级数 
	int M = log2(N); 
	//蝶形运算 从第一级到第M级 从左到右
	for(int l = 1; l <= M ; l ++ ){
		int L = M - l + 1;
		//第L级蝶形运算 
		//首先计算该级蝶形运算递增数间隔
		int B = 1 << (L-1);
		//旋转因子的种类数
		int P = 1 << (L-1);
		//同种蝶形运算的间隔
		int Q = 1 << (L);
		//依次计算同种蝶形运算
		for(int j = 0; j < P; j++){
			//计算同种蝶形运算的第j个蝶形运算
			//计算旋转因子 Wpn = exp(-j2pi p/n)
			// p = 0,1,2,...,P-1
			// n = 2^L
			// 所以声明一个复数类型的旋转因子
			int p = j;
			int n = 1 << L;	
			//实数部分是cos(2pi p/n) 虚数部分是-sin(2pi p/n)
			complex<DataType> W = complex<DataType>(cos(2*PI*p/n),-sin(2*PI*p/n));
			//计算该种蝶形运算
			for(int k = 0; k < N; k += Q){
				//数组下标
				int index = k  + j;
				complex<DataType> temp = X[index];
				X[index] = X[index] + X[index + B];
				X[index + B] = (temp - X[index + B])*W;
			}
		}
	}
	//将地址按照二进制反序
	//反序代码 原理和前面是一样的
	int J = N/2; 
	for (int i = 1; i < N - 1; i++) {
		if(i < J) {
			complex<DataType> temp = X[i];
			X[i] = X[J];
			X[J] = temp;
		}
		int k = N/2;
		while (k <= J) {
			//这一位是1
			J = J - k;
			k = k/2;
		}
		//把0变1
		J = J + k;
	}

	//做ifft时，最后的结果需要再除以N 并且再取共轭
	if(flag == 0){
		for(int i = 0; i < N; i++){
			DataType n = N;
			X[i].real(X[i].real()/n);
			X[i].imag(-X[i].imag()/n);
		}
	}
	return X;
}

/***
 * 一个测试函数将两种fft的结果进行比较
 * 进行ifft后再与原来的数据进行比较
 * 并且与原来的数据进行比较
 * 
 * 这个结果与matlab的结果是一样的
*/
void test_fft(){
	vector<complex<DataType>> x;
	x.resize(Num_FFT);
	int N = x.size();
	for(int i = 0; i < N; i++){
		x[i] = complex<DataType>(i,i);
	}
	vector<complex<DataType>> X1 = fft_dit(x);
	vector<complex<DataType>> X2 = fft_dif(x);
	DataType tolerance = 10; 
	for(int i = 0; i < N; i++){
		//assert(abs(X1[i].real() - X2[i].real()) < tolerance);
		//assert(abs(X1[i].imag() - X2[i].imag()) < tolerance);
		//if(abs(X1[i].real() - X2[i].real()) > tolerance){
		//	cout << X1[i].real() << " " << X2[i].real() << endl;
		//	cout << "error" << endl;
		//}
		
		//cout << X1[i] << " " << X2[i] << endl;
		
		//assert(X1[i].real() == X2[i].real());
	}
	//再做个逆变换恢复元数据
	vector<complex<DataType>> x1 = fft_dit(X1,0);
	vector<complex<DataType>> x2 = fft_dif(X2,0);
	for(int i = 0; i < N; i++){
		//cout << x1[i] << " " << x2[i] << endl;
		assert(abs(x1[i].real() - x2[i].real()) < tolerance);
		assert(abs(x1[i].imag() - x2[i].imag()) < tolerance);
		assert(abs(x1[i].real() - x[i].real()) < tolerance);
		assert(abs(x1[i].imag() - x[i].imag()) < tolerance);
	}
}


#endif