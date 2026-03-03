#ifndef SENSOR_H
#define SENSOR_H
#include<omnetpp.h>
#include<deque>
#include"../msg/Data_m.h"
#include"../msg/RTS_m.h"
#include"../msg/CTS_m.h"
#include"State.h"
#include <cstdlib>
#include <ctime>
#include <unordered_map>

using namespace omnetpp;
namespace omnetpp {
class Sensor:public cSimpleModule
{
    public:
        Sensor();
        virtual ~Sensor();

    private:
        cPar* txRate;
        cPar* iaTime;
        cPar* SNR_Threshold;
        //cPar* numSensors;
        int pkLenBit;

        simtime_t slotTime;
        simtime_t slotTime1;
        simtime_t slotTime2;
        bool isSlotted;
        double x,y,z;//三维坐标
        double serverX ,serverY ,serverZ;
        double auvX ,auvY ,auvZ;

        int numSensors;//节点数
        //numSensors 和数组大小问题
        //numSensors 被用来定义 allSensorsId 数组的大小，但你在声明时使用了 numSensors，
        //而它是在类中作为未初始化的变量声明的。在 C++ 中，数组的大小必须在编译时是已知的。

        long currentCollisionNumFrames;//当前碰撞转
        bool controlChannelBusy;//控制信道忙率
        State controlState;//控制信道状态
        State dataState;//数据信道状态

        double rtsSendPower;//rts的发送功率。
        double dataSendPower;//data的发送功率
        double dataSendDirection;//data的发送方向


        int id,serverId,dataDestId,rtsDestID;
        bool rts_isReceived;//是否收到data(ack功能)
        int rts_receivedID;//收到的谁发的data，，server以此来判断是否重发data或者删除缓存中的data信息。
        bool rts_isTransmit;
        int rts_dataID;
        int dataID;

        int  allSensorsId[50];
        //const double maxTranRange = 1500.0;
        //5.16      const double maxTranRange = 1500.0;---------->>>>>>>cPar* maxTranRange;
        //5.16       maxTranRange----------->>>>>>>>>>>>>maxTranRange->doubleValue()
        cPar* maxTranRange;
        cPar* rtsLenBit;
        cPar*  dataLenBit;
        cModule* server;
        cModule* auv;
        cModule* network;
        cMessage* getDestIdEvent;//调度获取目的节点id事件
        cMessage* startGenerateDataEvent;//调度生成数据事件
        cMessage* txRtsEvent;//调度传输RTS事件
        cMessage* computeRtsSendPowerEvent;//调度计算RTS发送能量事件
        cMessage* txDataEvent;
        cMessage* endRxDataEvent;

        cPar* auv_perceived_distance;
        cPar* data_dropRate;

        //缓存队列
        std::deque<Data*> dataSendDeque;//数据发送队列
        std::deque<Data*> DataRecvDeque;//数据接收队列
        std::deque<RTS*> RTSSendDeque;//控制帧请求发送队列
        std::deque<CTS*> CTSRecvDeque;//控制帧调度接收队列

        std::map<cModule*, double> neighborMap;
        std::unordered_map<int, std::tuple<double,double>> coordinateList_map;

        typedef void (Sensor::*EventFunctionPtr)(cMessage*);
        std::map<cMessage*, EventFunctionPtr> eventHandlers;

        std::tuple<int,double,double>destNode;


        //实验数据统计
        long sendDataNum = 0;//发送data数量
        long beRecvDataNum = 0;//被接受data数量，无重传信息=被接收。
        long recvDataNum = 0;//接收data数量

        cHistogram  BitReceived;// 累计接收的比特数
        simtime_t startTime;         // 开始时间
        simtime_t endTime;           // 结束时间
        cOutVector throughputVec;
        cOutVector delayVec;
        cOutVector energyVec;

        int receivedDataCount = 0;
        double TransmittedDistance = 0;
        int TransmittedDataCount = 0;
        simtime_t totalDelay = 0;
        double totalTxEnergy = 0;
        int receivedCTSCount = 0;
        int TransmittedRTSCount = 0;

        double ChannelUtilizationTime=0;

        /* ---------- 环境与协议参数 ---------- */
        /*double freq_kHz;      // 载频 (kHz)
        double snrMin_dB;     // 最小接收 SNR (dB)
        double noise_dB; // 本底噪声 (dB re 1µPa)
        double eta;  // 换能器效率 (0.3~0.7)
        double di_tx_dB;      // 发射端指向性指数 (dB)
        double di_rx_dB;      // 接收端指向性 (单水听器取 0 dB)*/
        cPar* freq_kHz;      // 载频 (kHz)
        cPar* snrMin_dB;     // 最小接收 SNR (dB)
        cPar* noise_dB; // 本底噪声 (dB re 1µPa)
        cPar* margin_dB;
        cPar* eta;  // 换能器效率 (0.3~0.7)
        cPar* di_tx_dB;      // 发射端指向性指数 (dB)
        cPar* di_rx_dB;      // 接收端指向性 (单水听器取 0 dB)

        std::map<cMessage*, CTS*> ctsToEventMap;  // CTS与事件的映射

    private:
        virtual void initialize() override;
        virtual void handleMessage(cMessage* msg) override;
        virtual void finish() override;

        void getDestId(cMessage* msg);//获取目的节点id
        void startGenerateData(cMessage* msg);//生成数据包和控制帧
        void startTxRts(cMessage* msg);//发送RTS
        void startComputeRtsSendPower(cMessage* msg);//计算rts的发送能量
        void startTxData(cMessage* msg);
        void endRxData(cMessage* msg);
        double getDistanceToAUV();
        // 计算下一阶段开始时间
        simtime_t calculateNextPhaseStartTime() const;
        // 预测指定时间点的AUV位置
        std::pair<double, double> predictAuvPositionAtTime(simtime_t targetTime) const;
        // 计算指定时间点与AUV的距离
        double getDistanceToAuvAtTime(simtime_t targetTime) const;

        // 连续收到cts，只会发送一个数据的bug
        void processCTSData(CTS* cts);
        void sendDataToAUV(Data* data, CTS* cts);
        void sendDataToSensor(Data* data, CTS* cts);
        double calculateTxEnergy(Data* data, double distance);


};
};
#endif // SENSOR_H
