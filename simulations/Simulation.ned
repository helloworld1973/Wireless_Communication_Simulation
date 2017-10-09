network Simulation
{
	submodules:
		TransmitterNodeModule[2] : TransmitterNode;
		ReceiverNodeModule : ReceiverNode;
		ChannelModule : Channel;
	connections:
	ReceiverNodeModule.gateForChnl <--> ChannelModule.gateNet++;
	TransmitterNodeModule[0].gateForChnl <--> ChannelModule.gateNet++;
	TransmitterNodeModule[1].gateForChnl <--> ChannelModule.gateNet++;
}