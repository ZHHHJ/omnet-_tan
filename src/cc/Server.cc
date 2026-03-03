#include"Server.h"

Define_Module(Server);


Server::Server()
{
    RxRtsEvent = new cMessage("RxRtsEvent");//调度接收RTS事件
    computeDispachEvent= new cMessage("computeDispachEvent");//调度调度算法
    eventHandlerss[RxRtsEvent] = &Server::RxRts;
    eventHandlerss[computeDispachEvent] = &Server::dispachAlgorithm;
    txCtsEvent = new cMessage("txCtsEvent");//调度发送cts事件
    eventHandlerss[txCtsEvent] = &Server::TxCts;
    txCtsEvent2 = new cMessage("txCtsEvent2");//调度发送cts2事件
    eventHandlerss[txCtsEvent2] = &Server::TxCts2;
    testEvent = new cMessage("testEvent");//调度test事件
    eventHandlerss[testEvent] = &Server::test;
    requestInfoList = new std::vector<std::tuple<int,int,int,int,int,int,int,double,double,double,std::string,double,bool>>();
    //std::cout << "Server::Server() end" << std::endl;
}



Server::~Server()
{
// 清理其他资源，如消息
    cancelAndDelete(RxRtsEvent);
    cancelAndDelete(computeDispachEvent);
    cancelAndDelete(txCtsEvent);
    cancelAndDelete(testEvent);
    // 清理 requestInfoList 内存
    if (requestInfoList) {
        delete requestInfoList;
        requestInfoList = nullptr;
    }



}

void Server::finish()
{


}

void Server::initialize()
{
    id = getId();
    //std::cout << "Server(id:"<<id<<")::initialize() start" << std::endl;
    SNR_Threshold = &par("SNR_Threshold");
    //std::cout << "Server(id:"<<id<<")::initialize() SNR_Threshold:"<<SNR_Threshold << std::endl;
    slotTime1= par("slotTime1");
    slotTime2= par("slotTime2");
    maxTranRange= &par("maxTranRange");
    numRTS=0;//接收RTS的数量
    txRate = &par("txRate");
    x = par("x");
    y = par("y");
    //auv = getModuleByPath("auv");
    //auvX = auv->par("x");
    //auvY = auv->par("y");

    ctsLenBit= &par("ctsLenBit");
    //z = par("z");

    //gate("in")->setDeliverOnReceptionStart(true);
    //你收到的警告表示 setDeliverOnReceptionStart 被弃用，并建议使用 setDeliverImmediately 函数。你可以将 Server.cc 文件中的相关代码进行修
    gate("in")->setDeliverImmediately(true);
    //gate("in")->setDeliverOnReceptionStart(true);

    network = getParentModule();
    auv = getModuleByPath("auv");
    if (!network) {
        EV << "Error: network module not found." << endl;
        throw cRuntimeError("network module not found");
    }
    numSensors = network->par("numSensors");
    //numSensors = network->par("numSensors");

    //getRout();

    currentCollisionNumFrames = 0;

    controlChannelBusy = false;

    controlState = IDLE;

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
    if (auv) {
        double auvXCur = auv->par("x").doubleValue();
        double auvYCur = auv->par("y").doubleValue();
        double distanceAuv = std::sqrt(std::pow(x - auvXCur, 2) + std::pow(y - auvYCur, 2));
        coordinateList_map[auv->getId()] = {auvXCur,auvYCur};
        neighborMap[auv] = distanceAuv;

    }
    //打印所有 sensor->getId(),测试连续发送cts的bug
    for (auto it = neighborMap.begin(); it != neighborMap.end(); ++it)
    {
        cModule* sensor = it->first;
        if (sensor != nullptr)
        {
            std::cout << "Sensor ID: " << sensor->getId() << std::endl;
        }
    }
    // 打印 neighborMap 的内容
    std::cout << "Neighbor Map (Server -> Sensor):" << std::endl;
    for (const auto& entry : neighborMap) {
        cModule* sensorModule = entry.first;
        double distance = entry.second;

        if (sensorModule) {
            std::cout << "Sensor[" << sensorModule->getFullName()
                      << "] (ID=" << sensorModule->getId()
                      << ") -> Distance: " << distance << " meters" << std::endl;
        }
    }




    //std::cout << "Current simulation time: " << simTime() << std::endl;

    //scheduleAt(simTime(), testEvent);
    scheduleAt(simTime()+slotTime1-1, computeDispachEvent);//第五秒开始计算调度信息

    //std::cout << "Server(id:"<<id<<")::initialize() end" << std::endl;

}

void  Server::handleMessage(cMessage* msg)
{
    //判断事件是否在eventHandlers中
    auto it = eventHandlerss.find(msg);
    //在eventHandlers中，使用Sensor()中设置方法。
    if (it != eventHandlerss.end()){
            EventFunctionPtr handler = it->second;
            (this->*handler)(msg);
    }else if (auto rts = dynamic_cast<RTS*>(msg)) {
        std::cout<<"                                                                收到RTS    "<<rts->getRts_name()<<"    current simulation time "<<simTime()<<endl;
        // 确保 requestList 的大小足够
        if (numRTS >= 0 /*&& numRTS < static_cast<int>(requestList.size())*/) {
       // if (requestList.size() > static_cast<size_t>(numRTS)) {
            requestList[numRTS] = {
                rts->getRts_srcID(),
                rts->getRts_dataDestID(),
                rts->getRts_dataID(),
                rts->getRts_dataLenBit(),
                rts->getRts_state(),
                rts->getRts_name(),
            };
            numRTS++;
            //scheduleAt(simTime(), computeDispachEvent);
            //打印requestList
            EV << "Server reveived mew RTS" << numRTS << endl;
            /*
            std::cout << "requestList: " << std::endl;
            for (const auto& [key, value] : requestList) {
                    std::cout << "        RTS(" << key << ")：源地址，data目的地址，dataId,数据长度：("
                              << std::get<0>(value) << ", "
                              << std::get<1>(value) << ", "
                              << std::get<2>(value) << ", "
                              << std::get<3>(value) << ")" << std::endl;

                }
            std::cout << "------------------------" << std::endl;
            std::cout << "***************接收一次结束*****************" << std::endl;
             */

        } else {
            EV << "Warning: requestList index out of bounds. numRTS = " << numRTS << endl;
        }
    } else {
        // 处理其他类型的消息
        EV << "Received unknown message type\n";
        delete msg; // 避免内存泄漏
    }
}




void Server::RxRts(cMessage* msg){

    //requestList[sensor->getId()] = {sensor->getId(),sensorX,sensorY}

}


