/*
-----------------------------------------------------------------------------
This source file is part of "esnetwork" library.
It is licensed under the terms of the BSD license.
For the latest info, see http://esnetwork.sourceforge.net

Copyright (c) 2012-2013 Lin Jia Jun (Joe Lam)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-----------------------------------------------------------------------------
*/

#ifndef _ESN_SHAREDDATA_H_
#define _ESN_SHAREDDATA_H_

#include <boost/thread/mutex.hpp>

#include "CommonData.h"

namespace esnlib
{

/// Shared Data class
class SharedData : public CommonData
{
public:

    SharedData(boost::mutex & mutex,
               std::map<int, int> & intintmap,
               std::map<std::string, int> & strintmap,
               std::map<int, std::string> & intstrmap,
               std::map<std::string, std::string> & strstrmap,
               std::map< int, boost::shared_ptr<IoBuffer> > & intbufmap,
               std::map< std::string, boost::shared_ptr<IoBuffer> > & strbufmap,
               std::map< int, boost::shared_ptr<Business> > & intobjmap,
               std::map< std::string, boost::shared_ptr<Business> > & strobjmap
               );
    virtual ~SharedData();

    /// Put integer value with integer key into map
    ///
    /// @param key The integer key
    /// @param value The integer value
    /// @return The size of the map
    virtual int PutInt(int key, int value);

    /// Put integer value with string key into map
    ///
    /// @param key The string key
    /// @param value The integer value
    /// @return The size of the map
    virtual int PutInt(const std::string& key, int value);

    /// Get integer value with integer key
    ///
    /// @param key The integer key
    /// @return The integer value
    virtual int GetInt(int key);

    /// Get integer value with string key
    ///
    /// @param key The string key
    /// @return The integer value
    virtual int GetInt(const std::string& key);

    /// Get the integer-integer map
    ///
    /// @return The integer-integer map
    virtual std::map<int, int> & GetIntIntMap();

    /// Get the string-integer map
    ///
    /// @return The string-integer map
    virtual std::map<std::string, int> & GetStrIntMap();

    /// Put string value with string key into map
    ///
    /// @param key The string key
    /// @param value The string value
    /// @return The size of the map
    virtual int PutStr(const std::string& key, const std::string& value);

    /// Put string value with integer key into map
    ///
    /// @param key The integer key
    /// @param value The string value
    /// @return The size of the map
    virtual int PutStr(int key, const std::string& value);

    /// Get string value with string key
    ///
    /// @param key The string key
    /// @return The string value
    virtual std::string GetStr(const std::string& key);

    /// Get string value with integer key
    ///
    /// @param key The integer key
    /// @return The string value
    virtual std::string GetStr(int key);

    /// Get the integer-string map
    ///
    /// @return The integer-string map
    virtual std::map<int, std::string> & GetIntStrMap();

    /// Get the string-string map
    ///
    /// @return The string-string map
    virtual std::map<std::string, std::string> & GetStrStrMap();

    /// Put buffer value with integer key into map
    ///
    /// @param key The integer key
    /// @param value The buffer value (a pointer of IO buffer)
    /// @return The size of the map
    virtual int PutBuf(int key, boost::shared_ptr<IoBuffer> value);

    /// Put buffer value with string key into map
    ///
    /// @param key The string key
    /// @param value The buffer value (a pointer of IO buffer)
    /// @return The size of the map
    virtual int PutBuf(const std::string& key, boost::shared_ptr<IoBuffer> value);

    /// Get buffer value with integer key
    ///
    /// @param key The integer key
    /// @return The buffer value (a pointer of IO buffer)
    virtual boost::shared_ptr<IoBuffer> GetBuf(int key);

    /// Get buffer value with string key
    ///
    /// @param key The string key
    /// @return The buffer value (a pointer of IO buffer)
    virtual boost::shared_ptr<IoBuffer> GetBuf(const std::string& key);

    /// Get the integer-buffer map
    ///
    /// @return The integer-buffer map
    virtual std::map< int, boost::shared_ptr<IoBuffer> > & GetIntBufMap();

    /// Get the string-buffer map
    ///
    /// @return The string-buffer map
    virtual std::map< std::string, boost::shared_ptr<IoBuffer> > & GetStrBufMap();



    /// Put object value with integer key into map
    ///
    /// @param key The integer key
    /// @param value The object value (a pointer of Business object)
    /// @return The size of the map
    virtual int PutObj(int key, boost::shared_ptr<Business> value);

    /// Put object value with string key into map
    ///
    /// @param key The string key
    /// @param value The object value (a pointer of Business object)
    /// @return The size of the map
    virtual int PutObj(const std::string& key, boost::shared_ptr<Business> value);

    /// Get object value with integer key
    ///
    /// @param key The integer key
    /// @return The object value (a pointer of Business object)
    virtual boost::shared_ptr<Business> GetObj(int key);

    /// Get object value with string key
    ///
    /// @param key The string key
    /// @return The object value (a pointer of Business object)
    virtual boost::shared_ptr<Business> GetObj(const std::string& key);

    /// Get the integer-object map
    ///
    /// @return The integer-object map
    virtual std::map< int, boost::shared_ptr<Business> > & GetIntObjMap();

    /// Get the string-object map
    ///
    /// @return The string-object map
    virtual std::map< std::string, boost::shared_ptr<Business> > & GetStrObjMap();



protected:
private:

    boost::mutex & m_mutex;

    boost::unique_lock<boost::mutex> m_lock;

    std::map<int, int> & m_intintmap;
    std::map<std::string, int> & m_strintmap;

    std::map<int, std::string> & m_intstrmap;
    std::map<std::string, std::string> & m_strstrmap;

    std::map< int, boost::shared_ptr<IoBuffer> > & m_intbufmap;
    std::map< std::string, boost::shared_ptr<IoBuffer> > & m_strbufmap;

    std::map< int, boost::shared_ptr<Business> > & m_intobjmap;
    std::map< std::string, boost::shared_ptr<Business> > & m_strobjmap;

};

typedef boost::shared_ptr<SharedData> SharedDataPtr;

}

#endif // _ESN_SHAREDDATA_H_

