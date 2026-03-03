#include "Auv.h"

using namespace omnetpp;

Define_Module(omnetpp::AUV);

AUV::AUV()
{
    moveTimer = nullptr;
    printTimer = nullptr;
    endRxDataEvent = nullptr;
}

AUV::~AUV()
{
    if (moveTimer) {
        cancelAndDelete(moveTimer);
        moveTimer = nullptr;
    }
    if (printTimer) {
        cancelAndDelete(printTimer);
        printTimer = nullptr;
    }
    if (endRxDataEvent) {
        cancelAndDelete(endRxDataEvent);
        endRxDataEvent = nullptr;
    }
}

void AUV::initialize()
{
    //实验数据统计
    startTime = simTime();

    receivedDataCount = 0;
    totalDelay = 0;
    totalTxEnergy = 0;
    TransmittedDistance = 0;
    state=IDLE;
    channelBusy = false;
    currentCollisionNumFrames = 0;//当前碰撞帧数
    throughput = 0;
    id = getId();
    // 读取初始位置（如未在 ini 中设置，默认为 0）
    positionXMeters = par("x").doubleValue();
    positionYMeters = par("y").doubleValue();

    // 若 deltaT 未设置，默认 1s；本测试忽略 vx/vy，使用圆周运动
    updateInterval = hasPar("deltaT") ? par("deltaT") : 1.0;
    if (updateInterval <= 0) updateInterval = 1.0;

    // 圆周运动测试参数：绕 (1500,1500)，半径 500，周期 100s（可调整）
    centerXMeters = 1500.0;
    centerYMeters = 1500.0;
    radiusMeters = 500.0;
    double periodSeconds = 1000.0;
     omegaRadPerSec = 2 * M_PI / periodSeconds; // 角速度
    // 初始相位由当前初始位置推导或置 0，这里置 0
    angleRadians = 0.0;

    moveTimer = new cMessage("moveTimer");
    scheduleNextMove(simTime() + updateInterval);

    // 初始化打印定时器：每 3 秒打印一次位置
    printInterval = 2.0;
    printTimer = new cMessage("printTimer");
    scheduleAt(simTime() + printInterval, printTimer);
    
    // 初始化 endRxDataEvent
    endRxDataEvent = new cMessage("endRxDataEvent");
    eventHandlers[endRxDataEvent] = &AUV::endRxData;
}

void AUV::handleMessage(cMessage* msg)
{
    //判断事件是否在eventHandlers中
    auto it = eventHandlers.find(msg);

    //在eventHandlers中，使用Sensor()中设置方法。
    if (it != eventHandlers.end()){
            EventFunctionPtr handler = it->second;
            (this->*handler)(msg);
    }else if (msg == moveTimer) {
        performMoveUpdate();
        scheduleNextMove(simTime() + updateInterval);
    } else if (msg == printTimer) {
        std::cout << "AUV pos=(" << par("x").doubleValue()
                  << ", " << par("y").doubleValue() << ")  t="
                  << simTime() << std::endl;
        EV << "AUV pos=(" << par("x").doubleValue()
                          << ", " << par("y").doubleValue() << ")  t="
                          << simTime() << endl;
        scheduleAt(simTime() + printInterval, printTimer);
    } else if(typeid(*msg) == typeid(Data)){//判断是否是DATA
        if (state == IDLE or state == RECV )
        {

            state = RECV;
            std::cout<<"AUV("<<id<<")   state = RECV"<<endl;
            std::cout<<"AUV("<<id<<")  channelBusy = "<<channelBusy<<endl;
            // 正常处理 Data 消息
            Data *data = dynamic_cast<Data*>(msg);
            std::cout<<"AUV("<<id<<")   收到DATA " <<"   Current simulation time: " << simTime() <<"  to: "<<data->getData_name()<< std::endl;

            simtime_t endRecvTime = simTime() + data->getDuration();

            if (!channelBusy)
            {
                std::cout<<"AUV("<<id<<")   channelBusy不忙进入RECV阶段"<<endl;
                channelBusy = true;
                std::cout<<"AUV("<<id<<")  channelBusy = true"<<endl;
                double dropRate = 0.05;  // 5% 丢包概率
                if (uniform(0, 1) < dropRate) {
                    EV << "Data dropped due to simulated packet loss. Time: " << simTime() << endl;
                    std::cout<< "丢包. Time: " << simTime() << endl;
                    channelBusy = false;
                    state = IDLE;
                    delete msg;
                    return;
                }

                // 正常处理 Data 消息
                Data *data = dynamic_cast<Data*>(msg);

                int srcId=data->getData_srcId();
                int destId=data->getData_destId();
                if(destId==id){
                    //判断是不是发给自己的。
                    std::cout <<"    AUV("<<id<< ")收到Data: " <<data->getData_name()<<"  是发送给自己的 Current simulation time: " << simTime()<<endl;
                    std::cout <<"    AUV("<<id<< ")收到Data: " <<data->getData_name()<<" endRecvTime="<<endRecvTime<<endl;
                    receivedDataCount++;
                    simtime_t delay1 = simTime() - data->getSendingTime();
                    totalDelay += delay1;

                    //delayVec.record(delay);
                    BitReceived.collect(data->getBitLength());
                    endTime = simTime();

                    DataRecvDeque.push_back(data);
                    recvNum++;
                    std::cout <<"    AUV("<<id<< ")  recvNum = "<<recvNum<<endl;

                    //simtime_t allTime = endTime - startTime;
                    //double throughput = BitReceived.getSum() / allTime.dbl();
                    // std::cout<<"AUV(id="<<id<<")收到的data Bit数为"<<BitReceived<<endl;

                    //std::cout<<"BitReceived.getSum()"<<BitReceived.getSum()<<"      allTime.dbl()"<<allTime.dbl()<<endl;
                   // std::cout<<"AUV(id="<<id<<")信道吞吐量为"<<throughput<<endl;
                }else{
                    std::cout <<"    AUV("<<id<< ")收到Data: " <<data->getData_name()<<"  不是发送给自己的 Current simulation time: " << simTime()<<endl;
                }


                EV << "start recv " << data->getFullName() << endl;

                //std::cout<<"AUV("<<id<<")  前scheduleAt(endRecvTime, endRxDataEvent)"<<endl;
                scheduleAt(endRecvTime, endRxDataEvent);
                //std::cout<<"AUV("<<id<<")  后scheduleAt(endRecvTime, endRxDataEvent)"<<endl;
                //

                //recvNum++;
                //std::cout <<"    AUV("<<id<< ")  recvNum = "<<recvNum<<endl;
            }
            else
            {
                EV << "collision!" << endl;

                if (currentCollisionNumFrames == 0)
                {
                    currentCollisionNumFrames = 2;
                    std::cout<<"AUV("<<id<<")  currentCollisionNumFrames = 2"<<endl;
                }
                else
                {
                    currentCollisionNumFrames++;
                    std::cout<<"AUV("<<id<<")  currentCollisionNumFrames ="<<currentCollisionNumFrames<<endl;
                }
            }
        }
        else
        {
            delete msg;
        }


    }else {
        // 未使用的消息，直接丢弃
        delete msg;
    }
}