void Server::dispachAlgorithm(cMessage* msg)
{
    //获取当前时间的auv的位置
    //cModule* auv = getModuleByPath("auv");
    if (auv) {
        double auvX = auv->par("x").doubleValue();
        double auvY = auv->par("y").doubleValue();
        std::cout<<"当前AUV位于("<< auvX
                          << ", " << auvY<< ")  t="
                          << simTime() <<"   Current simulation time: " << simTime() << std::endl;
        // 将 AUV 位置用于调度决策
        double distanceAuv = std::sqrt(std::pow(x - auvX, 2) + std::pow(y - auvY, 2));
        coordinateList_map[auv->getId()] = {auvX, auvY};
        neighborMap[auv] = distanceAuv;
    }


    ASSERT(msg==computeDispachEvent);

    // 计算下一阶段开始时间
    simtime_t t_next = calculateNextPhaseStartTime();
    std::cout << "Server调度算法开始，t_next=" << t_next << std::endl;
    // 获取AUV在t_next时刻的预测位置
    auto [auvX, auvY] = predictAuvPositionAtTime(t_next);
    std::cout << "AUV在t_next时刻预测位置: (" << auvX << ", " << auvY << ")" << std::endl;


    std::cout << "                                                                ###################################Server(id:"<<id<<")::dispachAlgorithm start," << "Current simulation time: " << simTime()  << std::endl;

    if (requestList.empty()) {
        std::cout << "Warning: requestList is empty" << std::endl;
        scheduleAt(simTime()+slotTime1+slotTime2, computeDispachEvent);//每轮第五秒开始计算调度信息
        return;
    }

    if (!requestInfoList) {//判断 requestInfoList是否为空，
        std::cout << "Error: requestInfoList is not initialized" << std::endl;
        return;
    }else{//判断 requestInfoList是否为空不为空则清空容器requestInfoList,保证每次执行调度算法的时候没有旧RTS存在
        requestInfoList->clear();
    }

/*
    //每一轮打印一次requestList（在每slotTime1+slotTime2：6+2=8s）
    std::cout << "                                                                ┍-----------------------------------------------------------------------┐" << std::endl;
    std::cout << "                                                                每一轮的requestList: " << std::endl;
    std::cout << "                                                                   RTSID,源地址，data目的地址，dataId,数据长度,状态（是否被读取）,rts_name"<< std::endl;
    for (const auto& [key, value] : requestList) {
            std::cout << "                                                                   RTS" << key << ":(  "
                      << std::get<0>(value) << ",       "
                      << std::get<1>(value) << ",       "
                      << std::get<2>(value) << ",       "
                      << std::get<3>(value) << ",       "
                      << std::get<4>(value) << ",       "
                      << std::get<5>(value) << "       )" << std::endl;

        }
    std::cout << "                                                                ┕-----------------------------------------------------------------------┘" << std::endl<< std::endl;;
*/

    // 重新排序requestList：AUV请求优先
    std::vector<std::pair<int, std::tuple<int,int,int,int,int,std::string>>> auvRequests;
    std::vector<std::pair<int, std::tuple<int,int,int,int,int,std::string>>> regularRequests;

    //std::vector<std::vector<Sector>> groups;
    //将RTS信息转化成 坐标、角度、半径、数据长度信息的组合
    // 遍历请求列表
    for (const auto& request : requestList) {

        int rtsID = request.first;
        int srcID = std::get<0>(request.second);  // 获取源ID
        int destID = std::get<1>(request.second); // 获取目标ID
        int dataID = std::get<2>(request.second);//获取数据ID
        int dataLenBit = std::get<3>(request.second);//获取数据长度
        int state = std::get<4>(request.second);//获取状态
        std::string rts_name = std::get<5>(request.second);//获取RTS——name
        //修改requestList中的状态，增加判断条件，避免重复读取
       /* if (state == 1) {  // 如果已经被处理过
            std::cout << "RTS " << rtsID << " 已经被处理过，跳过" << std::endl;
            continue;  // 跳过这个RTS
        }*/
        if (auv && destID == auv->getId()) {
                    auvRequests.push_back(request);
                } else {
                    regularRequests.push_back(request);
                }
        // 只处理待处理的RTS
        if (state != 0) {
            continue;
        }

        if (state == 0) {  // 该RTS待处理
                    std::cout << "                                                                RTS " << rtsID << " 待处理" << std::endl;
        }




        //std::cout << "┍-------------------------------------------------------------------┐" << std::endl;
        //std::cout << "处理requestList中的一个RTS" << std::endl;
       // std::cout << "Server(id:"<<id<<")::dispachAlgorithm requestList rtsID "<<rtsID<<"被读取"<< std::endl;
        // 确保 coordinateList_map 包含 srcID 和 destID
        if (coordinateList_map.find(srcID) == coordinateList_map.end() || coordinateList_map.find(destID) == coordinateList_map.end()) {
            std::cout<< "Error: coordinateList_map missing entry for srcID " << srcID << " or destID " << destID <<std::endl;
            continue;
        }
        // 从coordinateList_map获取源和目标传感器的坐标
        auto srcCoord = coordinateList_map.at(srcID);
        double srcX = std::get<0>(srcCoord);
        double srcY = std::get<1>(srcCoord);
        // 判断目标是否为AUV
        cModule* auv = getModuleByPath("auv");
        if (auv && destID == auv->getId()) {

            // 处理AUV目标请求
            double distance = std::sqrt(std::pow(auvX - srcX, 2) + std::pow(auvY - srcY, 2));
            double angle = std::atan2(auvY - srcY, auvX - srcX) * (180.0 / M_PI);
            if (angle < 0) angle += 360.0;

            double propagationDelay = distance / 1500.0;
            double transportationDelay = static_cast<double>(dataLenBit) / txRate->doubleValue();
            double delay = propagationDelay + transportationDelay;
            double ctsTransportationDelay = ctsLenBit->intValue() / txRate->doubleValue();

            // 添加到requestInfoList，标记为AUV请求
            requestInfoList->push_back({
                rtsID, srcID, destID, dataID, dataLenBit,
                static_cast<int>(srcX), static_cast<int>(srcY),
                distance, delay, angle, rts_name, ctsTransportationDelay,
                true  // 新增字段：是否为AUV请求
            });
            // 更新状态为已处理
            requestList[rtsID] = {

                std::get<0>(request.second),  // srcID
                std::get<1>(request.second),  // destID
                std::get<2>(request.second),  // dataID
                std::get<3>(request.second),  // dataLenBit
                1 , // 设置状态为已处理
                std::get<5>(request.second)

            };

            std::cout << "AUV请求: RTS" << rtsID << " 距离=" << distance
                      << " 角度=" << angle << " 延迟=" << delay << std::endl;

        }

    }

    for (const auto& request : requestList) {

        int rtsID = request.first;
        int srcID = std::get<0>(request.second);  // 获取源ID
        int destID = std::get<1>(request.second); // 获取目标ID
        int dataID = std::get<2>(request.second);//获取数据ID
        int dataLenBit = std::get<3>(request.second);//获取数据长度
        int state = std::get<4>(request.second);//获取状态
        std::string rts_name = std::get<5>(request.second);//获取RTS——name
        // 只处理待处理的RTS
        if (state != 0) {
            continue;
        }

        if (auv && destID == auv->getId()) {
            continue;
        }

            // 处理静态传感器目标请求（原有逻辑）

        if (coordinateList_map.find(destID) == coordinateList_map.end()) {
                std::cout << "Error: coordinateList_map missing entry for destID " << destID << std::endl;
                continue;
        }


        // 从coordinateList_map获取源和目标传感器的坐标
        auto srcCoord = coordinateList_map.at(srcID);
        double srcX = std::get<0>(srcCoord);
        double srcY = std::get<1>(srcCoord);
        auto destCoord = coordinateList_map.at(destID);
        //std::cout << "在坐标表中读取了源节点坐标和目的节点坐标"<< "Current simulation time: " << simTime()  << std::endl;

        // 计算源到目标的距离（欧几里得距离）
        double distance = std::sqrt(std::pow(std::get<0>(destCoord) - std::get<0>(srcCoord), 2) +
                                    std::pow(std::get<1>(destCoord) - std::get<1>(srcCoord), 2));

        //std::cout << "计算源到目标的距离（欧几里得距离）："<<  distance  << std::endl;
        double propagationDelay=distance/1500;
        //std::cout << "double propagationDelay=distance/1500;"<< std::endl;
        double transportationDelay=static_cast<double>(dataLenBit)/txRate->doubleValue();
       // std::cout << "double transportationDelay=static_cast<int>(dataLenBit)/txRate->intValue();"<< std::endl;
        double delay=propagationDelay+transportationDelay;
        double ctsTransportationDelay=ctsLenBit->intValue()/txRate->doubleValue();
       // std::cout << "延迟（传输延迟+传播延迟）："<<  delay  << std::endl;
        // 计算源到目标的角度
        double angle = std::atan2(std::get<1>(destCoord) - std::get<1>(srcCoord),
                                  std::get<0>(destCoord) - std::get<0>(srcCoord))* (180.0 / M_PI);
        if (angle < 0)
            angle += 360.0;

       // std::cout << "计算源到目标的角度："<<  angle  << std::endl;
        //添加请求信息到requestInfoList//源id，目的id，数据id，数据长度，（源节点坐标）x,y,距离，角度

        requestInfoList->push_back({
            rtsID,srcID, destID, dataID, static_cast<int>(dataLenBit),
            static_cast<int>(std::get<0>(srcCoord)), 
            static_cast<int>(std::get<1>(srcCoord)), 
            distance, delay,angle,rts_name,ctsTransportationDelay,false  // 非AUV请求
        });


        // 更新状态为已处理
        requestList[rtsID] = {

            std::get<0>(request.second),  // srcID
            std::get<1>(request.second),  // destID
            std::get<2>(request.second),  // dataID
            std::get<3>(request.second),  // dataLenBit
            1 , // 设置状态为已处理
            std::get<5>(request.second)

        };
        std::cout << "rtsID： "<<rtsID<<"  状态为已处理 "<< std::endl;
       // std::cout << "Server(id:"<<id<<")::dispachAlgorithm requestList rtsID "<<rtsID<<"被写入requestInfoList"<< std::endl;
       // std::cout << "┕-------------------------------------------------------------------┘" << std::endl<< std::endl;
       // std::cout << " " << std::endl;




    }

    // 打印requestInfoList
    std::cout << "                                                                ┍---------------------------------------requestInfoList-------------------------------------------------┐" << std::endl;
    for (const auto& info : *requestInfoList) {
        int RTSID = std::get<0>(info);
        int srcID = std::get<1>(info);
        int destID = std::get<2>(info);
        int dataID = std::get<3>(info);
        int dataLenBit1 = std::get<4>(info);
        int srcX = std::get<5>(info);
        int srcY = std::get<6>(info);
        double distance = std::get<7>(info);
        double delay = std::get<8>(info);
        double angle = std::get<9>(info);
        std::string rts_name = std::get<10>(info);
        double ctsTransportationDelay= std::get<11>(info);
        std::cout << "                                                                RTSID: " << RTSID << "源ID: " << srcID << ", 目标ID: " << destID << "数据ID: " << dataID <<  "数据长度 (bit): " << dataLenBit1 << "源节点坐标: (" << srcX << ", " << srcY << ")"  << "传播距离: " << distance  << "总延迟: " << delay << "角度 (radians): " << angle<<"rts_name "<<rts_name << std::endl;
    }
    std::cout << "                                                                ┕-------------------------------------------------------------------------------------------------------┘" << std::endl<< std::endl;




    std::vector<Sector> sectors;

    double angleWidth = 30;

    // 遍历所有请求信息，构造 Sector
    for (const auto& req : *requestInfoList) {
        Sector s;
        s.rtsId = std::get<0>(req);
        s.srcId = std::get<1>(req);
        s.destID= std::get<2>(req);
        s.dataId = std::get<3>(req);
        s.srcX = static_cast<double>(std::get<5>(req));
        s.srcY = static_cast<double>(std::get<6>(req));
        s.distance = std::get<7>(req);
        s.delay= std::get<8>(req);
        s.ctsTransportationDelay=std::get<11>(req);

        double angle = std::get<9>(req);

        s.startAngle = angle - angleWidth;
        s.endAngle = angle + angleWidth;
        s.rts_name = std::get<10>(req);
        sectors.push_back(s);
    }

    // 打印所有 Sector
    std::cout << "                                                                目前收到的所有扇形: " <<"Current simulation time: " << simTime() << std::endl;
    for (const auto& s : sectors) {
        std::cout << "                                                                rtsId:"<<s.rtsId<<"  Sector at (" << s.srcX << ", " << s.srcY << "), angle range: ["
                  << s.startAngle << ", " << s.endAngle << "], distance: "
                  << s.distance << std::endl;
    }

    auto groups = groupSectors_simulatedAnnealing_1(sectors);
    //auto groups =groupSectors(sectors);
    //扇形分组
    sectorGroups=groups;
    //打印扇形分组数据
    std::cout << "                                                                ┍-----------------------------------------------------------------------┐" << std::endl;

    for (size_t i = 0; i < groups.size(); ++i) {
        std::cout << "                                                                扇形分组: " << "Group " << i + 1 << "Group contains " <<  groups[i].size() << " sectors,"<<"(Current simulation time: " << simTime() << ")\n";
        for (const auto& s : groups[i]) {
            std::cout << "                                                                  Sector ID: " << s.rtsId << " at (" << s.srcX << ", " << s.srcY << "), angle range: ["
                      << s.startAngle << ", " << s.endAngle << "], distance: " << s.distance << "\n";
        }
    }
    std::cout << "                                                                ┕-------------------------------------------------------------------------------------------------------┘" << std::endl<< std::endl;

/*
    // 输出分组情况
    for (const auto& group : groups) {
        std::cout << "Group contains " << group.size() << " sectors:\n";
        for (const auto& sector : group) {
            std::cout << "Sector at (" << sector.srcX << ", " << sector.srcY << "), angle range: ["
                      << sector.startAngle << ", " << sector.endAngle << "], distance: " << sector.distance << "\n";
        }
    }

    */
    scheduleAt(simTime()+1, txCtsEvent);//一秒钟计算调度信息。然后发送CTS（5+1+8+8+8+8
    scheduleAt(simTime()+slotTime1+slotTime2, computeDispachEvent);//每轮第五秒开始计算调度信息（5+8+8+8+8+.....
   //调度发送RTS
    //std::cout << "############################################################################" << std::endl;
   // std::cout << "############################################################################" << std::endl;
    std::cout <<"                                                                Current simulation time: " << simTime() << std::endl;
    std::cout << "                                                                ###################################Server(id:"<<id<<")::dispachAlgorithm end" << "\n\n";
/*
    //本轮结束后删除 state==1 的条目
    for (auto it = requestList.begin(); it != requestList.end(); ) {
        if (std::get<4>(it->second) == 1)   // 已成功调度
            it = requestList.erase(it);     // 删除，腾出哈希槽
        else
            ++it;                           // 保留未调度的请求
    }
*/
}

