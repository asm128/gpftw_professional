// Original file created on: 2002/08/30 19:33 by Andreas Hartl. Visit http://www.runicsoft.com for updates and more information
#include "cho_bitmap_file.h"

#pragma pack(push, 1)

// BMP File header 
struct SHeaderFileBMP {
	uint8_t				Type[2]		;	// Identifier, must be BM
	uint32_t			Size		;	// Size of BMP file
	uint32_t			Reserved	;	// 0
	uint32_t			Offset		;	//
};

// BMP Information Header
struct SHeaderInfoBMP {
	uint32_t			Size		;	// Number of bytes in structure
	uint32_t			Width		;	// Width of Image
	uint32_t			Height		;	// Height of Image
	uint16_t			Planes		;	// Always 1
	uint16_t			Bpp			;	// Bits Per Pixel (must be 24 for now)
	uint32_t			Compression	;	// Must be 0 (uncompressed)
	uint32_t			XPPM		;	// X Pels Per Meter
	uint32_t			YPPM		;	// Y Pels Per Meter
	uint32_t			ClrUsed		;	// 0 for 24 bpp bmps
	uint32_t			ClrImp		;	// 0
	uint32_t			Dunno		;	// 0
};

#pragma pack( pop )

// Currently supporting only 24-bit bitmaps
					::cho::error_t																	cho::bmpFileLoad							(byte_t* source, ::cho::array_pod<::cho::SColorBGRA>& out_Colors, ::cho::grid_view<::cho::SColorBGRA>& out_ImageView)					{
	info_printf("sizeof(SBMPFHeader) = %u", (uint32_t)sizeof(SHeaderFileBMP));
	info_printf("sizeof(SBMPIHeader) = %u", (uint32_t)sizeof(SHeaderInfoBMP));
	//SHeaderFileBMP																							& fileHeader								= *(SHeaderFileBMP*)*source;	
	SHeaderInfoBMP																							& infoHeader								= *(SHeaderInfoBMP*)*(source + sizeof(SHeaderFileBMP));

	ree_if(infoHeader.Bpp != 24
		&& infoHeader.Bpp != 32
		&& infoHeader.Bpp != 8
		, "Unsupported bitmap format! Only 8, 24 and 32-bit bitmaps are supported.");	

	uint32_t																								nPixelCount									= infoHeader.Width * infoHeader.Height;
	ree_if(0 == nPixelCount, "Invalid BMP image size! Valid images are at least 1x1 pixels! This image claims to contain %ux%u pixels", infoHeader.Width, infoHeader.Height );	// make sure it contains data 
	out_Colors.resize(nPixelCount);
	ubyte_t																									* srcBytes									= (ubyte_t*)(source + sizeof(SHeaderFileBMP) + sizeof(SHeaderInfoBMP));
	bool																									b32Bit										= false;
	uint32_t																								colorSize									= 0;
	switch(infoHeader.Bpp) {
	default: error_printf("Unsupported BMP file! The image is not 24 bit."); return -1;
	case 32:
		b32Bit																								= true;
	case 24:
		colorSize																							= b32Bit ? 4 : 3;
		for( uint32_t y = 0; y < infoHeader.Height; y++ )
			for( uint32_t x = 0; x < infoHeader.Width; x++ ) {
				uint32_t																								linearIndexSrc								= y * infoHeader.Width * colorSize + (x * colorSize);
				out_Colors[y * infoHeader.Width + x]																= 
					{ srcBytes[linearIndexSrc + 2]
					, srcBytes[linearIndexSrc + 1]
					, srcBytes[linearIndexSrc + 0]
					, b32Bit ? srcBytes[linearIndexSrc + 3] : 0xFFU
					};
			}
		break;
	case 8 :
		for( uint32_t y = 0; y < infoHeader.Height; ++y )
			for( uint32_t x = 0; x < infoHeader.Width; ++x ) {
				uint32_t																								linearIndexSrc								= y * infoHeader.Width + x;
				out_Colors[linearIndexSrc]																				= 
					{ srcBytes[linearIndexSrc]
					, srcBytes[linearIndexSrc]
					, srcBytes[linearIndexSrc]
					, 0xFFU
					};
			}
		break;
	}
	out_ImageView																						= ::cho::grid_view<::cho::SColorBGRA>{out_Colors.begin(), infoHeader.Width, infoHeader.Height};
	return 0;
}

