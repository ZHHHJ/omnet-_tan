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

channel DataChannel_1 extends ned.DatarateChannel
{
    
    double distance  @unit(m);
    
    datarate=10Mbps;//数据传输速率，默认是0 意味着零传输时间，带宽无限大。
    delay=this.distance/1500m*1s;//延迟=距离除以1500m/s
    ber=1e-10;//误码率
    
}