void Server::TxCts(cMessage* msg){

    ASSERT(msg == txCtsEvent);
    std::cout <<"                                                                Current simulation time: " << simTime() << std::endl;
    std::cout << "                                                                Server(id:"<<id<<")::TxCts start" << std::endl;

    //每个扇形组内：CTS中的TxTime是相同的的（并发），
    //不同组内的CTS中的TxTime是根据前面组的的最大延迟递增累加的。
    //而CTS的发送时间是每个CTS都参与递增的，因为Server只有一个发送信道。

    simtime_t slotSum = slotTime1 + slotTime2;
    //每组延迟  simTime()+slotTime1+前面组别所用的延迟
    groupDelay=simTime()+slotSum;//当前时间是每次循环的5秒后进入的TxCTS，加一个大于3的数，再取slotTime1 + slotTime2 的整数倍保证Cts_TxTime（DATA的发送时间）能在每次循环的开始时
    //groupDelay 向下取为 slotTime1 + slotTime2 的整数倍
    simtime_t adjustedGroupDelay = floor(SIMTIME_DBL(groupDelay) / SIMTIME_DBL(slotSum)) * slotSum;//组间data发送延迟:同一组内data同时发送，组间延迟依次递增
    simtime_t   groupBegin=adjustedGroupDelay;
    simtime_t   ctsSendTime=simTime();//cts的发送时间(在server上）
    simtime_t   ctsArriveTime;//cts的到达时间(在sensor上)为了保证"cts到达时间"在"data的发送时间"之前


    // 输出分组情况
    for (const auto& group : sectorGroups) {
        groupInMaxDelay=0;
        std::cout << "                                                    IN   TxCTS function ：Group contains " << group.size() << " sectors:\n";
/*
        simtime_t slotSum = slotTime1 + slotTime2;
        //每组延迟  simTime()+slotTime1+前面组别所用的延迟
        groupDelay=simTime()+slotTime1;
        //groupDelay 向下取为 slotTime1 + slotTime2 的整数倍
        simtime_t adjustedGroupDelay = floor(SIMTIME_DBL(groupDelay) / SIMTIME_DBL(slotSum)) * slotSum;
*/
        for (const auto& sector : group) {
            //std::cout <<"   根据该扇形生成CTS：：：：：：：rtsId"<<sector.rtsId<< "Sector at (" << sector.srcX << ", "
            //        << sector.srcY << "), angle range: ["<< sector.startAngle << ", " << sector.endAngle
            //        << "], distance: " << sector.distance << "\n";

            if(groupInMaxDelay<sector.delay){
                groupInMaxDelay=sector.delay;

            }
            std::cout << "      group's delay  " << sector.delay << " sectors:\n";


            //生成CTS
            char ctsName[40];

            CTS* cts = new CTS(ctsName);
            ctsId= cts->getId();
            snprintf(ctsName, sizeof(ctsName), "cts(%d)-(%d)-to-(%d)",ctsId, id, sector.srcId);
            std::cout<<"                                                       Current simulation time: " << simTime() <<"生成一个CTS:"<<ctsName<<std::endl;
            cts->setCts_name(ctsName);
            cts->setCts_srcID(id);//cts源地址的本节点的id
            cts->setCts_destID(sector.srcId);//cts的目的地址是RTS的源地址
            cts->setCts_dataID(sector.dataId);
            cts->setCts_TxDirection(sector.angle);
            cts->setCts_TxTime(adjustedGroupDelay);//CTS的TxTime是个sensor安排的Data的传输时间，所以每组的第一个都是从8的整数倍开始的
            cts->setCts_TxPower(x);
            cts->setBitLength(ctsLenBit->intValue());
            cts->setRts_name(sector.rts_name.c_str());
            cts->setCts_transportationDelay(sector.ctsTransportationDelay);
            cts->setRts_id(sector.rtsId);
            cts->setCts_DataTxDelay1(sector.delay);
            cts->setCts_DataTxDelay2(0);

            //cts->setCts_TxChannel();

            std::cout<<"                                                          CTS参数:"<<ctsName<<"   rts_id:"<<cts->getRts_id()<<"   rts_name:"<<sector.rts_name<<"  data传输时间："<<cts->getCts_TxTime()<<"  方向:"<<cts->getCts_TxDirection()
                            <<" 延迟："<<sector.delay<<"  传输能量："<<cts->getCts_TxPower() << std::endl;

            std::cout<<"                                                          下一轮次的data发送的最终时间："<<groupBegin+slotTime1 + slotTime2<<endl;




            //计算传播延迟
            // 从coordinateList_map获取目标传感器的坐标
            auto ctsDestCoord = coordinateList_map.at(cts->getCts_destID());
            // 计算源到目标的距离（欧几里得距离）
            double distance = std::sqrt(std::pow(std::get<0>(ctsDestCoord) - x, 2) +
                                        std::pow(std::get<1>(ctsDestCoord) - y, 2));
            //std::cout << "计算源到目标的距离（欧几里得距离）："<<  distance  << std::endl;
            double propagationDelay=distance/1500;
            double transportationDelay=cts->getBitLength()/txRate->doubleValue();
            double delay=propagationDelay+transportationDelay;
            std::cout<<"                                                        cts的发送时间："<<ctsSendTime<<"    当前轮次的结束时间"<<groupBegin<<endl;//当前轮次结束时间=下一次轮次开始时间
            //CTS的发送时间不能超过当前轮次结束时间。


            //到达时间=发送时间+传输延迟+传播延迟。
            ctsArriveTime=ctsSendTime+(cts->getBitLength()/txRate->doubleValue())+propagationDelay;
            std::cout<<"                                                        cts的到达时间："<<ctsArriveTime<<"    当前轮次的结束时间"<<groupBegin<<endl;//当前轮次结束时间=下一次轮次开始时间
            ctsSendTime=ctsSendTime+(cts->getBitLength()/txRate->doubleValue());
            std::cout<<"                                                           TxCts:propagationDelay" <<propagationDelay<< endl;
            std::cout<<"                                                           TxCts:transportationDelay" <<transportationDelay<< endl;
            std::cout<<"                                                           TxCts:delay" <<transportationDelay+propagationDelay<< endl;



            //adjustedGroupDelay>groupBegin+slotTime1 + slotTime2:::data传输时间小于下一轮次结束的时间
            //
            if(adjustedGroupDelay>groupBegin+slotTime1 + slotTime2){
                std::cout<< "                                                            该data发送时间超过下一轮次的结束时间"<<cts->getRts_name()<<"   rts_id:"<<cts->getRts_id()<<endl;
                std::cout<< "                                                            所以将该RTS在requestList里中的状态改为未处理，下一轮重新生成CTS：  "<<cts->getRts_name()<<endl;
                // 将超出时间安排超出的RTS的状态重新改为未处理
                std::get<4>(requestList[cts->getRts_id()]) = 0;

            }else if(ctsArriveTime>groupBegin){
                std::cout<< "                                                            CTS到达时间超过当前轮次结束时间"<<cts->getRts_name()<<"   rts_id:"<<cts->getRts_id()<<endl;
                std::cout<< "                                                            所以将该RTS在requestList里中的状态改为未处理，下一轮重新生成CTS：  "<<cts->getRts_name()<<endl;
                // 将超出时间安排超出的RTS的状态重新改为未处理
                std::get<4>(requestList[cts->getRts_id()]) = 0;

            }else if(cts->getCts_TxTime()+cts->getCts_DataTxDelay1()>groupBegin+slotTime1 + slotTime2){
                std::cout<< "                                                            该data到达时间超过下一轮次的结束时间"<<cts->getRts_name()<<"   rts_id:"<<cts->getRts_id()<<endl;
                std::cout<< "                                                            所以将该RTS在requestList里中的状态改为未处理，下一轮重新生成CTS：  "<<cts->getRts_name()<<endl;
                // 将超出时间安排超出的RTS的状态重新改为未处理
                std::get<4>(requestList[cts->getRts_id()]) = 0;


            }else{
                //存入数据发送队列缓存。
            ctsSendDeque.push_back(cts);
           // std::cout<<"                                                    CTS参数:"<<ctsName<<"   rts_name:"<<sector.rts_name<<"  传输时间："<<cts->getCts_TxTime()<<"  方向:"<<cts->getCts_TxDirection()
             //   <<" 延迟："<<sector.delay<<"  传输能量："<<cts->getCts_TxPower() << std::endl;



            }



        }



        //调度时间的安排：每组的data的发生时间累加。
        adjustedGroupDelay=adjustedGroupDelay+groupInMaxDelay;

 /*
       std::cout << "                                                                adjustedGroupDelay: " <<adjustedGroupDelay<<std::endl;
        //打印数据发送队列
        std::cout << "                                                                Server(id:"<<id<<")CTS发送队列: " << std::endl;

        for (const auto& ctsPtr : ctsSendDeque) {
            if (ctsPtr) {
                std::cout <<"                                                                           "<< ctsPtr->getCts_name() << std::endl;
            } else {
                std::cout << "nullptr in RTSdeque" << std::endl;
            }
        }
*/

    }
    scheduleAt(simTime(),txCtsEvent2 );//生成CTS后第一个CTS会立即在TxCts2中发送后续CTS依次发送






}

