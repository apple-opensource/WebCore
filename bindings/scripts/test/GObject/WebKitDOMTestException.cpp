/*
    This file is part of the WebKit open source project.
    This file has been generated by generate-bindings.pl. DO NOT MODIFY!

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "config.h"
#include "WebKitDOMTestException.h"

#include "DOMObjectCache.h"
#include "ExceptionCode.h"
#include "JSMainThreadExecState.h"
#include "TestException.h"
#include "WebKitDOMBinding.h"
#include "gobject/ConvertToUTF8String.h"
#include "webkit/WebKitDOMTestExceptionPrivate.h"
#include "webkitdefines.h"
#include "webkitglobalsprivate.h"
#include "webkitmarshal.h"
#include <glib-object.h>
#include <wtf/GetPtr.h>
#include <wtf/RefPtr.h>

namespace WebKit {

WebKitDOMTestException* kit(WebCore::TestException* obj)
{
    g_return_val_if_fail(obj, 0);

    if (gpointer ret = DOMObjectCache::get(obj))
        return static_cast<WebKitDOMTestException*>(ret);

    return static_cast<WebKitDOMTestException*>(DOMObjectCache::put(obj, WebKit::wrapTestException(obj)));
}

WebCore::TestException* core(WebKitDOMTestException* request)
{
    g_return_val_if_fail(request, 0);

    WebCore::TestException* coreObject = static_cast<WebCore::TestException*>(WEBKIT_DOM_OBJECT(request)->coreObject);
    g_return_val_if_fail(coreObject, 0);

    return coreObject;
}

WebKitDOMTestException* wrapTestException(WebCore::TestException* coreObject)
{
    g_return_val_if_fail(coreObject, 0);

    // We call ref() rather than using a C++ smart pointer because we can't store a C++ object
    // in a C-allocated GObject structure. See the finalize() code for the matching deref().
    coreObject->ref();

    return WEBKIT_DOM_TEST_EXCEPTION(g_object_new(WEBKIT_TYPE_DOM_TEST_EXCEPTION, "core-object", coreObject, NULL));
}

} // namespace WebKit

G_DEFINE_TYPE(WebKitDOMTestException, webkit_dom_test_exception, WEBKIT_TYPE_DOM_OBJECT)

enum {
    PROP_0,
    PROP_NAME,
};

static void webkit_dom_test_exception_finalize(GObject* object)
{

    WebKitDOMObject* domObject = WEBKIT_DOM_OBJECT(object);
    
    if (domObject->coreObject) {
        WebCore::TestException* coreObject = static_cast<WebCore::TestException*>(domObject->coreObject);

        WebKit::DOMObjectCache::forget(coreObject);
        coreObject->deref();

        domObject->coreObject = 0;
    }


    G_OBJECT_CLASS(webkit_dom_test_exception_parent_class)->finalize(object);
}

static void webkit_dom_test_exception_set_property(GObject* object, guint propertyId, const GValue* value, GParamSpec* pspec)
{
    WebCore::JSMainThreadNullState state;
    switch (propertyId) {
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, propertyId, pspec);
        break;
    }
}


static void webkit_dom_test_exception_get_property(GObject* object, guint propertyId, GValue* value, GParamSpec* pspec)
{
    WebCore::JSMainThreadNullState state;
    WebKitDOMTestException* self = WEBKIT_DOM_TEST_EXCEPTION(object);
    WebCore::TestException* coreSelf = WebKit::core(self);
    switch (propertyId) {
    case PROP_NAME: {
        g_value_take_string(value, convertToUTF8String(coreSelf->name()));
        break;
    }
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, propertyId, pspec);
        break;
    }
}


static void webkit_dom_test_exception_constructed(GObject* object)
{

    if (G_OBJECT_CLASS(webkit_dom_test_exception_parent_class)->constructed)
        G_OBJECT_CLASS(webkit_dom_test_exception_parent_class)->constructed(object);
}

static void webkit_dom_test_exception_class_init(WebKitDOMTestExceptionClass* requestClass)
{
    GObjectClass* gobjectClass = G_OBJECT_CLASS(requestClass);
    gobjectClass->finalize = webkit_dom_test_exception_finalize;
    gobjectClass->set_property = webkit_dom_test_exception_set_property;
    gobjectClass->get_property = webkit_dom_test_exception_get_property;
    gobjectClass->constructed = webkit_dom_test_exception_constructed;

    g_object_class_install_property(gobjectClass,
                                    PROP_NAME,
                                    g_param_spec_string("name", /* name */
                                                           "test_exception_name", /* short description */
                                                           "read-only  gchar* TestException.name", /* longer - could do with some extra doc stuff here */
                                                           "", /* default */
                                                           WEBKIT_PARAM_READABLE));


}

static void webkit_dom_test_exception_init(WebKitDOMTestException* request)
{
}

gchar*
webkit_dom_test_exception_get_name(WebKitDOMTestException* self)
{
    g_return_val_if_fail(self, 0);
    WebCore::JSMainThreadNullState state;
    WebCore::TestException* item = WebKit::core(self);
    gchar* result = convertToUTF8String(item->name());
    return result;
}

