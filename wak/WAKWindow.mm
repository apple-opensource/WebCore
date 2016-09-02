//
//  WAKWindow.m
//
//  Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012 Apple Inc.  All rights reserved.
//

#import "config.h"
#import "WAKWindow.h"

#import "TileCache.h"
#import "WAKViewPrivate.h"
#import "WebCoreSystemInterface.h"
#import "WebCoreThreadRun.h"
#import "WebEvent.h"
#import "WKContentObservation.h"
#import "WKViewPrivate.h"
#import <QuartzCore/QuartzCore.h>
#import <wtf/TCSpinLock.h>

NSString * const WAKWindowScreenScaleDidChangeNotification = @"WAKWindowScreenScaleDidChangeNotification";
NSString * const WAKWindowVisibilityDidChangeNotification = @"WAKWindowVisibilityDidChangeNotification";

using namespace WebCore;

@protocol OrientationProvider
#if __IPHONE_OS_VERSION_MIN_REQUIRED >= 60000
- (BOOL)hasLandscapeOrientation;
#else
- (int)orientation;
#endif
@end

static WAKWindow *_WAKKeyWindow = nil;        // weak
static WebEvent *currentEvent = nil;
static id<OrientationProvider> gOrientationProvider;

@implementation WAKWindow {
    SpinLock _exposedScrollViewRectLock;
    CGRect _exposedScrollViewRect;
}

@synthesize useOrientationDependentFontAntialiasing = _useOrientationDependentFontAntialiasing;

- (id)initWithLayer:(CALayer *)layer
{
    self = [super init];
    if (!self)
        return nil;

    _hostLayer = [layer retain];

    _frame = [_hostLayer frame];
    _screenScale = wkGetScreenScaleFactor();
    
    _tileCache = new TileCache(self);

    _frozenVisibleRect = CGRectNull;

    _exposedScrollViewRectLock = SPINLOCK_INITIALIZER;
    _exposedScrollViewRect = CGRectNull;

    return self;
}

// This is used for WebViews that are not backed by the tile cache. Their content must be painted manually.
- (id)initWithFrame:(CGRect)frame
{
    self = [super init];
    if (!self)
        return nil;

    _frame = frame;
    _screenScale = wkGetScreenScaleFactor();

    _exposedScrollViewRectLock = SPINLOCK_INITIALIZER;
    _exposedScrollViewRect = CGRectNull;

    return self;
}

- (void)dealloc
{
    delete _tileCache;
    [_hostLayer release];
    
    [super dealloc];
}

- (void)setContentView:(WAKView *)aView
{
    [aView retain];
    [_contentView release];

    if (aView)
        _WKViewSetWindow([aView _viewRef], self);
    _contentView = aView;
}

- (WAKView *)contentView
{
    return _contentView;
}

- (void)close
{
    if (_contentView) {
        _WKViewSetWindow([_contentView _viewRef], nil);
        [_contentView release];
        _contentView = nil;
    }

    [_responderView release];
    _responderView = nil;
}

- (WAKView *)firstResponder
{
    return _responderView;
}

- (WAKView *)_newFirstResponderAfterResigning
{
    return _nextResponder;
}

- (NSPoint)convertBaseToScreen:(NSPoint)aPoint
{
    UNUSED_PARAM(aPoint);
    NSPoint p = { 0, 0 };
    return p;
}

- (NSPoint)convertScreenToBase:(NSPoint)aPoint
{
    UNUSED_PARAM(aPoint);
    NSPoint p = { 0, 0 };
    return p;
}

- (BOOL)isKeyWindow
{
    return YES || self == _WAKKeyWindow; 
}

- (void)makeKeyWindow
{
    if ([self isKeyWindow])
        return;
    
    _WAKKeyWindow = self;
}

- (BOOL)isVisible
{
    return _visible;
}

- (void)setVisible:(BOOL)visible
{
    if (_visible == visible)
        return;

    _visible = visible;

    WebThreadRun(^{
        [[NSNotificationCenter defaultCenter] postNotificationName:WAKWindowVisibilityDidChangeNotification object:self userInfo:nil];
    });
}

- (NSSelectionDirection)keyViewSelectionDirection
{
    return (NSSelectionDirection)0;
}

