#include "cho_texture.h"
#include "cho_color.h"
#include "cho_coord.h"
#include <memory> // this is required for ::std::swap()

#ifndef BITMAP_TARGET_H_98237498023745654654
#define BITMAP_TARGET_H_98237498023745654654

namespace cho
{

	template<typename _tCoord>
					::cho::error_t										drawPixelLight								(::cho::grid_view<::cho::SColorBGRA> & viewOffscreen, const ::cho::SCoord2<_tCoord> & sourcePosition, _tCoord factor, _tCoord range)								{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
		::cho::SCoord2<_tCoord>													maxRange									= {range, range};
		_tCoord																	colorUnit									= _tCoord(1.0 / maxRange.Length());
		for(int32_t y = -(int32_t)range - 2, blendCount = 1 + (int32_t)range + 2; y < blendCount; ++y)	// the + 2 - 2 is because we actually process more surrounding pixels in order to compensate for the flooring of the coordinates 
		for(int32_t x = -(int32_t)range - 2; x < blendCount; ++x) {								// as it causes a visual effect of the light being cut to a rectangle and having sharp borders.
			::cho::SCoord2<_tCoord>													blendPos									= sourcePosition + ::cho::SCoord2<_tCoord>{(_tCoord)x, (_tCoord)y};
			if( blendPos.x < viewOffscreen.width () && blendPos.x >= 0
			 && blendPos.y < viewOffscreen.height() && blendPos.y >= 0
			 ) {
				::cho::SCoord2<_tCoord>													brightDistance								= blendPos - sourcePosition;
				double																	brightDistanceLength						= brightDistance.Length();
				double																	colorFactor									= ::cho::min(fabs(brightDistanceLength * colorUnit), 1.0);
				if( y != (int32_t)sourcePosition.y
				 || x != (int32_t)sourcePosition.x
				 )
					viewOffscreen[(uint32_t)blendPos.y][(uint32_t)blendPos.x]				= ::cho::interpolate_linear(viewOffscreen[(uint32_t)blendPos.y][(uint32_t)blendPos.x], viewOffscreen[(uint32_t)sourcePosition.y][(uint32_t)sourcePosition.x], factor * (1.0f - colorFactor));
			}
		}
		return 0;
	}

	template<typename _tElement>
						::cho::error_t										updateSizeDependentTarget					(::cho::array_pod<_tElement>& out_colors, ::cho::grid_view<_tElement>& out_view, const ::cho::SCoord2<uint32_t>& newSize)											{ 
		ree_if(errored(out_colors.resize(newSize.x * newSize.y)), "Out of memory?");		// Update size-dependent resources.
		if( out_view.width () != newSize.x
		 || out_view.height() != newSize.y
		 ) 
			out_view																= {out_colors.begin(), newSize.x, newSize.y};
		return 0;
	}

	template<typename _tElement>
	static inline		::cho::error_t										updateSizeDependentTarget					(::cho::STexture<_tElement>& out_texture, const ::cho::SCoord2<uint32_t>& newSize)																					{ 
		return updateSizeDependentTarget(out_texture.Texels, out_texture.View, newSize);
	}
	template<typename _tElement>
						::cho::error_t										updateSizeDependentTexture					(::cho::array_pod<_tElement>& out_scaled, ::cho::grid_view<_tElement>& out_view, const ::cho::grid_view<_tElement>& in_view, const ::cho::SCoord2<uint32_t>& newSize)											{ 
		ree_if(errored(out_scaled.resize(newSize.x * newSize.y)), "Out of memory?");		// Update size-dependent resources.
		if( out_view.width () != newSize.x
		 || out_view.height() != newSize.y
		 ) { 
			out_view																= {out_scaled.begin(), newSize.x, newSize.y};
			if(in_view.size())
				error_if(errored(::cho::grid_scale(out_view, in_view)), "I believe this never fails.");
		}
		return 0;
	}

