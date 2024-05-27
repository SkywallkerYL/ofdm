/*************************************************************************
	> File Name: Simulation.h
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2024年05月27日 星期一 14时26分21秒
 ************************************************************************/
#ifndef SIMULATION
#define SIMULATION
#include "common.h"
#include "myfixed.h"
#include "DelayCorr.h"
#include "LeaderSequence.h"
#include "CarrierSync.h"
#include "SymbolSync.h"
#include "genbits.h"
#include "ChannelEqualization.h"
#include "cpandwindow.h"
#include "fft.h"
#include "transmitter.h"
#include "pilot.h"
#include "Reciver.h"
#include<iostream>
using namespace std;
/****
 * 一个完整的仿真
 * 包括数据的发送和接收
 * 最后检验接收到的数据和发送的数据是否一致 计算误码率
 * 
 * 
*/
void simulation() {
	//发送端
	//生成随机比特流
	vector<int> bitstream = generate_random_bitstream(Num_data_subcarriers*4);
	//生成传输数据 
	vector<complex<DataType>>  transmittedData = transmitter(bitstream);
	//接收端
	vector<int> received = reciver(transmittedData);
	vector<complex<DataType>> symbol = map_bitstream_to_constellation(bitstream);
	//打印一下发送的数据
	for (int i = 0; i < symbol.size(); i++) {
		cout << "symbol:["<<i <<"] " << symbol[i] << endl;
	}
	//计算误码率
	int error = 0;
	for (int i = 0; i < bitstream .size(); i++) {
		cout << "bitstream" << bitstream[i] << " received:" << received[i] << endl;
		if (bitstream[i] != received[i]) {
			error++;
		}
	}	
	cout << "错误比特：" << error  << endl;
	return ;
}






#endif