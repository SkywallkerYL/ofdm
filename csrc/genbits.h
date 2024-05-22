/*************************************************************************
	> File Name: genbits.h
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2024年05月20日 星期一 15时53分16秒
 ************************************************************************/
#ifndef GENBITS
#define GENBITS
#include "common.h"
#include<iostream>
using namespace std;

vector<int> generate_random_bitstream(int length = Num_subcarriers) {
	vector<int> bitstream;
	bitstream.resize(length);
	for (int i = 0; i < length; i++) {
#if Send_All_Zero
		bitstream[i] = 0;
#else
		bitstream[i] = rand() % 2;
#endif
	}
	return bitstream;    
}



#endif