	template<typename _tElement>
	static inline		::cho::error_t										updateSizeDependentTexture					(::cho::STexture<_tElement>& out_texture, const ::cho::grid_view<_tElement>& in_view, const ::cho::SCoord2<uint32_t>& newSize)																					{ 
		return updateSizeDependentTexture(out_texture.Texels, out_texture.View, in_view, newSize);
	}

	template<typename _tCoord, typename _tColor>
	static					::cho::error_t									drawRectangleBorder							(::cho::grid_view<_tColor>& bitmapTarget, const _tColor& value, const ::cho::SRectangle2D<_tCoord>& rectangle)		{
		int32_t																		yStart										= (int32_t)::cho::max(0, (int32_t)rectangle.Offset.y);
		int32_t																		yStop										= ::cho::min((int32_t)rectangle.Offset.y + (int32_t)rectangle.Size.y, (int32_t)bitmapTarget.height());
		int32_t																		xStart										= (int32_t)::cho::max(0, (int32_t)rectangle.Offset.x);
		int32_t																		xStop										= ::cho::min((int32_t)rectangle.Offset.x + (int32_t)rectangle.Size.x, (int32_t)bitmapTarget.width());
		if(yStart >= yStop || xStart >= xStop)
			return 0;
		for(int32_t x = xStart; x < xStop; ++x) 	
			bitmapTarget[yStart][x]											= value;
		memcpy(&bitmapTarget[yStop - 1][xStart], &bitmapTarget[yStart][xStart], sizeof(_tColor) * xStop - xStart);
		for(int32_t y = yStart + 1, yMax = (yStop - 1); y < yMax; ++y) {
			bitmapTarget[y][0]											= value;
			bitmapTarget[y][xStop - 1]									= value;
		}
		return 0;
	}

	template<typename _tCoord, typename _tColor>
	static					::cho::error_t									drawRectangle								(::cho::grid_view<_tColor>& bitmapTarget, const _tColor& value, const ::cho::SRectangle2D<_tCoord>& rectangle)		{
		int32_t																		yStart										= (int32_t)::cho::max(0, (int32_t)rectangle.Offset.y);
		int32_t																		yStop										= ::cho::min((int32_t)rectangle.Offset.y + (int32_t)rectangle.Size.y, (int32_t)bitmapTarget.height());
		int32_t																		xStart										= (int32_t)::cho::max(0, (int32_t)rectangle.Offset.x);
		int32_t																		xStop										= ::cho::min((int32_t)rectangle.Offset.x + (int32_t)rectangle.Size.x, (int32_t)bitmapTarget.width());
		if(yStart >= yStop || xStart >= xStop)
			return 0;
		for(int32_t x = xStart; x < xStop; ++x) 	
			bitmapTarget[yStart][x]													= value;
		for(int32_t y = yStart + 1; y < yStop; ++y)
			memcpy(&bitmapTarget[y][xStart], &bitmapTarget[yStart][xStart], sizeof(_tColor) * xStop - xStart);
		return 0;
	}

	template<typename _tCoord, typename _tColor>
	static					::cho::error_t									drawCircle									(::cho::grid_view<_tColor>& bitmapTarget, const _tColor& value, const ::cho::SCircle2D<_tCoord>& circle)			{
		int32_t																		xStop										= ::cho::min((int32_t)(circle.Center.x + circle.Radius), (int32_t)bitmapTarget.width	());
		double																		radiusSquared								= circle.Radius * circle.Radius;
		for(int32_t y = ::cho::max(0, (int32_t)(circle.Center.y - circle.Radius)), yStop = ::cho::min((int32_t)(circle.Center.y + circle.Radius), (int32_t)bitmapTarget.height()); y < yStop; ++y)
		for(int32_t x = ::cho::max(0, (int32_t)(circle.Center.x - circle.Radius)); x < xStop; ++x) {	
			::cho::SCoord2<int32_t>														cellCurrent									= {x, y};
			double																		distanceSquared								= (cellCurrent - circle.Center).LengthSquared();
			if(distanceSquared < radiusSquared) {
			 	for(const int32_t xLimit = ::cho::min((int32_t)circle.Center.x + ((int32_t)circle.Center.x - x), (int32_t)bitmapTarget.width()); x < xLimit; ++x)
					bitmapTarget[y][x]											= value;
				break;
			}
		}
		return 0;
	}

