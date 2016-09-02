/*
 * Copyright (C) 2008, Apple Inc. All rights reserved.
 *
 * Permission is granted by Apple to use this file to the extent
 * necessary to relink with LGPL WebKit files.
 *
 * No license or rights are granted by Apple expressly or by
 * implication, estoppel, or otherwise, to Apple patents and
 * trademarks. For the sake of clarity, no license or rights are
 * granted by Apple expressly or by implication, estoppel, or otherwise,
 * under any Apple patents, copyrights and trademarks to underlying
 * implementations of any application programming interfaces (APIs)
 * or to any functionality that is invoked by calling any API.
 */

#ifndef TouchEvent_h
#define TouchEvent_h

#include <wtf/Platform.h>


#include <wtf/RefPtr.h>
#include "MouseRelatedEvent.h"
#include "TouchList.h"

namespace WebCore {

class TouchEvent : public MouseRelatedEvent {
public:
    static PassRefPtr<TouchEvent> create()
    {
        return adoptRef(new TouchEvent);
    }
    static PassRefPtr<TouchEvent> create(
               const AtomicString& type, bool canBubble, bool cancelable, AbstractView* view, int detail, 
               int screenX, int screenY, int pageX, int pageY,
               bool ctrlKey, bool altKey, bool shiftKey, bool metaKey,
               TouchList* touches, TouchList* targetTouches, TouchList* changedTouches,
               float scale, float rotation, bool isSimulated = false)
    {
        return adoptRef(new TouchEvent(type, canBubble, cancelable, view, detail,
                                       screenX, screenY, pageX, pageY,
                                       ctrlKey, altKey, shiftKey, metaKey,
                                       touches, targetTouches, changedTouches,
                                       scale, rotation, isSimulated));
    }
    virtual ~TouchEvent() {}

    void initTouchEvent(const AtomicString& type, bool canBubble, bool cancelable, AbstractView* view, int detail, 
                        int screenX, int screenY, int clientX, int clientY,
                        bool ctrlKey, bool altKey, bool shiftKey, bool metaKey,
                        TouchList* touches, TouchList* targetTouches, TouchList* changedTouches,
                        float scale, float rotation);

    virtual bool isTouchEvent() const { return true; }

    TouchList* touches() const { return m_touches.get(); }
    TouchList* targetTouches() const { return m_targetTouches.get(); }
    TouchList* changedTouches() const { return m_changedTouches.get(); }

    float scale() const { return m_scale; }
    float rotation() const { return m_rotation; }

private:
    TouchEvent() { }
    TouchEvent(const AtomicString& type, bool canBubble, bool cancelable, AbstractView* view, int detail, 
               int screenX, int screenY, int pageX, int pageY,
               bool ctrlKey, bool altKey, bool shiftKey, bool metaKey,
               TouchList* touches, TouchList* targetTouches, TouchList* changedTouches,
               float scale, float rotation, bool isSimulated = false);

    RefPtr<TouchList> m_touches;
    RefPtr<TouchList> m_targetTouches;
    RefPtr<TouchList> m_changedTouches;
    float m_scale;
    float m_rotation;
};

} // namespace WebCore


#endif // TouchEvent_h