- (BOOL)makeFirstResponder:(NSResponder *)aResponder
{
    if (![aResponder isKindOfClass:[WAKView class]])
        return NO;

    WAKView *view = static_cast<WAKView*>(aResponder);
    BOOL result = YES;
    if (view != _responderView) {
        // We need to handle the case of the view not accepting to be a first responder,
        // where we don't need to resign as first responder.
        BOOL acceptsFirstResponder = view ? WKViewAcceptsFirstResponder([view _viewRef]) : YES;
        if (acceptsFirstResponder && _responderView) {
            _nextResponder = view;
            if (WKViewResignFirstResponder([_responderView _viewRef])) {
                _nextResponder = nil;
                [_responderView release];
                _responderView = nil;
            }  else {
                _nextResponder = nil;
                result = NO;
            }
        }

        if (result && view) {
            if (acceptsFirstResponder && WKViewBecomeFirstResponder([view _viewRef]))
                _responderView = [view retain];
            else
                result = NO;
        }
    }

    return result;
}

// FIXME: This method can lead to incorrect state. Remove if possible.
- (void)setFrame:(NSRect)frameRect display:(BOOL)flag
{
    UNUSED_PARAM(flag);
    _frame = frameRect;
}

// FIXME: the correct value if there is a host layer is likely to return [_hostLayer frame].
- (CGRect)frame
{
    return _frame;
}

- (void)setContentRect:(CGRect)rect
{
    if (CGRectEqualToRect(rect, _frame))
        return;
    _frame = rect;

    // FIXME: Size of the host layer is directly available so there is no real reason to save it here.
    // However we currently use this call to catch changes to the host layer size.
    if (_tileCache)
        _tileCache->hostLayerSizeChanged();
}

- (void)setScreenSize:(CGSize)size
{
    _screenSize = size;
}

- (CGSize)screenSize
{
    return _screenSize;
}

- (void)setAvailableScreenSize:(CGSize)size
{
    _availableScreenSize = size;
}

- (CGSize)availableScreenSize
{
    return _availableScreenSize;
}

- (void)setScreenScale:(CGFloat)scale
{
    _screenScale = scale;

    WebThreadRun(^{
        [[NSNotificationCenter defaultCenter] postNotificationName:WAKWindowScreenScaleDidChangeNotification object:self userInfo:nil];
    });
}

- (CGFloat)screenScale
{
    return _screenScale;
}

- (void)setRootLayer:(CALayer *)layer
{
    _rootLayer = layer;
}

- (CALayer *)rootLayer
{
    return _rootLayer;
}

- (void)sendEvent:(WebEvent *)anEvent
{
    ASSERT(anEvent);
    WebThreadRun(^{
        [self sendEventSynchronously:anEvent];
    });
}

- (void)sendEventSynchronously:(WebEvent *)anEvent
{
    ASSERT(anEvent);
    ASSERT(WebThreadIsLockedOrDisabled());
    WebEvent *lastEvent = currentEvent;
    currentEvent = [anEvent retain];

    switch (anEvent.type) {
        case WebEventMouseMoved:
        case WebEventScrollWheel:
            if (WAKView *hitView = [_contentView hitTest:(anEvent.locationInWindow)])
                [hitView handleEvent:anEvent];
            break;

        case WebEventMouseUp:
        case WebEventKeyDown:
        case WebEventKeyUp:
        case WebEventTouchChange:
            [_responderView handleEvent:anEvent];
            break;

        case WebEventMouseDown:
        case WebEventTouchBegin:
        case WebEventTouchEnd:
        case WebEventTouchCancel:
            if (WAKView *hitView = [_contentView hitTest:(anEvent.locationInWindow)]) {
                [self makeFirstResponder:hitView];
                [hitView handleEvent:anEvent];
            }
            break;

        default:
            ASSERT_NOT_REACHED();
            break;
    }

    [currentEvent release];
    currentEvent = lastEvent;
}

- (void)sendMouseMoveEvent:(WebEvent *)anEvent contentChange:(WKContentChange *)aContentChange
{
    WebThreadRun(^{
        [self sendEvent:anEvent];

        if (aContentChange)
            *aContentChange = WKObservedContentChange();
    });
}

- (void)setExposedScrollViewRect:(CGRect)exposedScrollViewRect
{
    SpinLockHolder locker(&_exposedScrollViewRectLock);
    _exposedScrollViewRect = exposedScrollViewRect;
}

