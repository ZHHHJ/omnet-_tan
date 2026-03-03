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

network Tangency
{
    parameters:
        int numSensors;
        int pkLenBit @unit(b);
        double slotTime @unit(s);
        @display("p=0,0;b=10000,10000");
 	submodules:
 	    sensor[numSensors]:Sensor{
 	        pkLenBit = parent.pkLenBit;
 	     	slotTime = parent.slotTime ;   
 	    }
 	    auv:AUV;
 	    server: Server;    
        
}
