simple Transceiver
{
	parameters:		
		int txPowerDBm;// transmit power
		int bitRate;// bit rate
		int csThreshDBm;// carrier sensing threshold
		int noisePowerDBm;// the power of noise
		int turnaroundTime;// transmit state <-> receive state
		int csTime;// time required to carry out carrier sensing operation
	gates:
		inout gateForMAC;
		inout gateForTXRXNode;
		
}