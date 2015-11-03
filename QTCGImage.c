// =============================================================================
//	QTCGImage.c
// =============================================================================
//

#include <Carbon/Carbon.h>
#include <QuickTime/QuickTime.h>

#include "QTCGImage.h"

// -----------------------------------------------------------------------------
//	macros
// -----------------------------------------------------------------------------
//
#define RECT_HEIGHT(R)	((R).bottom-(R).top)
#define RECT_WIDTH(R)	((R).right-(R).left)

// -----------------------------------------------------------------------------
//	GWorldImageBufferRelease
// -----------------------------------------------------------------------------
//
void
GWorldImageBufferRelease(
	void*						inInfo,
	const void*					inData,
	size_t						inSize )
{
	#pragma unused( inData, inSize )

	DisposePtr( (Ptr) inData );
}

// -----------------------------------------------------------------------------
//	CreateCGImageWithQTFromFile
// -----------------------------------------------------------------------------
//
OSStatus
CreateCGImageWithQTFromFile(
	FSRef*						inFSRef,
	CGImageRef*					outImage )
{
	OSStatus					err;
	GraphicsImportComponent		importer;
	FSSpec						fileSpec;
	GWorldPtr					gWorld = NULL;
	Rect						bounds;
	CGDataProviderRef			provider;
	CGColorSpaceRef				colorspace;
	long						width;
	long						height;
	long						rowbytes;
	Ptr							dataPtr;

	// Make an FSRef into an FSSpec
	err = FSGetCatalogInfo( inFSRef, kFSCatInfoNone, NULL, NULL, &fileSpec, NULL );
	require_noerr( err, CantMakeFSSpec );

	err = GetGraphicsImporterForFile( &fileSpec, &importer );
	require_noerr( err, CantGetImporter );
	
	err = GraphicsImportGetNaturalBounds( importer, &bounds );
	require_noerr( err, CantGetBounds );

	// Allocate the buffer
	width = RECT_WIDTH( bounds );
	height = RECT_HEIGHT( bounds );
	rowbytes = width * 4;
	dataPtr = NewPtr( height * rowbytes );
	require_action( dataPtr != NULL, CantAllocBuffer, err = memFullErr );
	
	err = NewGWorldFromPtr( &gWorld, 32, &bounds, NULL, NULL, NULL, dataPtr, rowbytes );
	require_noerr( err, CantCreateGWorld );

	err = GraphicsImportSetGWorld( importer, gWorld, GetGWorldDevice( gWorld) );
	require_noerr( err, CantSetGWorld );

	err = GraphicsImportDraw( importer );
	require_noerr( err, CantDraw );
	
	provider = CGDataProviderCreateWithData( NULL, dataPtr, height * rowbytes,
			GWorldImageBufferRelease );
	require_action( provider != NULL, CantCreateProvider, err = memFullErr );

	colorspace = CGColorSpaceCreateDeviceRGB();
	require_action( colorspace != NULL, CantCreateColorSpace, err = memFullErr );
	
	*outImage = CGImageCreate( width, height, 8, 32, rowbytes, colorspace,
			kCGImageAlphaPremultipliedFirst, provider, NULL, false, kCGRenderingIntentDefault );
	require_action( *outImage != NULL, CantCreateImage, err = memFullErr );
	
CantCreateImage:
	CGColorSpaceRelease( colorspace );

CantCreateColorSpace:
	CGDataProviderRelease( provider );

CantCreateProvider:
CantDraw:
CantSetGWorld:
	if ( gWorld != NULL )
		DisposeGWorld( gWorld );

CantCreateGWorld:
CantAllocBuffer:
CantGetBounds:
CantGetImporter:
CantMakeFSSpec:
	return err;
}