- (CGRect)exposedScrollViewRect
{
    {
        SpinLockHolder locker(&_exposedScrollViewRectLock);
        if (!CGRectIsNull(_exposedScrollViewRect))
            return _exposedScrollViewRect;
    }
    return [self visibleRect];
}

// Tiling

- (void)layoutTiles
{
    if (!_tileCache)
        return;    
    _tileCache->layoutTiles();
}

- (void)layoutTilesNow
{
    if (!_tileCache)
        return;
    _tileCache->layoutTilesNow();
}

- (void)layoutTilesNowForRect:(CGRect)rect
{
    if (!_tileCache)
        return;
    _tileCache->layoutTilesNowForRect(enclosingIntRect(rect));
}

- (void)setNeedsDisplay
{
    if (!_tileCache)
        return;
    _tileCache->setNeedsDisplay();
}

- (void)setNeedsDisplayInRect:(CGRect)rect
{
    if (!_tileCache)
        return;
    _tileCache->setNeedsDisplayInRect(enclosingIntRect(rect));
}

- (BOOL)tilesOpaque
{
    if (!_tileCache)
        return NO;
    return _tileCache->tilesOpaque();
}

- (void)setTilesOpaque:(BOOL)opaque
{
    if (!_tileCache)
        return;
    _tileCache->setTilesOpaque(opaque);
}

- (CGRect)_visibleRectRespectingMasksToBounds:(BOOL)respectsMasksToBounds
{
    if (!CGRectIsNull(_frozenVisibleRect))
        return _frozenVisibleRect;

    CALayer* layer = _hostLayer;
    CGRect bounds = [layer bounds];
    CGRect rect = bounds;
    CALayer* superlayer = [layer superlayer];

    while (superlayer && layer != _rootLayer) {
        CGRect rectInSuper = [superlayer convertRect:rect fromLayer:layer];
        if ([superlayer masksToBounds] || !respectsMasksToBounds)
            rect = CGRectIntersection([superlayer bounds], rectInSuper);
        else
            rect = rectInSuper;
        layer = superlayer;
        superlayer = [layer superlayer];
    }
    
    if (layer != _hostLayer) {
        rect = CGRectIntersection([layer bounds], rect);
        rect = [_hostLayer convertRect:rect fromLayer:layer];
    }
    
    return CGRectIntersection(bounds, rect);
}

- (CGRect)visibleRect
{
    return [self _visibleRectRespectingMasksToBounds:NO];
}

- (CGRect)extendedVisibleRect
{
    return [self _visibleRectRespectingMasksToBounds:YES];
}

- (void)removeAllNonVisibleTiles
{
    if (!_tileCache)
        return;
    _tileCache->removeAllNonVisibleTiles();
}

- (void)removeAllTiles
{
    if (!_tileCache)
        return;
    _tileCache->removeAllTiles();
}

- (void)removeForegroundTiles
{
    if (!_tileCache)
        return;
    _tileCache->removeForegroundTiles();
}

- (void)setTilingMode:(WAKWindowTilingMode)mode
{
    if (!_tileCache)
        return;
    _tileCache->setTilingMode((TileCache::TilingMode)mode);
}

- (WAKWindowTilingMode)tilingMode
{
    if (!_tileCache)
        return kWAKWindowTilingModeDisabled;
    return (WAKWindowTilingMode)_tileCache->tilingMode();
}

- (void)setTilingDirection:(WAKTilingDirection)tilingDirection
{
    if (!_tileCache)
        return;
    _tileCache->setTilingDirection((TileCache::TilingDirection)tilingDirection);
}

- (WAKTilingDirection)tilingDirection
{
    if (!_tileCache)
        return kWAKTilingDirectionDown;
    return (WAKTilingDirection)_tileCache->tilingDirection();
}

- (void)setZoomedOutTileScale:(float)scale
{
    if (!_tileCache)
        return;
    _tileCache->setZoomedOutScale(scale);
}

- (float)zoomedOutTileScale
{
    if (!_tileCache)
        return 1.0f;
    return _tileCache->zoomedOutScale();
}

- (void)setCurrentTileScale:(float)scale
{
    if (!_tileCache)
        return;
    _tileCache->setCurrentScale(scale);
}

- (float)currentTileScale
{
    if (!_tileCache)
        return 1.0f;
    return _tileCache->currentScale();
}

- (void)setKeepsZoomedOutTiles:(BOOL)keepsZoomedOutTiles
{
    if (!_tileCache)
        return;
    _tileCache->setKeepsZoomedOutTiles(keepsZoomedOutTiles);
}

