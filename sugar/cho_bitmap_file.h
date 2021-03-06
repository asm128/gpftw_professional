#include "cho_texture.h"
#include "cho_color.h"

#ifndef CHO_BITMAP_FILE_H_236544263544352344
#define CHO_BITMAP_FILE_H_236544263544352344

namespace cho
{
						::cho::error_t							bmpFileLoad							(const byte_t					* source		, ::cho::array_pod	<::cho::SColorBGRA>& out_Colors, ::cho::grid_view<::cho::SColorBGRA>& out_ImageView);
						::cho::error_t							bmpFileLoad							(FILE							* source		, ::cho::array_pod	<::cho::SColorBGRA>& out_Colors, ::cho::grid_view<::cho::SColorBGRA>& out_ImageView);
						::cho::error_t							bmpFileLoad							(const ::cho::view_const_string	& filename		, ::cho::array_pod	<::cho::SColorBGRA>& out_Colors, ::cho::grid_view<::cho::SColorBGRA>& out_ImageView);

						::cho::error_t							bmgFileLoad							(const byte_t					* source		, ::cho::array_pod	<::cho::SColorBGRA>& out_Colors, ::cho::grid_view<::cho::SColorBGRA>& out_ImageView);
						::cho::error_t							bmgFileLoad							(FILE							* source		, ::cho::array_pod	<::cho::SColorBGRA>& out_Colors, ::cho::grid_view<::cho::SColorBGRA>& out_ImageView);
						::cho::error_t							bmgFileLoad							(const ::cho::view_const_string	& filename		, ::cho::array_pod	<::cho::SColorBGRA>& out_Colors, ::cho::grid_view<::cho::SColorBGRA>& out_ImageView);

						::cho::error_t							bmgFileWrite						(byte_t							* destination	, const ::cho::grid_view<::cho::SColorBGRA>& in_ImageView);
						::cho::error_t							bmgFileWrite						(FILE							* destination	, const ::cho::grid_view<::cho::SColorBGRA>& in_ImageView);
						::cho::error_t							bmgFileWrite						(const ::cho::view_const_string	& filename		, const ::cho::grid_view<::cho::SColorBGRA>& in_ImageView);

	static inline		::cho::error_t							bmpFileLoad							(const byte_t					* source		, ::cho::STexture	<::cho::SColorBGRA>& out_Texture)	{ return bmpFileLoad(source		, out_Texture.Texels, out_Texture.View); }
	static inline		::cho::error_t							bmpFileLoad							(FILE							* source		, ::cho::STexture	<::cho::SColorBGRA>& out_Texture)	{ return bmpFileLoad(source		, out_Texture.Texels, out_Texture.View); }
	static inline		::cho::error_t							bmpFileLoad							(const ::cho::view_const_string	& filename		, ::cho::STexture	<::cho::SColorBGRA>& out_Texture)	{ return bmpFileLoad(filename	, out_Texture.Texels, out_Texture.View); }

	static inline		::cho::error_t							bmgFileLoad							(const byte_t					* source		, ::cho::STexture	<::cho::SColorBGRA>& out_Texture)	{ return bmgFileLoad(source		, out_Texture.Texels, out_Texture.View); }
	static inline		::cho::error_t							bmgFileLoad							(FILE							* source		, ::cho::STexture	<::cho::SColorBGRA>& out_Texture)	{ return bmgFileLoad(source		, out_Texture.Texels, out_Texture.View); }
	static inline		::cho::error_t							bmgFileLoad							(const ::cho::view_const_string	& filename		, ::cho::STexture	<::cho::SColorBGRA>& out_Texture)	{ return bmgFileLoad(filename	, out_Texture.Texels, out_Texture.View); }

}

#endif // CHO_BITMAP_FILE_H_236544263544352344
