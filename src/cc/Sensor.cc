#include "Sensor.h"
#include "Server.h"

Define_Module(Sensor);

Sensor::Sensor()
{
    getDestIdEvent = new cMessage("getDestIdEvent");//调度获取目的节点id事件
    startGenerateDataEvent = new cMessage("startGenerateDataEvent");//调度生成数据函数的事件
    txRtsEvent = new cMessage("txRtsEvent");//调度发送rts事件
    computeRtsSendPowerEvent = new cMessage("computeRtsSendPowerEvent");//调度计算RTS发送能量事件
    txDataEvent = new cMessage("txDataEvent");
    endRxDataEvent = new cMessage("endRxDataEvent");
    eventHandlers[getDestIdEvent] = &Sensor::getDestId;//调度获取目的节点id函数
    eventHandlers[startGenerateDataEvent] = &Sensor::startGenerateData;//调度生成数据函数
    eventHandlers[txRtsEvent] = &Sensor::startTxRts;//调度发送RTS函数
    eventHandlers[computeRtsSendPowerEvent] = &Sensor::startComputeRtsSendPower;//调度发送RTS函数
    eventHandlers[txDataEvent] = &Sensor::startTxData;
    eventHandlers[endRxDataEvent] = &Sensor::endRxData;
    //std::cout << "Sensor::Sensor() end" << std::endl;

}

Sensor::~Sensor()
{
    cancelAndDelete(startGenerateDataEvent);
    cancelAndDelete(txDataEvent);
    cancelAndDelete(endRxDataEvent);
    cancelAndDelete(getDestIdEvent);
    for (auto packet : dataSendDeque)
    {
        delete packet;
    }

    for (auto packet : DataRecvDeque)
    {
        delete packet;
    }
    for (auto packet : RTSSendDeque)
    {
        delete packet;
    }

    for (auto packet : CTSRecvDeque)
    {
        delete packet;
    }

}

void Sensor::initialize()
{
    //实验数据统计
    startTime = simTime();
    //delayVec.setName("endToEndDelay");
    //throughputVec.setName("throughput");
    //energyVec.setName("txEnergy");
    receivedDataCount = 0;
    totalDelay = 0;
    totalTxEnergy = 0;
    TransmittedDistance = 0;


    id = getId();
    //std::cout << "Sensor(id:"<<id<<")::initialize() start" << std::endl;
    txRate = &par("txRate");
    std::cout << "Sensor(id:"<<id<<")::txRate=" <<txRate<< std::endl;
    iaTime = &par("iaTime");
    SNR_Threshold = &par("SNR_Threshold");
    maxTranRange= &par("maxTranRange");
    slotTime = par("slotTime");
    slotTime1 = par("slotTime1");
    slotTime2 = par("slotTime2");
    rtsLenBit= &par("rtsLenBit");
    dataLenBit= &par("dataLenBit");

    /* ---------- 环境与协议参数 ---------- */
    freq_kHz = &par("freq_kHz");      // 载频 (kHz)
    snrMin_dB = &par("snrMin_dB");     // 最小接收 SNR (dB)
    margin_dB= &par("margin_dB");
    noise_dB = &par("noise_dB"); // 本底噪声 (dB re 1µPa)
    eta     = &par("eta");  // 换能器效率 (0.3~0.7)
    di_tx_dB = &par("di_tx_dB");      // 发射端指向性指数 (dB)
    di_rx_dB = &par("di_rx_dB");      // 接收端指向性 (单水听器取 0 dB)
    x = par("x");
    y = par("y");

    auv_perceived_distance= &par("auv_perceived_distance");
    isSlotted = slotTime>0;
    network = getParentModule();
    if (!network) {
           EV << "Error: network module not found." << endl;
           throw cRuntimeError("network module not found");
       }

    //获取所有节点的id
    numSensors = network->par("numSensors");
    //std::cout << "Sensor(id:"<<id<<").numSensors="<< numSensors << std::endl;
    for (int i = 0; i < numSensors; i++)
    {
        cModule* sensor = network->getSubmodule("sensor", i);//"sensor"

        if (sensor) {//// 确保使用父模块获取子模块
            double sensorX = sensor->par("x").doubleValue();
            double sensorY = sensor->par("y").doubleValue();
            //double sensorZ = sensor->par("z").doubleValue();
            double distance = std::sqrt(std::pow(x - sensorX, 2) + std::pow(y - sensorY, 2) );
            neighborMap[sensor] = distance;

            allSensorsId[i] = sensor->getId();
        } else {
            EV << "Warning: sensor submodule at index " << i << " not found." << endl;
        }

    }

    // 将 AUV 也加入 neighborMap 与 allSensorsId，在最后一个
    auv = getModuleByPath("auv");
    if (auv) {
        double auvXCur = auv->par("x").doubleValue();
        double auvYCur = auv->par("y").doubleValue();
        double distanceAuv = std::sqrt(std::pow(x - auvXCur, 2) + std::pow(y - auvYCur, 2));
        neighborMap[auv] = distanceAuv;
        // 追加 AUV 的 ID 到 allSensorsId 的下一个槽位（不覆盖已有传感器）
        if (numSensors >= 0 && numSensors < static_cast<int>(sizeof(allSensorsId)/sizeof(allSensorsId[0]))) {
            allSensorsId[numSensors] = auv->getId();
        }
    } else {
        EV << "Warning: AUV module not found at path 'auv'." << endl;
    }



// 打印 neighborMap 的内容
std::cout << "Neighbor Map (Sensor -> Distance):" << std::endl;
for (const auto& entry : neighborMap) {
    cModule* sensorModule = entry.first;
    double distance = entry.second;

    if (sensorModule) {
        std::cout << "Sensor[" << sensorModule->getFullName()
                  << "] (ID=" << sensorModule->getId()
                  << ") -> Distance: " << distance << " meters" << std::endl;
    }
}


/*
    //初始化 coordinateList_map  将所有sensor的位置信息存入coordinateList_map中。方便查找
        for (int i = 0; i < numSensors; i++)
        {
            cModule* sensor = network->getSubmodule("sensor", i);

            if (sensor) {
                double sensorX = sensor->par("x").doubleValue();
                double sensorY = sensor->par("y").doubleValue();
                //double sensorZ = sensor->par("z").doubleValue();
                double distance = std::sqrt(std::pow(x - sensorX, 2) + std::pow(y - sensorY, 2) );
                //double distance = std::sqrt(std::pow(x - sensorX, 2) + std::pow(y - sensorY, 2) + std::pow(z - sensorZ, 2));


                if (distance <= maxTranRange->doubleValue())
                {
                coordinateList_map[sensor->getId()] = {sensorX,sensorY};
                //double delay = distance / 1500.0;
                neighborMap[sensor] = distance;
                }
            } else {
                        EV << "Warning: sensor submodule at index " << i << " not found." << endl;
                    }
        }

*/


    //std::cout << "allSensorsId(id:"<<id<<") get finish" << std::endl;



    //z = par("z");

    //input in_control @directIn;
    //input in_data1 @directIn;
    //gate("in")->setDeliverOnReceptionStart(true);
    gate("in_control")->setDeliverImmediately(true);
    gate("in_data1")->setDeliverImmediately(true);



    dataDestId = -1;

    rtsSendPower= 0;


    server = getModuleByPath("server");
    if (server == nullptr) {//确认这些指针是否正确指向有效的对象。
                throw cRuntimeError("Server module not found!");
            }

    serverId = server->getId();

    serverX =server->par("x");
    serverY =server->par("y");
    //double serverZ =server->par("z");



    currentCollisionNumFrames = 0;//当前碰撞帧数

    controlChannelBusy = false;

    controlState=IDLE;//控制信道状态
    dataState=IDLE;//数据信道状态


    //触发随机获取目的节点事件。//
    scheduleAt(simTime(), getDestIdEvent);
    //iaTime时间后接触发生成数据事件。
    scheduleAt(simTime()+ iaTime->doubleValue(),startGenerateDataEvent);
    //scheduleAt(simTime()+slotTime1+slotTime2, txRtsEvent);
    //std::cout << "Sensor(id:"<<id<<")::initialize() end" << std::endl;


    std::cout << "Sensor(id:" << id<<")的数据统计：" << endl;
    std::cout << "sensor( " << id <<"):(x,y)=("<<x<<","<<y<<")"<< endl;
    std::cout << "TxRTSCount: " << TransmittedRTSCount << endl;
    std::cout << "RxCTSCount: " << receivedCTSCount << endl;
    std::cout << "TxDataCount: " << TransmittedDataCount  << endl;
    std::cout << "RxDataCount: " << receivedDataCount << endl;
    std::cout << "totalTxEnergy: " << totalTxEnergy  << endl;
    std::cout << "TransmittedDistance: " << TransmittedDistance  << endl;
    std::cout << "===============================================" << endl;








}

