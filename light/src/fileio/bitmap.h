//
// bitmap.h
//
// header file for MS bitmap format
//
//

#ifndef BITMAP_H
#define BITMAP_H

#include <stdio.h>
#include <string.h>
#include "../vecmath/vec.h"

#define BMP_BI_RGB        0L

typedef unsigned short	BMP_WORD; 
typedef unsigned int	BMP_DWORD; 
typedef int				BMP_LONG; 
 
typedef struct { 
	BMP_WORD	bfType; 
	BMP_DWORD	bfSize; 
	BMP_WORD	bfReserved1; 
	BMP_WORD	bfReserved2; 
	BMP_DWORD	bfOffBits; 
} BMP_BITMAPFILEHEADER; 
 
typedef struct { 
	BMP_DWORD	biSize; 
	BMP_LONG	biWidth; 
	BMP_LONG	biHeight; 
	BMP_WORD	biPlanes; 
	BMP_WORD	biBitCount; 
	BMP_DWORD	biCompression; 
	BMP_DWORD	biSizeImage; 
	BMP_LONG	biXPelsPerMeter; 
	BMP_LONG	biYPelsPerMeter; 
	BMP_DWORD	biClrUsed; 
	BMP_DWORD	biClrImportant; 
} BMP_BITMAPINFOHEADER; 

typedef struct {
  int num_pictures;
  int width;
  int height;
  Vec3d image_point;
  Vec3d camera_point;
  Vec3d v1;
  Vec3d v2;
} LIGHTFIELD_HEADER;

// global I/O routines
extern unsigned char *readBMP(const char *fname, int& width, int& height);
extern void writeBMP(const char *iname, int width, int height, unsigned char *data);
extern unsigned char *readLightfield(const char *fname, LIGHTFIELD_HEADER *h);
extern void writeLightfield(const char *iname, LIGHTFIELD_HEADER *h, unsigned char *data);

#endif