//TxCts2增加了 server连续发送cts中间的时间间隔（每发送一次CTS就增加一次CTS的传输延迟来调度下一次CTS发送）
void Server::TxCts2(cMessage* msg){
    //从ctsSendDeque取出来CTS并发送CTS
    //

    ASSERT(msg == txCtsEvent2);
    if(!ctsSendDeque.empty()) {
        CTS* txcts = ctsSendDeque.front();
        ctsSendDeque.pop_front();  // 从队列中取出并移除

        if (txcts) {

            //for--------》》》》while
            //虽然使用了 for (CTS* cts : ctsSendDeque) 进行遍历，但你始终只处理了 ctsSendDeque.front() 中的第一个元素，并没有随着遍历推进到下一个。这会导致：
            //每次 for 循环中处理的其实都是队头 ctsSendDeque.front()；
            //你并没有 pop_front()，所以队列始终不变；
            //如果 ctsSendDeque.front() 永远不为 nullptr，循环将执行多次、每次做同样的事，可能误以为 break 没生效；
            //实际你只是“遍历了一遍 ctsSendDeque”，但没有用遍历到的 cts 元素。

/*     for (CTS* cts : ctsSendDeque) {
        if (cts){
            CTS* txcts = ctsSendDeque.front();//从ctsSendDeque队列中取出第一个元素。
            //txcts->setRts_sendPower(rtsSendPower);
*/
            txcts->setCts_TxPower(0);
            //计算server到 sensor之间的延迟=传输延迟（固定，CTS长度固定）+传播延迟（根据两点之间的距离的计算）
            // 从coordinateList_map获取目标传感器的坐标
            auto ctsDestCoord = coordinateList_map.at(txcts->getCts_destID());
            //std::cout << "在坐标表中读取了源节点坐标和目的节点坐标"<< "Current simulation time: " << simTime()  << std::endl;

            // 计算源到目标的距离（欧几里得距离）
            double distance = std::sqrt(std::pow(std::get<0>(ctsDestCoord) - x, 2) +
                                        std::pow(std::get<1>(ctsDestCoord) - y, 2));

            //std::cout << "计算源到目标的距离（欧几里得距离）："<<  distance  << std::endl;
            double propagationDelay=distance/1500;
            //std::cout << "double propagationDelay=distance/1500;"<< std::endl;

            //double transportationDelay=txcts->getBitLength()/txRate->doubleValue();
            double transportationDelay=txcts->getBitLength()/txRate->doubleValue();
            ////////////////////cMessage的传输延迟是被默认忽略的所以,要用cPacket定义消息

           // std::cout << "double transportationDelay=static_cast<int>(dataLenBit)/txRate->intValue();"<< std::endl;
            double delay=propagationDelay+transportationDelay;
            std::cout <<"                                                     Current simulation time: " << simTime() <<"Server("<<id <<"):发送了一条CTS。"<<txcts->getCts_name()<<"   针对rts_id:"<<txcts->getRts_id()<<"   RTS："<<txcts->getRts_name()<<"   Current simulation time: " << simTime() << std::endl;
            std::cout<<"                                                           TxCts2:propagationDelay" <<propagationDelay<< endl;
            std::cout<<"                                                           TxCts2:transportationDelay" <<transportationDelay<< endl;
            std::cout<<"                                                           TxCts2:delay" <<delay<< endl;



           //遍历std::map<cModule*, double> neighborMap;，存有sensor可以用sendDirect直接发送。
            for (auto it = neighborMap.begin(); it != neighborMap.end(); ++it)
            {
                if (it->first->getId() == txcts->getCts_destID())
                {
                    // 找到了目标 sensor
                    std::cout << "                                                                Found sensor with ID = " << txcts->getCts_destID()
                              << ", delay = " << delay << std::endl;//it->second是距离
                    std::cout <<"                                                     Current simulation time: " << simTime() <<"Server("<<id <<"):发送了一条CTS。"<<txcts->getCts_name()<<"针对RTS："<<txcts->getRts_name()<<"   Current simulation time: " << simTime() << std::endl;
                    sendDirect(txcts->dup(),propagationDelay+transportationDelay, transportationDelay, it->first->gate("in_control"));

                    break;  // 找到后退出循环（如果只找一个目标）
                }
            }
            // === 设置延时触发下一次处理 ===
            simtime_t interCTSInterval = txcts->getCts_transportationDelay();  // 例如 0.01s 间隔发送下一条
            if (!ctsSendDeque.empty()) {
                scheduleAt(simTime() + interCTSInterval, txCtsEvent2);
            }


        }



    }



}
















//将调度信息放到CTS中按照距离从远到近发给Sensor节点。
void Server::dispach()
{

}

void Server::test(cMessage* msg){
    ASSERT(msg==testEvent);
    std::cout << "Current simulation time: " << simTime() << std::endl;
    std::cout << "###################################Server(id:"<<id<<")::test start" << std::endl;
    EV << "###################################Server(id:"<<id<<")::test start" << std::endl;

}







bool angleOverlap(double start1, double end1, double start2, double end2) {
    auto normalize = [](double angle) {
        while (angle < 0) angle += 360;
        while (angle >= 360) angle -= 360;
        return angle;
    };

    start1 = normalize(start1);
    end1 = normalize(end1);
    start2 = normalize(start2);
    end2 = normalize(end2);

    // 将角度范围转化为区间列表（可能有跨越0度的情况）
    auto getIntervals = [](double start, double end) {
        if (end < start) return std::vector<std::pair<double, double>>{{start, 360}, {0, end}};
        return std::vector<std::pair<double, double>>{{start, end}};
    };

    auto intervals1 = getIntervals(start1, end1);
    auto intervals2 = getIntervals(start2, end2);

    for (auto& a : intervals1) {
        for (auto& b : intervals2) {
            if (a.second > b.first && b.second > a.first) return true;
        }
    }
    return false;
}










//判断扇形相交函数
/*
bool Server::sectorsIntersect(const Sector& a, const Sector& b) {
    double dx = a.srcX - b.srcX;
    double dy = a.srcY - b.srcY;
    double dist = std::sqrt(dx * dx + dy * dy);

    if (dist > a.distance + b.distance) return false; // 太远，不可能相交
    if (!angleOverlap(a.startAngle, a.endAngle, b.startAngle, b.endAngle)) return false; // 角度不重叠

    return true; // 有可能相交
}
*/

//5.20  新算法    inline double deg2rad(double deg);
//bool angleInSector(double angle, double start, double end);
//bool pointInSector(double px, double py, const Server::Sector& s);
//std::pair<double, double> polarToCartesian(double cx, double cy, double r, double angleDeg)
//bool Server::sectorsIntersect(const Sector& a, const Sector& b)
// 将角度转换为弧度
inline double deg2rad(double deg) {
    return deg * M_PI / 180.0;
}

// 判断一个角度是否在一个角度区间内（考虑跨0度）
bool angleInSector(double angle, double start, double end) {
    angle = fmod(angle + 360, 360);
    start = fmod(start + 360, 360);
    end = fmod(end + 360, 360);

    if (start <= end)
        return angle >= start && angle <= end;
    else // 跨0度
        return angle >= start || angle <= end;
}

// 判断点 (px, py) 是否在扇形内
bool Server::pointInSector(double px, double py, const Server::Sector& s) {
    double dx = px - s.srcX;
    double dy = py - s.srcY;
    double dist = std::sqrt(dx * dx + dy * dy);
    if (dist > s.distance) return false;

    double angle = atan2(dy, dx) * 180.0 / M_PI;
    // 内部辅助函数可直接写在此处
    auto angleInSector = [](double angle, double start, double end) {
        angle = fmod(angle + 360, 360);
        start = fmod(start + 360, 360);
        end = fmod(end + 360, 360);
        if (start <= end)
            return angle >= start && angle <= end;
        else
            return angle >= start || angle <= end;
    };

    return angleInSector(angle, s.startAngle, s.endAngle);
}

// 计算弧端点坐标
std::pair<double, double> Server::polarToCartesian(double cx, double cy, double r, double angleDeg) {
    double angleRad = angleDeg * M_PI / 180.0;
    return {cx + r * std::cos(angleRad), cy + r * std::sin(angleRad)};
}