void Sensor::handleMessage(cMessage* msg)
{
    //判断事件是否在eventHandlers中
    auto it = eventHandlers.find(msg);

    // 检查是否是CTS数据发送事件
       if (ctsToEventMap.find(msg) != ctsToEventMap.end()) {
           CTS* cts = ctsToEventMap[msg];
           ctsToEventMap.erase(msg);

           // 处理这个特定CTS的数据发送
           processCTSData(cts);

           delete msg;
           return;
       }


    //在eventHandlers中，使用Sensor()中设置方法。
    if (it != eventHandlers.end()){
            EventFunctionPtr handler = it->second;
            (this->*handler)(msg);
    }else if(typeid(*msg) == typeid(CTS)){//不在eventHandlers中,先判断是不是CTS


        CTS *cts = dynamic_cast<CTS*>(msg);
        std::cout << "收到CTS " <<"   Current simulation time: " << simTime() <<"  to: "<<cts->getRts_name()<< std::endl;
        int srcId=cts->getCts_srcID();
        int destId=cts->getCts_destID();
        if(destId==id){//判断是不是发给自己的。是则按照CTS中的信息调用TxDataEvent
            receivedCTSCount++;
            std::cout <<"    sensor("<<id<< ")收到CTS: " <<cts->getCts_name()<<"    Cts_TxTime:"<<cts->getCts_TxTime() <<endl;

            //将CTS存到CTS接收队列中，然后在startTxData读取CTS，根据CTS中的参数，发送data
            CTSRecvDeque.push_back(cts);


            // 为这个CTS创建独立的数据发送事件
            char eventName[50];
            snprintf(eventName, sizeof(eventName), "txDataEvent_%d_%d",
                     cts->getCts_dataID(), cts->getCts_TxTime().raw());
            cMessage* individualTxEvent = new cMessage(eventName);

            // 将CTS与事件关联
            ctsToEventMap[individualTxEvent] = cts;

            // 调度到精确时间
            scheduleAt(cts->getCts_TxTime(), individualTxEvent);



            // 如果txDataEvent未调度，则调度它
            /*if (!txDataEvent->isScheduled()) {
                scheduleAt(cts->getCts_TxTime(), txDataEvent);
            }*/
            std::cout <<"   Current simulation time: " << simTime()<<"    sensor("<<id<< ")收到CTS: " <<cts->getCts_name()<<"    Cts_TxTime:"<<cts->getCts_TxTime() <<endl;

           // dataSendPower = cts->getCts_sendPower(); // 假设CTS中已包含功率参数
            //dataSendDirection = cts->getCts_direction();// 假设包含方向参数
            //dataSendPower=0;//data的发送功率
            //dataSendDirection=0;//data的发送方向

            /*if (txDataEvent->isScheduled()) {
                cancelEvent(txDataEvent);
            }*/
            //scheduleAt(cts->getCts_TxTime(), txDataEvent);
            //std::cout <<"   Current simulation time: " << simTime()<<"调度了在"<<cts->getCts_TxTime()<<"触发txDataEvent"<<endl;
        }


    }else if(typeid(*msg) == typeid(Data)){//判断是否是DATA
        // 模拟丢包率：例如5%丢包
        double dropRate = 0.05;  // 5% 丢包概率
        if (uniform(0, 1) < dropRate) {
            EV << "Data dropped due to simulated packet loss. Time: " << simTime() << endl;
            delete msg;
            return;
        }

        // 正常处理 Data 消息
        Data *data = dynamic_cast<Data*>(msg);
        std::cout << "收到DATA " <<"   Current simulation time: " << simTime() <<"  to: "<<data->getData_name()<< std::endl;

        int srcId=data->getData_srcId();
        int destId=data->getData_destId();

        if(destId==id){
            //判断是不是发给自己的。
            std::cout <<"    sensor("<<id<< ")收到Data: " <<data->getData_name()<<"   Current simulation time: " << simTime()<<endl;
            receivedDataCount++;
            simtime_t delay1 = simTime() - data->getSendingTime();

            totalDelay += delay1;

            //delayVec.record(delay);
            BitReceived.collect(data->getBitLength());
            endTime = simTime();
            /*
                未完成
                从CTS中读取信道

            **/
            //将CTS存到CTS接收队列中，然后在startTxData读取CTS，根据CTS中的参数，发送data
            DataRecvDeque.push_back(data);

            //simtime_t allTime = endTime - startTime;
            //double throughput = BitReceived.getSum() / allTime.dbl();
            // std::cout<<"sensor(id="<<id<<")收到的data Bit数为"<<BitReceived<<endl;

            //std::cout<<"BitReceived.getSum()"<<BitReceived.getSum()<<"      allTime.dbl()"<<allTime.dbl()<<endl;
           // std::cout<<"sensor(id="<<id<<")信道吞吐量为"<<throughput<<endl;
        }




    }


}

