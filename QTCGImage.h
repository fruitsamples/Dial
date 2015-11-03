// =============================================================================
//	QTCGImage.h
// =============================================================================
//

#ifndef QTCGImage_H_
#define QTCGImage_H_

#include <Carbon/Carbon.h>

#ifdef __cplusplus
extern "C"
{
#endif

// -----------------------------------------------------------------------------
//	prototypes
// -----------------------------------------------------------------------------
//
OSStatus
CreateCGImageWithQTFromFile(
	FSRef*				inFSRef,
	CGImageRef*			outImage );

#ifdef __cplusplus
}
#endif

#endif	// QTCGImage_H_
