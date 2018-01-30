#include "cho_grid_view.h"
#include "cho_color.h"
#include "cho_coord.h"
#include <memory> // this is required for ::std::swap()

#ifndef BITMAP_TARGET_H_98237498023745654654
#define BITMAP_TARGET_H_98237498023745654654

namespace cho
{
	struct SBitmapTargetBGRA	{ typedef SColorBGRA	TColor; ::cho::grid_view<TColor>	Colors; };
	
	template<typename _tCoord, typename _tTarget>
	static					::cho::error_t									drawRectangle								(_tTarget& bitmapTarget, const typename _tTarget::TColor& value, const ::cho::SRectangle2D<_tCoord>& rectangle)		{
		int32_t																		yStart										= (int32_t)::cho::max(0, (int32_t)rectangle.Offset.y);
		int32_t																		yStop										= ::cho::min((int32_t)rectangle.Offset.y + (int32_t)rectangle.Size.y, (int32_t)bitmapTarget.Colors.height());
		int32_t																		xStart										= (int32_t)::cho::max(0, (int32_t)rectangle.Offset.x);
		int32_t																		xStop										= ::cho::min((int32_t)rectangle.Offset.x + (int32_t)rectangle.Size.x, (int32_t)bitmapTarget.Colors.width());
		if(yStart >= yStop || xStart >= xStop)
			return 0;
		for(int32_t x = xStart; x < xStop; ++x) 	
			bitmapTarget.Colors		[yStart][x]											= value;
		for(int32_t y = yStart + 1; y < yStop; ++y)
			memcpy(&bitmapTarget.Colors[y][xStart], &bitmapTarget.Colors[yStart][xStart], sizeof(typename _tTarget::TColor) * xStop - xStart);
		return 0;
	}

	template<typename _tCoord, typename _tTarget>
	static					::cho::error_t									drawCircle									(_tTarget& bitmapTarget, const typename _tTarget::TColor& value, const ::cho::SCircle2D<_tCoord>& circle)			{
		int32_t																		xStop										= ::cho::min((int32_t)(circle.Center.x + circle.Radius), (int32_t)bitmapTarget.Colors.width	());
		double																		radiusSquared								= circle.Radius * circle.Radius;
		for(int32_t y = ::cho::max(0, (int32_t)(circle.Center.y - circle.Radius)), yStop = ::cho::min((int32_t)(circle.Center.y + circle.Radius), (int32_t)bitmapTarget.Colors.height	()); y < yStop; ++y)
		for(int32_t x = ::cho::max(0, (int32_t)(circle.Center.x - circle.Radius)); x < xStop; ++x) {	
			::cho::SCoord2<int32_t>														cellCurrent									= {x, y};
			double																		distanceSquared								= (cellCurrent - circle.Center).LengthSquared();
			if(distanceSquared < radiusSquared) {
			 	for(const int32_t xLimit = circle.Center.x + (circle.Center.x - x); x < xLimit; ++x)
					bitmapTarget.Colors		[y][x]											= value;
				break;
			}
		}
		return 0;
	}

	// A good article on this kind of triangle rasterization: https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/ 
	template<typename _tCoord, typename _tTarget>
	static					::cho::error_t									drawTriangle								(_tTarget& bitmapTarget, const typename _tTarget::TColor& value, const ::cho::STriangle2D<_tCoord>& triangle)		{
		::cho::SCoord2		<int32_t>												areaMin										= {(int32_t)::cho::min(::cho::min(triangle.A.x, triangle.B.x), triangle.C.x), (int32_t)::cho::min(::cho::min(triangle.A.y, triangle.B.y), triangle.C.y)};
		::cho::SCoord2		<int32_t>												areaMax										= {(int32_t)::cho::max(::cho::max(triangle.A.x, triangle.B.x), triangle.C.x), (int32_t)::cho::max(::cho::max(triangle.A.y, triangle.B.y), triangle.C.y)};
		const int32_t																xStop										= ::cho::min(areaMax.x, (int32_t)bitmapTarget.Colors.width());
		for(int32_t y = ::cho::max(areaMin.y, 0), yStop = ::cho::min(areaMax.y, (int32_t)bitmapTarget.Colors.height()); y < yStop; ++y)
		for(int32_t x = ::cho::max(areaMin.x, 0); x < xStop; ++x) {	
			const ::cho::SCoord2<int32_t>												cellCurrent									= {x, y};
			// Determine barycentric coordinates
			int																			w0											= ::cho::orient2d({triangle.A, triangle.B}, cellCurrent);
			int																			w1											= ::cho::orient2d({triangle.B, triangle.C}, cellCurrent);
			int																			w2											= ::cho::orient2d({triangle.C, triangle.A}, cellCurrent);
			if (w0 >= 0 && w1 >= 0 && w2 >= 0)  // If p is on or inside all edges, render pixel.
				bitmapTarget.Colors		[y][x]											= value;
		}
		return 0;
	}

	// Bresenham's line algorithm
	template<typename _tCoord, typename _tTarget>
	static					::cho::error_t									drawLine									(_tTarget& bitmapTarget, const typename _tTarget::TColor& value, const ::cho::SLine2D<_tCoord>& line)				{
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
				if(false == ::cho::in_range(x, 0, (int32_t)bitmapTarget.Colors.height()) || false == ::cho::in_range(y, 0, (int32_t)bitmapTarget.Colors.width()))
					continue;
				bitmapTarget.Colors		[x][y]											= value;
				error																	-= dy;
				if(error < 0) {
					y																		+= ystep;
					error																	+= dx;
				}
			}
		}
		else {
			for(int32_t x = (int32_t)x1, xStop = (int32_t)x2; x < xStop; ++x) {
				if(false == ::cho::in_range(y, 0, (int32_t)bitmapTarget.Colors.height()) || false == ::cho::in_range(x, 0, (int32_t)bitmapTarget.Colors.width()))
					continue;
				bitmapTarget.Colors		[y][x]											= value;
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
