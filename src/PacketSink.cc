#include <omnetpp.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
using namespace omnetpp;

class PacketSink : public cSimpleModule
{
public:
    PacketSink();
    ~PacketSink();

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    std::string filename;
};

Define_Module(PacketSink);

PacketSink::PacketSink()
    : cSimpleModule()
{

}

PacketSink::~PacketSink()
{

}

void PacketSink::initialize()
{
    filename = par("filename").str();
}

void PacketSink::handleMessage(cMessage *msg)
{

}
