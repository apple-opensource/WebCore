/*
 * Copyright (C) 2015 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CDMSessionAVStreamSession_h
#define CDMSessionAVStreamSession_h

#include "CDMSessionMediaSourceAVFObjC.h"
#include "SourceBufferPrivateAVFObjC.h"
#include <wtf/RetainPtr.h>
#include <wtf/WeakPtr.h>

#if ENABLE(ENCRYPTED_MEDIA_V2) && ENABLE(MEDIA_SOURCE)

OBJC_CLASS AVStreamSession;
OBJC_CLASS CDMSessionAVStreamSessionObserver;

namespace WebCore {

class CDMPrivateMediaSourceAVFObjC;

class CDMSessionAVStreamSession : public CDMSessionMediaSourceAVFObjC {
public:
    CDMSessionAVStreamSession(const Vector<int>& protocolVersions, CDMPrivateMediaSourceAVFObjC&, CDMSessionClient*);
    virtual ~CDMSessionAVStreamSession();

    // CDMSession
    virtual CDMSessionType type() override { return CDMSessionTypeAVStreamSession; }
    virtual RefPtr<Uint8Array> generateKeyRequest(const String& mimeType, Uint8Array* initData, String& destinationURL, unsigned short& errorCode, unsigned long& systemCode) override;
    virtual void releaseKeys() override;
    virtual bool update(Uint8Array*, RefPtr<Uint8Array>& nextMessage, unsigned short& errorCode, unsigned long& systemCode) override;

    // CDMSessionMediaSourceAVFObjC
    void addParser(AVStreamDataParser*) override;
    void removeParser(AVStreamDataParser*) override;

    void setStreamSession(AVStreamSession*);

protected:
    PassRefPtr<Uint8Array> generateKeyReleaseMessage(unsigned short& errorCode, unsigned long& systemCode);

    WeakPtrFactory<CDMSessionAVStreamSession> m_weakPtrFactory;
    RetainPtr<AVStreamSession> m_streamSession;
    RefPtr<Uint8Array> m_initData;
    RefPtr<Uint8Array> m_certificate;
    RetainPtr<NSData> m_expiredSession;
    RetainPtr<CDMSessionAVStreamSessionObserver> m_dataParserObserver;
    Vector<int> m_protocolVersions;
    enum { Normal, KeyRelease } m_mode;
};

inline CDMSessionAVStreamSession* toCDMSessionAVStreamSession(CDMSession* session)
{
    if (!session || session->type() != CDMSessionTypeAVStreamSession)
        return nullptr;
    return static_cast<CDMSessionAVStreamSession*>(session);
}

}

#endif

#endif // CDMSessionAVStreamSession_h