// Currently supporting only 24-bit bitmaps
					::cho::error_t																	cho::bmpFileLoad							(FILE* source, ::cho::array_pod<::cho::SColorBGRA>& out_Colors, ::cho::grid_view<::cho::SColorBGRA>& out_ImageView)					{
	info_printf("sizeof(SBMPFHeader) = %u", (uint32_t)sizeof(SHeaderFileBMP));
	info_printf("sizeof(SBMPIHeader) = %u", (uint32_t)sizeof(SHeaderInfoBMP));
	SHeaderFileBMP																							fileHeader									= {};	
	SHeaderInfoBMP																							infoHeader									= {};
	ree_if(fread(&fileHeader, 1, sizeof(SHeaderFileBMP), source) != sizeof(SHeaderFileBMP), "Failed to read file! File corrupt?");
	ree_if(fread(&infoHeader, 1, sizeof(SHeaderInfoBMP), source) != sizeof(SHeaderInfoBMP), "Failed to read file! File corrupt?");
	uint32_t																								nPixelCount									= infoHeader.Width * infoHeader.Height;
	ree_if(0 == nPixelCount, "Invalid BMP image size! Valid images are at least 1x1 pixels! This image claims to contain %ux%u pixels", infoHeader.Width, infoHeader.Height );	// make sure it contains data 

	ree_if(infoHeader.Compression != BI_RGB, "Unsupported bmp compression!");
	ree_if(infoHeader.Bpp != 24
		&& infoHeader.Bpp != 32
		&& infoHeader.Bpp != 8
		&& infoHeader.Bpp != 1
		, "Unsupported bitmap format! Only 8, 24 and 32-bit bitmaps are supported.");	

	uint32_t																								pixelSize								= infoHeader.Bpp == 1 ? 1 : infoHeader.Bpp / 8;
	::cho::array_pod<ubyte_t>																				srcBytes								= {};
	cho_necall(srcBytes.resize(nPixelCount * pixelSize), "Out of memory?");
	size_t																										readResult								= fread(&srcBytes[0], pixelSize, nPixelCount, source);
	ree_if(readResult != (size_t)nPixelCount, "Failed to read file! File corrupt?");
	cho_necall(out_Colors.resize(nPixelCount), "Out of memory?");
	bool																									b32Bit										= false;
	uint32_t																								colorSize									= 0;
	switch(infoHeader.Bpp) {
	default: error_printf("Unsupported BMP file! The image is not 24 bit."); return -1;
	case 32:
		b32Bit																								= true;
	case 24:
		colorSize																							= b32Bit ? 4 : 3;
		for( uint32_t y = 0; y < infoHeader.Height; y++ )
			for( uint32_t x = 0; x < infoHeader.Width; x++ ) {
				uint32_t																								linearIndexSrc								= y * infoHeader.Width * colorSize + (x * colorSize);
				out_Colors[y * infoHeader.Width + x]																= 
					{ srcBytes[linearIndexSrc + 2]
					, srcBytes[linearIndexSrc + 1]
					, srcBytes[linearIndexSrc + 0]
					, b32Bit ? srcBytes[linearIndexSrc + 3] : 0xFFU
					};
			}
		break;
	case 8 :
		for( uint32_t y = 0; y < infoHeader.Height; ++y )
			for( uint32_t x = 0; x < infoHeader.Width; ++x ) {
				uint32_t																								linearIndexSrc								= y * infoHeader.Width + x;
				out_Colors[linearIndexSrc]																				= 
					{ srcBytes[linearIndexSrc]
					, srcBytes[linearIndexSrc]
					, srcBytes[linearIndexSrc]
					, 0xFFU
					};
			}
		break;
	case 1 :
		for(uint32_t y = 0; y < infoHeader.Height; ++y)
			for(uint32_t x = 0; x < infoHeader.Width; ++x) {
				uint32_t																								linearIndexSrc								= y * (infoHeader.Width / 8) + x / 8;
				uint32_t																								linearIndexDst								= y * infoHeader.Width + x;
				out_Colors[linearIndexDst]																				= 
					{ srcBytes[linearIndexSrc] & (1U << (x % 8))
					, srcBytes[linearIndexSrc] & (1U << (x % 8))
					, srcBytes[linearIndexSrc] & (1U << (x % 8))
					, 0xFFU
					};
			}
		break;
	}
	out_ImageView																						= ::cho::grid_view<::cho::SColorBGRA>{out_Colors.begin(), infoHeader.Width, infoHeader.Height};
	return 0;
}
