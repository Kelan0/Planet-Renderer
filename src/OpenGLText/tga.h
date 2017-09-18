#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <corecrt_io.h>
#include <iostream>

//#include <windows.h>
//#include <gl/gl.h>

class TGA
{
public:
    enum TGAFormat
    {
        RGB = 0x1907,
        RGBA = 0x1908,
        ALPHA = 0x1906,
        UNKNOWN = -1
    };

    enum TGAError
    {
        TGA_NO_ERROR = 1,   // No error
        TGA_FILE_NOT_FOUND, // File was not found 
        TGA_BAD_IMAGE_TYPE, // Color mapped image or image is not uncompressed
        TGA_BAD_DIMENSION,  // Dimension is not a power of 2 
        TGA_BAD_BITS,       // Image bits is not 8, 24 or 32 
        TGA_BAD_DATA        // Image data could not be loaded 
	};

    TGA(void) : 
        m_texFormat(TGA::UNKNOWN),
        m_nImageWidth(0),
        m_nImageHeight(0),
        m_nImageBits(0),
        m_nImageData(NULL) {}

    ~TGA(void);

    TGA::TGAError load( const char *name );
    TGA::TGAError saveFromExternalData( const char *name, int w, int h, TGAFormat fmt, const unsigned char *externalImage );

    TGAFormat       m_texFormat;
    int             m_nImageWidth;
    int             m_nImageHeight;
    int             m_nImageBits;
    unsigned char * m_nImageData;
    
private:

    int returnError(FILE *s, int error);
    unsigned char *getRGBA(FILE *s, int size);
    unsigned char *getRGB(FILE *s, int size);
    unsigned char *getGray(FILE *s, int size);
    void           writeRGBA(FILE *s, const unsigned char *externalImage, int size);
    void           writeRGB(FILE *s, const unsigned char *externalImage, int size);
    void           writeGrayAsRGB(FILE *s, const unsigned char *externalImage, int size);
    void           writeGray(FILE *s, const unsigned char *externalImage, int size);
};
#ifndef TGA_NOIMPL
TGA::~TGA( void )
{
    if( m_nImageData != NULL )
    {
        free( m_nImageData );
        m_nImageData = NULL;
    }
}

int TGA::returnError( FILE *s, int error )
{
    // Called when there is an error loading the .tga texture file.
    fclose( s );
    return error;
}

unsigned char *TGA::getRGBA( FILE *s, int size )
{
    // Read in RGBA data for a 32bit image. 
    unsigned char *rgba;
    unsigned char temp;
    int bread;
    int i;

    rgba = (unsigned char *)malloc( size * 4 );

    if( rgba == NULL )
        return 0;

    bread = (int)fread( rgba, sizeof (unsigned char), size * 4, s ); 

    // TGA is stored in BGRA, make it RGBA  
    if( bread != size * 4 )
    {
        free( rgba );
        return 0;
    }

    for( i = 0; i < size * 4; i += 4 )
    {
        temp = rgba[i];
        rgba[i] = rgba[i + 2];
        rgba[i + 2] = temp;
    }

    m_texFormat = TGA::RGBA;
    return rgba;
}

unsigned char *TGA::getRGB( FILE *s, int size )
{
    // Read in RGB data for a 24bit image. 
    unsigned char *rgb;
    unsigned char temp;
    int bread;
    int i;

    rgb = (unsigned char*)malloc( size * 3 );

    if( rgb == NULL )
        return 0;

    bread = (int)fread( rgb, sizeof (unsigned char), size * 3, s );

    if(bread != size * 3)
    {
        free( rgb );
        return 0;
    }

    // TGA is stored in BGR, make it RGB  
    for( i = 0; i < size * 3; i += 3 )
    {
        temp = rgb[i];
        rgb[i] = rgb[i + 2];
        rgb[i + 2] = temp;
    }

    m_texFormat = TGA::RGB;

    return rgb;
}

unsigned char *TGA::getGray( FILE *s, int size )
{
    // Gets the grayscale image data.  Used as an alpha channel.
    unsigned char *grayData;
    int bread;

    grayData = (unsigned char*)malloc( size );

    if( grayData == NULL )
        return 0;

    bread = (int)fread( grayData, sizeof (unsigned char), size, s );

    if( bread != size )
    {
        free( grayData );
        return 0;
    }

    m_texFormat = TGA::ALPHA;

    return grayData;
}

