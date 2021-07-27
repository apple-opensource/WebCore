/*
 * Copyright (C) 2017 Apple Inc. All rights reserved.
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

#include "ColorGamut.h"
#include "HdrMetadataType.h"
#include "TransferFunction.h"
#include <wtf/Optional.h>
#include <wtf/text/WTFString.h>

namespace WebCore {

struct VideoConfiguration {
    String contentType;
    uint32_t width;
    uint32_t height;
    uint64_t bitrate;
    double framerate;
    Optional<bool> alphaChannel;
    Optional<ColorGamut> colorGamut;
    Optional<HdrMetadataType> hdrMetadataType;
    Optional<TransferFunction> transferFunction;

    template<class Encoder> void encode(Encoder&) const;
    template<class Decoder> static Optional<VideoConfiguration> decode(Decoder&);
};

template<class Encoder>
void VideoConfiguration::encode(Encoder& encoder) const
{
    encoder << contentType;
    encoder << width;
    encoder << height;
    encoder << bitrate;
    encoder << framerate;
    encoder << alphaChannel;
    encoder << colorGamut;
    encoder << hdrMetadataType;
    encoder << transferFunction;
}

template<class Decoder>
Optional<VideoConfiguration> VideoConfiguration::decode(Decoder& decoder)
{
    Optional<String> contentType;
    decoder >> contentType;
    if (!contentType)
        return WTF::nullopt;

    Optional<uint32_t> width;
    decoder >> width;
    if (!width)
        return WTF::nullopt;

    Optional<uint32_t> height;
    decoder >> height;
    if (!height)
        return WTF::nullopt;

    Optional<uint64_t> bitrate;
    decoder >> bitrate;
    if (!bitrate)
        return WTF::nullopt;

    Optional<double> framerate;
    decoder >> framerate;
    if (!framerate)
        return WTF::nullopt;

    Optional<Optional<bool>> alphaChannel;
    decoder >> alphaChannel;
    if (!alphaChannel)
        return WTF::nullopt;

    Optional<Optional<ColorGamut>> colorGamut;
    decoder >> colorGamut;
    if (!colorGamut)
        return WTF::nullopt;

    Optional<Optional<HdrMetadataType>> hdrMetadataType;
    decoder >> hdrMetadataType;
    if (!hdrMetadataType)
        return WTF::nullopt;

    Optional<Optional<TransferFunction>> transferFunction;
    decoder >> transferFunction;
    if (!transferFunction)
        return WTF::nullopt;

    return {{
        *contentType,
        *width,
        *height,
        *bitrate,
        *framerate,
        *alphaChannel,
        *colorGamut,
        *hdrMetadataType,
        *transferFunction,
    }};
}

} // namespace WebCore