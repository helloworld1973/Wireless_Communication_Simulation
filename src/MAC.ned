simple MAC
{
	parameters:
		int bufferSize;			// buffer size for higher layer packets		
		int maxBackoffs;		// maximum backoff counts
		double backoffDistribution;	// distribution for random backoff period
	gates:
        	inout gateForPacket;
		inout gateForTX;
}
