#include "cho_coord.h"
#include "cho_log.h"

#ifndef CHO_COLLISION_H_209384
#define CHO_COLLISION_H_209384

namespace cho
{
// Calculate intersection of two lines. return 1 if found, 0 if not found or -1 on error
	template<typename _tCoord>
					::cho::error_t											line_line_intersect			
		( const ::cho::SLine2D<_tCoord>	& line1
		, const ::cho::SLine2D<_tCoord>	& line2
		, ::cho::SCoord2<_tCoord>		& out_intersect_point
		)
	{
		double																		detL1						= ::cho::determinant(line1);
		double																		detL2						= ::cho::determinant(line2);
	
		double																		x1mx2						= line1.A.x - line1.B.x;
		double																		y1my2						= line1.A.y - line1.B.y;
	
		double																		x3mx4						= line2.A.x - line2.B.x;
		double																		y3my4						= line2.A.y - line2.B.y;
	
		double																		xnom						= ::cho::determinant(detL1, x1mx2, detL2, x3mx4);
		double																		ynom						= ::cho::determinant(detL1, y1my2, detL2, y3my4);
		double																		denom						= ::cho::determinant(x1mx2, y1my2, x3mx4, y3my4);
		if(denom == 0.0) { // Lines don't seem to cross
			out_intersect_point.x																= NAN;
			out_intersect_point.y																= NAN;
			return 0;
		}
		out_intersect_point.x																= (_tCoord)(xnom / denom);	
		out_intersect_point.y																= (_tCoord)(ynom / denom);
		ree_if(!isfinite(out_intersect_point.x) 
			|| !isfinite(out_intersect_point.y)
			, "Probably a numerical issue.");
		return 1; //All OK
	}

	template<typename _tCoord>
					::cho::error_t											point_in_segment
		( const ::cho::SLine2D<_tCoord>	& segment
		, const ::cho::SCoord2<_tCoord>	& point
		)
	{
		return (	point.x >=	::cho::min(segment.A.x, segment.B.x)
				&&	point.y >=	::cho::min(segment.A.y, segment.B.y)
				&&	point.x <=	::cho::max(segment.A.x, segment.B.x)
				&&	point.y <=	::cho::max(segment.A.y, segment.B.y)
				) ? 1 :  0;
	}
	// Calculate intersection of two lines. return 1 if found, 0 if not found or -1 on error
	template<typename _tCoord>
					::cho::error_t											line_segment_intersect			
		( const ::cho::SLine2D<_tCoord>	& line
		, const ::cho::SLine2D<_tCoord>	& segment
		, ::cho::SCoord2<_tCoord>		& out_intersect_point
		)
	{
		::cho::error_t																collision					= line_line_intersect(line, segment, out_intersect_point);
		return (1 == collision) ? point_in_segment(segment, out_intersect_point) : collision;
	}

	// Calculate intersection of two lines. return 1 if found, 0 if not found or -1 on error
	template<typename _tCoord>
					::cho::error_t											segment_segment_intersect			
		( const ::cho::SLine2D<_tCoord>	& segment1
		, const ::cho::SLine2D<_tCoord>	& segment2
		, ::cho::SCoord2<_tCoord>		& out_intersect_point
		)
	{
		::cho::error_t																collision					= line_segment_intersect(segment1, segment2, out_intersect_point);
		return (1 == collision) ? point_in_segment(segment1, out_intersect_point) : collision;
	}


	template<typename _tCoord>
						::cho::error_t										buildAABBSegments		
		( const ::cho::SCoord2<_tCoord> & center
		, const _tCoord					halfSizeBox
		,		::cho::SLine2D<_tCoord>	& left
		,		::cho::SLine2D<_tCoord>	& top
		,		::cho::SLine2D<_tCoord>	& right
		,		::cho::SLine2D<_tCoord>	& bottom
		) 
	{
		left																	= {center + ::cho::SCoord2<_tCoord>{-halfSizeBox, halfSizeBox}, center + ::cho::SCoord2<_tCoord>{-halfSizeBox,-halfSizeBox}};
		top																		= {center + ::cho::SCoord2<_tCoord>{ halfSizeBox, halfSizeBox}, center + ::cho::SCoord2<_tCoord>{-halfSizeBox, halfSizeBox}};
		right																	= {center + ::cho::SCoord2<_tCoord>{ halfSizeBox, halfSizeBox}, center + ::cho::SCoord2<_tCoord>{ halfSizeBox,-halfSizeBox}};
		bottom																	= {center + ::cho::SCoord2<_tCoord>{ halfSizeBox,-halfSizeBox}, center + ::cho::SCoord2<_tCoord>{-halfSizeBox,-halfSizeBox}};
		return 0;
	}

	template<typename _tCoord>
						::cho::error_t										buildAABBSegments		
		( const ::cho::SRectangle2D<_tCoord>	& rect
		,		::cho::SLine2D<_tCoord>			& left
		,		::cho::SLine2D<_tCoord>			& top
		,		::cho::SLine2D<_tCoord>			& right
		,		::cho::SLine2D<_tCoord>			& bottom
		) 
	{
		left																	= {rect.Offset														, rect.Offset + ::cho::SCoord2<_tCoord>{0			, rect.Size.y}};
		top																		= {rect.Offset + ::cho::SCoord2<_tCoord>{0, rect.Size.y}			, rect.Offset + ::cho::SCoord2<_tCoord>{rect.Size.x	, rect.Size.y}};
		right																	= {rect.Offset + ::cho::SCoord2<_tCoord>{rect.Size.x, rect.Size.y}	, rect.Offset + ::cho::SCoord2<_tCoord>{rect.Size.x, 0}};
		bottom																	= {rect.Offset														, rect.Offset + ::cho::SCoord2<_tCoord>{rect.Size.x, 0}};
		return 0;
	}

}


#endif // CHO_COLLISION_H_209384
