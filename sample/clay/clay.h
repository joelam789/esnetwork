/*
-----------------------------------------------------------------------------
This source file is part of "clay" library.
It is licensed under the terms of the MIT license.
For the latest info, see http://libclay.sourceforge.net

Copyright (c) 2010-2013 Lin Jia Jun (Joe Lam)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#ifndef __CLAY_H_INCLUDED__
#define __CLAY_H_INCLUDED__

#define CLAY_SVC_FUNC(func) (clay::core::CLAY_SVC_CLASS_FUNC)(&func)

#include <map>
#include <string>

namespace clay
{

namespace core
{

/// Service Provider Class
class ClayServiceProvider;

typedef int (*CLAY_SVC_STATIC_FUNC)(void*);

typedef int (ClayServiceProvider::*CLAY_SVC_CLASS_FUNC)(void*);

typedef std::map<int, CLAY_SVC_CLASS_FUNC> ClayServiceIntMap;
typedef std::map<std::string, CLAY_SVC_CLASS_FUNC> ClayServiceStrMap;

typedef std::map<int, ClayServiceProvider*> ClayServiceProviderIntMap;
typedef std::map<std::string, ClayServiceProvider*> ClayServiceProviderStrMap;

/// Service Provider Class
class ClayServiceProvider
{
protected:

    /// The code of the service provider(default value is 0)
    int m_code;

    /// The name of the service provider(default value is empty string)
    std::string m_name;

    /// The code map of the services
    ClayServiceIntMap m_svcintmap;

    /// The name map of the services
    ClayServiceStrMap m_svcstrmap;

    /// Remove all services
    void ClearServices();

    /// Register a service by its code
    ///
    /// @param iSvcCode The code of the service
    /// @param pSvcFunc The service function pointer
    /// @return Return -1 for fail
    int RegisterService(int iSvcCode, CLAY_SVC_CLASS_FUNC pSvcFunc);

    /// Register a service by its name
    ///
    /// @param sSvcName The name of the service
    /// @param pSvcFunc The service function pointer
    /// @return Return -1 for fail
    int RegisterService(const std::string& sSvcName, CLAY_SVC_CLASS_FUNC pSvcFunc);

    /// Unregister a service by its code
    ///
    /// @param iSvcCode The code of the service
    /// @return If the service is not found then return 0
    int UnregisterService(int iSvcCode);

    /// Unregister a service by its name
    ///
    /// @param sSvcName The name of the service
    /// @return If the service is not found then return 0
    int UnregisterService(const std::string& sSvcName);

    /// Get the count of the registered services
    ///
    /// @return The total count of the services
    int GetServiceCount();

    /// Setup all services
    virtual void SetupServices();

    /// Get the version of the service provider
    ///
    /// @param pParam The parameter(normally it should always be NULL)
    /// @return Return the version code of the service provider
    int GetVersion(void* pParam);

public:

    /// Get the code of the service provider
    ///
    /// @return Return the code of the service provider
    int GetCode();

    /// Get the name of the service provider
    ///
    /// @return Return the name of the service provider
    std::string GetName();

    /// Check the existence of the service by its code
    ///
    /// @param iSvcCode The code of the service
    /// @return Return true if the service with the code exists
    bool ServiceExists(int iSvcCode);

    /// Check the existence of the service by its name
    ///
    /// @param sSvcName The name of the service
    /// @return Return true if the service with the name exists
    bool ServiceExists(const std::string& sSvcName);

    /// Get the pointer of the service function by the service code
    ///
    /// @param iSvcCode The service code
    /// @return Return the pointer of the service function(or return NULL for none found)
    CLAY_SVC_CLASS_FUNC GetService(int iSvcCode);

    /// Get the pointer of the service function by the service name
    ///
    /// @param sSvcName The service name
    /// @return Return the pointer of the service function(or return NULL for none found)
    CLAY_SVC_CLASS_FUNC GetService(const std::string& sSvcName);

    /// Execute the service function by the service code
    ///
    /// @param iSvcCode The service code
    /// @param pParam The parameter passed to the service function
    /// @return The return code of the service function
    virtual int DoService(int iSvcCode, void* pParam);

    /// Execute the service function by the service name
    ///
    /// @param sSvcName The service name
    /// @param pParam The parameter passed to the service function
    /// @return The return code of the service function
    virtual int DoService(const std::string& sSvcName, void* pParam);

    /// Constructor function
    ///
    /// @param iCode The service code
    explicit ClayServiceProvider(int iCode);

    /// Constructor function
    ///
    /// @param sName The service name
    explicit ClayServiceProvider(const std::string& sName);

    /// Constructor function
    ///
    /// @param iCode The service code
    /// @param sName The service name
    ClayServiceProvider(int iCode, const std::string& sName);

    /// Destructor function
    virtual ~ClayServiceProvider();

};

/// Service Agent Class
class ClayServiceAgent
{

private:

    ClayServiceProviderIntMap  m_spintmap;
    ClayServiceProviderStrMap  m_spstrmap;

    ClayServiceAgent* m_external;

public:

    ClayServiceAgent();
    ~ClayServiceAgent();

    /// Register a service provider
    ///
    /// @param pServiceProvider The pointer of the service provider
    /// @return Return -1 if fail
    int RegSp(ClayServiceProvider* pServiceProvider);

    /// Unregister a service provider by its code
    ///
    /// @param iSpCode The code of the service provider
    /// @return If the service provider is not found then return 0
    int UnregSp(int iSpCode);

    /// Unregister a service provider by its name
    ///
    /// @param sSpName The name of the service provider
    /// @return If the service provider is not found then return 0
    int UnregSp(const std::string& sSpName);

    /// Get the service provider by its code
    ///
    /// @param iSpCode The code of the service provider
    /// @return The pointer of the service provider
    ClayServiceProvider* GetSp(int iSpCode);

    /// Get the service provider by its name
    ///
    /// @param sSpName The name of the service provider
    /// @return The pointer of the service provider
    ClayServiceProvider* GetSp(const std::string& sSpName);

    /// Get the count of the registered service providers
    ///
    /// @return The total count of the service providers
    int GetSpCount();

    /// Execute the service function by the provider code and the service code
    ///
    /// @param iSpCode The provider code
    /// @param iSvcCode The service code
    /// @param pParam The parameter passed to the service function
    /// @return The return code of the service function
    int CallService(int iSpCode, int iSvcCode, void* pParam);

    /// Execute the service function by the provider code and the service name
    ///
    /// @param iSpCode The provider code
    /// @param sSvcName The service name
    /// @param pParam The parameter passed to the service function
    /// @return The return code of the service function
    int CallService(int iSpCode, const std::string& sSvcName, void* pParam);

    /// Execute the service function by the provider name and the service code
    ///
    /// @param sSpName The provider name
    /// @param iSvcCode The service code
    /// @param pParam The parameter passed to the service function
    /// @return The return code of the service function
    int CallService(const std::string& sSpName, int iSvcCode, void* pParam);

    /// Execute the service function by the provider name and the service name
    ///
    /// @param sSpName The provider name
    /// @param sSvcName The service name
    /// @param pParam The parameter passed to the service function
    /// @return The return code of the service function
    int CallService(const std::string& sSpName, const std::string& sSvcName, void* pParam);

    /// Check the existence of the service provider by its code
    ///
    /// @param iSpCode The code of the service provider
    /// @return Return true if the service provider with the code exists
    bool SpExists(int iSpCode);

    /// Check the existence of the service provider by its name
    ///
    /// @param sSpName The name of the service provider
    /// @return Return true if the service provider with the name exists
    bool SpExists(const std::string& sSpName);

    /// Check the existence of the service by provider code and service code
    ///
    /// @param iSpCode The provider code
    /// @param iSvcCode The service code
    /// @return Return true if the service exists
    bool SvcExists(int iSpCode, int iSvcCode);

    /// Check the existence of the service by provider code and service name
    ///
    /// @param iSpCode The provider code
    /// @param sSvcName The service name
    /// @return Return true if the service exists
    bool SvcExists(int iSpCode, const std::string& sSvcName);

    /// Check the existence of the service by provider name and service code
    ///
    /// @param sSpName The provider name
    /// @param iSvcCode The service code
    /// @return Return true if the service exists
    bool SvcExists(const std::string& sSpName, int iSvcCode);

    /// Check the existence of the service by provider name and service name
    ///
    /// @param sSpName The provider name
    /// @param sSvcName The service name
    /// @return Return true if the service exists
    bool SvcExists(const std::string& sSpName, const std::string& sSvcName);

    /// Remove all service providers
    void Clear();

    /// Set external agent (normally it will be called only when integrating with external DLL file)
    ///
    /// @param pAgent The pointer of another service agent object
    void SetExternalAgent(ClayServiceAgent* pAgent);

    /// Get current active service agent
    ///
    /// @return The pointer of current active agent
    static ClayServiceAgent* GetAgent()
    {
        // Meyer's Singleton ...
        static ClayServiceAgent instance;

        ClayServiceAgent* pAgent = instance.m_external;

        if(pAgent != NULL) return pAgent;
        else return &instance;
    }


};


} // end of clay::core


namespace lib
{

/** \addtogroup Clay
 *  @{
 */