void AUV::performMoveUpdate()
{
    // 匀速圆周：x = cx + R cosθ, y = cy + R sinθ, θ += ωΔt
    double dt = updateInterval.dbl();
    angleRadians += omegaRadPerSec * dt;

    // 保持角度在 [-pi, pi] 或 [0, 2pi] 非必要，仅为数值稳定
    if (angleRadians > 2 * M_PI) angleRadians -= 2 * M_PI;
    if (angleRadians < 0) angleRadians += 2 * M_PI;

    positionXMeters = centerXMeters + radiusMeters * std::cos(angleRadians);
    positionYMeters = centerYMeters + radiusMeters * std::sin(angleRadians);

    // 写回到参数，供其他模块读取
    par("x").setDoubleValue(positionXMeters);
    par("y").setDoubleValue(positionYMeters);
}

void AUV::scheduleNextMove(simtime_t atTime)
{
    scheduleAt(atTime, moveTimer);
}

void AUV::finish()
{
    // 可在此输出最终位置或统计数据
    recordScalar("final_x", positionXMeters);
    recordScalar("final_y", positionYMeters);

    simtime_t totalTime = endTime - startTime;
    //EV <<"AUV(id="<<id<<")收到的data Bit数为"<<BitReceived<<endl;
    //double throughput = BitReceived.getSum() / totalTime.dbl();

    //EV <<"Total Received Bits: "<<BitReceived.getSum()<<", Total Time: "<<totalTime<<endl;
    //EV <<"AUV(id="<<id<<")信道吞吐量为"<<throughput<<" bps"<<endl;
    //recordScalar("throughput_bps", throughput);  // 记录吞吐量(比特/秒)

    double avgDelay = (receivedDataCount > 0) ? totalDelay.dbl() / receivedDataCount : 0;
    throughput = (endTime - startTime).dbl() > 0 ? BitReceived.getSum() / (endTime - startTime).dbl() : 0;
    simtime_t totalSimTime = simTime() - startTime;
    throughput = totalSimTime.dbl() > 0 ? BitReceived.getSum() / totalSimTime.dbl() : 0;
    recordScalar("RxDataCount", receivedDataCount);
    recordScalar("RxDataBit", BitReceived.getSum());
    recordScalar("averageDelay", avgDelay);
    recordScalar("throughput_bps", throughput);// 记录吞吐量(比特/秒)
    recordScalar("totalTxEnergy", totalTxEnergy);
    recordScalar("RxCTSCount", receivedCTSCount);
    recordScalar("TxRTSCount", TransmittedRTSCount);
    recordScalar("TxDataCount", TransmittedDataCount);
    recordScalar("TransmittedDistance",TransmittedDistance);
    std::cout << "ID: " << id << endl;
    //std::cout << "AUV( " << id <<"):(x,y)=("<<x<<","<<y<<")"<< endl;
    std::cout << "TxRTSCount: " << TransmittedRTSCount << endl;
    std::cout << "RxCTSCount: " << receivedCTSCount << endl;
    std::cout << "TxDataCount: " << TransmittedDataCount  << endl;
    std::cout << "RxDataCount: " << receivedDataCount << endl;
    std::cout << "totalTxEnergy: " << totalTxEnergy  << endl;
    std::cout << "TransmittedDistance: " << TransmittedDistance  << endl;
    std::cout << "averageDelay: " << avgDelay << endl;
    std::cout << "throughput_bps: " << throughput << endl;

}

void AUV::endRxData(cMessage* msg){

    ASSERT(msg == endRxDataEvent);
    channelBusy = false;
    std::cout<<"AUV("<<id<<")   channelBusy = false"<<endl;
    state = HANDING_RECV;
    std::cout<<"AUV("<<id<<")   state = HANDING_RECV"<<endl;
        state = IDLE;
        std::cout<<"AUV("<<id<<")   state = IDLE"<<endl;



}

