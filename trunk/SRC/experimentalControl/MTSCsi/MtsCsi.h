#ifndef MtsCsi_h
#define MtsCsi_h

#ifdef BUILDING_MTSCSI
#define MTSCSI_EXPORT _declspec(dllexport)
#else
#define MTSCSI_EXPORT _declspec(dllimport)
#endif

namespace Mts
{

class MTSCSI_EXPORT ICsiController;

class MTSCSI_EXPORT ICsiDegreeOfFreedom {
public:
    virtual ~ICsiDegreeOfFreedom();

    virtual const char *    getName() const = 0;
    virtual void            setName(const char *name) = 0;

    virtual const char *    getMts793ControlChannel() const = 0;
    virtual void            setMts793ControlChannel(const char *ctrlChan) = 0;

    virtual const char *    getMts793ControlMode() const = 0;
    virtual void            setMts793ControlMode(const char *ctrlMode) = 0;
};

class MTSCSI_EXPORT ICsiDegreeOfFreedomList {
public:
    virtual ~ICsiDegreeOfFreedomList();

    virtual int     count() const = 0;
    virtual void    erase(int index) = 0;
    virtual void    clear() = 0;
    virtual void    add(ICsiDegreeOfFreedom* dof) = 0;
	virtual void	move(int baseIndex, int newIndex) = 0;

    virtual ICsiDegreeOfFreedom& operator[](int index) = 0;
};

class MTSCSI_EXPORT ICsiFeedbackSignal {
public:
    virtual ~ICsiFeedbackSignal();

    virtual const char *    getName() const = 0;
    virtual void            setName(const char *name) = 0;

    virtual const char *    getMts793Signal() const = 0;
    virtual void            setMts793Signal(const char *sigName) = 0;
};

class MTSCSI_EXPORT ICsiFeedbackSignalList {
public:
    virtual ~ICsiFeedbackSignalList();

    virtual int     count() const = 0;
    virtual void    erase(int index) = 0;
    virtual void    clear() = 0;
    virtual void    add(ICsiFeedbackSignal* fdbkSig) = 0;
	virtual void	move(int baseIndex, int newIndex) = 0;

    virtual ICsiFeedbackSignal& operator[](int index) = 0;
};

class MTSCSI_EXPORT ICsiControlPoint {
public:
    virtual ~ICsiControlPoint();

    virtual const char *    getName() const = 0;
    virtual void            setName(const char *name) = 0;

    virtual ICsiDegreeOfFreedomList&    getDegreesOfFreedom() = 0;
    virtual ICsiFeedbackSignalList&     getFeedbackSignals() = 0;
};

class MTSCSI_EXPORT ICsiControlPointList {
public:
    virtual ~ICsiControlPointList();

    virtual int     count() const = 0;
    virtual void    erase(int index) = 0;
    virtual void    clear() = 0;
    virtual void    add(ICsiControlPoint* ctrlPt) = 0;
	virtual void	move(int baseIndex, int newIndex) = 0;
    
	virtual ICsiControlPoint&   operator[](int index) = 0;
};

class MTSCSI_EXPORT ICsiDimensionUnit {
public:
    virtual ~ICsiDimensionUnit();

    virtual const char *    getDimension() const = 0;
    virtual void            setDimension(const char *dimName) = 0;

    virtual const char *    getUnit() const = 0;
    virtual void            setUnit(const char *unitName) = 0;
};

class MTSCSI_EXPORT ICsiDimensionUnitList {
public:
    virtual ~ICsiDimensionUnitList();

    virtual int     count() const = 0;
    virtual void    erase(int index) = 0;
    virtual void    clear() = 0;
    virtual void    add(ICsiDimensionUnit* dimUnit) = 0;
    
	virtual ICsiDimensionUnit&   operator[](const char *dimName) = 0;
    virtual ICsiDimensionUnit&   operator[](int index) = 0;
};

class MTSCSI_EXPORT ICsiConfiguration {
public:
    virtual ~ICsiConfiguration();

    virtual const char *    getName() const = 0;
    virtual void            setName(const char *name) = 0;

    virtual const char *    getFileName() const = 0;
    virtual void            setFileName(const char *fileName) = 0;

    virtual void clear() = 0;
    virtual void load(const char *cfgName) = 0;
    virtual void save(const char *cfgName) = 0;

    virtual ICsiControlPointList&   getControlPoints() = 0;
    virtual ICsiDimensionUnitList&  getDimensionUnits() = 0;
};

class MTSCSI_EXPORT ICsiException {
public:
    virtual ~ICsiException();

    virtual const char *what() const = 0;
};

class MTSCSI_EXPORT ICsiRamp {
public:
    virtual ~ICsiRamp();

    virtual double      getRampTime() const = 0;
    virtual void        setRampTime(double time) = 0;
    virtual int         getChannelCount() const = 0;
    virtual void        setChannelCount(int signals) = 0;
	virtual bool		getWaitUntilCompletion() const = 0;
	virtual void		setWaitUntilCompletion(bool block) = 0;
    virtual double&     operator[](int index) = 0;
};

typedef enum CsiProfileDataType
{
	CsiProfileData_CSV,
	CsiProfileData_CSVwithTime,
} CsiProfileDataType;

class MTSCSI_EXPORT ICsiProfile {
public:
    virtual ~ICsiProfile();

