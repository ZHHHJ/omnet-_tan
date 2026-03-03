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

simple  Server
{
   parameters:
       double x @unit(m);
       double y @unit(m);
       //double z @unit(m);
       //int address;
       
       volatile double SNR_Threshold;
       double slotTime @unit(s);  
       double slotTime1 @unit(s); 
       double slotTime2 @unit(s); 
       double maxTranRange @unit(m);
       double txRate @unit(bps);
       int ctsLenBit @unit(b);
        //@display("p=500,500;b=50,50");  // 添加显示设置
	gates:
	    input in @directIn;
        //input in1;  // 接收Data
        
}