#include<omnetpp.h>
#include<deque>
#include <unordered_map>
#include <iostream>
#include"../msg/RTS_m.h"
#include"../msg/CTS_m.h"
#include <random>
#include <iterator>
#include "State.h"
#include<cmath>
#include <mutex>
#include <algorithm>
#include <numeric>
#include <set>
#include <random>
#include <climits>

using namespace omnetpp;

class Server:public cSimpleModule
{

public:
    Server();
    ~Server();

    // 存储扇形的信息
    struct Sector {
        int rtsId;
        int srcId;
        int destID;
        int dataId;
        double srcX; // 扇形的源节点X坐标
        double srcY; // 扇形的源节点Y坐标
        double startAngle; // 扇形的起始角度
        double endAngle; // 扇形的结束角度
        double distance; // 扇形的距离
        double angle;//扇形的朝向角度
        //simtime_t delay;
        double delay;
        std::string rts_name;
        double ctsTransportationDelay;

    };

private:
    cPar* SNR_Threshold;
    //cPar* slotTime1;
    //cPar* slotTime2;
    simtime_t slotTime1;
    simtime_t slotTime2;
    cPar* maxTranRange;
    cPar* txRate;

    double x,y,z;
    //double auvX ,auvY ,auvZ;
    int id;
    int numSensors;
    int numRTS;//总节点数、server接收RTS的数量
    long currentCollisionNumFrames;
    bool controlChannelBusy;




    State controlState;
    State dataState;


    cModule* network;
    cModule* auv;

    cMessage* endRxDataEvent;
    cMessage* RxRtsEvent;//调度接收RTS事件
    cMessage* computeDispachEvent;
    cMessage* txCtsEvent;
    cMessage* txCtsEvent2;
    cMessage* testEvent;

/*CTS用到的参数  */
    int ctsId;
    //CTS缓存队列
    std::deque<CTS*> ctsSendDeque;//数据发送队列
    simtime_t groupDelay;//组别之间的延迟
    double groupInMaxDelay;//每个组内最大延迟
    std::map<cModule*, double> neighborMap;//参考uan-aloha,存节点，方便在TxCts函数中之间向sensor节点发送消息
    cPar* ctsLenBit;

    std::map<int,int> netMap;
    //std::deque<Data*> recvDeque;

    //定义消息和处理事件的map
    typedef void (Server::*EventFunctionPtr)(cMessage*);
    std::map<cMessage*,EventFunctionPtr> eventHandlerss;

    //const double maxTranRange = 1500;


    long dispatchNum;//调度数
    long recvRequestNum;//接收请求数量

    //三元组请求表，存储 源地址，目的地址，dataid,数据长度,状态（是否被读取）
    //std::tuple<int,int,long>requestList;
    std::unordered_map<int,std::tuple<int,int,int,double,bool,std::string>>requestList;
    int requestArray;


    //坐标三元组，id，x，y
    //std::tuple<int,double,double>coordinateList;
    //std::unordered_map基于哈希表实现，查找操作的平均时间复杂度是常数级（O(1)），适用于频繁查找。
    std::unordered_map<int, std::tuple<double,double>> coordinateList_map;//存储节点的位置信息（二维坐标）
    std::vector<std::tuple<int,int,int,int,int,int,int,double,double,double,std::string,double,bool>>* requestInfoList;//RTS的信息
    //requestInfoList 作为引用成员： 你定义了 std::vector<std::tuple<int,int,int,int,int,int,double, double>>& requestInfoList 作为引用类型的成员变量。引用类型的成员变量必须在初始化列表中进行初始化，而不是在构造函数体内。这样会导致编译错误，因为引用必须始终绑定到一个有效的对象。
    // 将 requestInfoList 改为指针类型


    std::vector<std::tuple<double, double, double, double, double>> sectors;

    // 定义一个结构来存储坐标
    struct Coordinate {
        int x;
        int y;
    };

    // 存储分组后的扇区信息
    std::vector<std::vector<Sector>> sectorGroups;

    std::mutex mtx; // 互斥锁

    cCanvas* canvas;
    // 图形相关
        cRingFigure* sectorRing;  // 用于绘制扇形
        std::vector<cOvalFigure*> sectorCircles;  // 用于绘制动画效果
        cPacket* lastPacket;  // 用于跟踪当前传输

        // 动画相关参数
        double idleAnimationSpeed;
        double transmissionEdgeAnimationSpeed;
        double midtransmissionAnimationSpeed;

        // 常量
        static constexpr double propagationSpeed = 1500.0;  // 声速，单位：m/s
        static constexpr double ringMaxRadius = 1000.0;     // 最大半径
        static constexpr double circlesMaxRadius = 1000.0;  // 最大圆圈半径


private:
    virtual void initialize() override;
    virtual void handleMessage(cMessage* msg)override;
    virtual void finish()override;

    void RxRts(cMessage* msg);//接收RTS

   // void randomId(std::unordered_map<int, std::tuple<int,double,double>>);
    void endRxData(cMessage* msg);
    void dispachAlgorithm(cMessage* msg);
    void dispach();
    //加 static 4/14 17：42
    static std::tuple<double, double> calculateSectorAngles(double angle, double sectorAngle );
    //加 static 4/14 17：42


    void TxCts(cMessage* msg);//发送调度信息CTS
    void TxCts2(cMessage* msg);//发送调度信息CTS2
    void test(cMessage* msg);//测试
    // 判断两个扇形是否相交（可选公开）
    //bool sectorsIntersect(const Sector& a, const Sector& b);

    // 主功能函数：将扇形分成多个组，每组内互不相交
    std::vector<std::vector<Sector>> groupSectors(const std::vector<Sector>& sectors);
    std::vector<std::vector<Sector>> groupSectors_simulatedAnnealing(const std::vector<Sector>& sectors);
    std::vector<std::vector<Sector>> groupSectors_simulatedAnnealing_1(const std::vector<Sector>& sectors);
    std::vector<std::vector<Sector>> groupSectors_simulatedAnnealing_2(const std::vector<Sector>& sectors);
    std::vector<std::vector<Sector>> groupSectors_simulatedAnnealing_3(const std::vector<Sector>& sectors);
    void optimizeGroups(std::vector<std::vector<Sector>>& groups,const std::vector<Server::Sector>& sectors);
    void optimizeGroupsSafe(std::vector<std::vector<Sector>>& groups,const std::vector<Server::Sector>& sectors);
    double deg2rad(double deg);
    bool angleInSector(double angle, double start, double end);
    bool pointInSector(double px, double py, const Server::Sector& s);
    std::pair<double, double> polarToCartesian(double cx, double cy, double r, double angleDeg);
    bool sectorsIntersect(const Sector& a, const Sector& b);
    //virtual void refreshDisplay() const override;
    //void initializeCanvas();

    // 计算下一阶段开始时间
    simtime_t calculateNextPhaseStartTime() const;

    // 预测指定时间点的AUV位置
    std::pair<double, double> predictAuvPositionAtTime(simtime_t targetTime) const;

    // 为涉及AUV的请求计算预测距离和角度
    void processAuvRequests(simtime_t targetTime);

};
