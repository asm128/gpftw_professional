#include "ftw_log.h"
#include "ftw_eval.h"

#include <exception>	// for ::std::exception

#ifndef FTW_ARRAY_VIEW_H_2398472395543
#define FTW_ARRAY_VIEW_H_2398472395543

namespace ftw
{
	template <typename _tElement>
	class array_view {
		// Properties / Member Variables
							_tElement				* Data						= 0;
							uint32_t				Count						= 0;
	public:
		// Constructors
		inline constexpr							array_view					()																			noexcept	= default;
		inline										array_view					(_tElement* dataElements, uint32_t elementCount)										: Data(dataElements), Count(elementCount)										{
			throw_if(0 == dataElements && 0 != elementCount, ::std::exception(""), "Invalid parameters.");	// Crash if we received invalid parameters in order to prevent further malfunctioning.
		}

		template <size_t _elementCount>
		inline constexpr							array_view					(_tElement (&_dataElements)[_elementCount])									noexcept	: Data(_dataElements), Count(_elementCount)										{}

		template <size_t _elementCount>
		inline constexpr							array_view					(_tElement (&_dataElements)[_elementCount], uint32_t elementCount)			noexcept	: Data(_dataElements), Count(::ftw::min(_elementCount, elementCount))			{}

		// Operators
							_tElement&				operator[]					(uint32_t index)																		{ throw_if(0 == Data, ::std::exception(""), "Uninitialized array pointer."); throw_if(index >= Count, ::std::exception(""), "Invalid index."); return Data[index]; }
							const _tElement&		operator[]					(uint32_t index)													const				{ throw_if(0 == Data, ::std::exception(""), "Uninitialized array pointer."); throw_if(index >= Count, ::std::exception(""), "Invalid index."); return Data[index]; }

		// Methods
		inline				_tElement*				begin						()																			noexcept	{ return Data;			}
		inline				_tElement*				end							()																			noexcept	{ return Data + Count;	}

		inline constexpr	const _tElement*		begin						()																	const	noexcept	{ return Data;			}
		inline constexpr	const _tElement*		end							()																	const	noexcept	{ return Data + Count;	}

		inline constexpr	uint32_t				size						()																	const	noexcept	{ return Count;			}
	};
}

#endif FTW_ARRAY_VIEW_H_2398472395543
