//
// Generated file, do not edit! Created by nedtool 5.0 from SignalStartMessage.msg.
//

#ifndef __SIGNALSTARTMESSAGE_M_H
#define __SIGNALSTARTMESSAGE_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0500
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



/**
 * Class generated from <tt>SignalStartMessage.msg:2</tt> by nedtool.
 * <pre>
 * packet SignalStartMessage
 * {
 *     int identifier;
 * 
 *     int transmitPowerDBm;
 * 
 *     int positionX;
 *     int positionY;
 * 
 *     bool collidedFlag = false;
 * }
 * </pre>
 */
class SignalStartMessage : public ::omnetpp::cPacket
{
  protected:
    int identifier;
    int transmitPowerDBm;
    int positionX;
    int positionY;
    bool collidedFlag;

  private:
    void copy(const SignalStartMessage& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const SignalStartMessage&);

  public:
    SignalStartMessage(const char *name=nullptr, int kind=0);
    SignalStartMessage(const SignalStartMessage& other);
    virtual ~SignalStartMessage();
    SignalStartMessage& operator=(const SignalStartMessage& other);
    virtual SignalStartMessage *dup() const {return new SignalStartMessage(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b);

    // field getter/setter methods
    virtual int getIdentifier() const;
    virtual void setIdentifier(int identifier);
    virtual int getTransmitPowerDBm() const;
    virtual void setTransmitPowerDBm(int transmitPowerDBm);
    virtual int getPositionX() const;
    virtual void setPositionX(int positionX);
    virtual int getPositionY() const;
    virtual void setPositionY(int positionY);
    virtual bool getCollidedFlag() const;
    virtual void setCollidedFlag(bool collidedFlag);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const SignalStartMessage& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, SignalStartMessage& obj) {obj.parsimUnpack(b);}


#endif // ifndef __SIGNALSTARTMESSAGE_M_H
