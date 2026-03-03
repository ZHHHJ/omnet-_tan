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

simple Sensor
{
    parameters:
        @class(omnetpp::Sensor);  // 确保这里指定了正确的类
        double x @unit(m);
	    double y @unit(m);
	    //double z @unit(m);
	    //int address;
	    volatile double SNR_Threshold;
	    
	    // getParentModule()->setParameter("myParam", myParam);
	    //setParameter()方法允许你在仿真过程中修改模块的参数。
	    volatile double sendPower @unit(W);	
        volatile double sendFrequency @unit(kHz);	
        volatile double txRate @unit(bps);	
        volatile double iaTime @unit(s);	
        
        int dataLenBit @unit(b);
        int rtsLenBit @unit(b);
        int pkLenBit @unit(b);
        double slotTime @unit(s);  
        double slotTime1 @unit(s); 
        double slotTime2 @unit(s); 
	    double maxTranRange @unit(m);
	    
	    double freq_kHz @unit(kHz);
        double snrMin_dB @unit(dB);
        double margin_dB @unit(dB);
        double noise_dB @unit(dB );
        double eta ;
        double di_tx_dB @unit(dB);
        double di_rx_dB @unit(dB);
	    double auv_perceived_distance @unit(m);
	    double data_dropRate;
	    gates:
	    input in_control @directIn;
	    input in_data1 @directIn;
	    input in_data2 @directIn;
	    input in_data3 @directIn;
	    input in_data4 @directIn;
	    
	    
	    
	    
}
