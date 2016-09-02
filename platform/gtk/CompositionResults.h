/*
 * Copyright (C) 2012 Igalia S.L.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
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

#ifndef CompositionResults_h
#define CompositionResults_h

#include <wtf/text/WTFString.h>

namespace WebCore {

struct CompositionResults {
    CompositionResults()
        : preeditCursorOffset(0)
    {
    }

    CompositionResults(String simpleString)
        : simpleString(simpleString)
        , preeditCursorOffset(0)
    {
    }

    CompositionResults(String confirmedComposition, String preedit, int preeditCursorOffset)
        : confirmedComposition(confirmedComposition)
        , preedit(preedit)
        , preeditCursorOffset(preeditCursorOffset)
    {
    }

    bool compositionUpdated() const
    {
        return !confirmedComposition.isNull() || !preedit.isNull();
    }

    // Some simple input methods return a string for all keyboard events. This
    // value should be treated as the string representation of the keycode.
    String simpleString;

    // If the input method had a "real" composition it will be stored here.
    String confirmedComposition;

    // The event may have caused the preedit to update.
    String preedit;
    int preeditCursorOffset;
};

}

#endif // CompositionResults_h
