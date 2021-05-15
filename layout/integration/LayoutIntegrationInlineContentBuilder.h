/*
 * Copyright (C) 2020 Apple Inc. All rights reserved.
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

#if ENABLE(LAYOUT_FORMATTING_CONTEXT)

#include <wtf/Vector.h>

namespace WebCore {

class RenderBlockFlow;

namespace Layout {
class InlineFormattingState;
class LayoutState;
}

namespace LayoutIntegration {

struct InlineContent;
struct LineLevelVisualAdjustmentsForRuns;

class InlineContentBuilder {
public:
    InlineContentBuilder(const Layout::LayoutState&, const RenderBlockFlow&, const BoxTree&);

    void build(const Layout::InlineFormattingState&, InlineContent&) const;

private:
    using LineLevelVisualAdjustmentsForRunsList = Vector<LineLevelVisualAdjustmentsForRuns>;

    LineLevelVisualAdjustmentsForRunsList computeLineLevelVisualAdjustmentsForRuns(const Layout::InlineFormattingState&) const;
    void createDisplayLineRuns(const Layout::InlineFormattingState&, InlineContent&, const LineLevelVisualAdjustmentsForRunsList&) const;
    void createDisplayLines(const Layout::InlineFormattingState&, InlineContent&, const LineLevelVisualAdjustmentsForRunsList&) const;

    const Layout::LayoutState& m_layoutState;
    const RenderBlockFlow& m_blockFlow;
    const BoxTree& m_boxTree;
};

}
}
#endif