	// A good article on this kind of triangle rasterization: https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/ 
	template<typename _tCoord, typename _tColor>
	static					::cho::error_t									drawTriangle								(::cho::grid_view<_tColor>& bitmapTarget, const _tColor& value, const ::cho::STriangle2D<_tCoord>& triangle)		{
		::cho::SCoord2	<int32_t>													areaMin										= {(int32_t)::cho::min(::cho::min(triangle.A.x, triangle.B.x), triangle.C.x), (int32_t)::cho::min(::cho::min(triangle.A.y, triangle.B.y), triangle.C.y)};
		::cho::SCoord2	<int32_t>													areaMax										= {(int32_t)::cho::max(::cho::max(triangle.A.x, triangle.B.x), triangle.C.x), (int32_t)::cho::max(::cho::max(triangle.A.y, triangle.B.y), triangle.C.y)};
		const int32_t																xStop										= ::cho::min(areaMax.x, (int32_t)bitmapTarget.width());
		for(int32_t y = ::cho::max(areaMin.y, 0), yStop = ::cho::min(areaMax.y, (int32_t)bitmapTarget.height()); y < yStop; ++y)
		for(int32_t x = ::cho::max(areaMin.x, 0); x < xStop; ++x) {	
			const ::cho::SCoord2<int32_t>												cellCurrent									= {x, y};
			// Determine barycentric coordinates
			int																			w0											= ::cho::orient2d({triangle.A, triangle.B}, cellCurrent);
			int																			w1											= ::cho::orient2d({triangle.B, triangle.C}, cellCurrent);
			int																			w2											= ::cho::orient2d({triangle.C, triangle.A}, cellCurrent);
			if (w0 >= 0 && w1 >= 0 && w2 >= 0)  // If p is on or inside all edges, render pixel.
				bitmapTarget[y][x]											= value;
		}
		return 0;
	}

	// Bresenham's line algorithm
	template<typename _tCoord, typename _tColor>
	static					::cho::error_t									drawLine									(::cho::grid_view<_tColor>& bitmapTarget, const _tColor& value, const ::cho::SLine2D<_tCoord>& line)				{
		float																		x1											= (float)line.A.x
			,																		y1											= (float)line.A.y
			,																		x2											= (float)line.B.x
			,																		y2											= (float)line.B.y
			;
		const bool																	steep										= (::fabs(y2 - y1) > ::fabs(x2 - x1));
		if(steep){
			::std::swap(x1, y1);
			::std::swap(x2, y2);
		}
		if(x1 > x2) {
			::std::swap(x1, x2);
			::std::swap(y1, y2);
		}
		const float																	dx											= x2 - x1;
		const float																	dy											= ::fabs(y2 - y1);
		float																		error										= dx / 2.0f;
		const int32_t																ystep										= (y1 < y2) ? 1 : -1;
		int32_t																		y											= (int32_t)y1;
		if(steep) {
			for(int32_t x = (int32_t)x1, xStop = (int32_t)x2; x < xStop; ++x) {
				if(false == ::cho::in_range(x, 0, (int32_t)bitmapTarget.height()) || false == ::cho::in_range(y, 0, (int32_t)bitmapTarget.width()))
					continue;
				bitmapTarget[x][y]														= value;
				error																	-= dy;
				if(error < 0) {
					y																		+= ystep;
					error																	+= dx;
				}
			}
		}
		else {
			for(int32_t x = (int32_t)x1, xStop = (int32_t)x2; x < xStop; ++x) {
				if(false == ::cho::in_range(y, 0, (int32_t)bitmapTarget.height()) || false == ::cho::in_range(x, 0, (int32_t)bitmapTarget.width()))
					continue;
				bitmapTarget[y][x]														= value;
				error																	-= dy;
				if(error < 0) {
					y																		+= ystep;
					error																	+= dx;
				}
			}
		}
		return 0;
	}
} // namespace

#endif // BITMAP_TARGET_H_98237498023745654654
