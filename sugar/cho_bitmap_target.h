#include "cho_texture.h"
#include "cho_color.h"
#include "cho_coord.h"
#include <memory> // this is required for ::std::swap()

#ifndef BITMAP_TARGET_H_98237498023745654654
#define BITMAP_TARGET_H_98237498023745654654

namespace cho
{
	template<typename _tCoord, typename _tCell>
					::cho::error_t											drawPixelBrightness								(::cho::grid_view<_tCell> & viewOffscreen, const ::cho::SCoord2<_tCoord> & sourcePosition, const _tCell& colorLight, float factor, double range)								{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
		::cho::SCoord2<double>														maxRange										= {range, range};
		_tCoord																		rangeUnit										= _tCoord(1.0 / maxRange.Length());
		for(int32_t y = -(int32_t)range - 1, blendCount = 1 + (int32_t)range + 1; y < blendCount; ++y)	// the + 1 - 1 is because we actually process more surrounding pixels in order to compensate for the flooring of the coordinates 
		for(int32_t x = -(int32_t)range - 1; x < blendCount; ++x) {										// as it causes a visual effect of the light being cut to a rectangle and having sharp borders.
			if(x || y) {
				::cho::SCoord2<_tCoord>													blendPos										= sourcePosition + ::cho::SCoord2<_tCoord>{(_tCoord)x, (_tCoord)y};
				if( blendPos.x < (int32_t)viewOffscreen.width () && blendPos.x >= 0
				 && blendPos.y < (int32_t)viewOffscreen.height() && blendPos.y >= 0
				 ) {
					::cho::SCoord2<_tCoord>													brightDistance									= blendPos - sourcePosition;
					double																	brightDistanceLength							= brightDistance.Length();
					double																	distanceFactor									= brightDistanceLength * rangeUnit;
					if(distanceFactor <= 1.0)
						viewOffscreen[(uint32_t)blendPos.y][(uint32_t)blendPos.x]			= ::cho::interpolate_linear(viewOffscreen[(uint32_t)blendPos.y][(uint32_t)blendPos.x], colorLight, factor * (1.0f - distanceFactor));
				}
			}
		}
		return 0;
	}