	virtual bool		getWaitUntilCompletion() const = 0;
	virtual void		setWaitUntilCompletion(bool block) = 0;
    virtual double      getDeltaT() const = 0;
    virtual void        setDeltaT(double deltaT) = 0;
    virtual int         getChannelCount() const = 0;
    virtual void        setChannelCount(int channels) = 0;
    virtual int         getPointCount() const = 0;
    virtual void        setPointCount(int points) = 0;
    virtual double&     point(int chanIndex, int ptIndex) = 0;

    virtual void		clear() = 0;
    virtual void		load(CsiProfileDataType dataType, const char *datName) = 0;
    virtual void		save(CsiProfileDataType dataType, const char *datName) = 0;
};
    
class MTSCSI_EXPORT ICsiCtlrEvents {
public:
    virtual ~ICsiCtlrEvents();

    virtual void rampComplete(ICsiController *ctlr, int rampId, double *vals);
    virtual void profileComplete(ICsiController *ctlr, int profileId, double *vals);
    virtual void error(const char *errMessage);
};

class MTSCSI_EXPORT IMts793Station {
public:
    virtual ~IMts793Station();

    virtual const char * getName() const = 0;
};

class MTSCSI_EXPORT IMts793StationList {
public:
    virtual ~IMts793StationList();

    virtual int count() const = 0;

    virtual const IMts793Station& operator[](int index) const = 0;
};

class MTSCSI_EXPORT IMts793ControlMode {
public:
    virtual ~IMts793ControlMode();

    virtual const char * getName() const = 0;
	virtual const char * getFeedbackSignal() const = 0;
};

class MTSCSI_EXPORT IMts793ControlModeList {
public:
    virtual ~IMts793ControlModeList();

    virtual int count() const = 0;

    virtual const IMts793ControlMode& operator[](int index) const = 0;
};

class MTSCSI_EXPORT IMts793ControlChannel {
public:
    virtual ~IMts793ControlChannel();

    virtual const char * getName() const = 0;

    virtual const IMts793ControlModeList& getModes() const = 0;
};

class MTSCSI_EXPORT IMts793ControlChannelList {
public:
    virtual ~IMts793ControlChannelList();

    virtual int count() const = 0;

    virtual const IMts793ControlChannel& operator[](int index) const = 0;
};

class MTSCSI_EXPORT IMts793Signal {
public:
    virtual ~IMts793Signal();

    virtual const char * getName() const = 0;
	virtual const char * getDimension() const = 0;
};

class MTSCSI_EXPORT IMts793SignalList {
public:
    virtual ~IMts793SignalList();

    virtual int count() const = 0;

    virtual const IMts793Signal& operator[](int index) const = 0;
};

class MTSCSI_EXPORT IMts793Unit {
public:
    virtual ~IMts793Unit();

    virtual const char * getName() const = 0;
};

class MTSCSI_EXPORT IMts793UnitList {
public:
    virtual ~IMts793UnitList();

    virtual int count() const = 0;

    virtual const IMts793Unit& operator[](int index) const = 0;
};

class MTSCSI_EXPORT IMts793Dimension {
public:
    virtual ~IMts793Dimension();

    virtual const char * getName() const = 0;
	
	virtual const IMts793Unit&		getDefaultUnit() const = 0;
    virtual const IMts793UnitList&	getUnits() const = 0;
};

class MTSCSI_EXPORT IMts793DimensionList {
public:
    virtual ~IMts793DimensionList();

    virtual int count() const = 0;

    virtual const IMts793Dimension& operator[](int index) const = 0;
};

typedef enum CsiControllerState
{
	CsiCtlrInitial,
    CsiCtlrReady,
	CsiCtlrHardwareReady,
	CsiCtlrTestRunning,
} CsiControllerState;

class MTSCSI_EXPORT ICsiController {
public:
    virtual ~ICsiController();

	virtual const IMts793StationList&		 getStations() const = 0;
    virtual const IMts793ControlChannelList& getControlChannels() const = 0;
    virtual const IMts793SignalList&         getSignals() const = 0;
    virtual const IMts793DimensionList&      getDimensions() const = 0;

	virtual const char *	getStationName() const = 0;
	virtual void			setStationName(const char *stationName) = 0;

    virtual ICsiConfiguration& getConfiguration() = 0;

    virtual void loadConfiguration(const char *cfgFile) = 0;
    virtual void saveConfiguration(const char *cfgFile) = 0;

    virtual void reset() = 0;
    virtual void startHardware() = 0;
    virtual void startTest() = 0;
    virtual void stopTest() = 0;
    virtual void stopHardware() = 0;

	virtual void resetTime() = 0;
	
	virtual CsiControllerState	getState() const = 0;

    virtual ICsiCtlrEvents *getEventHandler() = 0;
    virtual void            setEventHandler(ICsiCtlrEvents *evts) = 0;

    virtual int  runRamp(ICsiRamp* ramp) = 0;
    virtual int  runProfile(ICsiProfile* profile) = 0;
    virtual bool acquireFeedback(int rampId, double *vals) = 0;
};

class MTSCSI_EXPORT CsiFactory {
public:
    static ICsiController      * newController();
    static ICsiControlPoint    * newControlPoint();
    static ICsiConfiguration   * newConfiguration();
    static ICsiDegreeOfFreedom * newDegreeOfFreedom();
    static ICsiDimensionUnit   * newDimensionUnit();
    static ICsiFeedbackSignal  * newFeedbackSignal();
    static ICsiProfile         * newProfile();
    static ICsiRamp            * newRamp();
};

}

#endif
