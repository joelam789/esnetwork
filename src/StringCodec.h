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

#ifndef _ESN_STRINGCODEC_H_
#define _ESN_STRINGCODEC_H_

#include "IoFilter.h"

namespace esnlib
{

/// Common String Codec Filter class, a filter who can extract a string(ends with specific char) from incoming bytes
class StringCodec : public IoFilter
{
public:

    /// Constructor function
    StringCodec();

    /// Constructor function
    ///
    /// @param separator The separator, a character that the string will end with
    /// @param maxstrlen The maximum length of the string
    StringCodec(char separator, int maxstrlen);

    /// Destructor function
    virtual ~StringCodec();

    /// Extract useful data (the string) from the incoming bytes
    ///
    /// @param session Current session
    /// @param data The pointer of the incoming bytes
    /// @param readylist The list used to save the useful data extracted
    /// @return Return true if some useful data has been extracted successfully
    virtual bool Extract(SessionPtr session, IoBufferPtr data, std::vector<IoBufferPtr>& readylist);

    /// Encode the raw buffer
    ///
    /// @param session Current session
    /// @param data The pointer of the raw buffer (the pointer of the string's first char)
    /// @return The pointer of the IO buffer encoded
    virtual IoBufferPtr Encode(SessionPtr session, void* data);

    /// Decode the IO buffer
    ///
    /// @param session Current session
    /// @param data The pointer of the IO buffer encoded
    /// @return The original raw buffer (the pointer of the string's first char)
    virtual void* Decode(SessionPtr session, IoBufferPtr data);

protected:
private:

    char m_separator;
    int m_maxstrlen;

};

typedef boost::shared_ptr<StringCodec> StringCodecPtr;

}

#endif // _ESN_STRINGCODEC_H_