void Sensor::finish()
{
    simtime_t totalTime = endTime - startTime;
    //EV <<"sensor(id="<<id<<")收到的data Bit数为"<<BitReceived<<endl;
    //double throughput = BitReceived.getSum() / totalTime.dbl();

    //EV <<"Total Received Bits: "<<BitReceived.getSum()<<", Total Time: "<<totalTime<<endl;
    //EV <<"sensor(id="<<id<<")信道吞吐量为"<<throughput<<" bps"<<endl;
    //recordScalar("throughput_bps", throughput);  // 记录吞吐量(比特/秒)

    double avgDelay = (receivedDataCount > 0) ? totalDelay.dbl() / receivedDataCount : 0;
    double throughput = (endTime - startTime).dbl() > 0 ? BitReceived.getSum() / (endTime - startTime).dbl() : 0;

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
    std::cout << "sensor( " << id <<"):(x,y)=("<<x<<","<<y<<")"<< endl;
    std::cout << "TxRTSCount: " << TransmittedRTSCount << endl;
    std::cout << "RxCTSCount: " << receivedCTSCount << endl;
    std::cout << "TxDataCount: " << TransmittedDataCount  << endl;
    std::cout << "RxDataCount: " << receivedDataCount << endl;
    std::cout << "totalTxEnergy: " << totalTxEnergy  << endl;
    std::cout << "TransmittedDistance: " << TransmittedDistance  << endl;
    std::cout << "averageDelay: " << avgDelay << endl;
    std::cout << "throughput_bps: " << throughput << endl;
}

//处理事件getDestIdEvent的方法，
//目的是随机选择一个节点的id
void Sensor::getDestId(cMessage* msg){
    std::cout<<"进入sensor("<<id<<"getDestId函数"<<"   Current simulation time: " << simTime()<<endl;

    // 兼容事件触发和直接调用
       if (msg != nullptr) {
           ASSERT(msg == getDestIdEvent);
       }

    ASSERT(msg==getDestIdEvent);
    // 使用模块 ID 和仿真时间作为随机数种子

    // 计算下一阶段开始时间
    simtime_t t_next = calculateNextPhaseStartTime();

    int seed = id + static_cast<int>(simTime().dbl() * 1000);
    srand(seed);

    // 获取一个随机传感器的 ID
    if (numSensors > 0) {
        //int randomIndex = intrand(numSensors);
        //dataDestId = allSensorsId[randomIndex];
        bool valid = false;
        int randomIndex;
        int currentId = getId(); // 获取当前对象的id
        while (!valid) {
            std::cout<<"numSensors="<< numSensors<<endl;
            randomIndex = intrand(numSensors+1);
            dataDestId = allSensorsId[randomIndex];
            if (dataDestId != currentId && dataDestId !=allSensorsId[numSensors] ) {
                std::cout<<"allSensorsId[numSensors+1]====="<< allSensorsId[numSensors]<<endl;
                std::cout<<"dataDestId="<< dataDestId<<"randomIndex="<<randomIndex<<"   numSensors="<< numSensors<<endl;
                //判断能否选择auv作为目的地址；能，而且选择3、6、9、auv的频次几乎
                /*if(dataDestId == numSensors+1){
                    EV << "(id:"<<id<<"sensor选择了auv作为目的节点" << endl;
                    std::cout<< "(id:"<<id<<"sensor选择了auv作为目的节点" << std::endl;
                }*/
                /*if(dataDestId == 3){
                    EV << "(id:"<<id<<"sensor选择了3作为目的节点" << endl;
                    std::cout<< "(id:"<<id<<"sensor选择了3作为目的节点" << std::endl;
                }
                if(dataDestId == 6){
                    EV << "(id:"<<id<<"sensor选择了6作为目的节点" << endl;
                    std::cout<< "(id:"<<id<<"sensor选择了6作为目的节点" << std::endl;
                }
                if(dataDestId == 9){
                    EV << "(id:"<<id<<"sensor选择了9作为目的节点" << endl;
                    std::cout<< "(id:"<<id<<"sensor选择了9作为目的节点" << std::endl;
                }*/
                int targets[] = {2,3,4,5,6,7,8,9,10,11,12};
                int i = 0;

                while (i < 11) {
                    if (dataDestId == targets[i]) {
                        EV << "(id:" << id << " sensor选择了" << targets[i] << "作为目的节点"<<"   Current simulation time: " << simTime()  << endl;
                        std::cout << "(id:" << id << " sensor选择了" << targets[i] << "作为目的节点"<<"   Current simulation time: " << simTime()  << std::endl;
                    }
                    i++;
                }
                valid = true;
            }else if (dataDestId ==allSensorsId[numSensors]) {



                std::cout<<"dataDestId="<< dataDestId<<"   numSensors="<< numSensors<<endl;
                EV << "(id:"<<id<<"sensor选择了auv作为目的节点" << endl;
                std::cout<< "(id:"<<id<<"sensor选择了id:"<<dataDestId<<" -auv作为目的节点"<<"   Current simulation time: " << simTime() << std::endl;

                //计算当前时刻与AUV的距离
                double dAuv = getDistanceToAUV();  // 若找不到 AUV 会返回 -1
                // 计算t_next时刻与AUV的距离
                double dAuvAtTNext = getDistanceToAuvAtTime(t_next);

                // 目标是 AUV：仅当距离小于阈值才认为可用
                //if (dAuv >= 0 && dAuv < auv_perceived_distance->doubleValue()) {

                //dAuv----->dAuvAtTNext
                if (dAuvAtTNext >= 0 && dAuvAtTNext < auv_perceived_distance->doubleValue()) {
                    //std::cout<< "dAuv:"<<dAuv<<"auv_perceived_distance"<<auv_perceived_distance << std::endl;
                    valid = true;
                    std::cout << "Sensor(" << id << ") 选择AUV作为目标，"<< "t_next=" << t_next << "时距离=" << dAuvAtTNext << std::endl;
                    //EV << "(id:"<<id<<"sensor在auv的"<<auv_perceived_distance<<"范围内，可以作为目的节点" << endl;
                    //std::cout<< "(id:"<<id<<"sensor在auv的"<<auv_perceived_distance<<"范围内，可以作为目的节点"  << std::endl;
                } else {
                    std::cout << "Sensor(" << id << ") AUV在t_next=" << t_next << "时距离=" << dAuvAtTNext << "超出范围，重新选择" << std::endl;
                    //EV << "(id:"<<id<<"sensor  不在 auv的"<<auv_perceived_distance<<"范围内，重新寻找目的节点" << endl;
                    //std::cout<< "(id:"<<id<<"sensor 不在 auv的"<<auv_perceived_distance<<"范围内，重新寻找目的节点"  << std::endl;
                    // 不满足接近条件，继续循环重新抽取目的节点
                    valid = false;
                }
            } else {

                EV << "(id:"<<id<<"Warning: dataDestId is same as current id, reselecting..." << endl;
                // 如果所有传感器id都等于currentId，这里会无限循环，需要处理这种情况
            }
        }
    } else {
        EV << "Warning: numSensors is zero. No destination available." << endl;
        dataDestId = -1; // 设置为无效值
    }
    // 如果消息已经被调度，则取消它
    /*if (cScheduler(getDestIdEvent)) {
        cancelEvent(getDestIdEvent);
    }
*/
    // 重新调度消息（直接重新生成新DestID
    //scheduleAt(simTime() + iaTime->doubleValue(), getDestIdEvent);
    //scheduleAt(simTime() , getDestIdEvent);
    //cancelAndDelete(txDataEvent);
    //std::cout << "Sensor(id:"<<id<<")::getDestId() (dataDestId:"<<dataDestId<<")end" << std::endl;
    std::cout<<"退出sensor("<<id<<"dataDestId函数"<<"   Current simulation time: " << simTime()<<endl;
}



