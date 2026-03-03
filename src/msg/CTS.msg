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

//
// TODO generated message class
//
packet CTS {
	string cts_name;
	string rts_name;
	int rts_id;
	
    int cts_srcID;//CTS的发送者是server
    int cts_destID;//CTS的接收者&&data发送者&&RTS发送者data
    int cts_dataID;//sensor根据ID判断是不是重发。
    //double cts_TxTime;
	simtime_t cts_TxTime;//server给sensor安排的发送时间。
	
	double cts_DataTxDelay1;//server给sensor安排的发送传输延迟
	double cts_DataTxDelay2;//server给sensor安排的发送传播延迟
	
    double cts_TxChannel;//server给sensor安排的信道。
    double cts_TxPower;//server给sensor安排的功率。
    double cts_TxDirection;//server给sensor安排的方向。
    double cts_transportationDelay;//cts的传输延迟
}
