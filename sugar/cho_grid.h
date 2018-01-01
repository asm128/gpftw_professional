#include "cho_array.h"
#include "cho_grid_view.h"

#ifndef CHO_GRID_H_9802374982374
#define CHO_GRID_H_9802374982374

namespace cho
{
	template<typename _tCell, size_t _sizeWidth, size_t _sizeDepth>
	struct grid_static : public grid_view<_tCell> {
							typedef										_tCell									TCell;
							typedef										grid_view<_tCell>						TGridView;

		static constexpr	const uint32_t								Width									= (uint32_t)_sizeWidth;
		static constexpr	const uint32_t								Depth									= (uint32_t)_sizeDepth;

							TCell										Cells	[_sizeDepth][_sizeWidth]		= {};

		inline constexpr												grid_static								()																		: grid_view(&Cells[0][0], _sizeWidth, _sizeDepth)	{}

		inline				::cho::error_t								read									(const byte_t* input, uint32_t* inout_bytesRead)						{
			ree_if(0 == input, "Invalid input pointer!");
			TGridView															inputGrid								= {(_tCell*)input, Width, Depth};
			*inout_bytesRead												+= sizeof(_tCell) * size();
			for(uint32_t y = 0; y < Depth; ++y) {
				for(uint32_t x = 0; x < Width; ++x)
					::cho::podcpy(&Cells[y][x], inputGrid[y][x]);
			}
			return 0;
		}

		inline				::cho::error_t								write									(byte_t* input, uint32_t* inout_bytesWritten)		const				{
			ree_if(0 == input && 0 == inout_bytesWritten, "Invalid input!");
			if(0 != inout_bytesWritten)
				*inout_bytesWritten												+= sizeof(_tCell) * size();	// Just return the size required to store this.

			if(0 == input) 
				return 0;

			TGridView															newStorage								= {(_tCell*)input, Width, Depth};
			for(uint32_t y = 0; y < Depth; ++y) {
				for(uint32_t x = 0; x < Width; ++x)
					::cho::podcpy(&newStorage[i], &Data[i]);
			}
			return 0;
		}
	};

	template<typename _tCell>
	struct grid_base : public ::cho::grid_view<_tCell> {
		typedef					_tCell						TCell;

		using					grid_view<_tCell>::			Width ;
		using					grid_view<_tCell>::			Height;
								uint32_t					Size										= 0;

		inline constexpr									grid_base									()																			noexcept	= default;
		inline constexpr									grid_base									(const grid_base<_tCell>&	other)											noexcept	= delete;
		inline constexpr									grid_base									(const grid_base<_tCell>&&	other)											noexcept	= delete;

								grid_base<_tCell>&			operator =									(const grid_base<_tCell>&	other)														= delete;
								grid_base<_tCell>&			operator =									(const grid_base<_tCell>&&	other)														= delete;
		// This helper method is used to prevent redundancies. It returns a safe integer of the same or a higher value than the one passed as argument.
		inline constexpr		uint32_t					calc_reserve_size							(const uint32_t newSize)											const	noexcept	{ return ::cho::max(newSize, newSize + ::cho::max(newSize >> 1, 4U));						}
		inline constexpr		uint32_t					calc_malloc_size							(const uint32_t newSize)											const	noexcept	{ return ::cho::max(newSize*(uint32_t)sizeof(_tBase), Count*(uint32_t)sizeof(_tBase));	}
	};	

	template<typename _tPOD>
	struct grid_pod : public ::cho::grid_base<_tPOD> {
		using					grid_base					::TCell;
		typedef					grid_pod<_tPOD>				TGrid;

		inline												~grid_pod									()																						{ safe_cho_free(Data);		}
		inline constexpr									grid_pod									()																			noexcept	= default;
		inline												grid_pod									(uint32_t initialSize)																	{ resize(initialSize);		}
		inline												grid_pod									(TGrid&& other)																noexcept	{
			Size												= other.Size	;
			Width 												= other.Width 	;
			Height												= other.Height	;
			Data												= other.Data	;

			other.Size											= other.Width = other.Height					= 0;
			other.Data											= 0;
		}
															grid_pod									(const TGrid& other)															{
			if(other.Count) {
				const uint32_t											newSize										= other.Width * other.Height;
				const uint32_t											reserveSize									= calc_reserve_size(newSize);
				uint32_t												mallocSize									= calc_malloc_size(reserveSize);
				throw_if(mallocSize != (reserveSize * (uint32_t)sizeof(_tPOD)), "", "Alloc size overflow. Requested size: %u. malloc size: %u.", reserveSize, mallocSize)
				else {
					::cho::auto_cho_free									safeguard;
					Data												= (_tPOD*)(safeguard.Handle = ::cho::cho_malloc(mallocSize));
						 throw_if(0 == Data			, "", "Failed to allocate array. Requested size: %u. ", (uint32_t)newSize)
					else throw_if(0 == other.Data	, "", "%s", "other.Data is null!")
					else {
						for(uint32_t i = 0, count = newSize; i<count; ++i)
							Data[i]												= other[i];
						Size												= (uint32_t)reserveSize;
						Width												= other.Width;
						Height												= other.Height;
						safeguard.Handle									= 0;
					}
				}
			}
		}
		template <size_t _otherCount>
															grid_pod									(const _tPOD (&other)[_otherCount])														{
			if(other.Width * other.Height) {
				const uint32_t											newSize										= other.Width * other.Height;
				const uint32_t											reserveSize									= calc_reserve_size(newSize);
				uint32_t												mallocSize									= calc_malloc_size(reserveSize);
				throw_if(mallocSize != (reserveSize * (uint32_t)sizeof(_tPOD)), "", "Alloc size overflow. Requested size: %u. malloc size: %u.", reserveSize, mallocSize)
				else {
					::cho::auto_cho_free									safeguard;
					Data												= (_tPOD*)(safeguard.Handle = ::cho::cho_malloc(mallocSize));
						 throw_if(0 == Data			, "", "Failed to allocate array. Requested size: %u. ", (uint32_t)newSize) 
					else throw_if(0 == other.Data	, "", "%s", "other.Data is null!") 
					else {
						for(uint32_t i = 0, count = newSize; i < count; ++i)
							Data[i]												= other[i];
						Size												= (uint32_t)reserveSize;
						Width												= other.Width;
						Height												= other.Height;
						safeguard.Handle									= 0;
					}
				}
			} // 
		}
							TGrid&							operator =									(const TGrid& other)															{
			throw_if(resize(other.Count) != (int32_t)other.Count, "", "Failed to assign array.");
			for(uint32_t iElement = 0; iElement < other.Count; ++iElement)
				operator[](iElement)								= other[iElement];
			return *this;
		}
		//// This method doesn't call destructors of the contained PODs.
		//inline				int32_t							clear										()																			noexcept	{ return Count = 0; }
	};	
}

#endif // CHO_GRID_H_9802374982374
