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
packet RTS {
    string rts_name;
    bool rts_isReceived = false;//是否收到data
   	int rts_receivedID;//收到的谁发的data，，server以此来判断是否重发data或者删除缓存中的data信息。
   	bool rts_isTransmit = false;//是否有data要发送：若是,则继续读取后面的消息;若否，则停止接收后续信息。
    int rts_srcID;//data发送者&&RTS发送者
    int rts_dataDestID;//data接收者，RTS的接收者永远是Server
    int rts_dataID;
   	int rts_dataLenBit;//data长度
   	bool rts_state;//状态（是否被读取）
   	double rts_sendPower;//rts的发送能量
    

    
}