//处理事件startGenerateDataEvent的方法，
//目的是生成一个数据包。并放到数据发送队列dataSendDeque中。
//生成一个数据包的请求帧，放到控制帧请求发送队列RTSSendDeque中。在调度一个"请求发送事件"
void Sensor::startGenerateData(cMessage* msg)
{
    std::cout<<"进入sensor("<<id<<"startGenerateData函数 "<<"   Current simulation time: " << simTime()<<endl;

    ASSERT(msg == startGenerateDataEvent);
    //scheduleAt(simTime(), getDestIdEvent);
    // 直接调用获取目标ID，不使用事件调度
    getDestId(nullptr);  // 传入nullptr表示不是事件触发
    //生成数据
    char dataName[40];
    snprintf(dataName, sizeof(dataName), "data-(%d)-to-(%d)", id, dataDestId);

    Data* data = new Data(dataName);
    data->setData_name(dataName);
    data->setData_srcId(id);//data源地址
    data->setData_destId(dataDestId);//data目的地址
    data->setData_id(data->getId());
    //data->setNextId(nextId);//暂时不考虑多跳。

    data->setSendingTime(simTime());  // 设置发送时间，这是生成data的时间

    // 初始化随机种子
    srand(static_cast<unsigned int>(time(0))+  id);
    // 生成一个 500 到 1000 之间的随机数作为随机数据长度
   // pkLenBit = 500 + rand() % 501;//随机数据长度500-1000
    // 使用OMNeT++的随机数生成器
    pkLenBit = 500 + intrand(501); // 500-1000之间的随机数
    data->setBitLength(dataLenBit->intValue());//固定长度300B
    //data->setBitLength(pkLenBit);//随机长度随机数据长度500-1000
    std::cout<<"sensor("<<id<<")生成了 "<<data->getData_name()<<"   Current simulation time: " << simTime()<<endl;
    std::cout<< "    dataBitLength:"<<data->getBitLength()<<endl;

    //data->setBitLength(pkLenBit);//data长度
    dataID= data->getId();

    //sendPower、sendFrequency需要调度之后才有具体值，可以考虑data中删除该参数。

    //存入数据发送队列缓存。
    dataSendDeque.push_back(data);

   /* //打印数据发送队列
    std::cout << "Sensor(id:"<<id<<")数据发送队列: " <<"Current simulation time: " << simTime() << std::endl;
    for (auto dataPtr : dataSendDeque) {
        if (dataPtr) {
            dataPtr->printOn(std::cout);  // 输出到标准输出流
            std::cout << std::endl;       // 换行，便于阅读
        } else {
            std::cout << "nullptr" << std::endl;
        }
    }
*/

    //生成RTS
    char rtsName[40];
    snprintf(rtsName, sizeof(rtsName), "RTS-(%d)-to-(%d)", id, dataDestId);//rts的目的地址永远是server，所以存的是数据的目的地址
    //std::cout << "RTS生成 :sensor(" <<id<<")to server("<< dataDestId  <<")"<< std::endl;
    RTS* rts = new RTS(rtsName);
    rts->setRts_name(rtsName);
    rts->setRts_srcID(id);//rts源地址
    rts->setRts_dataDestID(dataDestId);//rts目的地址永远是server，在destID处写入data的目的地址
    rts->setRts_dataID(dataID);//数据id

    //10.22,发现rts设置的data长度不是getdata长度，导致，packet SA-50部分的实验无效。，只增加了data的计算长度而在调度时用的pkLenBit(500-1000b)
    //rts->setRts_dataLenBit(pkLenBit);//500-1000bit随机数据长度
    rts->setRts_dataLenBit(data->getBitLength());//500-1000bit随机数据长度

    rts->setRts_state(false);
    rts->setBitLength(rtsLenBit->intValue());
    std::cout<< "    rtsBitLength:"<<rts->getBitLength()<<endl;
    std::cout<<"sensor("<<id<<")生成了 "<<rts->getRts_name()<<"   Current simulation time: " << simTime()<<endl;

    //rts->setDisplayString("b=15,15,rect;o=blue");
    EV << "RTS is of type: " << rts->getClassName() << endl;

    EV << "has setDisplayString? " << typeid(*rts).name() << endl;
    /**
     * 未定义
    rts->setRts_isReceived(rts_isReceived);//是否收到data(ack功能)
    rts->setRts_receivedID(rts_receivedID);//收到的谁发的data，，server以此来判断是否重发data或者删除缓存中的data信息。
    rts->setRts_isTransmit(rts_isTransmit);
**/
    //存入RTS发送队列缓存。
    RTSSendDeque.push_back(rts);

/*
    //打印RTS发送队列缓存。
    std::cout << "Sensor(id:"<<id<<")RTS发送队列: " << std::endl;
    for (const auto& rtsPtr : RTSSendDeque) {
        if (rtsPtr) {
            std::cout <<"        "<< rtsPtr << std::endl;
        } else {
            std::cout << "nullptr in RTSdeque" << std::endl;
        }
    }
*/

    scheduleAt(simTime(), txRtsEvent);

    //scheduleAt(simTime()+ iaTime->doubleValue(), getDestIdEvent);// iaTime时间后，重新获取目的节点id
    scheduleAt(simTime() + iaTime->doubleValue(), startGenerateDataEvent);// iaTime时间后，再次生成data和rts



    /*
    if (controlState == IDLE)//判断控制信道的状态，选择是否按照时隙 触发传输RTS事件
        {
            if (!isSlotted)
            {
                controlState = HANDING_TRAN;

                scheduleAt(simTime(), computeRtsSendPowerEvent);
                scheduleAt(simTime(), txRtsEvent);

            }
            else
            {
                controlState = READY_TRAN;

                //scheduleAt(slotTime * ceil(simTime()/slotTime), txRtsEvent);
                // 使用 std::ceil 来确保函数被正确识别
                scheduleAt(slotTime * std::ceil(simTime().dbl()/slotTime), txRtsEvent);
            }
        }

    */
    std::cout<<"退出sensor("<<id<<"startGenerateData函数 "<<"   Current simulation time: " << simTime()<<endl;

}
//处理computeRtsSendPower事件
//计算rts的发送能量
//根据本节点的位置和server的位置计算距离，然后根据公式计算所需的能量。
//固定网络，只需要初始化的时候计算一次，但考虑到之后可能涉及到动态节点，故每次在发送RTS之前计算RTS发送所需要的能量
void Sensor::startComputeRtsSendPower(cMessage* msg){


    ASSERT(msg==computeRtsSendPowerEvent);


    /*
     *未完成
     *RTS发送能量计算公式
         double distance = sqrt(pow(x - serverX, 2) + pow(y - serverY, 2));
        // 简化的路径损耗模型（需根据实际需求调整）
        double pathLoss = 1.0 / (distance * distance);
        rtsSendPower = 1e-3 / pathLoss; // 假设接收灵敏度为1mW
     */
    rtsSendPower= 0;


}

