#
# OMNeT++/OMNEST Makefile for Wireless_Communication_Simulation
#
# This file was generated with the command:
#  opp_makemake -f --deep
#

# Name of target to be created (-o option)
TARGET = Wireless_Communication_Simulation$(EXE_SUFFIX)

# User interface (uncomment one) (-u option)
USERIF_LIBS = $(ALL_ENV_LIBS) # that is, $(TKENV_LIBS) $(QTENV_LIBS) $(CMDENV_LIBS)
#USERIF_LIBS = $(CMDENV_LIBS)
#USERIF_LIBS = $(TKENV_LIBS)
#USERIF_LIBS = $(QTENV_LIBS)

# C++ include paths (with -I)
INCLUDE_PATH = -I.

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS =

# Output directory
PROJECT_OUTPUT_DIR = ../out
PROJECTRELATIVE_PATH = src
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc, .msg and .sm files
OBJS = \
    $O/Channel.o \
    $O/MAC.o \
    $O/PacketGenerator.o \
    $O/PacketSink.o \
    $O/Transceiver.o \
    $O/AppMessage_m.o \
    $O/CSMAInternalMessage_m.o \
    $O/CSRequestMessage_m.o \
    $O/CSResponseMessage_m.o \
    $O/MacMessage_m.o \
    $O/SignalStartMessage_m.o \
    $O/SignalStopMessage_m.o \
    $O/TransmissionConfirmMessage_m.o \
    $O/TransmissionIndicationMessage_m.o \
    $O/TransmissionRequestMessage_m.o

# Message files
MSGFILES = \
    AppMessage.msg \
    CSMAInternalMessage.msg \
    CSRequestMessage.msg \
    CSResponseMessage.msg \
    MacMessage.msg \
    SignalStartMessage.msg \
    SignalStopMessage.msg \
    TransmissionConfirmMessage.msg \
    TransmissionIndicationMessage.msg \
    TransmissionRequestMessage.msg

# SM files
SMFILES =

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc or configuser.vc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIB_SUBDIR = $(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)
OMNETPP_LIBS = -L"$(OMNETPP_LIB_SUBDIR)" -L"$(OMNETPP_LIB_DIR)" -loppmain$D $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)

COPTS = $(CFLAGS)  $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)
SMCOPTS =

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE and have object
# files depend on it (except when "make depend" was called)
COPTS_FILE = $O/.last-copts
ifneq ($(MAKECMDGOALS),depend)
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
$(shell $(MKPATH) "$O" && echo "$(COPTS)" >$(COPTS_FILE))
endif
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# <<<
#------------------------------------------------------------------------------

# Main target
all: $O/$(TARGET)
	$(Q)$(LN) $O/$(TARGET) .

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile
	@$(MKPATH) $O
	@echo Creating executable: $@
	$(Q)$(CXX) $(LDFLAGS) -o $O/$(TARGET)  $(OBJS) $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS)

.PHONY: all clean cleanall depend msgheaders smheaders

.SUFFIXES: .cc

$O/%.o: %.cc $(COPTS_FILE)
	@$(MKPATH) $(dir $@)
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(qecho) MSGC: $<
	$(Q)$(MSGC) -s _m.cc $(MSGCOPTS) $?

%_sm.cc %_sm.h: %.sm
	$(qecho) SMC: $<
	$(Q)$(SMC) -c++ -suffix cc $(SMCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

smheaders: $(SMFILES:.sm=_sm.h)

clean:
	$(qecho) Cleaning...
	$(Q)-rm -rf $O
	$(Q)-rm -f Wireless_Communication_Simulation Wireless_Communication_Simulation.exe libWireless_Communication_Simulation.so libWireless_Communication_Simulation.a libWireless_Communication_Simulation.dll libWireless_Communication_Simulation.dylib
	$(Q)-rm -f ./*_m.cc ./*_m.h ./*_sm.cc ./*_sm.h

cleanall: clean
	$(Q)-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(qecho) Creating dependencies...
	$(Q)$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES) $(SM_CC_FILES)  ./*.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/AppMessage_m.o: AppMessage_m.cc \
	AppMessage_m.h
$O/CSMAInternalMessage_m.o: CSMAInternalMessage_m.cc \
	CSMAInternalMessage_m.h
$O/CSRequestMessage_m.o: CSRequestMessage_m.cc \
	CSRequestMessage_m.h
$O/CSResponseMessage_m.o: CSResponseMessage_m.cc \
	CSResponseMessage_m.h
$O/Channel.o: Channel.cc \
	SignalStartMessage_m.h \
	SignalStopMessage_m.h
$O/MAC.o: MAC.cc \
	AppMessage_m.h \
	CSRequestMessage_m.h \
	CSResponseMessage_m.h \
	MacMessage_m.h \
	TransmissionConfirmMessage_m.h \
	TransmissionIndicationMessage_m.h \
	TransmissionRequestMessage_m.h
$O/MacMessage_m.o: MacMessage_m.cc \
	MacMessage_m.h
$O/PacketGenerator.o: PacketGenerator.cc \
	AppMessage_m.h
$O/PacketSink.o: PacketSink.cc \
	AppMessage_m.h
$O/SignalStartMessage_m.o: SignalStartMessage_m.cc \
	SignalStartMessage_m.h
$O/SignalStopMessage_m.o: SignalStopMessage_m.cc \
	SignalStopMessage_m.h
$O/Transceiver.o: Transceiver.cc \
	AppMessage_m.h \
	CSRequestMessage_m.h \
	CSResponseMessage_m.h \
	MacMessage_m.h \
	SignalStartMessage_m.h \
	SignalStopMessage_m.h \
	TransmissionConfirmMessage_m.h \
	TransmissionIndicationMessage_m.h \
	TransmissionRequestMessage_m.h
$O/TransmissionConfirmMessage_m.o: TransmissionConfirmMessage_m.cc \
	TransmissionConfirmMessage_m.h
$O/TransmissionIndicationMessage_m.o: TransmissionIndicationMessage_m.cc \
	TransmissionIndicationMessage_m.h
$O/TransmissionRequestMessage_m.o: TransmissionRequestMessage_m.cc \
	TransmissionRequestMessage_m.h

