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

#include "SharedData.h"

using namespace esnlib;

SharedData::SharedData(boost::mutex & mutex,
                       std::map<int, int> & intintmap,
                       std::map<std::string, int> & strintmap,
                       std::map<int, std::string> & intstrmap,
                       std::map<std::string, std::string> & strstrmap,
                       std::map< int, boost::shared_ptr<IoBuffer> > & intbufmap,
                       std::map< std::string, boost::shared_ptr<IoBuffer> > & strbufmap,
                       std::map< int, boost::shared_ptr<Business> > & intobjmap,
                       std::map< std::string, boost::shared_ptr<Business> > & strobjmap)
: m_mutex(mutex)
, m_lock(mutex)
, m_intintmap(intintmap)
, m_strintmap(strintmap)
, m_intstrmap(intstrmap)
, m_strstrmap(strstrmap)
, m_intbufmap(intbufmap)
, m_strbufmap(strbufmap)
, m_intobjmap(intobjmap)
, m_strobjmap(strobjmap)
{
    //ctor

    //printf("Locked SharedData: %d\n", (int)this);
}

SharedData::~SharedData()
{
    //dtor
    m_lock.unlock();

    //printf("Unlocked SharedData: %d\n", (int)this);
}

int SharedData::PutInt(int key, int value)
{
    std::map<int, int>::iterator it = m_intintmap.find(key);
	if (it != m_intintmap.end()) m_intintmap.erase(it);
	m_intintmap.insert(std::map<int, int>::value_type(key, value));
	return m_intintmap.size();
}
int SharedData::PutInt(const std::string& key, int value)
{
    std::map<std::string, int>::iterator it = m_strintmap.find(key);
	if (it != m_strintmap.end()) m_strintmap.erase(it);
	m_strintmap.insert(std::map<std::string, int>::value_type(key, value));
	return m_strintmap.size();
}
int SharedData::GetInt(int key)
{
    int value = 0;
    std::map<int, int>::iterator it = m_intintmap.find(key);
	if (it != m_intintmap.end()) value = it->second;
	return value;
}
int SharedData::GetInt(const std::string& key)
{
    int value = 0;
    std::map<std::string, int>::iterator it = m_strintmap.find(key);
	if (it != m_strintmap.end()) value = it->second;
	return value;
}

std::map<int, int> & SharedData::GetIntIntMap()
{
    return m_intintmap;
}
std::map<std::string, int> & SharedData::GetStrIntMap()
{
    return m_strintmap;
}

int SharedData::PutStr(const std::string& key, const std::string& value)
{
    std::map<std::string, std::string>::iterator it = m_strstrmap.find(key);
	if (it != m_strstrmap.end()) m_strstrmap.erase(it);
	m_strstrmap.insert(std::map<std::string, std::string>::value_type(key, value));
	return m_strstrmap.size();
}

int SharedData::PutStr(int key, const std::string& value)
{
    std::map<int, std::string>::iterator it = m_intstrmap.find(key);
	if (it != m_intstrmap.end()) m_intstrmap.erase(it);
	m_intstrmap.insert(std::map<int, std::string>::value_type(key, value));
	return m_intstrmap.size();
}

std::string SharedData::GetStr(const std::string& key)
{
    std::string value = "";
    std::map<std::string, std::string>::iterator it = m_strstrmap.find(key);
	if (it != m_strstrmap.end()) value = it->second;
	return value;
}
std::string SharedData::GetStr(int key)
{
    std::string value = "";
    std::map<int, std::string>::iterator it = m_intstrmap.find(key);
	if (it != m_intstrmap.end()) value = it->second;
	return value;
}

std::map<int, std::string> & SharedData::GetIntStrMap()
{
    return m_intstrmap;
}
std::map<std::string, std::string> & SharedData::GetStrStrMap()
{
    return m_strstrmap;
}

int SharedData::PutBuf(int key, boost::shared_ptr<IoBuffer> value)
{
    std::map< int, boost::shared_ptr<IoBuffer> >::iterator it = m_intbufmap.find(key);
	if (it != m_intbufmap.end()) m_intbufmap.erase(it);
	m_intbufmap.insert(std::map< int, boost::shared_ptr<IoBuffer> >::value_type(key, value));
	return m_intbufmap.size();
}
int SharedData::PutBuf(const std::string& key, boost::shared_ptr<IoBuffer> value)
{
    std::map< std::string, boost::shared_ptr<IoBuffer> >::iterator it = m_strbufmap.find(key);
	if (it != m_strbufmap.end()) m_strbufmap.erase(it);
	m_strbufmap.insert(std::map< std::string, boost::shared_ptr<IoBuffer> >::value_type(key, value));
	return m_strbufmap.size();
}

boost::shared_ptr<IoBuffer> SharedData::GetBuf(int key)
{
    boost::shared_ptr<IoBuffer> value;
    std::map< int, boost::shared_ptr<IoBuffer> >::iterator it = m_intbufmap.find(key);
	if (it != m_intbufmap.end()) value = it->second;
	return value;
}
boost::shared_ptr<IoBuffer> SharedData::GetBuf(const std::string& key)
{
    boost::shared_ptr<IoBuffer> value;
    std::map< std::string, boost::shared_ptr<IoBuffer> >::iterator it = m_strbufmap.find(key);
	if (it != m_strbufmap.end()) value = it->second;
	return value;
}

std::map< int, boost::shared_ptr<IoBuffer> > & SharedData::GetIntBufMap()
{
    return m_intbufmap;
}
std::map< std::string, boost::shared_ptr<IoBuffer> > & SharedData::GetStrBufMap()
{
    return m_strbufmap;
}


int SharedData::PutObj(int key, boost::shared_ptr<Business> value)
{
    std::map< int, boost::shared_ptr<Business> >::iterator it = m_intobjmap.find(key);
	if (it != m_intobjmap.end()) m_intobjmap.erase(it);
	m_intobjmap.insert(std::map< int, boost::shared_ptr<Business> >::value_type(key, value));
	return m_intobjmap.size();
}
int SharedData::PutObj(const std::string& key, boost::shared_ptr<Business> value)
{
    std::map< std::string, boost::shared_ptr<Business> >::iterator it = m_strobjmap.find(key);
	if (it != m_strobjmap.end()) m_strobjmap.erase(it);
	m_strobjmap.insert(std::map< std::string, boost::shared_ptr<Business> >::value_type(key, value));
	return m_strobjmap.size();
}

boost::shared_ptr<Business> SharedData::GetObj(int key)
{
    boost::shared_ptr<Business> value;
    std::map< int, boost::shared_ptr<Business> >::iterator it = m_intobjmap.find(key);
	if (it != m_intobjmap.end()) value = it->second;
	return value;
}
boost::shared_ptr<Business> SharedData::GetObj(const std::string& key)
{
    boost::shared_ptr<Business> value;
    std::map< std::string, boost::shared_ptr<Business> >::iterator it = m_strobjmap.find(key);
	if (it != m_strobjmap.end()) value = it->second;
	return value;
}

std::map< int, boost::shared_ptr<Business> > & SharedData::GetIntObjMap()
{
    return m_intobjmap;
}
std::map< std::string, boost::shared_ptr<Business> > & SharedData::GetStrObjMap()
{
    return m_strobjmap;
}

