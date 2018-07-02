#include "cho_array.h"
#include "cho_grid_view.h"
#include "cho_bit_view.h"

#ifndef CHO_TEXTURE_H_902387498237
#define CHO_TEXTURE_H_902387498237

namespace cho
{
	template<typename _tTexel>
	struct STexture {
		typedef				_tTexel												TTexel;

							::cho::array_pod<_tTexel>							Texels										;
							::cho::grid_view<_tTexel>							View										;

		constexpr																STexture									()													= default;
																				STexture									(const ::cho::grid_view<_tTexel>& other)				{ 
			Texels																	= other;
			View																	= {Texels.begin(), other.metrics()};
			return *this; 
		}

																				STexture									(const ::cho::STexture<_tTexel>& other)				{ 
			Texels																	= other.Texels;
			View																	= {Texels.begin(), other.View.metrics()};
			return *this; 
		}

							::cho::STexture<_tTexel>&							operator=									(const ::cho::grid_view<_tTexel>& other)			{ 
			Texels																	= {other.begin(), other.size()};
			View																	= {Texels.begin(), other.metrics()};
			return *this; 
		}

							::cho::STexture<_tTexel>&							operator=									(const ::cho::STexture<_tTexel>& other)				{ 
			Texels																	= other.Texels;
			View																	= {Texels.begin(), other.View.metrics()};
			return *this; 
		}


		inline				::cho::array_view<_tTexel>							operator[]									(uint32_t index)									{ return View[index]; }
		inline	const		::cho::array_view<_tTexel>							operator[]									(uint32_t index)	const							{ return View[index]; }

							::cho::error_t										resize										(uint32_t newSizeX, uint32_t newSizeY)				{ cho_necall(Texels.resize(newSizeX * newSizeY), "cannot resize?"); View = {Texels.begin(), newSizeX, newSizeY}; return 0; }
		inline				::cho::error_t										resize										(::cho::SCoord2<uint32_t> newSize)					{ return resize(newSize.x, newSize.y); }
	}; // struct

	template<typename _tTexel>
	struct STextureMonochrome {
		typedef				_tTexel												TTexel;

							::cho::array_pod		<_tTexel>					Texels										;
							::cho::bit_view	<_tTexel>					View										;
							uint32_t											Pitch										= 0;

		constexpr																STextureMonochrome							()													= default;
																				STextureMonochrome							(const ::cho::bit_view<_tTexel>& other)		{ 
			Texels																	= other;
			View																	= {Texels.begin(), Texels.size()};
			Pitch																	= other.Pitch;
			return *this; 
		}

																				STextureMonochrome							(const ::cho::STextureMonochrome<_tTexel>& other)	{ 
			Texels																	= other.Texels;
			View																	= {Texels.begin(), Texels.size()};
			Pitch																	= other.Pitch;
			return *this; 
		}

							::cho::STextureMonochrome<_tTexel>&					operator=									(const ::cho::bit_view<_tTexel>& other)		{ 
			Texels																	= other;
			View																	= {Texels.begin(), Texels.size()};
			Pitch																	= other.Pitch;
			return *this; 
		}

							::cho::STextureMonochrome<_tTexel>&					operator=									(const ::cho::STextureMonochrome<_tTexel>& other)	{ 
			Texels																	= other.Texels;
			View																	= {Texels.begin(), Texels.size()};
			Pitch																	= other.Pitch;
			return *this; 
		}

							::cho::error_t										resize										(uint32_t newSizeX, uint32_t newSizeY)				{ cho_necall(Texels.resize(newSizeX * newSizeY), "cannot resize?"); View = {Texels.begin(), newSizeX * newSizeY}; Pitch = newSizeX; return 0; }
		inline				::cho::error_t										resize										(const ::cho::SCoord2<uint32_t> & newSize)			{ return resize(newSize.x, newSize.y); }
	}; // struct



	template<typename _tTexel>
	struct STextureProcessable {
		typedef				_tTexel												TTexel;

							::cho::STexture<_tTexel>							Original;
							::cho::STexture<_tTexel>							Processed;
	}; // struct
} // namespace 

#endif // CHO_TEXTURE_H_902387498237
