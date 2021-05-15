/*
 * Copyright (C) 2020 Igalia S.L. All rights reserved.
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

#pragma once

#if ENABLE(WEBXR)

#include "ExceptionOr.h"
#include <JavaScriptCore/Float32Array.h>
#include <wtf/IsoMalloc.h>
#include <wtf/RefCounted.h>

namespace WebCore {

struct DOMPointInit;
class DOMPointReadOnly;

class WebXRRigidTransform : public RefCounted<WebXRRigidTransform> {
    WTF_MAKE_ISO_ALLOCATED_EXPORT(WebXRRigidTransform, WEBCORE_EXPORT);
public:
    static Ref<WebXRRigidTransform> create();
    WEBCORE_EXPORT static ExceptionOr<Ref<WebXRRigidTransform>> create(const DOMPointInit&, const DOMPointInit&);
    WEBCORE_EXPORT ~WebXRRigidTransform();

    const DOMPointReadOnly& position() const;
    const DOMPointReadOnly& orientation() const;
    Ref<Float32Array> matrix() const;
    Ref<WebXRRigidTransform> inverse() const;

private:
    WebXRRigidTransform(const DOMPointInit&, const DOMPointInit&);

    Ref<DOMPointReadOnly> m_position;
    Ref<DOMPointReadOnly> m_orientation;
    RefPtr<Float32Array> m_matrix;
};

} // namespace WebCore

#endif // ENABLE(WEBXR)
