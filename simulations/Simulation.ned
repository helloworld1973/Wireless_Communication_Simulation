network Simulation
{
	submodules:
		TransmitterNodeModule[2] : TransmitterNode;
		ReceiverNodeModule : ReceiverNode;
		ChannelModule : Channel;
	connections:
	ReceiverNodeModule.gateForChnl <--> ChannelModule.gateChnl++;
	TransmitterNodeModule[0].gateForChnl <--> ChannelModule.gateChnl++;
	TransmitterNodeModule[1].gateForChnl <--> ChannelModule.gateChnl++;
}