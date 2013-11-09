/*
 *  SoraRTTIException.h
 *
 *  Created by Griffin Bu on 5/22/11.
 *  Copyright 2011 Studio GameMaster(www.gamemastercn.com). All rights reserved.
 **/

/*
 This software is provided 'as-is', without any express or implied
 warranty. In no event will the authors be held liable for any
 damages arising from the use of this software.

 Permission is granted to anyone to use this software for any
 purpose, including commercial applications, and to alter it and
 redistribute it freely, subject to the following restrictions:

 1. The origin of this software must not be misrepresented; you must
 not claim that you wrote the original software. If you use this
 software in a product, an acknowledgment in the product documentation
 would be appreciated but is not required.

 2. Altered source versions must be plainly marked as such, and
 must not be misrepresented as being the original software.

 3. This notice may not be removed or altered from any source
 distribution.
 */


#ifndef SORA_RTTI_EXCEPTION_H_
#define SORA_RTTI_EXCEPTION_H_

#include <string>

namespace sora
{

    class SoraRTTIException
    {
    public:
        SoraRTTIException(const std::string& message): mMessage(message) {}

        std::string get() const
        {
            return mMessage;
        }

    private:
        std::string mMessage;
    };

} // namespace sora


#endif // SORA_RTTI_EXCEPTION_H_