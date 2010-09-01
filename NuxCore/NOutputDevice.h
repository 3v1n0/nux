#ifndef NOUTPUTDEVICE_H
#define NOUTPUTDEVICE_H

NAMESPACE_BEGIN

class NSerializer;

class NOutputDevice
{
public:
    NOutputDevice()
    : m_ObjectDestroyed(FALSE)
    { }

    virtual ~NOutputDevice()
    {
        m_ObjectDestroyed = TRUE;
    }

    BOOL m_terminated;
    virtual void Serialize( const TCHAR* V, const TCHAR* LogPrefix) = 0;
    virtual void Flush();
    virtual void Shutdown();

    VARARG_DECL( void/*FuncRet*/, void/*StaticFuncRet*/, {}/*Return*/, LogFunction/*FuncName*/, VARARG_NONE/*Pure*/, const TCHAR*/*FmtType*/, VARARG_NONE/*ExtraDecl*/, VARARG_NONE/*ExtraParam*/ );

protected:
    BOOL m_ObjectDestroyed;
};

/*!
    Output to null device.
*/
class NNullOutput : public NOutputDevice
{
    INL_DECLARE_GLOBAL_OBJECT(NNullOutput, NGlobalSingletonInitializer);
public:
    void Serialize( const TCHAR* V, const TCHAR* LogPrefix)
    {}
};

/*!
    Output to log file.
*/
class NOutputLogFile : public NOutputDevice
{
    INL_DECLARE_GLOBAL_OBJECT(NOutputLogFile, NGlobalSingletonInitializer);
    //NOutputLogFile( const TCHAR* InFilename);

public:
    /*!
        Closes output device and cleans up.
    */
    void Shutdown();

    /*!
        Flush the write cache so the output to the file isn't truncated.
    */
    void Flush();

    /*!
        Write a stream to the log file.
        
        @param Data         Stream characters to write.
        @param LogPrefix    A string to write before the input stream of characters.
    */
    void Serialize(const TCHAR* Data, const TCHAR* LogPrefix);

private:
    NSerializer*    m_LogSerializer;
    NString         m_Filename;
    bool            m_Opened;
    bool            m_Closed;

    /*!
        Serialize data directly to the log file without any type of conversion or preprocessing.

        @param Data     String of char to write to the output file.
    */
    void SerializeRaw(const TCHAR* Data);
};

/*!
    Output to microsoft visual console.
*/
class NOutputVisualDebugConsole : public NOutputDevice
{
    INL_DECLARE_GLOBAL_OBJECT(NOutputVisualDebugConsole, NGlobalSingletonInitializer);
public:

    //! Write data to visual studio output debug console.
    /*!
        @param  Data        Text to log.
        @param  LogPrefix	Prefix for the text
    */
    void Serialize(const TCHAR* Data, const TCHAR* LogPrefix);
};

class NOutputDeviceManager : public NOutputDevice
{
public:
    NOutputDeviceManager(){}
    ~NOutputDeviceManager(){}

    virtual void AddOutputDevice(NOutputDevice* OutputDevice) = 0;
    virtual void RemoveOutputDevice(NOutputDevice* OutputDevice) = 0;
    virtual bool IsRedirectingTo(NOutputDevice* OutputDevice) = 0;
};

class NOutputDeviceRedirector : public NOutputDeviceManager
{
    INL_DECLARE_GLOBAL_OBJECT(NOutputDeviceRedirector, NGlobalSingletonInitializer);
public:
    virtual void AddOutputDevice(NOutputDevice* OutputDevice);
    virtual void RemoveOutputDevice(NOutputDevice* OutputDevice);
    virtual bool IsRedirectingTo(NOutputDevice* OutputDevice);
    void Serialize(const TCHAR* Data, const TCHAR* LogPrefix);
    void Flush();

    void Shutdown();

private:
    std::vector<NOutputDevice*> OutputDevices;
};

NAMESPACE_END

#endif // NOUTPUTDEVICE_H
