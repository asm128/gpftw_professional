#include "cho_ascii_color.h"
#include "cho_grid_view.h"
#include "cho_error.h"
#include "cho_coord.h"
#include "cho_color.h"
#include "cho_array.h"
#include <vector>

#ifndef CHO_ASCII_TARGET_H_29874982734
#define CHO_ASCII_TARGET_H_29874982734

namespace cho
{
	typedef					::cho::array_pod<::cho::SColorRGBA>				SPalette;

	struct SASCIITarget {
								::cho::grid_view<uint8_t>						Characters									= {};
								::cho::grid_view<uint16_t>						Colors										= {};

		inline constexpr		uint32_t										Width										()																	const	noexcept	{ return Characters.width	(); }
		inline constexpr		uint32_t										Height										()																	const	noexcept	{ return Characters.height	(); }
	};

							::cho::error_t									asciiTargetCreate							(::cho::SASCIITarget& target, uint32_t width, uint32_t height);
							::cho::error_t									asciiTargetDestroy							(::cho::SASCIITarget& target);
							::cho::error_t									asciiTargetClear							(::cho::SASCIITarget& target, uint8_t character = ' ', uint16_t color = ASCII_COLOR_WHITE);

	template<typename _tUnit>
	static inline			::cho::error_t									asciiTargetCreate							(::cho::SASCIITarget& target, const ::cho::SCoord2<_tUnit>& size)						{ return ::cho::asciiTargetCreate(console, (uint32_t)size.x, (uint32_t)size.y); }

	// ------------------------------------------------------ 
	struct SASCIICell {
								uint8_t											Character	;
								uint16_t										Color		;
	};

	static inline			::cho::error_t									drawRectangle								(::cho::SASCIITarget& asciiTarget, const ::cho::SASCIICell& value, const ::cho::SRectangle2D<int32_t>& rectangle)	{
		for(int32_t y = (int32_t)::cho::max(0, rectangle.Offset.y), yStop = ::cho::min((int32_t)(rectangle.Offset.y + rectangle.Size.y), (int32_t)asciiTarget.Height	()); y < yStop; ++y)
		for(int32_t x = (int32_t)::cho::max(0, rectangle.Offset.x), xStop = ::cho::min((int32_t)(rectangle.Offset.x + rectangle.Size.x), (int32_t)asciiTarget.Width		()); x < xStop; ++x) {	
			asciiTarget.Characters	[y][x]											= value.Character;
			asciiTarget.Colors		[y][x]											= value.Color;
		}
		return 0;
	}

	static inline			::cho::error_t									drawCircle									(::cho::SASCIITarget& asciiTarget, const ::cho::SASCIICell& value, const ::cho::SCircle2D<int32_t>& circle)			{
		for(int32_t y = ::cho::max(0, (int32_t)(circle.Center.y - circle.Radius)), yStop = ::cho::min((int32_t)(circle.Center.y + circle.Radius), (int32_t)asciiTarget.Height	()); y < yStop; ++y)
		for(int32_t x = ::cho::max(0, (int32_t)(circle.Center.x - circle.Radius)), xStop = ::cho::min((int32_t)(circle.Center.x + circle.Radius), (int32_t)asciiTarget.Width	()); x < xStop; ++x) {	
			::cho::SCoord2<int32_t>														cellCurrent									= {x, y};
			double																		distance									= (cellCurrent - circle.Center).Length();
			if(distance < circle.Radius) {
				asciiTarget.Characters	[y][x]											= value.Character;
				asciiTarget.Colors		[y][x]											= value.Color;
			}
		}
		return 0;
	}

	// A good article on this kind of triangle rasterization: https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/ 
	template<typename _tCoord>
	static inline			::cho::error_t									drawTriangle								(::cho::SASCIITarget& asciiTarget, const ::cho::SASCIICell& value, const ::cho::STriangle2D<_tCoord>& triangle)		{
		::cho::SCoord2		<int32_t>												areaMin										= {(int32_t)::cho::min(::cho::min(triangle.A.x, triangle.B.x), triangle.C.x), (int32_t)::cho::min(::cho::min(triangle.A.y, triangle.B.y), triangle.C.y)};
		::cho::SCoord2		<int32_t>												areaMax										= {(int32_t)::cho::max(::cho::max(triangle.A.x, triangle.B.x), triangle.C.x), (int32_t)::cho::max(::cho::max(triangle.A.y, triangle.B.y), triangle.C.y)};
		for(int32_t y = ::cho::max(areaMin.y, 0), yStop = ::cho::min(areaMax.y, (int32_t)asciiTarget.Height	()); y < yStop; ++y)
		for(int32_t x = ::cho::max(areaMin.x, 0), xStop = ::cho::min(areaMax.x, (int32_t)asciiTarget.Width	()); x < xStop; ++x) {	
			const ::cho::SCoord2<int32_t>												cellCurrent									= {x, y};
			// Determine barycentric coordinates
			int																			w0											= ::cho::orient2d({triangle.A, triangle.B}, cellCurrent);
			int																			w1											= ::cho::orient2d({triangle.B, triangle.C}, cellCurrent);
			int																			w2											= ::cho::orient2d({triangle.C, triangle.A}, cellCurrent);
			if (w0 >= 0 && w1 >= 0 && w2 >= 0) { // If p is on or inside all edges, render pixel.
				asciiTarget.Characters	[y][x]											= value.Character;
				asciiTarget.Colors		[y][x]											= value.Color;
			}
		}
		return 0;
	}

	// Bresenham's line algorithm
	template<typename _tCoord>
	static					::cho::error_t									drawLine									(::cho::SASCIITarget& asciiTarget, const ::cho::SASCIICell& value, const ::cho::SLine2D<_tCoord>& line)				{
		float																		x1											= (float)line.A.x
			,																		y1											= (float)line.A.y
			,																		x2											= (float)line.B.x
			,																		y2											= (float)line.B.y
			;
		const bool																	steep										= (fabs(y2 - y1) > fabs(x2 - x1));
		if(steep){
			::std::swap(x1, y1);
			::std::swap(x2, y2);
		}
		if(x1 > x2) {
			::std::swap(x1, x2);
			::std::swap(y1, y2);
		}
		const float																	dx											= x2 - x1;
		const float																	dy											= fabs(y2 - y1);
		float																		error										= dx / 2.0f;
		const int32_t																ystep										= (y1 < y2) ? 1 : -1;
		int32_t																		y											= (int32_t)y1;
		if(steep) {
			for(int32_t x = (int32_t)x1, xStop = (int32_t)x2; x < xStop; ++x) {
				if(false == ::cho::in_range(x, 0, (int32_t)asciiTarget.Height()) || false == ::cho::in_range(y, 0, (int32_t)asciiTarget.Width()))
					continue;
				asciiTarget.Characters	[x][y]											= value.Character;
				asciiTarget.Colors		[x][y]											= value.Color;
				error																	-= dy;
				if(error < 0) {
					y																		+= ystep;
					error																	+= dx;
				}
			}
		}
		else {
			for(int32_t x = (int32_t)x1, xStop = (int32_t)x2; x < xStop; ++x) {
				if(false == ::cho::in_range(y, 0, (int32_t)asciiTarget.Height()) || false == ::cho::in_range(x, 0, (int32_t)asciiTarget.Width()))
					continue;
				asciiTarget.Characters	[y][x]											= value.Character;
				asciiTarget.Colors		[y][x]											= value.Color;
				error																	-= dy;
				if(error < 0) {
					y																		+= ystep;
					error																	+= dx;
				}
			}
		}
		return 0;
	}
}

#endif // CHO_ASCII_TARGET_H_29874982734
