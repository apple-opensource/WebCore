/**
 * This file is part of the DOM implementation for KDE.
 *
 * (C) 1999-2003 Lars Knoll (knoll@kde.org)
 * (C) 2002-2003 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2002, 2005, 2006 Apple Computer, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "CSSStyleRule.h"

#include "CSSMutableStyleDeclaration.h"
#include "CSSSelector.h"

namespace WebCore {

CSSStyleRule::CSSStyleRule(StyleBase* parent)
    : CSSRule(parent)
{
}

CSSStyleRule::~CSSStyleRule()
{
    if (m_style)
        m_style->setParent(0);
}

String CSSStyleRule::selectorText() const
{
    String str;
    for (CSSSelector* s = selectorList().first(); s; s = CSSSelectorList::next(s)) {
        if (s != selectorList().first())
            str += ", ";
        str += s->selectorText();
    }
    return str;
}

void CSSStyleRule::setSelectorText(const String& /*selectorText*/, ExceptionCode& /*ec*/)
{
    // FIXME: Implement!
}

String CSSStyleRule::cssText() const
{
    String result = selectorText();

    result += " { ";
    result += m_style->cssText();
    result += "}";

    return result;
}

bool CSSStyleRule::parseString(const String& /*string*/, bool /*strict*/)
{
    // FIXME
    return false;
}

void CSSStyleRule::setDeclaration(PassRefPtr<CSSMutableStyleDeclaration> style)
{
    m_style = style;
}

} // namespace WebCore
