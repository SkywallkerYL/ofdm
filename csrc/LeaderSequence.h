/*************************************************************************
	> File Name: LeaderSequence.h
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2024年05月24日 星期五 13时05分36秒
 ************************************************************************/
#ifndef LEADERSEQUENCE
#define LEADERSEQUENCE
#include<iostream>
#include "common.h"
#include "myfixed.h"
#include "fft.h"
using namespace std;
#include <complex>
#include <vector>
#include <cmath>
#include <algorithm>

vector<complex<DataType>> generateLeaderSequence() {
    vector<complex<DataType>> S = {
    complex<DataType>(0, 0), complex<DataType>(0, 0), complex<DataType>(0, 0), complex<DataType>(0, 0),
    complex<DataType>(0, 0), complex<DataType>(0, 0), complex<DataType>(0, 0), complex<DataType>(0, 0),
    complex<DataType>(1, 1), complex<DataType>(0, 0), complex<DataType>(0, 0), complex<DataType>(0, 0),
    complex<DataType>(-1, -1), complex<DataType>(0, 0), complex<DataType>(0, 0), complex<DataType>(0, 0),
    complex<DataType>(1, 1), complex<DataType>(0, 0), complex<DataType>(0, 0), complex<DataType>(0, 0),
    complex<DataType>(-1, -1), complex<DataType>(0, 0), complex<DataType>(0, 0), complex<DataType>(0, 0),
    complex<DataType>(-1, -1), complex<DataType>(0, 0), complex<DataType>(0, 0), complex<DataType>(0, 0),
    complex<DataType>(1, 1), complex<DataType>(0, 0), complex<DataType>(0, 0), complex<DataType>(0, 0),
    complex<DataType>(0, 0), complex<DataType>(0, 0), complex<DataType>(0, 0), complex<DataType>(0, 0),
    complex<DataType>(-1, -1), complex<DataType>(0, 0), complex<DataType>(0, 0), complex<DataType>(0, 0),
    complex<DataType>(-1, -1), complex<DataType>(0, 0), complex<DataType>(0, 0), complex<DataType>(0, 0),
    complex<DataType>(1, 1), complex<DataType>(0, 0), complex<DataType>(0, 0), complex<DataType>(0, 0),
    complex<DataType>(1, 1), complex<DataType>(0, 0), complex<DataType>(0, 0), complex<DataType>(0, 0),
    complex<DataType>(1, 1), complex<DataType>(0, 0), complex<DataType>(0, 0), complex<DataType>(0, 0),
    complex<DataType>(1, 1), complex<DataType>(0, 0), complex<DataType>(0, 0), complex<DataType>(0, 0),
    complex<DataType>(0, 0), complex<DataType>(0, 0), complex<DataType>(0, 0), complex<DataType>(0, 0)
	};
	vector<complex<DataType>> L = LongTrainSeq;

    // Ensure the power of OFDM symbols is stable
    for(auto& s : S) {
        s *= sqrt(13.0 / 6.0);
    }

    // 64-point inverse Fourier transform, into the time domain
    // !!! Adjust the frequency point order of the STS frequency domain through the IFFT function to positive frequency (0~31) and negative frequency (-32~-1)
    // In C++, you need to use a library like FFTW to perform FFT and IFFT. Here we assume that the function fftshift_ifft exists.
    //首先进行循环移位
	vector<complex<DataType>> S_shifted = S;
	for(int i = 0; i < 32; ++i) {
		S_shifted[i] = S[32 + i];
		S_shifted[32 + i] = S[i];
	}
	S = S_shifted;
	// 64-point inverse Fourier transform, into the time domain

	vector<complex<DataType>> short_cp = fft_dif(S, 0);

    // Take the first 1~16 points, you can verify that the values of 17~32, 33~48, 49~64 are all copies of 1~16
    short_cp = vector<complex<DataType>>(short_cp.begin(), short_cp.begin() + 16);

    vector<complex<DataType>> short_str = short_cp;

    // Generate 160 data
    for(int f = 0; f < 9; ++f) {
        short_str.insert(short_str.end(), short_cp.begin(), short_cp.end());
    }

    // Long training sequence generation
    // !!! Adjust the frequency point order of the LTS frequency domain through the IFFT function to positive frequency (0~31) and negative frequency (-32~-1)
    //首先进行循环移位
	vector<complex<DataType>> L_shifted = L;
	for(int i = 0; i < 32; ++i) {
		L_shifted[i] = L[32 + i];
		L_shifted[32 + i] = L[i];
	}
	L = L_shifted;
	
	vector<complex<DataType>> long_cp = fft_dif(L, 0);

    // The last 32 data
    vector<complex<DataType>> long1(long_cp.begin() + 32, long_cp.end());
    vector<complex<DataType>> long2 = long_cp;
    vector<complex<DataType>> long_str;
    long_str.insert(long_str.end(), long1.begin(), long1.end());
    long_str.insert(long_str.end(), long2.begin(), long2.end());
    long_str.insert(long_str.end(), long2.begin(), long2.end());

    vector<complex<DataType>> preamble;
    preamble.insert(preamble.end(), short_str.begin(), short_str.end());
    preamble.insert(preamble.end(), long_str.begin(), long_str.end());

    // Window processing for the first and last, the performance on the hardware is shifted to the right by one bit
    // The 161st data window processing
    preamble[160] = preamble[160] * 0.5 + preamble[0] * 0.5;
    // The first data window processing
    preamble[0] = preamble[0] * 0.5;

    // Short + long
    vector<complex<DataType>> LeaderSequence = preamble;

    return LeaderSequence;
}



#endif

