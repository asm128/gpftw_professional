#include "cho_array.h"
#include "cho_grid_view.h"

#ifndef CHO_GRID_H_9802374982374
#define CHO_GRID_H_9802374982374

namespace cho
{
	template<typename _tCell>
	struct grid_base : public ::cho::grid_view<_tCell> {
		typedef					_tCell						TCell;

		using					grid_view<_tCell>::			Width;
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
		typedef					grid_pod<_tPOD>			TGrid;

		inline												~grid_pod									()																						{ safe_cho_free(Data);		}
		inline constexpr									grid_pod									()																			noexcept	= default;
		inline												grid_pod									(uint32_t initialSize)																	{ resize(initialSize);		}
		inline												grid_pod									(TGrid&& other)																noexcept	{
			Size												= other.Size	;
			Count												= other.Count	;
			Data												= other.Data	;

			other.Size											= other.Count									= 0;
			other.Data											= 0;
		}
															grid_pod									(const TGrid& other)															{
			if(other.Count) {
				const uint32_t											newSize										= other.Count;
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
						Count												= other.Count;
						safeguard.Handle									= 0;
					}
				}
			}
		}
		template <size_t _otherCount>
															grid_pod									(const _tPOD (&other)[_otherCount])														{
			if(other.Count) {
				const uint32_t											newSize										= other.Count;
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
						Count												= other.Count;
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
