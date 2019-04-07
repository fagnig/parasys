/*
	Author: 02347 Staff
	Date: 03/06
	
	BMP file specification was used according to:
		https://en.wikipedia.org/wiki/BMP_file_format
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "readBMP.h"
#include "writeBMP.h"

#define BMPHEADERSIZE	14
#define DIBHEADERSIZE	40

void writeLittleEndian16(int16_t in, FILE* file) {
	int8_t b[2];
	b[1] = (in >> 8);
	b[0] = in;
	fwrite((void*)b, 1, 2, file);
}

void writeLittleEndian32(int32_t in, FILE* file) {
	int8_t b[4];
	b[3] = (in >> 24);
	b[2] = (in >> 16);
	b[1] = (in >> 8);
	b[0] = in;
	fwrite((void*)b, 1, 4, file);
}

void createPixelArrayFromImage(Image img, PIXEL_ARRAY* img_out) {
	img_out->sizeX = img.sizeX;
	img_out->sizeY = img.sizeY;
	img_out->data = malloc(sizeof(int32_t) * img.sizeX * img.sizeY);
	for (int i = 0; i < img.sizeY * img.sizeX * 3; i+= 3) {
		int32_t pixel = 0;
		pixel += (img.data[i] << 16);
		pixel += (img.data[i + 1] << 8);
		pixel += img.data[i + 2];
		img_out->data[i / 3] = pixel;
	}
}

void createImageFromPixelArray(PIXEL_ARRAY img_in, Image* img) {
	img->sizeX = img_in.sizeX;
	img->sizeY = img_in.sizeY;
	img->data = malloc(img_in.sizeX * img_in.sizeY * 3);
	for (int i = 0; i < img_in.sizeX * img_in.sizeY; i++) {
		img->data[i * 3] =  img_in.data[i] >> 16;
		img->data[i * 3 + 1] = img_in.data[i] >> 8;
		img->data[i * 3 + 2] = img_in.data[i];
	}
}

void writeImage(char* filename, Image img) {
	FILE* file;
	
	file = fopen (filename, "w");
	
	/*BMP HEADER*/
	BMPHEADER bmpheader;
	bmpheader.ID = 0x4d42;
	bmpheader.SIZE = 3 * img.sizeX * img.sizeY + BMPHEADERSIZE + DIBHEADERSIZE;
	bmpheader.RESERVED1 = 0;
	bmpheader.RESERVED2 = 0;
	bmpheader.PIXEL_ARRAY_OFFSET = BMPHEADERSIZE + DIBHEADERSIZE;
	
	writeLittleEndian16(bmpheader.ID, file);
	writeLittleEndian32(bmpheader.SIZE, file);
	writeLittleEndian16(bmpheader.RESERVED1, file);
	writeLittleEndian16(bmpheader.RESERVED2, file);
	writeLittleEndian32(bmpheader.PIXEL_ARRAY_OFFSET, file);
	
	/*DIB HEADER*/
	DIBHEADER dibheader;
	dibheader.size = DIBHEADERSIZE;
	dibheader.width = img.sizeX;
	dibheader.height = img.sizeY;
	dibheader.planes = 1;
	dibheader.bitcount = 24;
	dibheader.compression = 0;
	dibheader.sizeImage = img.sizeX * img.sizeY * 3;
	dibheader.xpixpermeter = 0;
	dibheader.ypixpermeter = 0;
	dibheader.colorsused = 0;
	dibheader.colorsimportant = 0;
	
	writeLittleEndian32(dibheader.size, file);
	writeLittleEndian32(dibheader.width, file);
	writeLittleEndian32(dibheader.height, file);
	writeLittleEndian16(dibheader.planes, file);
	writeLittleEndian16(dibheader.bitcount, file);
	writeLittleEndian32(dibheader.compression, file);
	writeLittleEndian32(dibheader.sizeImage, file);
	writeLittleEndian32(dibheader.xpixpermeter, file);
	writeLittleEndian32(dibheader.ypixpermeter, file);
	writeLittleEndian32(dibheader.colorsused, file);
	writeLittleEndian32(dibheader.colorsimportant, file);

	
	/*PIXEL ARRAY*/
	
	for (int i = 0; i < img.sizeX * img.sizeY * 3; i+=3) {
		fwrite((void*)(img.data + i + 2), 1, 1, file);
		fwrite((void*)(img.data + i + 1), 1, 1, file);
		fwrite((void*)img.data + i, 1, 1, file);
	}
	
	fclose(file);
}