/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#ifndef NetworkStateNotifier_h
#define NetworkStateNotifier_h

#include <wtf/Noncopyable.h>


namespace WebCore {

#if (PLATFORM(QT) && ENABLE(QT_BEARER))
class NetworkStateNotifierPrivate;
#endif

class NetworkStateNotifier : public Noncopyable {
public:
    NetworkStateNotifier();
    void setNetworkStateChangedFunction(void (*)());
    
    bool onLine() const { return m_isOnLine; }
    
    void setIsOnLine(bool isOnLine);

#if (PLATFORM(QT) && ENABLE(QT_BEARER))
    void setNetworkAccessAllowed(bool);
#endif

private:    
    bool m_isOnLine;
    void (*m_networkStateChangedFunction)();

    void updateState();

};


NetworkStateNotifier& networkStateNotifier();
    
};

#endif // NetworkStateNotifier_h