//判断扇形相交函数
bool Server::sectorsIntersect(const Sector& a, const Sector& b) {
    // 特殊处理：完全相同的扇形
    if (a.srcX == b.srcX && a.srcY == b.srcY &&
        a.startAngle == b.startAngle && a.endAngle == b.endAngle &&
        a.distance == b.distance) {
        return true; // 完全相同的扇形认为相交
    }

    // 特殊处理：同一源点的扇形
    if (a.srcX == b.srcX && a.srcY == b.srcY) {
        // 检查角度范围是否重叠
        return angleOverlap(a.startAngle, a.endAngle, b.startAngle, b.endAngle);
    }
    // Step 1: 点包含检查（3 个点：圆心 + 弧两端）
    auto [ax1, ay1] = polarToCartesian(a.srcX, a.srcY, a.distance, a.startAngle);
    auto [ax2, ay2] = polarToCartesian(a.srcX, a.srcY, a.distance, a.endAngle);

    auto [bx1, by1] = polarToCartesian(b.srcX, b.srcY, b.distance, b.startAngle);
    auto [bx2, by2] = polarToCartesian(b.srcX, b.srcY, b.distance, b.endAngle);

    // 检查 a 是否包含 b 的三个关键点
    if (pointInSector(b.srcX, b.srcY, a) ||
        pointInSector(bx1, by1, a) ||
        pointInSector(bx2, by2, a)) {
        return true;
    }

    // 检查 b 是否包含 a 的三个关键点
    if (pointInSector(a.srcX, a.srcY, b) ||
        pointInSector(ax1, ay1, b) ||
        pointInSector(ax2, ay2, b)) {
        return true;
    }

    // Step 2: 线段与弧线相交检测（近似处理）
    // 把每个扇形拆成：两条边线段（圆心到两端点） + 一段弧线（暂略为多段近似线段）

    // 辅助函数：检测两线段是否相交
    auto segmentsIntersect = [](double x1, double y1, double x2, double y2,
                                double x3, double y3, double x4, double y4) {
        auto cross = [](double x0, double y0, double x1, double y1) {
            return x0 * y1 - y0 * x1;
        };
        double d1 = cross(x4 - x3, y4 - y3, x1 - x3, y1 - y3);
        double d2 = cross(x4 - x3, y4 - y3, x2 - x3, y2 - y3);
        double d3 = cross(x2 - x1, y2 - y1, x3 - x1, y3 - y1);
        double d4 = cross(x2 - x1, y2 - y1, x4 - x1, y4 - y1);
        return (d1 * d2 < 0 && d3 * d4 < 0);
    };

    // 边线段
    std::vector<std::pair<double, double>> aLines = {
        {ax1, ay1}, {ax2, ay2}
    };
    std::vector<std::pair<double, double>> bLines = {
        {bx1, by1}, {bx2, by2}
    };

    // 相交检测：a 的边与 b 的边
    for (auto& pa : aLines) {
        for (auto& pb : bLines) {
            if (segmentsIntersect(a.srcX, a.srcY, pa.first, pa.second,
                                  b.srcX, b.srcY, pb.first, pb.second)) {
                return true;
            }
        }
    }

    // TODO：加入弧线与线段之间的近似相交检测
    // 可通过将弧线近似为若干短线段来实现（略）

    return false;
}



//扇区分组
std::vector<std::vector<Server::Sector>> Server::groupSectors(const std::vector<Server::Sector>& sectors) {
    std::cout << "                                                                进入扇形分组函数" << simTime() << std::endl;
    int n = sectors.size();
    std::vector<std::vector<bool>> adj(n, std::vector<bool>(n, false));

    // 构建邻接矩阵
    for (int i = 0; i < n; ++i)
        for (int j = i + 1; j < n; ++j)
            if (sectorsIntersect(sectors[i], sectors[j])) {
                adj[i][j] = adj[j][i] = true;
            }

    // 图着色分组（贪心）
    std::vector<bool> assigned(n, false);
    std::vector<std::vector<Sector>> groups;

    while (true) {
        std::vector<Sector> group;
        std::vector<bool> used(n, false);
        bool added = false;

        for (int i = 0; i < n; ++i) {
            if (!assigned[i]) {
                bool conflict = false;
                for (int j = 0; j < n; ++j) {
                    if (used[j] && adj[i][j]) {
                        conflict = true;
                        break;
                    }
                }
                if (!conflict) {
                    used[i] = true;
                    group.push_back(sectors[i]);
                    assigned[i] = true;
                    added = true;
                }
            }
        }

        if (!added) break;
        groups.push_back(group);
    }

    return groups;
}

// 计算下一阶段开始时间
simtime_t Server::calculateNextPhaseStartTime() const {
    simtime_t slotSum = slotTime1 + slotTime2;
    simtime_t currentTime = simTime();

    // 计算当前时间对slotSum的余数
    simtime_t remainder = fmod(currentTime.dbl(), slotSum.dbl());

    // 计算下一阶段开始时间
    simtime_t t_next = currentTime + (slotSum - remainder);

    return t_next;
}
// 预测指定时间点的AUV位置
std::pair<double, double> Server::predictAuvPositionAtTime(simtime_t targetTime) const {
    cModule* auv = getModuleByPath("auv");
    if (!auv) {
        return {-1, -1};
    }

    // 获取AUV当前参数
    double currentX = auv->par("x").doubleValue();
    double currentY = auv->par("y").doubleValue();

    // 获取AUV运动参数
    double vx = auv->hasPar("vx") ? auv->par("vx").doubleValue() : 0.0;
    double vy = auv->hasPar("vy") ? auv->par("vy").doubleValue() : 0.0;

    // 计算时间差
    double dt = (targetTime - simTime()).dbl();

    // 预测位置
    double predictedX = currentX + vx * dt;
    double predictedY = currentY + vy * dt;

    return {predictedX, predictedY};
}
  
std::vector<std::vector<Server::Sector>> Server::groupSectors_simulatedAnnealing(const std::vector<Server::Sector>& sectors) {
    std::cout << "进入模拟退火扇形分组函数" << simTime() << std::endl;

    int n = sectors.size();
    if (n == 0) {
        std::cout << "扇形数量为0，返回空结果" << std::endl;
        return {};
    }

    std::cout << "扇形数量: " << n << std::endl;

    // 1. 构建冲突图
    std::vector<std::vector<bool>> adj(n, std::vector<bool>(n, false));
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (sectorsIntersect(sectors[i], sectors[j])) {
                adj[i][j] = adj[j][i] = true;
            }
        }
    }

    // 2. 初始化随机数生成器
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> prob_dist(0.0, 1.0);

    // 3. 模拟退火参数（降低复杂度）
    const double initial_temp = 20.0;      // 降低初始温度
    const double final_temp = 0.5;         // 提高终止温度
    const double cooling_rate = 0.95;      // 加快降温
    const int iters_per_temp = std::min(50, 2 * n);  // 减少迭代次数

    // 4. 简化的能量函数
    auto calculate_energy = [&](const std::vector<int>& solution) -> double {
        if (solution.empty()) return 1000.0;

        // 计算组数
        int max_group = *std::max_element(solution.begin(), solution.end());
        int num_groups = max_group + 1;

        // 计算冲突数
        int num_conflicts = 0;
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (adj[i][j] && solution[i] == solution[j]) {
                    num_conflicts++;
                }
            }
        }

        // 简化的能量函数
        double energy = num_groups + num_conflicts * 50.0;

        return energy;
    };

    // 5. 简化的初始解生成
    std::vector<int> current_solution(n, 0);
    int num_groups_initial = 1;

    // 简单的贪心分配
    for (int i = 0; i < n; ++i) {
        int best_group = 0;
        bool found_group = false;

        // 尝试分配到现有组
        for (int g = 0; g < num_groups_initial; ++g) {
            bool can_assign = true;
            for (int j = 0; j < i; ++j) {
                if (current_solution[j] == g && adj[i][j]) {
                    can_assign = false;
                    break;
                }
            }
            if (can_assign) {
                current_solution[i] = g;
                found_group = true;
                break;
            }
        }

        // 如果无法分配到现有组，创建新组
        if (!found_group) {
            current_solution[i] = num_groups_initial++;
        }
    }

    std::vector<int> best_solution = current_solution;
    double current_energy = calculate_energy(current_solution);
    double best_energy = current_energy;

    std::cout << "初始解能量: " << current_energy << ", 组数: " << num_groups_initial << std::endl;

    double temp = initial_temp;
    int no_improvement_count = 0;
    const int max_no_improvement = 100;  // 减少最大无改进次数

    // 6. 简化的模拟退火主循环
    while (temp > final_temp && no_improvement_count < max_no_improvement) {
        bool improved = false;

        for (int i = 0; i < iters_per_temp; ++i) {
            std::vector<int> new_solution = current_solution;

            // 简单的邻域生成：随机移动一个扇形
            std::uniform_int_distribution<> sector_dist(0, n - 1);
            int sector_to_move = sector_dist(gen);

            // 随机选择新组
            std::uniform_int_distribution<> group_dist(0, num_groups_initial);
            int new_group = group_dist(gen);

            if (new_solution[sector_to_move] != new_group) {
                new_solution[sector_to_move] = new_group;

                // 计算新解的能量
                double new_energy = calculate_energy(new_solution);

                // Metropolis准则
                double delta_energy = new_energy - current_energy;

                if (delta_energy < 0 || prob_dist(gen) < std::exp(-delta_energy / temp)) {
                    current_solution = new_solution;
                    current_energy = new_energy;

                    if (current_energy < best_energy) {
                        best_solution = current_solution;
                        best_energy = current_energy;
                        improved = true;
                        no_improvement_count = 0;
                    }
                }
            }
        }

        if (!improved) {
            no_improvement_count++;
        }

        // 降温
        temp *= cooling_rate;
    }

    std::cout << "退火完成，最佳能量: " << best_energy << std::endl;

    // 7. 安全地转换结果
    int final_num_groups = 1;
    if (!best_solution.empty()) {
        int max_group = *std::max_element(best_solution.begin(), best_solution.end());
        final_num_groups = max_group + 1;
    }

    std::cout << "最终组数: " << final_num_groups << std::endl;

    std::vector<std::vector<Sector>> groups(final_num_groups);
    for (int i = 0; i < n; ++i) {
        if (best_solution[i] >= 0 && best_solution[i] < final_num_groups) {
            groups[best_solution[i]].push_back(sectors[i]);
        }
    }

    // 8. 移除空组（安全方式）
    std::vector<std::vector<Sector>> non_empty_groups;
    for (const auto& group : groups) {
        if (!group.empty()) {
            non_empty_groups.push_back(group);
        }
    }

    std::cout << "最终非空组数: " << non_empty_groups.size() << std::endl;

    return non_empty_groups;
   }