inline TGA::TGAError TGA::load( const char *name )
{
    // Loads up a targa file. Supported types are 8,24 and 32 
    // uncompressed images.
    unsigned char type[4];
    unsigned char info[7];
    FILE *s = NULL;
    int size = 0;
    
	errno_t err;

	if ((err = fopen_s(&s, name, "rb")) != 0)
	{
		char errorString[1000];
		strerror_s(errorString, err);

		fprintf(stderr, "cannot open font file '%s': %s\n", name, errorString);
		return TGA_FILE_NOT_FOUND;
	}

    fread( &type, sizeof (char), 3, s );   // Read in colormap info and image type, byte 0 ignored
    fseek( s, 12, SEEK_SET);			   // Seek past the header and useless info
    fread( &info, sizeof (char), 6, s );

    if( type[1] != 0 || (type[2] != 2 && type[2] != 3) )
        returnError( s, TGA_BAD_IMAGE_TYPE );

    m_nImageWidth  = info[0] + info[1] * 256; 
    m_nImageHeight = info[2] + info[3] * 256;
    m_nImageBits   = info[4]; 

    size = m_nImageWidth * m_nImageHeight;

    // Make sure we are loading a supported type  
    if( m_nImageBits != 32 && m_nImageBits != 24 && m_nImageBits != 8 )
        returnError( s, TGA_BAD_BITS );

    if( m_nImageBits == 32 )
        m_nImageData = getRGBA( s, size );
    else if( m_nImageBits == 24 )
        m_nImageData = getRGB( s, size );	
    else if( m_nImageBits == 8 )
        m_nImageData = getGray( s, size );

    // No image data 
    if( m_nImageData == NULL )
        returnError( s, TGA_BAD_DATA );

    fclose( s );

    return TGA_NO_ERROR;
}

void TGA::writeRGBA( FILE *s, const unsigned char *externalImage, int size )
{
    // Read in RGBA data for a 32bit image. 
    unsigned char *rgba;
    int bread;
    int i;

    rgba = (unsigned char *)malloc( size * 4 );

    // switch RGBA to BGRA
    for( i = 0; i < size * 4; i += 4 )
    {
        rgba[i + 0] = externalImage[i + 2];
        rgba[i + 1] = externalImage[i + 1];
        rgba[i + 2] = externalImage[i + 0];
        rgba[i + 3] = externalImage[i + 3];
    }

    bread = (int)fwrite( rgba, sizeof (unsigned char), size * 4, s ); 
    free( rgba );
}

void TGA::writeRGB( FILE *s, const unsigned char *externalImage, int size )
{
    // Read in RGBA data for a 32bit image. 
    unsigned char *rgb;
    int bread;
    int i;

    rgb = (unsigned char *)malloc( size * 3 );

    // switch RGB to BGR
    for( i = 0; i < size * 3; i += 3 )
    {
        rgb[i + 0] = externalImage[i + 2];
        rgb[i + 1] = externalImage[i + 1];
        rgb[i + 2] = externalImage[i + 0];
    }

    bread = (int)fwrite( rgb, sizeof (unsigned char), size * 3, s ); 
    free( rgb );
}

void TGA::writeGrayAsRGB( FILE *s, const unsigned char *externalImage, int size )
{
    // Read in RGBA data for a 32bit image. 
    unsigned char *rgb;
    int bread;
    int i;

    rgb = (unsigned char *)malloc( size * 3 );

    // switch RGB to BGR
    int j = 0;
    for( i = 0; i < size * 3; i += 3, j++ )
    {
        rgb[i + 0] = externalImage[j];
        rgb[i + 1] = externalImage[j];
        rgb[i + 2] = externalImage[j];
    }

    bread = (int)fwrite( rgb, sizeof (unsigned char), size * 3, s ); 
    free( rgb );
}

void TGA::writeGray( FILE *s, const unsigned char *externalImage, int size )
{
    // Gets the grayscale image data.  Used as an alpha channel.
    int bread;

    bread = (int)fwrite( externalImage, sizeof (unsigned char), size, s );
}

TGA::TGAError TGA::saveFromExternalData( const char *name, int w, int h, TGA::TGAFormat fmt, const unsigned char *externalImage )
{
    static unsigned char type[] = {0,0,2};
    static unsigned char dummy[] = {0,0,0,0,0,0,0,0,0};
    static unsigned char info[] = {0,0,0,0,0,0};
    FILE *s = NULL;
    int size = 0;
    
    if(!(fopen_s(&s, name, "wb")))
        return TGA_FILE_NOT_FOUND;

    fwrite( type, sizeof (char), 3, s );   // Read in colormap info and image type, byte 0 ignored
    fwrite( dummy, sizeof (char), 9, s );   // Read in colormap info and image type, byte 0 ignored

    info[0] = w & 0xFF;
    info[1] = (w>>8) & 0xFF;
    info[2] = h & 0xFF;
    info[3] = (h>>8) & 0xFF;
    switch(fmt)
    {
    case ALPHA:
        info[4] = 8;
        break;
    case RGB:
        info[4] = 24;
        break;
    case RGBA:
        info[4] = 32;
        break;
    }
    fwrite( info, sizeof (char), 6, s );

    size = w*h;
    switch(fmt)
    {
    case ALPHA:
        writeGray(s, externalImage, size);
        break;
    case RGB:
        writeGrayAsRGB/*writeRGB*/(s, externalImage, size);
        break;
    case RGBA:
        writeRGBA(s, externalImage, size);
        break;
    }

    fclose( s );

    return TGA_NO_ERROR;
}
#endif
