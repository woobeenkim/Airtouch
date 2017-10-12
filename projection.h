/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/
#pragma once
#include "pxcprojection.h"
#include "pxccapture.h"
#include "pxcsession.h"
#include <vector>

using namespace std;

class Projection {
public:

	Projection( PXCSession *session, PXCCapture::Device *device, PXCImage::ImageInfo *dinfo, PXCImage::ImageInfo *cinfo );
	~Projection( void );

    PXCImage* ColorToDepthByQueryUVMap   ( PXCImage *color, PXCImage *depth );
    PXCImage* DepthToColorByQueryInvUVMap( PXCImage *color, PXCImage *depth );
    PXCImage* DepthToWorldByQueryVertices( PXCImage *depth, PXCPoint3DF32 light );

    void DepthToColor( PXCImage *color, PXCImage *depth, vector<PXCPointF32> dcords, vector<PXCPointF32> &ccords );
    void DepthToWorld( PXCImage *depth, vector<PXCPointF32> dcords, vector<PXCPointF32> &wcords );
    void ColorToDepth( PXCImage *color, PXCImage *depth, vector<PXCPointF32> ccords, vector<PXCPointF32> &dcords );
    void ColorToWorld( PXCImage *color, PXCImage *depth, vector<PXCPointF32> ccords, vector<PXCPointF32> &wcords );
    void WorldToColor( PXCImage *color, PXCImage *depth, vector<PXCPointF32> wcords, vector<PXCPointF32> &ccords );
    void WorldToDepth( PXCImage *depth, vector<PXCPointF32> wcords, vector<PXCPointF32> &dcords );

    PXCImage* CreateDepthMappedToColor( PXCImage *color, PXCImage *depth );
    PXCImage* CreateColorMappedToDepth( PXCImage *color, PXCImage *depth );

    PXCImage *drawDepth;
    PXCImage *drawColor;
    PXCImage *drawVertices;


    PXCProjection *projection;
    vector<PXCPointF32>   uvMap;
    vector<PXCPointF32>   invUVMap;
    vector<PXCPoint3DF32> vertices;

    /* prohibit using copy & assignment constructors */
    Projection(Projection&) {}
    Projection& operator= (Projection&) { return *this; }
};