//处理txRtsEvent事件
//发送RTS
void Sensor::startTxRts(cMessage* msg){
    ASSERT(msg==txRtsEvent);
    //if (controlState == HANDING_TRAN or controlState == READY_TRAN)
    //    {
    //       controlState = TRAN;
            RTS* rts = RTSSendDeque.front();//从RTSSendDeque队列中取出第一个元素。


            rts->setRts_sendPower(rtsSendPower);
            //计算传播延迟。距离/1500；;
            //double distance = std::sqrt(std::pow(x - serverX, 2) + std::pow(y - serverY, 2) + std::pow(z - serverZ, 2));
            double distance = std::sqrt(std::pow(x - serverX, 2) + std::pow(y - serverY, 2));
            double delay = 0;
            /*if (distance <= maxTranRange->doubleValue()){//没意义
                delay = distance / 1500.0;
            }*/
            delay = distance / 1500.0;
            //传输延迟 bit/数据传输速率
            //simtime_t duration = rts->getBitLength() / txRate->doubleValue();
/*
            double txPower = 1.0;  // 发送功率(W)，可配置
            double txEnergy = txPower *distance* duration.dbl();
 */
            /* ---------- ① Thorp 吸收系数 α(f) (dB/m) ---------- */
            double f2 = freq_kHz->doubleValue() * freq_kHz->doubleValue();
            double alpha_dBPerKm = 0.11 * f2 / (1 + f2)       // 硼酸弛豫
                                 + 44  * f2 / (4100 + f2)    // MgSO4 弛豫
                                 + 2.75e-4 * f2              // 纯水粘滞
                                 + 0.003;                    // 低频附加项
            double alpha_dBPerM = alpha_dBPerKm / 1000.0;    // 转成 dB/m  :contentReference[oaicite:0]{index=0}

            /* ---------- ② 传播损失 & 源级 ---------- */

            double TL_dB = 20 * log10(distance) + alpha_dBPerM * distance; // 20logd + αd
            double SL_dB = TL_dB + noise_dB->doubleValue() - di_rx_dB->doubleValue() + snrMin_dB->doubleValue()+margin_dB->doubleValue();        // 声纳方程

            /* ---------- ③ 声功率 -> 电功率 ---------- */
            double Pac_W = pow(10.0, (SL_dB - 170.8) / 10.0);              // 170.8 常数  :contentReference[oaicite:1]{index=1}
            double Ptx_omni_W = Pac_W / eta->doubleValue();                               // 考虑效率
            double Ptx_W = Ptx_omni_W * pow(10.0, -di_tx_dB->doubleValue() / 10.0);       // 指向性节省

            /* ---------- ④ 计算持续时间 & 能量 ---------- */
            simtime_t duration = rts->getBitLength() / txRate->doubleValue();
            double txEnergy_J = Ptx_W * duration.dbl();                    // 能量 = 功率 × 时间




            totalTxEnergy += txEnergy_J;
            //energyVec.record(txEnergy);

            //打印RTS发送队列缓存。
            std::cout << "Sensor(id:"<<id<<")RTS发送队列: " << std::endl;
            for (const auto& rtsPtr : RTSSendDeque) {
                if (rtsPtr) {
                    std::cout <<"        "<< rtsPtr->getRts_name() << std::endl;
                } else {
                    std::cout << "nullptr in RTSdeque" << std::endl;
                }
            }
            std::cout<<"发送RTS"<<"   Current simulation time: " << simTime() << std::endl;
            std::cout <<"   Sensor("<<id <<"):发送了一条RTS。"<<rts->getRts_name()<< std::endl;
            //rts->setDisplayString("b=15,15,rect;o=blue");
            EV << "RTS is of type: " << rts->getClassName() << endl;
            sendDirect(rts->dup(),delay+duration, duration, server->gate("in"));

            TransmittedRTSCount++;

            /*delete rts;
            rts = nullptr;

            scheduleAt(simTime() + duration, txRtsEvent);

            */
        //}
            RTSSendDeque.pop_front();
}