std::vector<std::vector<Server::Sector>> Server::groupSectors_simulatedAnnealing_1(const std::vector<Server::Sector>& sectors) {
    std::cout << "进入模拟退火扇形分组函数" << simTime() << std::endl;

    int n = sectors.size();
    if (n == 0) {
        std::cout << "扇形数量为0，返回空结果" << std::endl;
        return {};
    }

    std::cout << "扇形数量: " << n << std::endl;

    // 1. 构建冲突图
    std::vector<std::vector<bool>> adj(n, std::vector<bool>(n, false));
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (sectorsIntersect(sectors[i], sectors[j])) {
                adj[i][j] = adj[j][i] = true;
            }
        }
    }

    // 2. 初始化随机数生成器
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> prob_dist(0.0, 1.0);

    // 3. 模拟退火参数
    const double initial_temp = 50.0;
    const double final_temp = 0.1;
    const double cooling_rate = 0.98;
    const int iters_per_temp = std::max(100, 5 * n);

    cModule* auv = getModuleByPath("auv");

    // 4. 完整的能量函数
    //auto calculate_energy = [&](const std::vector<int>& solution) -> double {
    auto calculate_energy = [n, &adj, &sectors, auv](const std::vector<int>& solution) -> double {
        if (solution.empty()) return 1000.0;

        // 计算组数
        std::set<int> unique_groups(solution.begin(), solution.end());
        int num_groups = unique_groups.size();

        // 计算冲突数
        int num_conflicts = 0;
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (adj[i][j] && solution[i] == solution[j]) {
                    num_conflicts++;
                }
            }
        }

        // 计算组内扇形数分布
        /*std::vector<int> group_sizes(num_groups, 0);
        for (int group_id : solution) {
            group_sizes[group_id]++;
        }*/
        std::vector<int> group_sizes(num_groups, 0);
        for (int group_id : solution) {
            if (group_id >= 0 && group_id < num_groups) {
                group_sizes[group_id]++;
            }
        }
        double variance = 0.0;
        double mean = static_cast<double>(n) / num_groups;
        for (int size : group_sizes) {
            variance += (size - mean) * (size - mean);
        }
        variance /= num_groups;

        // 能量函数：组数 + 冲突惩罚 + 分布均匀性
        double energy = num_groups + num_conflicts * 100.0 + variance * 0.1;

        // AUV请求优先级奖励

        if (auv) {
            int auvId = auv->getId();
            for (int i = 0; i < n; ++i) {
                if (sectors[i].destID == auvId) {
                    if (group_sizes[solution[i]] <= 3) {
                        energy -= 0.5;
                    }
                }
            }
        }

        return energy;
    };

    // 5. 智能初始解生成
    std::vector<int> current_solution(n, -1);
    int num_groups_initial = 0;

    // 按优先级排序扇形
    std::vector<int> sector_order(n);
    std::iota(sector_order.begin(), sector_order.end(), 0);


    if (auv) {
        int auvId = auv->getId();
        std::sort(sector_order.begin(), sector_order.end(),
                 [&](int a, int b) {
                     bool a_is_auv = (sectors[a].destID == auvId);
                     bool b_is_auv = (sectors[b].destID == auvId);
                     if (a_is_auv != b_is_auv) return a_is_auv;
                     return sectors[a].distance < sectors[b].distance;
                 });
    }

    // 贪心分配
    for (int idx : sector_order) {
        int best_group = -1;
        int min_conflicts = INT_MAX;

        // 尝试分配到现有组
        for (int g = 0; g < num_groups_initial; ++g) {
            int conflicts = 0;
            for (int j = 0; j < n; ++j) {
                if (current_solution[j] == g && adj[idx][j]) {
                    conflicts++;
                }
            }
            if (conflicts < min_conflicts) {
                min_conflicts = conflicts;
                best_group = g;
            }
        }

        // 如果现有组都有冲突，创建新组
        if (min_conflicts > 0) {
            best_group = num_groups_initial++;
        }

        current_solution[idx] = best_group;
    }

    std::vector<int> best_solution = current_solution;
    double current_energy = calculate_energy(current_solution);
    double best_energy = current_energy;

    std::cout << "初始解能量: " << current_energy << ", 组数: " << num_groups_initial << std::endl;

    double temp = initial_temp;
    int no_improvement_count = 0;
    const int max_no_improvement = 1000;

    // 6. 模拟退火主循环
    while (temp > final_temp && no_improvement_count < max_no_improvement) {
        bool improved = false;

        for (int i = 0; i < iters_per_temp; ++i) {
            std::vector<int> new_solution = current_solution;

            // 邻域生成策略
            if (prob_dist(gen) < 0.7) {
                // 随机移动
                std::uniform_int_distribution<> sector_dist(0, n - 1);
                int sector_to_move = sector_dist(gen);

                // 计算可移动的组
                std::vector<int> valid_groups;
                for (int g = 0; g < num_groups_initial; ++g) {
                    if (g != new_solution[sector_to_move]) {
                        bool can_move = true;
                        for (int j = 0; j < n; ++j) {
                            if (j != sector_to_move && new_solution[j] == g && adj[sector_to_move][j]) {
                                can_move = false;
                                break;
                            }
                        }
                        if (can_move) {
                            valid_groups.push_back(g);
                        }
                    }
                }

                if (!valid_groups.empty()) {
                    std::uniform_int_distribution<> group_dist(0, valid_groups.size() - 1);
                    new_solution[sector_to_move] = valid_groups[group_dist(gen)];
                } else {
                    continue;
                }
            } else {
                // 交换两个扇形的组
                std::uniform_int_distribution<> sector_dist(0, n - 1);
                int sector1 = sector_dist(gen);
                int sector2 = sector_dist(gen);

                if (sector1 != sector2 && new_solution[sector1] != new_solution[sector2]) {
                    // 检查交换后是否无冲突
                    bool valid_swap = true;
                    for (int j = 0; j < n; ++j) {
                        if (j != sector1 && adj[sector1][j] && new_solution[j] == new_solution[sector2]) {
                            valid_swap = false;
                            break;
                        }
                        if (j != sector2 && adj[sector2][j] && new_solution[j] == new_solution[sector1]) {
                            valid_swap = false;
                            break;
                        }
                    }

                    if (valid_swap) {
                        std::swap(new_solution[sector1], new_solution[sector2]);
                    } else {
                        continue;
                    }
                } else {
                    continue;
                }
            }

            // 计算新解的能量
            double new_energy = calculate_energy(new_solution);

            // Metropolis准则
            double delta_energy = new_energy - current_energy;

            if (delta_energy < 0 || prob_dist(gen) < std::exp(-delta_energy / temp)) {
                current_solution = new_solution;
                current_energy = new_energy;

                if (current_energy < best_energy) {
                    best_solution = current_solution;
                    best_energy = current_energy;
                    improved = true;
                    no_improvement_count = 0;
                }
            }
        }

        if (!improved) {
            no_improvement_count++;
        }

        // 降温
        temp *= cooling_rate;
    }

    std::cout << "退火完成，最佳能量: " << best_energy << std::endl;

    // 7. 安全地转换结果
    int final_num_groups = 1;
    if (!best_solution.empty()) {

        int max_group = 0;
        if (!best_solution.empty()) {
            auto max_it = std::max_element(best_solution.begin(), best_solution.end());
            if (max_it != best_solution.end()) {
                max_group = *max_it;
            }
        }

        //int max_group = *std::max_element(best_solution.begin(), best_solution.end());
        final_num_groups = max_group + 1;
    }

    std::cout << "最终组数: " << final_num_groups << std::endl;

    std::vector<std::vector<Sector>> groups(final_num_groups);
    for (int i = 0; i < n; ++i) {
        if (best_solution[i] >= 0 && best_solution[i] < final_num_groups) {
            groups[best_solution[i]].push_back(sectors[i]);
        }
    }
/*
    // 8. 移除空组
    groups.erase(std::remove_if(groups.begin(), groups.end(),
                 [](const std::vector<Sector>& group){ return group.empty(); }),
                 groups.end());

    // 9. 最终优化：尝试合并组
    optimizeGroups(groups, sectors);

    std::cout << "最终非空组数: " << groups.size() << std::endl;

    return groups;*/

    // 8. 安全地移除空组
    std::vector<std::vector<Sector>> non_empty_groups;
    for (const auto& group : groups) {
        if (!group.empty()) {
            non_empty_groups.push_back(group);
        }
    }

    // 9. 安全地优化组
    optimizeGroupsSafe(non_empty_groups, sectors);

    return non_empty_groups;

}

//
void Server::optimizeGroups(std::vector<std::vector<Sector>>& groups,
                           const std::vector<Sector>& sectors) {
    bool improved = true;
    while (improved) {
        improved = false;
        for (int i = 0; i < groups.size(); ++i) {
            for (int j = i + 1; j < groups.size(); ++j) {
                // 检查是否可以合并组i和组j
                bool can_merge = true;
                for (const auto& s1 : groups[i]) {
                    for (const auto& s2 : groups[j]) {
                        if (sectorsIntersect(s1, s2)) {
                            can_merge = false;
                            break;
                        }
                    }
                    if (!can_merge) break;
                }

                if (can_merge) {
                    groups[i].insert(groups[i].end(), groups[j].begin(), groups[j].end());
                    groups.erase(groups.begin() + j);
                    improved = true;
                    break;
                }
            }
            if (improved) break;
        }
    }
}
// 安全的优化函数
void Server::optimizeGroupsSafe(std::vector<std::vector<Sector>>& groups,
                               const std::vector<Sector>& sectors) {
    bool improved = true;
    while (improved) {
        improved = false;
        for (int i = 0; i < groups.size() && !improved; ++i) {
            for (int j = i + 1; j < groups.size() && !improved; ++j) {
                // 检查是否可以合并组i和组j
                bool can_merge = true;
                for (const auto& s1 : groups[i]) {
                    for (const auto& s2 : groups[j]) {
                        if (sectorsIntersect(s1, s2)) {
                            can_merge = false;
                            break;
                        }
                    }
                    if (!can_merge) break;
                }

                if (can_merge) {
                    // 安全地合并组
                    groups[i].insert(groups[i].end(), groups[j].begin(), groups[j].end());
                    groups.erase(groups.begin() + j);
                    improved = true;
                }
            }
        }
    }
}

