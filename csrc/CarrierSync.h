/*************************************************************************
	> File Name: CarrierSync.h
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2024年05月24日 星期五 14时34分26秒
 ************************************************************************/
#ifndef CARRIERSYNC
#define CARRIERSYNC
/****
 * ofdm同步
 * 主要是为了解决
 * STO 符号定时偏差
 * CFO 载波频偏
 * 
 * 
 * 
 * 
 * 
 * 
*/
#include "common.h"
#include "myfixed.h"
#include "DelayCorr.h"

#include<iostream>
using namespace std;

/***
 * 首先是数据分流
 * 将分组检测模块的结果分为
 * 用于做粗频率估计的5个短训练序列
 * 剩余5个短训练序列
 * 待补偿的2个长训练序列 和数据符号
*/
//1 分离5个短训练序列
vector<complex<DataType>> getShortTrainingSequence(const vector<complex<DataType>>& data) {
	vector<complex<DataType>> shortTrainingSequence;
	shortTrainingSequence.resize(Short_Train_Symbols*5);
	for (int i = 0; i < Short_Train_Symbols*5; i++) {
		shortTrainingSequence[i] = data[i];
	}
	return shortTrainingSequence;
}
//2 剩余5个短训练序列
vector<complex<DataType>> getShortTrainingSequenceRest(const vector<complex<DataType>>& data) {
	vector<complex<DataType>> shortTrainingSequenceRest;
	shortTrainingSequenceRest.resize(Short_Train_Symbols*5);
	for (int i = 0; i < Short_Train_Symbols*5; i++) {
		shortTrainingSequenceRest[i] = data[i+Short_Train_Symbols*5];
	}
	return shortTrainingSequenceRest;
}
//3 待补偿的2个长训练序列 和数据符号
vector<complex<DataType>> getLongTrainingSequenceAndData(const vector<complex<DataType>>& data) {
	vector<complex<DataType>> longTrainingSequenceAndData;
	int n = data.size();
	longTrainingSequenceAndData.resize(n-Short_Train_Symbols*10);
	for (int i = 0; i < longTrainingSequenceAndData.size(); i++) {
		longTrainingSequenceAndData[i] = data[i+Short_Train_Symbols*10];
	}
	return longTrainingSequenceAndData;
}
/****
 * 
 * 角度估计模块
 * 
 * 输入一个复数 提取出角度 即相位信息
 * 
 * 需要计算的是atan2(imag,real)
 * 
*/

DataType angleEstimating(const complex<DataType>& data) {
	//这里的单位是弧度 范围是(-pi,pi)
	DataType angle = atan2(data.imag(),data.real());
	return angle;
}



/*****
 * 载波频偏估计
 * 主要利用5个短训练序列 进行4组的延时相关性计算
 * 获得4次相关计算的16点长度累加和
 * 然后将4次累加和送入角度估计模块 得到4次的角度估计
 * 最后对4次的角度估计进行平均得到最终的频率偏移估计
 * 
 *返回一个频率偏移估计值 
*/
DataType CFOEstimating(const vector<complex<DataType>>& shortTrainingSequence) {
	//对数据进行延迟
	vector<complex<DataType>> shortTrainingSequence1 = shortTrainingSequence;
	vector<complex<DataType>> shortTrainingSequence2 = shortTrainingSequence;
	//Seq1 前面补16个0 表示数据延迟16个周期
	for (int i = 0; i < Short_Train_Symbols; i++) {
		shortTrainingSequence1.insert(shortTrainingSequence1.begin(),complex<DataType>(0,0));
	}
	//Seq2 后面补16个0
	for (int i = 0; i < Short_Train_Symbols; i++) {
		shortTrainingSequence2.push_back(complex<DataType>(0,0));
	}

	//进行4次相关计算
	vector<complex<DataType>> corr_sum = delay_corr(shortTrainingSequence1,shortTrainingSequence2,0);
	//将4次相关计算的16点长度累加和送入角度估计模块
	//将4次的估计提取出来
	/***
	 * 这里要注意一下sum的第16个其实是第一个短训练序列和0的相关性
	 * 第32个是第一个短训练序列和第二个短训练序列的相关性
	 * 第48个是第二个短训练序列和第三个短训练序列的相关性
	 * 第64个是第三个短训练序列和第四个短训练序列的相关性
	 * 第80个是第四个短训练序列和第五个短训练序列的相关性
	 * 所以从32 48 64 80取出来
	*/
	complex<DataType> corr_1 = corr_sum[Short_Train_Symbols*2];
	complex<DataType> corr_2 = corr_sum[Short_Train_Symbols*3];
	complex<DataType> corr_3 = corr_sum[Short_Train_Symbols*4];
	complex<DataType> corr_4 = corr_sum[Short_Train_Symbols*5];


	//得到4次的角度估计
	DataType angle1 = angleEstimating(corr_1);
	DataType angle2 = angleEstimating(corr_2);
	DataType angle3 = angleEstimating(corr_3);
	DataType angle4 = angleEstimating(corr_4);
	//对4次的角度估计进行平均得到最终的频率偏移估计
	//打印一下和
	cout << "corr_1:" << corr_1 << "\t" << "corr_2:" << corr_2 << "\t" << "corr_3:" << corr_3 << "\t" << "corr_4:" << corr_4 << endl;
	//打印一下
	cout << "angle1:" << angle1 << "\t" << "angle2:" << angle2 << "\t" << "angle3:" << angle3 << "\t" << "angle4:" << angle4 << endl;
	DataType CFOEstimate = (angle1 + angle2 + angle3 + angle4) / DataType(4);
	CFOEstimate = CFOEstimate / (DataType)Short_Train_Symbols;
	return CFOEstimate;
}
/***
 * 根据频率偏移估计值
 * 计算sin 和 cos
 * 返回一个复数
 * 
*/
complex<DataType> CFOEstimate2Complex(const DataType& CFOEstimate) {
	//这里的单位是弧度
	DataType sinTheta = sin(CFOEstimate);
	DataType cosTheta = cos(CFOEstimate);
	complex<DataType> CFOEstimateComplex(cosTheta,sinTheta);
	return CFOEstimateComplex;
}