	template<typename _tCoord, typename _tCell>
					::cho::error_t											drawPixelLight									(::cho::grid_view<_tCell> & viewOffscreen, const ::cho::SCoord2<_tCoord> & sourcePosition, const _tCell& colorLight, float maxFactor, double range)								{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
		if( ((uint32_t)sourcePosition.x) < viewOffscreen.width	()
		 && ((uint32_t)sourcePosition.y) < viewOffscreen.height	()
		 )
			viewOffscreen[(uint32_t)sourcePosition.y][(uint32_t)sourcePosition.x]	= colorLight;
		return drawPixelBrightness(viewOffscreen, sourcePosition, colorLight, maxFactor, range);
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

	// This implementation is incorrect. The problem is that it draws borders even if it shuoldn't. I never tested it but I believe that's what the code says.
	template<typename _tCoord, typename _tColor>
	static					::cho::error_t									drawRectangleBorder							(::cho::grid_view<_tColor>& bitmapTarget, const _tColor& value, const ::cho::SRectangle2D<_tCoord>& rectangle)		{
		int32_t																		yStart										= (int32_t)::cho::max(0, (int32_t)rectangle.Offset.y);
		int32_t																		yStop										= ::cho::min((int32_t)rectangle.Offset.y + (int32_t)rectangle.Size.y, (int32_t)bitmapTarget.height());
		int32_t																		xStart										= (int32_t)::cho::max(0, (int32_t)rectangle.Offset.x);
		int32_t																		xStop										= ::cho::min((int32_t)rectangle.Offset.x + (int32_t)rectangle.Size.x, (int32_t)bitmapTarget.width());
		if(yStart >= yStop || xStart >= xStop)
			return 0;
		for(int32_t x = xStart; x < xStop; ++x) 	
			bitmapTarget[yStart][x]													= value;
		memcpy(&bitmapTarget[yStop - 1][xStart], &bitmapTarget[yStart][xStart], sizeof(_tColor) * xStop - xStart);
		for(int32_t y = yStart + 1, yMax = (yStop - 1); y < yMax; ++y) {
			bitmapTarget[y][0]														= value;
			bitmapTarget[y][xStop - 1]												= value;
		}
		return (yStop - yStart) * 2 + (xStop - xStart) * 2;
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
		int32_t																		pixelsDrawn									= 0;
		for(int32_t y = ::cho::max(0, (int32_t)(circle.Center.y - circle.Radius)), yStop = ::cho::min((int32_t)(circle.Center.y + circle.Radius), (int32_t)bitmapTarget.height()); y < yStop; ++y)
		for(int32_t x = ::cho::max(0, (int32_t)(circle.Center.x - circle.Radius)); x < xStop; ++x) {	
			::cho::SCoord2<int32_t>														cellCurrent									= {x, y};
			double																		distanceSquared								= (cellCurrent - circle.Center).LengthSquared();
			if(distanceSquared < radiusSquared) {
			 	for(const int32_t xLimit = ::cho::min((int32_t)circle.Center.x + ((int32_t)circle.Center.x - x), (int32_t)bitmapTarget.width()); x < xLimit; ++x) {
					bitmapTarget[y][x]														= value;
					++pixelsDrawn;
				}
				break;
			}
		}
		return pixelsDrawn;
	}

	template<typename _tCoord, typename _tColor>
	static					::cho::error_t									drawCircle									(const ::cho::SCoord2<uint32_t>& targetMetrics, const ::cho::SCircle2D<_tCoord>& circle, ::cho::array_pod<::cho::SCoord2<int32_t>>& out_Points)			{
		int32_t																		xStop										= ::cho::min((int32_t)(circle.Center.x + circle.Radius), (int32_t)targetMetrics.x);
		double																		radiusSquared								= circle.Radius * circle.Radius;
		int32_t																		pixelsDrawn									= 0;
		for(int32_t y = ::cho::max(0, (int32_t)(circle.Center.y - circle.Radius)), yStop = ::cho::min((int32_t)(circle.Center.y + circle.Radius), (int32_t)targetMetrics.y); y < yStop; ++y)
		for(int32_t x = ::cho::max(0, (int32_t)(circle.Center.x - circle.Radius)); x < xStop; ++x) {	
			::cho::SCoord2<int32_t>														cellCurrent									= {x, y};
			double																		distanceSquared								= (cellCurrent - circle.Center).LengthSquared();
			if(distanceSquared < radiusSquared) {
			 	for(const int32_t xLimit = ::cho::min((int32_t)circle.Center.x + ((int32_t)circle.Center.x - x), (int32_t)targetMetrics.x); x < xLimit; ++x) {
					out_Points.push_back({x, y});
					++pixelsDrawn;
				}
				break;
			}
		}
		return pixelsDrawn;
	}

	// A good article on this kind of triangle rasterization: https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/ 
	template<typename _tCoord, typename _tColor>
	static					::cho::error_t									drawTriangle								(::cho::grid_view<_tColor>& bitmapTarget, const _tColor& value, const ::cho::STriangle2D<_tCoord>& triangle)										{
		::cho::SCoord2	<int32_t>													areaMin										= {(int32_t)::cho::min(::cho::min(triangle.A.x, triangle.B.x), triangle.C.x), (int32_t)::cho::min(::cho::min(triangle.A.y, triangle.B.y), triangle.C.y)};
		::cho::SCoord2	<int32_t>													areaMax										= {(int32_t)::cho::max(::cho::max(triangle.A.x, triangle.B.x), triangle.C.x), (int32_t)::cho::max(::cho::max(triangle.A.y, triangle.B.y), triangle.C.y)};
		const int32_t																xStop										= ::cho::min(areaMax.x, (int32_t)bitmapTarget.width());
		int32_t																		pixelsDrawn									= 0;
		for(int32_t y = ::cho::max(areaMin.y, 0), yStop = ::cho::min(areaMax.y, (int32_t)bitmapTarget.height()); y < yStop; ++y)
		for(int32_t x = ::cho::max(areaMin.x, 0); x < xStop; ++x) {	
			const ::cho::SCoord2<int32_t>												cellCurrent									= {x, y};
			// Determine barycentric coordinates
			int																			w0											= ::cho::orient2d({triangle.B, triangle.A}, cellCurrent);	// ::cho::orient2d({triangle.A, triangle.B}, cellCurrent);
			if(w0 < 0)
				continue;
			int																			w1											= ::cho::orient2d({triangle.C, triangle.B}, cellCurrent);	// ::cho::orient2d({triangle.B, triangle.C}, cellCurrent);
			if(w1 < 0)
				continue;
			int																			w2											= ::cho::orient2d({triangle.A, triangle.C}, cellCurrent);	// ::cho::orient2d({triangle.C, triangle.A}, cellCurrent);
			if(w2 < 0)
				continue;
			//if (w0 >= 0 && w1 >= 0 && w2 >= 0) { // If p is on or inside all edges, render pixel.
				bitmapTarget[y][x]														= value;
				++pixelsDrawn;
			//}
		}
		return pixelsDrawn;
	}

	template<typename _tCoord>
	static					::cho::error_t									drawTriangle								(const ::cho::SCoord2<uint32_t>& targetMetrics, const ::cho::STriangle2D<_tCoord>& triangle, ::cho::array_pod<::cho::SCoord2<int32_t>>& out_Points)		{
		::cho::SCoord2	<int32_t>													areaMin										= {(int32_t)::cho::min(::cho::min(triangle.A.x, triangle.B.x), triangle.C.x), (int32_t)::cho::min(::cho::min(triangle.A.y, triangle.B.y), triangle.C.y)};
		::cho::SCoord2	<int32_t>													areaMax										= {(int32_t)::cho::max(::cho::max(triangle.A.x, triangle.B.x), triangle.C.x), (int32_t)::cho::max(::cho::max(triangle.A.y, triangle.B.y), triangle.C.y)};
		const int32_t																xStop										= ::cho::min(areaMax.x, (int32_t)targetMetrics.x);
		int32_t																		pixelsDrawn									= 0;
		for(int32_t y = ::cho::max(areaMin.y, 0), yStop = ::cho::min(areaMax.y, (int32_t)targetMetrics.y); y < yStop; ++y)
		for(int32_t x = ::cho::max(areaMin.x, 0); x < xStop; ++x) {	
			const ::cho::SCoord2<int32_t>												cellCurrent									= {x, y};
			// Determine barycentric coordinates
			int																			w0											= ::cho::orient2d({triangle.B, triangle.A}, cellCurrent);	// ::cho::orient2d({triangle.A, triangle.B}, cellCurrent);
			if(w0 < 0)
				continue;
			int																			w1											= ::cho::orient2d({triangle.C, triangle.B}, cellCurrent);	// ::cho::orient2d({triangle.B, triangle.C}, cellCurrent);
			if(w1 < 0)
				continue;
			int																			w2											= ::cho::orient2d({triangle.A, triangle.C}, cellCurrent);	// ::cho::orient2d({triangle.C, triangle.A}, cellCurrent);
			if(w2 < 0)
				continue;
			//if (w0 >= 0 && w1 >= 0 && w2 >= 0) { // If p is on or inside all edges, render pixel.
			out_Points.push_back({x, y});
			++pixelsDrawn;
			//}
		}
		return pixelsDrawn;
	}

	typedef		::cho::error_t												(*cho_raster_callback)						(void* bitmapTarget, const ::cho::SCoord2<uint32_t>& bitmapMetrics, const ::cho::SCoord2<uint32_t>& cellPos, const void* value);

	// Bresenham's line algorithm
	template<typename _tCoord, typename _tColor>
	static					::cho::error_t									rasterLine									(::cho::grid_view<_tColor>& bitmapTarget, const _tColor& value, const ::cho::SLine2D<_tCoord>& line, cho_raster_callback callback)				{
		::cho::SCoord2<float>														A											= line.A.Cast<float>();
		::cho::SCoord2<float>														B											= line.B.Cast<float>();
		const bool																	steep										= (::fabs(B.y - A.y) > ::fabs(B.x - A.x));
		if(steep){
			::std::swap(A.x, A.y);
			::std::swap(B.x, B.y);
		}
		if(A.x > B.x) {
			::std::swap(A.x, B.x);
			::std::swap(A.y, B.y);
		}
		const ::cho::SCoord2<float>													d											= {B.x - A.x, ::fabs(B.y - A.y)};
		float																		error										= d.x / 2.0f;
		const int32_t																ystep										= (A.y < B.y) ? 1 : -1;
		int32_t																		y											= (int32_t)A.y;
		int32_t																		pixelsDrawn									= 0;
		if(steep) {
			for(int32_t x = (int32_t)A.x, xStop = (int32_t)B.x; x < xStop; ++x) {
				if(::cho::in_range(x, 0, (int32_t)bitmapTarget.height()) && ::cho::in_range(y, 0, (int32_t)bitmapTarget.width())) {
					callback(bitmapTarget.begin(), bitmapTarget.metrics(), {(uint32_t)x, (uint32_t)y}, &value);
					++pixelsDrawn;
				}
				error																	-= d.y;
				if(error < 0) {
					y																		+= ystep;
					error																	+= d.x;
				}
			}
		}
		else {
			for(int32_t x = (int32_t)A.x, xStop = (int32_t)B.x; x < xStop; ++x) {
				if(::cho::in_range(y, 0, (int32_t)bitmapTarget.height()) && ::cho::in_range(x, 0, (int32_t)bitmapTarget.width())) {
					callback(bitmapTarget.begin(), bitmapTarget.metrics(), {(uint32_t)x, (uint32_t)y}, &value);
					++pixelsDrawn;
				}
				error																	-= d.y;
				if(error < 0) {
					y																		+= ystep;
					error																	+= d.x;
				}
			}
		}
		return pixelsDrawn;
	}

	// Bresenham's line algorithm
	template<typename _tCoord, typename _tColor>
	static					::cho::error_t									drawLine									(::cho::grid_view<_tColor>& bitmapTarget, const _tColor& value, const ::cho::SLine2D<_tCoord>& line)				{
		::cho::SCoord2<float>														A											= line.A.Cast<float>();
		::cho::SCoord2<float>														B											= line.B.Cast<float>();
		const bool																	steep										= (::fabs(B.y - A.y) > ::fabs(B.x - A.x));
		if(steep){
			::std::swap(A.x, A.y);
			::std::swap(B.x, B.y);
		}
		if(A.x > B.x) {
			::std::swap(A.x, B.x);
			::std::swap(A.y, B.y);
		}
		const ::cho::SCoord2<float>													d											= {B.x - A.x, ::fabs(B.y - A.y)};
		float																		error										= d.x / 2.0f;
		const int32_t																ystep										= (A.y < B.y) ? 1 : -1;
		int32_t																		y											= (int32_t)A.y;
		int32_t																		pixelsDrawn									= 0;
		if(steep) {
			for(int32_t x = (int32_t)A.x, xStop = (int32_t)B.x; x < xStop; ++x) {
				if(::cho::in_range(x, 0, (int32_t)bitmapTarget.height()) && ::cho::in_range(y, 0, (int32_t)bitmapTarget.width())) {
					bitmapTarget[x][y]														= value;
					++pixelsDrawn;
				}
				error																	-= d.y;
				if(error < 0) {
					y																		+= ystep;
					error																	+= d.x;
				}
			}
		}
		else {
			for(int32_t x = (int32_t)A.x, xStop = (int32_t)B.x; x < xStop; ++x) {
				if(::cho::in_range(y, 0, (int32_t)bitmapTarget.height()) && ::cho::in_range(x, 0, (int32_t)bitmapTarget.width())) {
					bitmapTarget[y][x]														= value;
					++pixelsDrawn;
				}
				error																	-= d.y;
				if(error < 0) {
					y																		+= ystep;
					error																	+= d.x;
				}
			}
		}
		return pixelsDrawn;
	}

	// Bresenham's line algorithm
	template<typename _tCoord>
	static					::cho::error_t									drawLine									(const ::cho::SCoord2<uint32_t>& targetMetrics, const ::cho::SLine2D<_tCoord>& line, ::cho::array_pod<::cho::SCoord2<int32_t>>& out_Points)				{
		::cho::SCoord2<float>														A											= line.A.Cast<float>();
		::cho::SCoord2<float>														B											= line.B.Cast<float>();
		const bool																	steep										= (::fabs(B.y - A.y) > ::fabs(B.x - A.x));
		if(steep){
			::std::swap(A.x, A.y);
			::std::swap(B.x, B.y);
		}
		if(A.x > B.x) {
			::std::swap(A.x, B.x);
			::std::swap(A.y, B.y);
		}
		const ::cho::SCoord2<float>													d											= {B.x - A.x, ::fabs(B.y - A.y)};
		float																		error										= d.x / 2.0f;
		const int32_t																ystep										= (A.y < B.y) ? 1 : -1;
		int32_t																		y											= (int32_t)A.y;
		int32_t																		pixelsDrawn									= 0;
		if(steep) {
			for(int32_t x = (int32_t)A.x, xStop = (int32_t)B.x; x < xStop; ++x) {
				if(::cho::in_range(x, 0, (int32_t)targetMetrics.y) && ::cho::in_range(y, 0, (int32_t)targetMetrics.x)) {
					out_Points.push_back({y, x});
					++pixelsDrawn;
				}
				error																	-= d.y;
				if(error < 0) {
					y																		+= ystep;
					error																	+= d.x;
				}
			}
		}
		else {
			for(int32_t x = (int32_t)A.x, xStop = (int32_t)B.x; x < xStop; ++x) {
				if(::cho::in_range(y, 0, (int32_t)targetMetrics.y) && ::cho::in_range(x, 0, (int32_t)targetMetrics.x)) {
					out_Points.push_back({x, y});
					++pixelsDrawn;
				}
				error																	-= d.y;
				if(error < 0) {
					y																		+= ystep;
					error																	+= d.x;
				}
			}
		}
		return pixelsDrawn;
	}

} // namespace

#endif // BITMAP_TARGET_H_98237498023745654654