- (BOOL)keepsZoomedOutTiles
{
    if (!_tileCache)
        return NO;
    return _tileCache->keepsZoomedOutTiles();
}

- (TileCache*)tileCache
{
    return _tileCache;
}

- (BOOL)hasPendingDraw
{
    if (!_tileCache)
        return NO;
     return _tileCache->hasPendingDraw();
}

- (void)setContentReplacementImage:(CGImageRef)contentReplacementImage
{
    if (!_tileCache)
        return;
    _tileCache->setContentReplacementImage(contentReplacementImage);
}

- (CGImageRef)contentReplacementImage
{
    if (!_tileCache)
        return NULL;
    return _tileCache->contentReplacementImage().get();
}

- (void)displayRect:(NSRect)rect
{
    [[self contentView] displayRect:rect];
}

- (void)willRotate
{
    [self freezeVisibleRect];
}

- (void)didRotate
{
    [self unfreezeVisibleRect];
}

- (void)freezeVisibleRect
{
    _frozenVisibleRect = [self visibleRect];
}

- (void)unfreezeVisibleRect
{
    _frozenVisibleRect = CGRectNull;
}

+ (void)setOrientationProvider:(id)provider
{
    // This is really the UIWebDocumentView class that calls into UIApplication to get the orientation.
    gOrientationProvider = provider;
}

+ (BOOL)hasLandscapeOrientation
{
    // this should be perfectly thread safe
#if __IPHONE_OS_VERSION_MIN_REQUIRED >= 60000
    return [gOrientationProvider hasLandscapeOrientation];
#else
    int orientation = [gOrientationProvider orientation];
    return orientation == 90 || orientation == -90;
#endif
}

- (CALayer*)hostLayer
{
    return _hostLayer;
}

- (void)setTileBordersVisible:(BOOL)visible
{
    if (!_tileCache)
        return;

    _tileCache->setTileBordersVisible(visible);
}

- (void)setTilePaintCountsVisible:(BOOL)visible
{
    if (!_tileCache)
        return;

    _tileCache->setTilePaintCountersVisible(visible);
}

- (void)setAcceleratedDrawingEnabled:(BOOL)enabled
{
    if (!_tileCache)
        return;

    _tileCache->setAcceleratedDrawingEnabled(enabled);
}

- (void)dumpTiles
{
    CGRect savedFrozenVisibleRect = _frozenVisibleRect;
    NSLog(@"=================");
    if (!CGRectIsNull(_frozenVisibleRect)) {
        NSLog(@"VISIBLE RECT IS CACHED: [%6.1f %6.1f %6.1f %6.1f]", _frozenVisibleRect.origin.x, _frozenVisibleRect.origin.y, _frozenVisibleRect.size.width, _frozenVisibleRect.size.height);
        _frozenVisibleRect = CGRectNull;
    }
    CGRect visibleRect = [self visibleRect];
    NSLog(@"visibleRect = [%6.1f %6.1f %6.1f %6.1f]", visibleRect.origin.x, visibleRect.origin.y, visibleRect.size.width, visibleRect.size.height);
    _frozenVisibleRect = savedFrozenVisibleRect;
    _tileCache->dumpTiles();
}

- (void)setTileControllerShouldUseLowScaleTiles:(BOOL)lowScaleTiles
{
    if (!_tileCache)
        return;

    _tileCache->setTileControllerShouldUseLowScaleTiles(lowScaleTiles);
}

- (NSString *)description
{
    NSMutableString *description = [NSMutableString stringWithFormat:@"<%@: WAK: %p; ", [self class], self];

    CGRect frame = [self frame];
    [description appendFormat:@"frame = (%g %g; %g %g); ", frame.origin.x, frame.origin.y, frame.size.width, frame.size.height];

    [description appendFormat:@"first responder = WK %p; ", _responderView];
    [description appendFormat:@"next responder = WK %p; ", _nextResponder];

    [description appendFormat:@"layer = %@>", [_hostLayer description]];

    return description;
}

+ (WebEvent *)currentEvent
{
    return currentEvent;
}

- (NSString *)recursiveDescription
{
    NSMutableString *info = [NSMutableString string];

    [info appendString:[self description]];

    [[self contentView] _appendDescriptionToString:info atLevel:1];

    return info;
}

@end