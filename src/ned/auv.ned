//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package src.ned;

simple AUV
{
    parameters:
        @class(omnetpp::AUV);
        double x @unit(m) @mutable(true);
        double y @unit(m) @mutable(true);
        //double z @unit(m);
        //int address;
        volatile double SNR_Threshold;
        double slotTime @unit(s);
        double slotTime1 @unit(s);
        double slotTime2 @unit(s);
        double maxTranRange @unit(m);
        double txRate @unit(bps);
        int ctsLenBit @unit(b);
        // 运动参数（可选）
        double vx @unit(mps) = default(0mps);
        double vy @unit(mps) = default(0mps);
        double deltaT @unit(s) = default(0s); // 0 表示不移动
        //@display("p=500,500;b=50,50");
    gates:
        input in @directIn;
       	input in_data1 @directIn;// 接收Data
        
}