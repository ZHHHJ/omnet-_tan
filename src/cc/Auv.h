#ifndef AUV_H
#define AUV_H

#include"State.h"
#include <omnetpp.h>
#include<deque>
#include"../msg/Data_m.h"
#include <cstdlib>
#include <ctime>
#include <unordered_map>
#include <math.h>
namespace omnetpp {

class AUV : public cSimpleModule {
  public:
    AUV();
    virtual ~AUV();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage* msg) override;
    virtual void finish() override;

  private:
    // 定时移动事件
    cMessage* moveTimer;
    cMessage* printTimer ;
    // 位置与运动参数（从 NED 参数读取；若未配置则使用默认值）
    double positionXMeters;
    double positionYMeters;
    double velocityXMps;
    double velocityYMps;
    simtime_t updateInterval; // deltaT
    double printInterval;
    double omegaRadPerSec;
    double angleRadians;
    double centerXMeters;
    double centerYMeters;
    double radiusMeters;
    int id;

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
    double throughput=0;
    double ChannelUtilizationTime=0;
    long currentCollisionNumFrames;//当前碰撞转
    bool channelBusy;
    State state;
    cMessage* endRxDataEvent;
    std::deque<Data*> DataRecvDeque;//数据接收队列
    typedef void (AUV::*EventFunctionPtr)(cMessage*);
    std::map<cMessage*, EventFunctionPtr> eventHandlers;

    long recvNum = 0;
  private:
    void scheduleNextMove(simtime_t atTime);
    void performMoveUpdate();
    void endRxData(cMessage* msg);
};

} //

#endif // AUV_H


