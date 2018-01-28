#include "cho_log.h"
#include "cho_eval.h"

#include <exception>	// for ::std::exception

#ifndef CHO_ARRAY_VIEW_BIT_H_9276349872384
#define CHO_ARRAY_VIEW_BIT_H_9276349872384

namespace cho
{
	template <typename _tElement>
	struct bit_array_view_proxy {
							_tElement						& Element;
							uint8_t							Offset;

							operator						bool						()																	const				{ return Element & (1 << Offset); }
							bit_array_view_proxy&			operator=					(bool value)																			{ value ? Element |= (1 << Offset) : Element &= ~(1 << Offset); return *this; }
	};

	template <typename _tElement>
	class bit_array_view {
	protected:
		// Properties / Member Variables
		static constexpr	const uint32_t					ELEMENT_BITS				= sizeof(_tElement) * 8;

							_tElement						* Data						= 0;
							uint32_t						Count						= 0;
	public:
		// Constructors
		inline constexpr									bit_array_view				()																			noexcept	= default;
		inline												bit_array_view				(_tElement* dataElements, uint32_t elementCount)										: Data(dataElements), Count(elementCount)										{
			throw_if(0 == dataElements && 0 != elementCount, ::std::exception(""), "Invalid parameters.");	// Crash if we received invalid parameters in order to prevent further malfunctioning.
		}

		template <size_t _elementCount>
		inline constexpr									bit_array_view				(_tElement (&_dataElements)[_elementCount])									noexcept	: Data(_dataElements), Count(_elementCount * ELEMENT_BITS)											{}

		template <size_t _elementCount>
		inline constexpr									bit_array_view				(_tElement (&_dataElements)[_elementCount], uint32_t elementCount)						: Data(_dataElements), Count(::cho::min(_elementCount * ELEMENT_BITS, elementCount))	{
			throw_if(elementCount > (_elementCount * ELEMENT_BITS), ::std::exception(""), "Out of range count.");
		}

		// Operators
							bit_array_view_proxy<_tElement>	operator[]					(uint32_t index)																		{ 
			throw_if(0 == Data, ::std::exception(""), "Uninitialized array pointer."); 
			throw_if(index >= Count, ::std::exception(""), "Invalid index."); 
			const uint32_t											offsetRow					= index / ELEMENT_BITS;
			const uint32_t											offsetBit					= index % ELEMENT_BITS;
			_tElement												& selectedElement			= Data[offsetRow];
			return {selectedElement, (uint8_t)offsetBit};
		}

							bool							operator[]					(uint32_t index)													const				{ 
			throw_if(0 == Data, ::std::exception(""), "Uninitialized array pointer."); 
			throw_if(index >= Count, ::std::exception(""), "Invalid index."); 
			const uint32_t											offsetElement				= index / ELEMENT_BITS;
			const uint32_t											offsetLocal					= index % ELEMENT_BITS;
			const _tElement											& selectedElement			= Data[offsetElement];
			return selectedElement & (1 << offsetLocal); 
		}

		// Methods
		inline				_tElement*						begin						()																			noexcept	{ return Data;							}
		inline				_tElement*						end							()																			noexcept	{ return Data + Count / ELEMENT_BITS;	}

		inline constexpr	const _tElement*				begin						()																	const	noexcept	{ return Data;							}
		inline constexpr	const _tElement*				end							()																	const	noexcept	{ return Data + Count / ELEMENT_BITS;	}

		inline constexpr	uint32_t						size						()																	const	noexcept	{ return Count;							}
	};
} // namespace

#endif // CHO_ARRAY_VIEW_BIT_H_9276349872384