/// Get system time as a simple string
///
/// @return Return date time in string format
std::string GetTimeStr();

/// Write a message to log file
///
/// @param sMsg The message
void Log(const char* sMsg);

/// Get current active service agent
///
/// @return The pointer of current active service agent
clay::core::ClayServiceAgent* GetAgent();

/// Set current active service agent
///
/// @param pAgent The pointer of a service agent
void SetAgent(void* pAgent);

/// Register a service provider
///
/// @param pServiceProvider The pointer of the service provider
/// @return Return -1 if fail
int RegisterSp(clay::core::ClayServiceProvider* pServiceProvider);

/// Get the count of the registered service providers
///
/// @return The total count of the service providers
int GetSpCount();

/// Get the service provider by its code
///
/// @param iSpCode The code of the service provider
/// @return The pointer of the service provider
clay::core::ClayServiceProvider* GetProvider(int iSpCode);

/// Get the service provider by its name
///
/// @param sSpName The name of the service provider
/// @return The pointer of the service provider
clay::core::ClayServiceProvider* GetProvider(const std::string& sSpName);

/// Check the existence of the service provider by its code
///
/// @param iSpCode The code of the service provider
/// @return Return true if the service provider with the code exists
bool ProviderExists(int iSpCode);

/// Check the existence of the service provider by its name
///
/// @param sSpName The name of the service provider
/// @return Return true if the service provider with the name exists
bool ProviderExists(const std::string& sSpName);

