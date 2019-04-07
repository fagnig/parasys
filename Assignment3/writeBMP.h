typedef struct BMPHEADER {
	int16_t ID;
	int32_t SIZE;
	int16_t RESERVED1;
	int16_t RESERVED2;
	int32_t PIXEL_ARRAY_OFFSET;
} BMPHEADER;

typedef struct DIBHEADER {
  int32_t size;
  int32_t width;
  int32_t height;
  int16_t planes;
  int16_t bitcount;
  int32_t compression;
  int32_t sizeImage;
  int32_t xpixpermeter;
  int32_t ypixpermeter;
  int32_t colorsused;
  int32_t colorsimportant;
} DIBHEADER;

typedef struct PIXEL_ARRAY {
    unsigned long sizeX;
    unsigned long sizeY;
    int32_t *data;
} PIXEL_ARRAY;

void writeLittleEndian16(int16_t in, FILE* file);
void writeLittleEndian32(int32_t in, FILE* file);
void writeImage(char*, Image);
void createPixelArrayFromImage(Image, PIXEL_ARRAY*);
void createImageFromPixelArray(PIXEL_ARRAY, Image*);