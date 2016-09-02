/*
 * TileLayerPool.mm
 * WebCore
 *
 * Copyright (C) 2011, Apple Inc.  All rights reserved.
 *
 * No license or rights are granted by Apple expressly or by implication,
 * estoppel, or otherwise, to Apple copyrights, patents, trademarks, trade
 * secrets or other rights.
 */

#include "config.h"
#include "TileLayerPool.h"

#if PLATFORM(IOS)

#include "Logging.h"
#include "MemoryPressureHandler.h"
#include "TileLayer.h"
#include "TileGrid.h"
#include <wtf/CurrentTime.h>

namespace WebCore {
    
static const double capacityDecayTime = 5;
    
TileLayerPool::TileLayerPool()
    : m_totalBytes(0)
    , m_capacity(0)
    , m_lastAddTime(0)
    , m_needsPrune(false)
{
}

TileLayerPool* TileLayerPool::sharedPool()
{
    static TileLayerPool* sharedPool = new TileLayerPool;
    return sharedPool;
}

unsigned TileLayerPool::bytesBackingLayerWithPixelSize(const IntSize& size)
{
    return size.width() * size.height() * 4;
}

TileLayerPool::LayerList& TileLayerPool::listOfLayersWithSize(const IntSize& size, AccessType accessType)
{
    ASSERT(!m_layerPoolMutex.tryLock());
    HashMap<IntSize, LayerList>::iterator it = m_reuseLists.find(size);
    if (it == m_reuseLists.end()) {
        it = m_reuseLists.add(size, LayerList()).iterator;
        m_sizesInPruneOrder.append(size);
    } else if (accessType == MarkAsUsed) {
        m_sizesInPruneOrder.remove(m_sizesInPruneOrder.reverseFind(size));
        m_sizesInPruneOrder.append(size);
    }
    return it->value;
}

void TileLayerPool::addLayer(const RetainPtr<TileLayer>& layer)
{
    IntSize layerSize([layer.get() frame].size);
    layerSize.scale([layer.get() contentsScale]);
    if (!canReuseLayerWithSize(layerSize))
        return;

    if (memoryPressureHandler().hasReceivedMemoryPressure()) {
        LOG(MemoryPressure, "Under memory pressure: %s, totalBytes: %d", __PRETTY_FUNCTION__, m_totalBytes);
        return;
    }

    MutexLocker locker(m_layerPoolMutex);
    listOfLayersWithSize(layerSize).prepend(layer);
    m_totalBytes += bytesBackingLayerWithPixelSize(layerSize);
    
    m_lastAddTime = currentTime();
    schedulePrune();
}

RetainPtr<TileLayer> TileLayerPool::takeLayerWithSize(const IntSize& size)
{
    if (!canReuseLayerWithSize(size))
        return nil;
    MutexLocker locker(m_layerPoolMutex);
    LayerList& reuseList = listOfLayersWithSize(size, MarkAsUsed);
    if (reuseList.isEmpty())
        return nil;
    m_totalBytes -= bytesBackingLayerWithPixelSize(size);
    return reuseList.takeFirst();
}

void TileLayerPool::setCapacity(unsigned capacity)
{
    MutexLocker reuseLocker(m_layerPoolMutex);
    if (capacity < m_capacity)
        schedulePrune();
    m_capacity = capacity;
}
    
unsigned TileLayerPool::decayedCapacity() const
{
    // Decay to one quarter over capacityDecayTime
    double timeSinceLastAdd = currentTime() - m_lastAddTime;
    if (timeSinceLastAdd > capacityDecayTime)
        return m_capacity / 4;
    float decayProgess = float(timeSinceLastAdd / capacityDecayTime);
    return m_capacity / 4 + m_capacity * 3 / 4 * (1.f - decayProgess);
}

void TileLayerPool::schedulePrune()
{
    ASSERT(!m_layerPoolMutex.tryLock());
    if (m_needsPrune)
        return;
    m_needsPrune = true;
    dispatch_time_t nextPruneTime = dispatch_time(DISPATCH_TIME_NOW, 1 * NSEC_PER_SEC);
    dispatch_after(nextPruneTime, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        prune();
    });
}

void TileLayerPool::prune()
{
    MutexLocker locker(m_layerPoolMutex);
    ASSERT(m_needsPrune);
    m_needsPrune = false;
    unsigned shrinkTo = decayedCapacity();
    while (m_totalBytes > shrinkTo) {
        ASSERT(!m_sizesInPruneOrder.isEmpty());
        IntSize sizeToDrop = m_sizesInPruneOrder.first();
        LayerList& oldestReuseList = m_reuseLists.find(sizeToDrop)->value;
        if (oldestReuseList.isEmpty()) {
            m_reuseLists.remove(sizeToDrop);
            m_sizesInPruneOrder.remove(0);
            continue;
        }
#if LOG_TILING
        NSLog(@"dropping layer of size %d x %d", sizeToDrop.width(), sizeToDrop.height());
#endif
        m_totalBytes -= bytesBackingLayerWithPixelSize(sizeToDrop);
        // The last element in the list is the oldest, hence most likely not to
        // still have a backing store.
        oldestReuseList.removeLast();
    }
    if (currentTime() - m_lastAddTime <= capacityDecayTime)
        schedulePrune();
}

void TileLayerPool::drain()
{
    MutexLocker reuseLocker(m_layerPoolMutex);
    m_reuseLists.clear();
    m_sizesInPruneOrder.clear();
    m_totalBytes = 0;
}

}

#endif
