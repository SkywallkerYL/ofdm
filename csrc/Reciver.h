/*************************************************************************
	> File Name: Reciver.h
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2024年05月27日 星期一 15时03分26秒
 ************************************************************************/
#ifndef RECIVER
#define RECIVER
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
#include "pilot.h"
#include "FrequencySync.h"
#include "PhaseTracking.h"
#include "bit2constellation.h"
#include<iostream>
using namespace std;


/****
 * 数据接收端
 * 主要实现了接收端的流程
 * 1. 分组检测 
 * 2. 载波同步
 * 3. 符号同步 
 * 4. 去除循环前缀
 * 5. 过一边fft 
 * 6. 频率同步 
 * 7. 剩余相位补偿
 * 8. 信道估计和均衡
 * 9. 解调 
 * 返回解调后的数据
*/
vector<int> reciver(vector<complex<DataType>> receivedData) {
	//分组检测
	int start = 0;
	vector<complex<DataType>> data = delay_corr_Group_Detection(receivedData,start);
	//载波同步
	//进行频偏估计 
	//分离5个短训练序列
	vector<complex<DataType>> shortTrainingSequence = getShortTrainingSequence(data);
	//进行粗频率估计
	DataType CFOEstimate = CFOEstimating(shortTrainingSequence);
	cout << "CFOEstimate:" << CFOEstimate << endl;
	//对所有数据进行频率偏移补偿
	vector<complex<DataType>> data_compensation = CFOCompensation(data, CFOEstimate);
	//提取10个短训练序列
	vector<complex<DataType>> shortTrainingSequenceData = getShortTrainingSequence(data_compensation);
	vector<complex<DataType>> shortTrainingSequenceData1 = getShortTrainingSequenceRest(data_compensation);
	//两个拼接起来
	vector<complex<DataType>> shortTrainingSequenceData2;
	shortTrainingSequenceData2.resize(shortTrainingSequenceData.size() + shortTrainingSequenceData1.size());
	for (int i = 0; i < shortTrainingSequenceData.size(); i++) {
		shortTrainingSequenceData2[i] = shortTrainingSequenceData[i];
	}
	for (int i = 0; i < shortTrainingSequenceData1.size(); i++) {
		shortTrainingSequenceData2[i + shortTrainingSequenceData.size()] = shortTrainingSequenceData1[i];
	}
	//找到短训练序列的结束点
	int peak = matchFilter(shortTrainingSequenceData2);
	cout << "peak:" << peak << endl;
	//从结束点后，取出长训练序列和数据
	vector<complex<DataType>> longTrainingSequenceAndData;
	longTrainingSequenceAndData.resize(data_compensation.size()-peak-1);
	cout << "longTrainingSequenceAndData.size():" << longTrainingSequenceAndData.size() << endl;
	//取出长训练序列和数据
	for (int i = 0; i < longTrainingSequenceAndData.size(); i++) {
		longTrainingSequenceAndData[i] = data_compensation[i+peak+1];
	}
	//取出长训练序列   
	vector<complex<DataType>> longTrainingSequence;
	longTrainingSequence.resize(Num_Long_Train_Symbols);
	for (int i = 0; i < Num_Long_Train_Symbols; i++) {
		longTrainingSequence[i] = longTrainingSequenceAndData[i];
	}
	//取出数据
	vector<complex<DataType>> DataFeild;
	DataFeild.resize(longTrainingSequenceAndData.size()-Num_Long_Train_Symbols);
	for (int i = 0; i < DataFeild.size(); i++) {
		DataFeild[i] = longTrainingSequenceAndData[i+Num_Long_Train_Symbols];
	}
	//去除CP  每80个一组，去除前16个
	//计算一下数据的个数 
	//这里要注意一下，实际通讯的时候是知道数据的个数的，所以这里就简单的计算一下
	//并且截取的时候是会多一些噪声在尾部
	int Num_Data = DataFeild.size()/Num_OFDM;
	vector<complex<DataType>> DataFeildNoCP;
	if(DataFeild.size()%Num_OFDM != 0){
		Num_Data = Num_Data - 1;
	}
	cout << "Num_Data:" << Num_Data << endl;
	DataFeildNoCP.resize(Num_Data*Num_FFT);
	for (int i = 0; i < Num_Data; i++) {
		for (int j = 0; j < Num_FFT; j++) {
			DataFeildNoCP[i*Num_FFT+j] = DataFeild[i*Num_OFDM+j+Num_CP];
		}
	}
	//打印一下
	for (int i = 0; i < Num_Data*Num_FFT; i++) {
		cout << "DataFeildNoCP[" << i << "]:" << DataFeildNoCP[i] << endl;
	}
	//数据进行fft 
	vector<complex<DataType>> DataFeildNoCPGroup;
	DataFeildNoCPGroup.resize(Num_Data*Num_FFT);
	for (int i = 0; i < Num_Data; i++) {
		vector<complex<DataType>> DataFeildNoCPGroupTemp;
		DataFeildNoCPGroupTemp.resize(Num_FFT);
		for (int j = 0; j < Num_FFT; j++) {
			DataFeildNoCPGroupTemp[j] = DataFeildNoCP[i*Num_FFT+j];
		}
		//进行fft
		vector<complex<DataType>> FreData = fft_dif(DataFeildNoCPGroupTemp);
		//打印一下
		for (int j = 0; j < Num_FFT; j++) {
			cout << "FreData[" << j << "]:" << FreData[j] << endl;
		}
		//保存
		for (int j = 0; j < Num_FFT; j++) {
			DataFeildNoCPGroup[i*Num_FFT+j] = FreData[j];
		}
	}
	// 对数据进行频率同步 和剩余相位补偿
	vector<complex<DataType>> DataFeildNoCPGroupCompensation;
	DataFeildNoCPGroupCompensation.resize(Num_Data*Num_FFT);
	//对每一组数据进行频率同步
	for (int i = 0; i < Num_Data; i++) {
		//取出一组数据
		vector<complex<DataType>> DataFeildNoCPGroupTemp;
		DataFeildNoCPGroupTemp.resize(Num_FFT);
		for (int j = 0; j < Num_FFT; j++) {
			DataFeildNoCPGroupTemp[j] = DataFeildNoCPGroup[i*Num_FFT+j];
		}
		//进行频率同步
		//这里这两个信号暂时还不理解他们的作用，所以暂时没用上
		int timectrl,timevalid;
		vector<complex<DataType>> DataFeildNoCPGroupCompensationTemp = frequency_sync(DataFeildNoCPGroupTemp,timectrl,timevalid);
		//剩余相位补偿
		vector<complex<DataType>> DataFeildNoCPGroupCompensationTemp1 = PhaseTracking(DataFeildNoCPGroupCompensationTemp);
		//保存
		for (int j = 0; j < Num_FFT; j++) {
			DataFeildNoCPGroupCompensation[i*Num_FFT+j] = DataFeildNoCPGroupCompensationTemp1[j];
		}
	}
	//将数据和长训练序列送入信道估计和均衡模块 
	vector<complex<DataType>> DataFeildNoCPGroupCompensationAndLongTrainingSequence;
	DataFeildNoCPGroupCompensationAndLongTrainingSequence.resize(Num_Data*Num_FFT+Num_Long_Train_Symbols);
	//先放入长训练序列
	for (int i = 0; i < Num_Long_Train_Symbols; i++) {
		DataFeildNoCPGroupCompensationAndLongTrainingSequence[i] = longTrainingSequence[i];
	}
	//放入数据
	for (int i = 0; i < Num_Data*Num_FFT; i++) {
		DataFeildNoCPGroupCompensationAndLongTrainingSequence[i+Num_Long_Train_Symbols] = DataFeildNoCPGroupCompensation[i];
	}
	//信道估计和均衡
	//存储能量
	vector<complex<DataType>> ERlts;
	ERlts.resize(Num_FFT);
	vector<complex<DataType>> DataEqualization = channelEqualization(DataFeildNoCPGroupCompensationAndLongTrainingSequence,ERlts);
	//把数据 和能量移位回来 
	//移位后的能量
	vector<complex<DataType>> EShift = shift_remove_pilot(ERlts,0);
	vector<DataType> Ereal;
	Ereal.resize(Num_data_subcarriers);
	for (int i = 0; i < Num_data_subcarriers; i++) {
		Ereal[i] = EShift[i].real();
		cout << "ERlts[" << i << "]:" << Ereal[i] << endl;
	}

	vector<complex<DataType>> DataEqualizationShift;
	DataEqualizationShift.resize(Num_Data*Num_data_subcarriers);
	//打印
	for (int i = 0; i < Num_Data*Num_FFT; i++) {
		cout << "DataEqualization[" << i << "]:" << DataEqualization[i] << endl;
	}
	for (int i = 0; i < Num_Data; i++) {
		//64个里面取48个
		vector<complex<DataType>> DataEqualizationTemp;
		DataEqualizationTemp.resize(Num_FFT);
		for (int j = 0; j < Num_FFT; j++) {
			DataEqualizationTemp[j] = DataEqualization[i*Num_FFT+j];
		}
		//去除导频
		vector<complex<DataType>> DataEqualizationTemp1 = shift_remove_pilot(DataEqualizationTemp,0);
		//打印
		for (int j = 0; j < Num_data_subcarriers; j++) {
			cout << "DataEqualizationTemp1[" << j << "]:" << DataEqualizationTemp1[j] << endl;
		}
		//放入
		for (int j = 0; j < Num_data_subcarriers; j++) {
			DataEqualizationShift[i*Num_data_subcarriers+j] = DataEqualizationTemp1[j];
		}
	}
	//解调
	vector<int> demodulatedData = demodulate(DataEqualizationShift, Ereal);

	return demodulatedData;
}



#endif