/****
 * 载波频偏补偿
 * 将数据进行频率偏移补偿
 * 
 * 返回补偿后的数据
*/
vector<complex<DataType>> CFOCompensation(const vector<complex<DataType>>& data,const DataType& CFOEstimate) {
	vector<complex<DataType>> data_compensation;
	data_compensation.resize(data.size());
	//因子取反 
	DataType Frequency = -CFOEstimate;
	//对数据进行频率偏移补偿
	cout << "compensationFactor:" << CFOEstimate2Complex(Frequency) << endl;
	for (int i = 0; i < data.size(); i++) {
		//根据频率偏移估计值计算出补偿因子
		complex<DataType> compensationFactor = CFOEstimate2Complex(Frequency);
		
		//对数据进行频率偏移补偿
		if(i > 160){
			//cout << "data["<< i<<"]:" << data[i] << endl;
		}
		data_compensation[i] = data[i] * compensationFactor;
		//对频率进行累加 这里其实是用累加器代替乘法器
		Frequency -= CFOEstimate;
		//限定在-pi到pi之间
		DataType pi = PI;
		DataType two_pi = pi+pi;
		if(Frequency > pi){
			Frequency -= two_pi;
		}else if(Frequency < pi){
			Frequency += two_pi;
		}

	}
	return data_compensation;
}


/***
 * 测试函数 测测频率偏移估计的功能是否正常
 * 
 * 
*/
void test_CFOEstimating(){
	//生成训练序列
	vector<complex<DataType>> Leader_Sequence = generateLeaderSequence();
	//为方便观察，给前导数据补0
	vector<complex<DataType>> ofdm_symbol(Leader_Sequence.size()+200);
	//前100个数据是0
	for (int i = 0; i < 100; i++) {
		complex<DataType> zero(0,0);
		ofdm_symbol[i] = zero;
	}
	//前导数据
	for (int i = 0; i < Leader_Sequence.size(); i++) {
		ofdm_symbol[i+100] = Leader_Sequence[i];
	}
	//后边的数据是0
	for (int i = 0; i < 100; i++) {
		complex<DataType> zero(0,0);
		ofdm_symbol[i+100+Leader_Sequence.size()] = zero;
	}
	//返回确定的数据的起始点和结束点后的数据
	int start = 100;
	vector<complex<DataType>> data = delay_corr_Group_Detection(ofdm_symbol,start);

	//分离数据
	vector<complex<DataType>> shortTrainingSequence = getShortTrainingSequence(data);
	//进行频率偏移估计
	DataType CFOEstimate = CFOEstimating(shortTrainingSequence);

	//打印
	cout << "CFOEstimate:" << CFOEstimate << endl;
	//提取出长序列和数据
	vector<complex<DataType>> longTrainingSequenceAndData = getLongTrainingSequenceAndData(data);
	//进行频率偏移补偿
	vector<complex<DataType>> data_compensation = CFOCompensation(longTrainingSequenceAndData,CFOEstimate);
	//打印
	//for (int i = 0; i < data_compensation.size(); i++) {
	//	cout << data_compensation[i] << endl;
	//}
}

#endif