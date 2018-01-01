#include "cho_bitmap_file.h"

#pragma pack(push, 1)

//BMP File header 
struct SHeaderFileBMP
{
	unsigned char	m_ucType[2];	//Identifier, must be BM
	unsigned int	m_uiSize;		//Size of BMP file
	unsigned int	m_uiReserved;	//0
	unsigned int	m_uiOffset;	
};

//BMP Information Header
struct SHeaderInfoBMP
{
	unsigned int	m_uiSize;			// Number of bytes in structure
	unsigned int	m_uiWidth;			// Width of Image
	unsigned int	m_uiHeight;		// Height of Image
	unsigned short	m_uiPlanes;		// Always 1
	unsigned short	m_uiBpp;			// Bits Per Pixel (must be 24 for now)
	unsigned int	m_uiCompression;	// Must be 0 (uncompressed)
	unsigned int	m_uiXPPM;			// X Pels Per Meter
	unsigned int	m_uiYPPM;			// Y Pels Per Meter
	unsigned int	m_uiClrUsed;		// 0 for 24 bpp bmps
	unsigned int	m_uiClrImp;		// 0
	unsigned int	m_dunno;			// 0
};

#pragma pack( pop )

					::cho::error_t																	cho::bmpFileLoad							(byte_t* source, ::cho::array_pod<::cho::SColorBGRA>& output)					{
	info_printf("sizeof(SBMPFHeader) = %u", (int)sizeof(SHeaderFileBMP));
	info_printf("sizeof(SBMPIHeader) = %u", (uint32_t)sizeof(SHeaderInfoBMP));
	//SHeaderFileBMP																							& fileHeader								= *(SHeaderFileBMP*)*source;	
	SHeaderInfoBMP																							& infoHeader								= *(SHeaderInfoBMP*)*(source + sizeof(SHeaderFileBMP));

	ree_if(infoHeader.m_uiBpp != 24, "%s", "Unsupported BMP file! The image is not 24 bit.");	// make sure its 24 bpp 

	uint32_t																								nPixelCount									= infoHeader.m_uiWidth * infoHeader.m_uiHeight;
	ree_if(0 == nPixelCount, "Invalid BMP image size! Valid images are at least 1x1 pixels! This image claims to contain %ux%u pixels", infoHeader.m_uiWidth, infoHeader.m_uiHeight );	// make sure it contains data 
	output.resize(nPixelCount);
	ubyte_t																									* srcBytes									= (ubyte_t*)(source + sizeof(SHeaderFileBMP) + sizeof(SHeaderInfoBMP));
	bool																									b32Bit										= false;
	uint32_t																								colorSize									= 0;
	switch(infoHeader.m_uiBpp) {
	default: error_printf("Unsupported BMP file! The image is not 24 bit."); return -1;
	case 32:
		b32Bit																								= true;
	case 24:
		colorSize																							= b32Bit ? 4 : 3;
		for( uint32_t y = 0; y < infoHeader.m_uiHeight; y++ )
			for( uint32_t x = 0; x < infoHeader.m_uiWidth; x++ ) {
				uint32_t																								linearIndexSrc								= y * infoHeader.m_uiWidth * colorSize + (x * colorSize);
				output[y * infoHeader.m_uiWidth + x]																= 
					{ srcBytes[linearIndexSrc + 2]
					, srcBytes[linearIndexSrc + 1]
					, srcBytes[linearIndexSrc + 0]
					, b32Bit ? srcBytes[linearIndexSrc + 3] : 0xFFU
					};
			}
		break;
	case 8 :
		for( uint32_t y = 0; y < infoHeader.m_uiHeight; ++y )
			for( uint32_t x = 0; x < infoHeader.m_uiWidth; ++x ) {
				uint32_t																								linearIndexSrc								= y * infoHeader.m_uiWidth + x;
				output[linearIndexSrc]																				= 
					{ srcBytes[linearIndexSrc]
					, srcBytes[linearIndexSrc]
					, srcBytes[linearIndexSrc]
					, 0xFFU
					};
			}
		break;
	}
	return 0;
}
