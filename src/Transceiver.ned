simple Transceiver
{
	parameters:		
		int txPowerDBm;// transmit power
		int bitRate;// bit rate
		int csThreshDBm;// carrier sensing threshold
		int noisePowerDBm;// the power of noise
		int turnaroundTime;// turn around time
		int csTime;// time required to carry out carrier sensing operation
	gates:
		inout gate;// output out;
		
}