/// Check the existence of the service by provider code and service code
///
/// @param iSpCode The provider code
/// @param iSvcCode The service code
/// @return Return true if the service exists
bool ServiceExists(int iSpCode, int iSvcCode);

/// Check the existence of the service by provider code and service name
///
/// @param iSpCode The provider code
/// @param sSvcName The service name
/// @return Return true if the service exists
bool ServiceExists(int iSpCode, const std::string& sSvcName);

/// Check the existence of the service by provider name and service code
///
/// @param sSpName The provider name
/// @param iSvcCode The service code
/// @return Return true if the service exists
bool ServiceExists(const std::string& sSpName, int iSvcCode);

/// Check the existence of the service by provider name and service name
///
/// @param sSpName The provider name
/// @param sSvcName The service name
/// @return Return true if the service exists
bool ServiceExists(const std::string& sSpName, const std::string& sSvcName);

/// Execute the service function by the provider code and the service code
///
/// @param iSpCode The provider code
/// @param iSvcCode The service code
/// @param pParam The parameter passed to the service function
/// @return The return code of the service function
int CallService(int iSpCode, int iSvcCode, void* pParam = NULL);

/// Execute the service function by the provider code and the service name
///
/// @param iSpCode The provider code
/// @param sSvcName The service name
/// @param pParam The parameter passed to the service function
/// @return The return code of the service function
int CallService(int iSpCode, const std::string& sSvcName, void* pParam = NULL);

/// Execute the service function by the provider name and the service code
///
/// @param sSpName The provider name
/// @param iSvcCode The service code
/// @param pParam The parameter passed to the service function
/// @return The return code of the service function
int CallService(const std::string& sSpName, int iSvcCode, void* pParam = NULL);

/// Execute the service function by the provider name and the service name
///
/// @param sSpName The provider name
/// @param sSvcName The service name
/// @param pParam The parameter passed to the service function
/// @return The return code of the service function
int CallService(const std::string& sSpName, const std::string& sSvcName, void* pParam = NULL);

/// Remove all service providers
void Clear();

/** @} */

} // end of clay::lib

} // end of clay


#endif // __CLAY_H_INCLUDED__