// 优化版本的模拟退火算法 - groupSectors_simulatedAnnealing_2（cursor）
// 主要优化：1. 增量能量计算 2. 减少迭代次数 3. 简化邻域生成 4. 提前终止
std::vector<std::vector<Server::Sector>> Server::groupSectors_simulatedAnnealing_2(const std::vector<Server::Sector>& sectors) {
    std::cout << "进入优化版模拟退火扇形分组函数 v2" << simTime() << std::endl;

    int n = sectors.size();
    if (n == 0) {
        std::cout << "扇形数量为0，返回空结果" << std::endl;
        return {};
    }

    std::cout << "扇形数量: " << n << std::endl;

    // 1. 构建冲突图 (保持不变，O(n²))
    std::vector<std::vector<bool>> adj(n, std::vector<bool>(n, false));
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (sectorsIntersect(sectors[i], sectors[j])) {
                adj[i][j] = adj[j][i] = true;
            }
        }
    }

    // 2. 初始化随机数生成器
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> prob_dist(0.0, 1.0);

    // 3. 优化的模拟退火参数
    const double initial_temp = 30.0;      // 降低初始温度 (从50降到30)
    const double final_temp = 0.5;         // 提高终止温度 (从0.1提高到0.5)
    const double cooling_rate = 0.95;       // 加快降温 (从0.98改为0.95)
    const int iters_per_temp = std::max(50, 3 * n);  // 减少迭代次数 (从max(100, 5n)改为max(50, 3n))
    const int max_no_improvement = 200;    // 减少最大无改进次数 (从1000改为200)

    cModule* auv = getModuleByPath("auv");

    // 4. 优化的能量函数 - 使用增量计算
    // 辅助函数：计算基础能量（组数 + 冲突数）
    auto calculate_base_energy = [n, &adj](const std::vector<int>& solution, 
                                           std::vector<int>& group_sizes_out) -> double {
        if (solution.empty()) return 1000.0;

        // 计算组数和组大小
        int max_group = 0;
        group_sizes_out.clear();
        for (int g : solution) {
            if (g > max_group) max_group = g;
        }
        group_sizes_out.resize(max_group + 1, 0);
        for (int g : solution) {
            if (g >= 0 && g <= max_group) {
                group_sizes_out[g]++;
            }
        }
        int num_groups = max_group + 1;

        // 计算冲突数 - 只检查相邻的冲突
        int num_conflicts = 0;
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (adj[i][j] && solution[i] == solution[j]) {
                    num_conflicts++;
                }
            }
        }

        return num_groups + num_conflicts * 100.0;
    };

    // 增量能量计算：只计算变化部分的能量差
    auto calculate_energy_delta = [n, &adj, &sectors, auv](
        const std::vector<int>& old_solution,
        const std::vector<int>& new_solution,
        int changed_sector1, int changed_sector2,
        double old_energy, const std::vector<int>& old_group_sizes) -> double {
        
        // 快速计算能量差
        double delta = 0.0;
        
        // 计算组数变化
        std::set<int> old_groups(old_solution.begin(), old_solution.end());
        std::set<int> new_groups(new_solution.begin(), new_solution.end());
        delta += (new_groups.size() - old_groups.size());

        // 计算冲突数变化 - 只检查改变的扇形的冲突
        int conflict_delta = 0;
        std::vector<int> changed_sectors = {changed_sector1};
        if (changed_sector2 >= 0) changed_sectors.push_back(changed_sector2);
        
        for (int idx : changed_sectors) {
            for (int j = 0; j < n; ++j) {
                if (j != idx && adj[idx][j]) {
                    bool old_conflict = (old_solution[idx] == old_solution[j]);
                    bool new_conflict = (new_solution[idx] == new_solution[j]);
                    if (old_conflict && !new_conflict) conflict_delta--;
                    else if (!old_conflict && new_conflict) conflict_delta++;
                }
            }
        }
        delta += conflict_delta * 100.0;

        return delta;
    };

    // 完整能量函数（用于初始化和关键检查）
    auto calculate_full_energy = [n, &adj, &sectors, auv](const std::vector<int>& solution) -> double {
        if (solution.empty()) return 1000.0;

        std::vector<int> group_sizes;
        double base_energy = calculate_base_energy(solution, group_sizes);
        
        // 计算方差
        int num_groups = group_sizes.size();
        if (num_groups == 0) return base_energy;
        
        double mean = static_cast<double>(n) / num_groups;
        double variance = 0.0;
        for (int size : group_sizes) {
            variance += (size - mean) * (size - mean);
        }
        variance /= num_groups;

        double energy = base_energy + variance * 0.1;

        // AUV请求优先级奖励
        if (auv) {
            int auvId = auv->getId();
            for (int i = 0; i < n; ++i) {
                if (sectors[i].destID == auvId && solution[i] >= 0 && solution[i] < num_groups) {
                    if (group_sizes[solution[i]] <= 3) {
                        energy -= 0.5;
                    }
                }
            }
        }

        return energy;
    };

    // 5. 简化的初始解生成 (优化：O(n²) 而不是 O(n³))
    std::vector<int> current_solution(n, -1);
    int num_groups_initial = 0;

    // 按优先级排序扇形
    std::vector<int> sector_order(n);
    std::iota(sector_order.begin(), sector_order.end(), 0);

    if (auv) {
        int auvId = auv->getId();
        std::sort(sector_order.begin(), sector_order.end(),
                 [&](int a, int b) {
                     bool a_is_auv = (sectors[a].destID == auvId);
                     bool b_is_auv = (sectors[b].destID == auvId);
                     if (a_is_auv != b_is_auv) return a_is_auv;
                     return sectors[a].distance < sectors[b].distance;
                 });
    }

    // 优化的贪心分配：使用冲突列表加速
    std::vector<std::vector<int>> conflict_lists(n);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (adj[i][j]) {
                conflict_lists[i].push_back(j);
            }
        }
    }

    for (int idx : sector_order) {
        int best_group = -1;
        int min_conflicts = INT_MAX;

        // 尝试分配到现有组
        for (int g = 0; g < num_groups_initial; ++g) {
            int conflicts = 0;
            // 只检查冲突列表中的扇形
            for (int j : conflict_lists[idx]) {
                if (current_solution[j] == g) {
                    conflicts++;
                }
            }
            if (conflicts < min_conflicts) {
                min_conflicts = conflicts;
                best_group = g;
            }
        }

        // 如果现有组都有冲突，创建新组
        if (min_conflicts > 0) {
            best_group = num_groups_initial++;
        }

        current_solution[idx] = best_group;
    }

    std::vector<int> best_solution = current_solution;
    std::vector<int> group_sizes;
    double current_energy = calculate_full_energy(current_solution);
    double best_energy = current_energy;

    std::cout << "初始解能量: " << current_energy << ", 组数: " << num_groups_initial << std::endl;

    double temp = initial_temp;
    int no_improvement_count = 0;
    int consecutive_no_accept = 0;
    const int max_consecutive_no_accept = 50;  // 连续不接受新解的最大次数

    // 6. 优化的模拟退火主循环
    while (temp > final_temp && no_improvement_count < max_no_improvement) {
        bool improved = false;
        bool accepted = false;

        for (int i = 0; i < iters_per_temp; ++i) {
            std::vector<int> new_solution = current_solution;
            int changed_sector1 = -1, changed_sector2 = -1;

            // 优化的邻域生成：优先使用交换策略 (O(n))
            if (prob_dist(gen) < 0.8) {  // 80%概率使用交换策略
                // 交换两个扇形的组
                std::uniform_int_distribution<> sector_dist(0, n - 1);
                int sector1 = sector_dist(gen);
                int sector2 = sector_dist(gen);

                if (sector1 != sector2 && new_solution[sector1] != new_solution[sector2]) {
                    // 快速检查交换后是否无冲突
                    bool valid_swap = true;
                    for (int j : conflict_lists[sector1]) {
                        if (j != sector2 && new_solution[j] == new_solution[sector2]) {
                            valid_swap = false;
                            break;
                        }
                    }
                    if (valid_swap) {
                        for (int j : conflict_lists[sector2]) {
                            if (j != sector1 && new_solution[j] == new_solution[sector1]) {
                                valid_swap = false;
                                break;
                            }
                        }
                    }

                    if (valid_swap) {
                        std::swap(new_solution[sector1], new_solution[sector2]);
                        changed_sector1 = sector1;
                        changed_sector2 = sector2;
                    } else {
                        continue;
                    }
                } else {
                    continue;
                }
            } else {
                // 随机移动 (20%概率)
                std::uniform_int_distribution<> sector_dist(0, n - 1);
                int sector_to_move = sector_dist(gen);

                // 快速查找可移动的组
                std::vector<int> valid_groups;
                for (int g = 0; g < num_groups_initial; ++g) {
                    if (g != new_solution[sector_to_move]) {
                        bool can_move = true;
                        for (int j : conflict_lists[sector_to_move]) {
                            if (new_solution[j] == g) {
                                can_move = false;
                                break;
                            }
                        }
                        if (can_move) {
                            valid_groups.push_back(g);
                        }
                    }
                }

                if (!valid_groups.empty()) {
                    std::uniform_int_distribution<> group_dist(0, valid_groups.size() - 1);
                    new_solution[sector_to_move] = valid_groups[group_dist(gen)];
                    changed_sector1 = sector_to_move;
                } else {
                    continue;
                }
            }

            // 增量计算能量差
            double energy_delta = calculate_energy_delta(
                current_solution, new_solution, 
                changed_sector1, changed_sector2,
                current_energy, group_sizes);
            double new_energy = current_energy + energy_delta;

            // 如果能量差太大，重新完整计算（避免累积误差）
            if (std::abs(energy_delta) > 50.0) {
                new_energy = calculate_full_energy(new_solution);
            }

            // Metropolis准则
            double delta_energy = new_energy - current_energy;

            if (delta_energy < 0 || prob_dist(gen) < std::exp(-delta_energy / temp)) {
                current_solution = new_solution;
                current_energy = new_energy;
                accepted = true;
                consecutive_no_accept = 0;

                if (current_energy < best_energy) {
                    best_solution = current_solution;
                    best_energy = current_energy;
                    improved = true;
                    no_improvement_count = 0;
                }
            } else {
                consecutive_no_accept++;
            }

            // 提前终止：如果连续多次不接受新解，提前退出
            if (consecutive_no_accept > max_consecutive_no_accept) {
                break;
            }
        }

        if (!improved) {
            no_improvement_count++;
        }

        // 降温
        temp *= cooling_rate;
    }

    std::cout << "退火完成，最佳能量: " << best_energy << std::endl;

    // 7. 安全地转换结果
    int final_num_groups = 1;
    if (!best_solution.empty()) {
        int max_group = 0;
        auto max_it = std::max_element(best_solution.begin(), best_solution.end());
        if (max_it != best_solution.end()) {
            max_group = *max_it;
        }
        final_num_groups = max_group + 1;
    }

    std::cout << "最终组数: " << final_num_groups << std::endl;

    std::vector<std::vector<Sector>> groups(final_num_groups);
    for (int i = 0; i < n; ++i) {
        if (best_solution[i] >= 0 && best_solution[i] < final_num_groups) {
            groups[best_solution[i]].push_back(sectors[i]);
        }
    }

    // 8. 安全地移除空组
    std::vector<std::vector<Sector>> non_empty_groups;
    for (const auto& group : groups) {
        if (!group.empty()) {
            non_empty_groups.push_back(group);
        }
    }

    // 9. 安全地优化组（简化版，限制迭代次数）
    int optimize_iterations = 0;
    const int max_optimize_iterations = 10;  // 限制优化迭代次数
    bool improved = true;
    while (improved && optimize_iterations < max_optimize_iterations) {
        improved = false;
        optimize_iterations++;
        for (int i = 0; i < non_empty_groups.size() && !improved; ++i) {
            for (int j = i + 1; j < non_empty_groups.size() && !improved; ++j) {
                bool can_merge = true;
                for (const auto& s1 : non_empty_groups[i]) {
                    for (const auto& s2 : non_empty_groups[j]) {
                        if (sectorsIntersect(s1, s2)) {
                            can_merge = false;
                            break;
                        }
                    }
                    if (!can_merge) break;
                }

                if (can_merge) {
                    non_empty_groups[i].insert(non_empty_groups[i].end(), 
                                               non_empty_groups[j].begin(), 
                                               non_empty_groups[j].end());
                    non_empty_groups.erase(non_empty_groups.begin() + j);
                    improved = true;
                }
            }
        }
    }

    return non_empty_groups;
}
//gemini优化版本
std::vector<std::vector<Server::Sector>> Server::groupSectors_simulatedAnnealing_3(const std::vector<Server::Sector>& sectors) {
    // 0. 基础检查
    int n = sectors.size();
    if (n == 0) return {};
    
    // 参数配置 (针对 MCU 调整)
    const double initial_temp = 10.0;     // 降低初始温度
    const double final_temp = 0.1;
    const double cooling_rate = 0.95;     // 较快的降温
    const int max_steps_per_temp = n;     // 每个温度下的迭代次数与 N 线性相关
    
    // 权重配置
    const double W_CONFLICT = 100.0;      // 冲突的惩罚极其巨大
    const double W_GROUP = 1.0;           // 组数的惩罚较小
    const double W_AUV = 5.0;             // AUV 拥挤惩罚

    // 1. 预计算冲突矩阵 (O(N^2)) - 仍然必要，但只做一次
    // 使用 vector<uint8_t> 模拟 bool 矩阵以节省空间，或者使用 vector<vector<int>> 存储邻居列表以加速遍历
    std::vector<std::vector<int>> adj_list(n); 
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (sectorsIntersect(sectors[i], sectors[j])) {
                adj_list[i].push_back(j);
                adj_list[j].push_back(i);
            }
        }
    }

    // 2. 初始化状态 (使用固定大小容器，避免动态分配)
    std::vector<int> solution(n);           // 每个扇区所属的组 ID
    std::vector<int> group_sizes(n + 1, 0); // 每个组的大小 (最大可能 N 组)
    int current_conflicts = 0;
    int active_groups_count = 0; // 这是一个近似值，用于快速估算

    // 贪心初始化 (O(N^2))
    int max_group_used = 0;
    for (int i = 0; i < n; ++i) {
        int best_g = 0;
        // 尝试放入现有组
        for (; best_g <= max_group_used; ++best_g) {
            bool conflict = false;
            for (int neighbor : adj_list[i]) {
                if (neighbor < i && solution[neighbor] == best_g) { // neighbor < i 确保只检查已分配的
                    conflict = true; 
                    break;
                }
            }
            if (!conflict) break;
        }
        solution[i] = best_g;
        group_sizes[best_g]++;
        if (best_g > max_group_used) max_group_used = best_g;
    }
    
    // 统计初始能量状态
    // 注意：贪心初始化后冲突应为0，但为了通用性我们还是计算一下
    active_groups_count = 0;
    for(int g=0; g<=n; ++g) {
        if(group_sizes[g] > 0) active_groups_count++;
    }
    
    // 识别 AUV 节点 (用于特殊优化)
    std::vector<bool> is_auv_target(n, false);
    cModule* auv_ptr = getModuleByPath("auv");
    if (auv_ptr) {
        int auvId = auv_ptr->getId();
        for(int i=0; i<n; ++i) {
            if(sectors[i].destID == auvId) is_auv_target[i] = true;
        }
    }

    // 辅助 lambda：计算单个节点的局部冲突数 (O(Degree))
    auto count_node_conflicts = [&](int node_idx, int group_id) -> int {
        int conflicts = 0;
        for (int neighbor : adj_list[node_idx]) {
            if (solution[neighbor] == group_id) {
                conflicts++;
            }
        }
        return conflicts;
    };

    // 当前总能量 (简化版)
    // Energy = W_c * Conflicts + W_g * Groups
    // AUV 惩罚暂时只在 Delta 中计算
    double current_energy = W_GROUP * active_groups_count; 
    // (贪心初始化保证了初始冲突为0，如果有冲突需要在这里加上)

    // 3. 模拟退火主循环
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> node_dist(0, n - 1);
    std::uniform_real_distribution<> prob_dist(0.0, 1.0);

    double temp = initial_temp;

    while (temp > final_temp) {
        for (int k = 0; k < max_steps_per_temp; ++k) {
            // A. 随机选择一个节点移动
            int node = node_dist(gen);
            int old_group = solution[node];
            
            // B. 随机选择一个新组 (在当前最大组数范围内，或者开辟新组)
            // 限制搜索范围在 [0, max_group_used + 1] 以避免搜索空间过大
            std::uniform_int_distribution<> group_dist(0, max_group_used + 1);
            int new_group = group_dist(gen);

            if (old_group == new_group) continue;

            // C. 增量计算能量变化 (Delta Evaluation) - 核心优化点
            
            // 1. 冲突变化: 减去旧组的冲突，加上新组的冲突
            int conflicts_in_old = count_node_conflicts(node, old_group);
            int conflicts_in_new = count_node_conflicts(node, new_group);
            int delta_conflicts = conflicts_in_new - conflicts_in_old;

            // 2. 组数变化
            int delta_groups = 0;
            if (group_sizes[old_group] == 1) delta_groups--; // 旧组变空
            if (group_sizes[new_group] == 0) delta_groups++; // 新组开启

            // 3. AUV 拥挤度惩罚 (可选)
            double delta_auv_penalty = 0;
            if (is_auv_target[node]) {
                // 简单的启发式：如果移入的组已经很小，奖励；如果移入大组，惩罚
                // 这里为了 MCU 性能，简化为：不鼓励 AUV 节点所在的组超过 3 个成员
                if (group_sizes[new_group] >= 3) delta_auv_penalty += W_AUV;
                if (group_sizes[old_group] > 3)  delta_auv_penalty -= W_AUV;
            }

            // 总能量变化
            double delta_E = (delta_conflicts * W_CONFLICT) + 
                             (delta_groups * W_GROUP) + 
                             delta_auv_penalty;

            // D. Metropolis 准则
            if (delta_E <= 0 || prob_dist(gen) < std::exp(-delta_E / temp)) {
                // 接受状态
                solution[node] = new_group;
                group_sizes[old_group]--;
                group_sizes[new_group]++;
                
                // 更新搜索边界
                if (new_group > max_group_used) max_group_used = new_group;
                // 如果 max_group_used 变空了，其实应该缩减边界，但为了性能可以忽略，反正循环会处理
                
                current_energy += delta_E;
            }
        }
        temp *= cooling_rate;
    }

    // 4. 结果组装 (O(N))
    // 找出实际的最大组号
    int final_max_group = 0;
    for(int x : solution) if(x > final_max_group) final_max_group = x;
    
    std::vector<std::vector<Sector>> result(final_max_group + 1);
    for (int i = 0; i < n; ++i) {
        result[solution[i]].push_back(sectors[i]);
    }

    // 移除空组
    std::vector<std::vector<Sector>> final_output;
    final_output.reserve(final_max_group + 1);
    for(auto &g : result) {
        if(!g.empty()) final_output.push_back(std::move(g));
    }
    
    // 最后的安全合并 (O(G^2)) - 可选，视 MCU 时间预算而定
    optimizeGroupsSafe(final_output, sectors);

    return final_output;
}