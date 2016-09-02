/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
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
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "JavaRuntimeObject.h"

#include "JavaInstanceJSC.h"
#include "JSDOMBinding.h"
#include "runtime/ObjectPrototype.h"

#if ENABLE(JAVA_BRIDGE)

namespace JSC {
namespace Bindings {

const ClassInfo JavaRuntimeObject::s_info = { "JavaRuntimeObject", &RuntimeObject::s_info, 0, 0, CREATE_METHOD_TABLE(JavaRuntimeObject) };

JavaRuntimeObject::JavaRuntimeObject(ExecState* exec, JSGlobalObject* globalObject, Structure* structure, PassRefPtr<JavaInstance> instance)
    : RuntimeObject(exec, globalObject, structure, instance)
{
}

void JavaRuntimeObject::finishCreation(JSGlobalObject* globalObject)
{
    Base::finishCreation(globalObject);
    ASSERT(inherits(&s_info));
}

JavaInstance* JavaRuntimeObject::getInternalJavaInstance() const
{
    return static_cast<JavaInstance*>(getInternalInstance());
}



}
}

#endif // ENABLE(JAVA_BRIDGE)