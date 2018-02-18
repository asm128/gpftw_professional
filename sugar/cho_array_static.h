#include "cho_array_view.h"

#ifndef CHO_ARRAY_STATIC_H_2983749823749826534465243
#define CHO_ARRAY_STATIC_H_2983749823749826534465243
namespace cho
{
	template<typename _tCell, uint32_t _sizeArray>
	struct array_static : public array_view<_tCell> {
							_tCell						Storage	[_sizeArray]						= {};
	
		constexpr										array_static								()																						{
			array_view<_tCell>::Data						= Storage;
			array_view<_tCell>::Count						= _sizeArray;
		}
														array_static								(::std::initializer_list<_tCell> init)													{ 
			array_view<_tCell>::Data						= Storage;
			array_view<_tCell>::Count						= _sizeArray;
			throw_if(errored(init.size() > _sizeArray), ::std::exception(), "Failed to resize array! Why?");
			for(uint32_t i = 0, count = init.size(); i < count; ++i)
				Storage[i]										= *(init.begin() + i);
		}
	};
} // namespace

#endif // CHO_ARRAY_STATIC_H_2983749823749826534465243