void Sensor::startTxData(cMessage* msg){

    ASSERT(msg==txDataEvent);
        if (1>=0)
                {
           std::cout <<"sensor("<<id<< ")进入startTxData函数 " <<"Current simulation time: " << simTime() << std::endl;

        /*
             * 参考uan-aloha的初版
            if (dataState == HANDING_TRAN or dataState == READY_TRAN)
            {

                dataState = TRAN;
                Data* data = dataSendDeque.front();//从DataSendDeque队列中取出第一个元素。
                //RTSSendDeque.pop_front();

                //data->setData_sendPower(dataSendPower);

                //实际不需要计算，传播延迟，只需要按照server调度的方向和能量发送即可
                //由于实现较难，故依然使用sendDirect方法，可以达到实际需求
                //计算传播延迟。距离/1500；;
                double distance = std::sqrt(std::pow(x - serverX, 2) + std::pow(y - serverY, 2));
                double delay = 0;
                if (distance <= maxTranRange->doubleValue()){
                    delay = distance / 1500.0;
                }

                //传输延迟 bit/数据传输速率
                simtime_t duration = data->getBitLength() / txRate->doubleValue();
                //从server获取目的节点（方向）
                sendDirect(data->dup(),delay, duration, server->gate("in1"));
               */

            //5.15新加的DATA发送流程，
                if (!CTSRecvDeque.empty()) {
                    std::cout <<"sensor("<<id<< ")CTSRecvDeque不为空 " <<"Current simulation time: " << simTime() << std::endl;

                        CTS* cts = CTSRecvDeque.back();
                        CTSRecvDeque.pop_front();
                        //从CTS中取出 data，txtime
                        int Cts_dataID=cts->getCts_dataID();
                        cts->getCts_TxTime();
                        cts->getCts_TxPower();
                        cts->getCts_TxTime();
                        double delay=cts->getCts_DataTxDelay1()+cts->getCts_DataTxDelay2();
                       //在dataSendDeque中找到dataid=cts中的data 的data数据
                        Data* dataToSend = nullptr;
                        for (auto data : dataSendDeque) {

                            if (data->getData_id() == Cts_dataID) {
                                dataToSend = data;
                                std::cout <<"sensor("<<id<< ")找到CTS中的data " <<"Current simulation time: " << simTime() << std::endl;

                                break;
                            }
                        }
                        if (!dataToSend) {
                            EV << "Sensor(" << getId() << "): 未找到 Data ID = " << dataID << "，跳过该 CTS。" << endl;
                              // 处理下一条 CTS
                        }
                        // 如果目标节点是 AUV
                        if (dataToSend->getData_destId() == auv->getId()) {
                            std::cout <<"sensor("<<id<< ")的data的目标是auv " <<"Current simulation time: " << simTime() << std::endl;
                            std::cout <<"发送Data"<<"   Current simulation time: " << simTime() << std::endl;

                            /* ---------- ① Thorp 吸收系数 α(f) (dB/m) ---------- */
                            double f2 = freq_kHz->doubleValue() * freq_kHz->doubleValue();
                            double alpha_dBPerKm = 0.11 * f2 / (1 + f2)       // 硼酸弛豫
                                                 + 44  * f2 / (4100 + f2)    // MgSO4 弛豫
                                                 + 2.75e-4 * f2              // 纯水粘滞
                                                 + 0.003;                    // 低频附加项
                            double alpha_dBPerM = alpha_dBPerKm / 1000.0;    // 转成 dB/m  :contentReference[oaicite:0]{index=0}

                            /* ---------- ② 传播损失 & 源级 ---------- */
                            double distance = getDistanceToAUV();
                            double TL_dB = 20 * log10(distance) + alpha_dBPerM * distance; // 20logd + αd
                            double SL_dB = TL_dB + noise_dB->doubleValue() - di_rx_dB->doubleValue() + snrMin_dB->doubleValue()+margin_dB->doubleValue();        // 声纳方程

                            /* ---------- ③ 声功率 -> 电功率 ---------- */
                            double Pac_W = pow(10.0, (SL_dB - 170.8) / 10.0);              // 170.8 常数  :contentReference[oaicite:1]{index=1}
                            double Ptx_omni_W = Pac_W / eta->doubleValue();                               // 考虑效率
                            double Ptx_W = Ptx_omni_W * pow(10.0, -di_tx_dB->doubleValue() / 10.0);       // 指向性节省

                            /* ---------- ④ 计算持续时间 & 能量 ---------- */
                            simtime_t duration = dataToSend->getBitLength() / txRate->doubleValue();
                            double txEnergy_J = Ptx_W * duration.dbl();                    // 能量 = 功率 × 时间

                            if (!std::isfinite(txEnergy_J) || txEnergy_J < 0) {
                                EV_WARN << "Non-finite txEnergy at node " << getFullPath()
                                        << " d=" << distance
                                        << " Ptx=" << Ptx_W
                                        << " dur=" << duration
                                        << " eta=" << eta->doubleValue()
                                        << " TL=" << TL_dB
                                        << " SL=" << SL_dB
                                        << endl;
                                txEnergy_J = 0.0;   // 不污染总量
                            }
                            totalTxEnergy += txEnergy_J;
                            TransmittedDistance += distance;
                            dataToSend->setSendingTime(simTime());
                            sendDirect(dataToSend->dup(),delay,0, auv->gate("in_data1"));
                            TransmittedDataCount++;
                            std::cout <<"    Sensor("<<id <<"):发送了一条Data。"<<dataToSend->getData_name()<<"Current simulation time: " << simTime() << std::endl;
                            std::cout <<"    Sensor("<<id <<"):发送了一条Data。"<<dataToSend->getSendingTime()<<"Current simulation time: " << simTime() << std::endl;


                        }else{//发送给普通节点的data


                            for (auto it = neighborMap.begin(); it != neighborMap.end(); ++it)
                            {
                               // std::cout << "[DEBUG] neighbor module id: " << it->first->getId()
                               //               << " (" << it->first->getFullPath() << ")" << std::endl;
                                if (it->first->getId() ==  dataToSend->getData_destId())//判断找到的sensor的id是否与data的目的id相同。
                                {
                                    std::cout <<"sensor("<<id<< ")找到data的目标id " <<"Current simulation time: " << simTime() << std::endl;
                                    std::cout <<"neighborMap  first->getId()"<<it->first->getId()<<"Current simulation time: " << simTime() << std::endl;
                                    EV <<"neighborMap  first->getId()"<<it->first->getId()<<"Current simulation time: " << simTime() << std::endl;

                                        std::cout <<"发送Data"<<"   Current simulation time: " << simTime() << std::endl;
   /*
                                        simtime_t duration = dataToSend->getBitLength() / txRate->doubleValue();
                                        double txPower = 1.0;  // 假设为 1W（建议用 par("txPower") 配置）
                                        double txEnergy = txPower * it->second*duration.dbl();
                                        totalTxEnergy += txEnergy;
                                        //energyVec.record(txEnergy);
     */

                                        /* ---------- ① Thorp 吸收系数 α(f) (dB/m) ---------- */
                                        double f2 = freq_kHz->doubleValue() * freq_kHz->doubleValue();
                                        double alpha_dBPerKm = 0.11 * f2 / (1 + f2)       // 硼酸弛豫
                                                             + 44  * f2 / (4100 + f2)    // MgSO4 弛豫
                                                             + 2.75e-4 * f2              // 纯水粘滞
                                                             + 0.003;                    // 低频附加项
                                        double alpha_dBPerM = alpha_dBPerKm / 1000.0;    // 转成 dB/m  :contentReference[oaicite:0]{index=0}

                                        /* ---------- ② 传播损失 & 源级 ---------- */
                                        double distance=it->second;
                                        double TL_dB = 20 * log10(distance) + alpha_dBPerM * distance; // 20logd + αd
                                        double SL_dB = TL_dB + noise_dB->doubleValue() - di_rx_dB->doubleValue() + snrMin_dB->doubleValue()+margin_dB->doubleValue();        // 声纳方程

                                        /* ---------- ③ 声功率 -> 电功率 ---------- */
                                        double Pac_W = pow(10.0, (SL_dB - 170.8) / 10.0);              // 170.8 常数  :contentReference[oaicite:1]{index=1}
                                        double Ptx_omni_W = Pac_W / eta->doubleValue();                               // 考虑效率
                                        double Ptx_W = Ptx_omni_W * pow(10.0, -di_tx_dB->doubleValue() / 10.0);       // 指向性节省

                                        /* ---------- ④ 计算持续时间 & 能量 ---------- */
                                        simtime_t duration = dataToSend->getBitLength() / txRate->doubleValue();
                                        double txEnergy_J = Ptx_W * duration.dbl();                    // 能量 = 功率 × 时间

                                        if (!std::isfinite(txEnergy_J) || txEnergy_J < 0) {
                                            EV_WARN << "Non-finite txEnergy at node " << getFullPath()
                                                    << " d=" << distance
                                                    << " Ptx=" << Ptx_W
                                                    << " dur=" << duration
                                                    << " eta=" << eta->doubleValue()
                                                    << " TL=" << TL_dB
                                                    << " SL=" << SL_dB
                                                    << endl;
                                            txEnergy_J = 0.0;   // 不污染总量
                                        }

                                        //emit(txEnergySignal, txEnergy_J);      // 若已注册信号
                                        totalTxEnergy += txEnergy_J;
                                        TransmittedDistance += distance;
                                        std::cout <<"    Sensor("<<id <<"):txEnergy_J="<<txEnergy_J<<"Current simulation time: " << simTime() << std::endl;
                                        std::cout <<"    Sensor("<<id <<"):distance="<<distance<<"Current simulation time: " << simTime() << std::endl;



                                        std::cout <<"    Sensor("<<id <<"):dataLenBit="<<dataToSend->getBitLength() <<"Current simulation time: " << simTime() << std::endl;
                                        std::cout <<"    Sensor("<<id <<"):delay="<<delay<<std::endl;
                                        std::cout <<"    Sensor("<<id <<"):delay1="<<cts->getCts_DataTxDelay1()<<"  delay2="<<cts->getCts_DataTxDelay2()<< std::endl;


                                        //dataToSend->setSendingTime(simTime());
                                        sendDirect(dataToSend->dup(),delay,0, it->first->gate("in_data1"));
                                        TransmittedDataCount++;
                                        std::cout <<"    Sensor("<<id <<"):发送了一条Data。"<<dataToSend->getData_name()<<"Current simulation time: " << simTime() << std::endl;
                                        std::cout <<"    Sensor("<<id <<"):发送了一条Data。生成数据时间"<<dataToSend->getSendingTime()<<"Current simulation time: " << simTime() << std::endl;

   /*
                                    // 计算延迟
                                     auto destCoord = coordinateList_map.at(dataToSend->getData_destId());
                                     double distance = std::sqrt(std::pow(std::get<0>(destCoord) - x, 2) +
                                                                 std::pow(std::get<1>(destCoord) - y, 2));
                                     double propagationDelay = distance / 1500.0;
                                     double transportationDelay = dataToSend->getData_dataLenBit() / txRate->doubleValue();


                                     std::cout <<"Sensor("<<id <<"):发送了一条Data。"<<dataToSend->getData_name()<<"Current simulation time: " << simTime() << std::endl;


                                     sendDirect(dataToSend->dup(),propagationDelay, transportationDelay, it->first->gate("in_data1"));
   */


                                    break;  // 找到后退出循环（如果只找一个目标）
                                }
                            }

                        }

                    }

                }


}

