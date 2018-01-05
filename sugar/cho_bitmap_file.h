#include "cho_array.h"
#include "cho_grid_view.h"
#include "cho_color.h"

#ifndef CHO_BITMAP_FILE_H_236544263544352344
#define CHO_BITMAP_FILE_H_236544263544352344

namespace cho
{
						::cho::error_t							bmpFileLoad							(byte_t	* source, ::cho::array_pod<::cho::SColorBGRA>& out_Colors, ::cho::grid_view<::cho::SColorBGRA>& out_ImageView);
						::cho::error_t							bmpFileLoad							(FILE	* source, ::cho::array_pod<::cho::SColorBGRA>& out_Colors, ::cho::grid_view<::cho::SColorBGRA>& out_ImageView);
}

#endif // CHO_BITMAP_FILE_H_236544263544352344