void Sensor::endRxData(cMessage* msg){


}
//获取当前sensor和auv的距离
double Sensor::getDistanceToAUV() {
    if (!auv) {
        auv = getModuleByPath("auv");  // 或根据实际路径
        if (!auv) return -1;  // 错误处理
    }
    double auvX = auv->par("x").doubleValue();
    double auvY = auv->par("y").doubleValue();
    return std::sqrt(std::pow(x - auvX, 2) + std::pow(y - auvY, 2));
}
// 计算下一阶段开始时间
simtime_t Sensor::calculateNextPhaseStartTime() const {
    simtime_t slotSum = slotTime1 + slotTime2;
    simtime_t currentTime = simTime();

    // 计算当前时间对slotSum的余数
    simtime_t remainder = fmod(currentTime.dbl(), slotSum.dbl());

    // 计算下一阶段开始时间
    simtime_t t_next = currentTime + (slotSum - remainder);

    return t_next;
}
// 预测指定时间点的AUV位置
std::pair<double, double> Sensor::predictAuvPositionAtTime(simtime_t targetTime) const {
    if (!auv) {
        return {-1, -1};  // 错误值
    }

    // 获取AUV当前参数
    double currentX = auv->par("x").doubleValue();
    double currentY = auv->par("y").doubleValue();

    // 获取AUV运动参数（从AUV模块或配置中获取）
    double vx = auv->hasPar("vx") ? auv->par("vx").doubleValue() : 0.0;
    double vy = auv->hasPar("vy") ? auv->par("vy").doubleValue() : 0.0;

    // 计算时间差
    double dt = (targetTime - simTime()).dbl();

    // 预测位置（简单线性预测）
    double predictedX = currentX + vx * dt;
    double predictedY = currentY + vy * dt;

    return {predictedX, predictedY};
}
// 计算指定时间点与AUV的距离
double Sensor::getDistanceToAuvAtTime(simtime_t targetTime) const {
    auto [auvX, auvY] = predictAuvPositionAtTime(targetTime);

    if (auvX < 0 || auvY < 0) {
        return -1;  // 错误值
    }

    // 计算距离
    double distance = std::sqrt(std::pow(x - auvX, 2) + std::pow(y - auvY, 2));
    return distance;
}

void Sensor::processCTSData(CTS* cts) {
    std::cout << "sensor(" << id << ") 处理CTS: " << cts->getCts_name()
              << " 时间: " << simTime() << std::endl;

    // 1. 从CTS中提取信息
    int ctsDataID = cts->getCts_dataID();
    simtime_t ctsTxTime = cts->getCts_TxTime();
    double ctsTxPower = cts->getCts_TxPower();
    double ctsTxDirection = cts->getCts_TxDirection();
    double delay = cts->getCts_DataTxDelay1() + cts->getCts_DataTxDelay2();

    std::cout << "CTS信息: dataID=" << ctsDataID
              << " txTime=" << ctsTxTime
              << " txPower=" << ctsTxPower << std::endl;

    // 2. 在dataSendDeque中找到对应的数据
    Data* dataToSend = nullptr;
    for (auto it = dataSendDeque.begin(); it != dataSendDeque.end(); ++it) {
        if ((*it)->getData_id() == ctsDataID) {
            dataToSend = *it;
            std::cout << "sensor(" << id << ") 找到CTS对应的数据: "
                      << dataToSend->getData_name() << std::endl;
            break;
        }
    }

    if (!dataToSend) {
        std::cout << "sensor(" << id << ") 错误：未找到Data ID=" << ctsDataID
                  << " 对应的数据" << std::endl;
        return;
    }

    // 3. 检查数据目标并发送
    if (dataToSend->getData_destId() == auv->getId()) {
        // 目标为AUV的数据发送
        std::cout << "sensor(" << id << ") 数据目标是AUV，开始发送" << std::endl;
        sendDataToAUV(dataToSend, cts);
    } else {
        // 目标为其他传感器的数据发送
        std::cout << "sensor(" << id << ") 数据目标是传感器，开始发送" << std::endl;
        sendDataToSensor(dataToSend, cts);
    }

    // 4. 从队列中移除已处理的数据
    for (auto it = dataSendDeque.begin(); it != dataSendDeque.end(); ++it) {
        if ((*it)->getData_id() == ctsDataID) {
            dataSendDeque.erase(it);
            break;
        }
    }
}

void Sensor::sendDataToAUV(Data* data, CTS* cts) {
    std::cout << "sensor(" << id << ") 发送数据到AUV: " << data->getData_name() << std::endl;

    // 计算到AUV的距离
    double distance = getDistanceToAUV();
    if (distance < 0) {
        std::cout << "sensor(" << id << ") 错误：无法获取AUV距离" << std::endl;
        return;
    }

    // 计算能量和延迟
    double txEnergy = calculateTxEnergy(data, distance);
    double propagationDelay = distance / 1500.0;
    simtime_t duration = data->getBitLength() / txRate->doubleValue();
    double totalDelay = cts->getCts_DataTxDelay1() + cts->getCts_DataTxDelay2();
    std::cout << "     sensor(" << id << ") ：totalTxEnergy" << totalTxEnergy<< std::endl;
    std::cout << "sensor(" << id << ") 发送参数: 距离=" << distance
              << " 能量=" << txEnergy << " 延迟=" << propagationDelay << std::endl;
    // 更新统计信息
    totalTxEnergy += txEnergy;
    TransmittedDistance += distance;
    TransmittedDataCount++;
    std::cout << "     sensor(" << id << ") ：totalTxEnergy" << totalTxEnergy<< std::endl;


    // 直接发送到AUV,10.18之前，应该是（totalDelay，0）
    sendDirect(data->dup(), propagationDelay, duration, auv->gate("in_data1"));
    //sendDirect(data->dup(), totalDelay, 0, auv->gate("in_data1"));
    //
}


void Sensor::sendDataToSensor(Data* data, CTS* cts) {
    std::cout << "sensor(" << id << ") 发送数据到传感器: " << data->getData_name() << std::endl;

    // 使用neighborMap查找目标传感器（参考第853行代码）
    cModule* targetSensor = nullptr;
    double distance = 0.0;

    for (auto it = neighborMap.begin(); it != neighborMap.end(); ++it) {
        if (it->first->getId() == data->getData_destId()) {
            targetSensor = it->first;
            distance = it->second;  // neighborMap中存储的是距离
            std::cout << "sensor(" << id << ") 找到目标传感器 ID=" << data->getData_destId()
                      << " 距离=" << distance << std::endl;
            break;
        }
    }

    if (!targetSensor) {
        std::cout << "sensor(" << id << ") 错误：未找到目标传感器 ID="
                  << data->getData_destId() << std::endl;
        return;
    }

    // 计算能量（使用现有的能量计算逻辑）
    double txEnergy = calculateTxEnergy(data, distance);

    // 计算延迟
    double propagationDelay = distance / 1500.0;
    simtime_t duration = data->getBitLength() / txRate->doubleValue();
    double totalDelay = cts->getCts_DataTxDelay1() + cts->getCts_DataTxDelay2();
    std::cout << "     sensor(" << id << ") ：totalTxEnergy" << totalTxEnergy<< std::endl;
    std::cout << "sensor(" << id << ") 发送参数: 距离=" << distance
              << " 能量=" << txEnergy << " 延迟=" << propagationDelay << std::endl;
    // 更新统计信息
    totalTxEnergy += txEnergy;
    TransmittedDistance += distance;
    TransmittedDataCount++;
    std::cout << "     sensor(" << id << ") ：totalTxEnergy" << totalTxEnergy<< std::endl;


    // 发送到目标传感器（参考第920行代码）
    sendDirect(data->dup(), totalDelay, 0, targetSensor->gate("in_data1"));
}


double Sensor::calculateTxEnergy(Data* data, double distance) {
    // 使用现有的能量计算逻辑
    double f2 = freq_kHz->doubleValue() * freq_kHz->doubleValue();
    double alpha_dBPerKm = 0.11 * f2 / (1 + f2) + 44 * f2 / (4100 + f2) + 2.75e-4 * f2 + 0.003;
    double alpha_dBPerM = alpha_dBPerKm / 1000.0;

    double TL_dB = 20 * log10(distance) + alpha_dBPerM * distance;
    double SL_dB = TL_dB + noise_dB->doubleValue() - di_rx_dB->doubleValue() + snrMin_dB->doubleValue()+margin_dB->doubleValue();

    double Pac_W = pow(10.0, (SL_dB - 170.8) / 10.0);
    double Ptx_omni_W = Pac_W / eta->doubleValue();
    double Ptx_W = Ptx_omni_W * pow(10.0, -di_tx_dB->doubleValue() / 10.0);

    simtime_t duration = data->getBitLength() / txRate->doubleValue();
    double txEnergy_J = Ptx_W * duration.dbl();
    std::cout << "     sensor(" << id << ") " << data->getData_destId()<< " 距离=" << distance << "     data->getBitLength()=" << data->getBitLength()<< std::endl;
    if (!std::isfinite(txEnergy_J) || txEnergy_J < 0) {
        txEnergy_J = 0.0;
    }

    return txEnergy_J;


}
