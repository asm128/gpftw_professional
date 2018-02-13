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
		, ::cho::SCoord2<_tCoord>		& out_point
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
			out_point.x																= NAN;
			out_point.y																= NAN;
			return 0;
		}
		out_point.x																= (_tCoord)(xnom / denom);	
		out_point.y																= (_tCoord)(ynom / denom);
		ree_if(!isfinite(out_point.x) 
			|| !isfinite(out_point.y)
			, "Probably a numerical issue");
		return 1; //All OK
	}

	// Calculate intersection of two lines. return 1 if found, 0 if not found or -1 on error
	template<typename _tCoord>
					::cho::error_t											line_segment_intersect			
		( const ::cho::SLine2D<_tCoord>	& line
		, const ::cho::SLine2D<_tCoord>	& segment
		, ::cho::SCoord2<_tCoord>		& out_point
		)
	{
		::cho::error_t																collision					= line_line_intersect(line, segment, out_point);
		if(1 == collision) {
			collision 
				= ( out_point.x >=	::cho::min(segment.A.x, segment.B.x)
				&&	out_point.y >=	::cho::min(segment.A.y, segment.B.y)
				&&	out_point.x <=	::cho::max(segment.A.x, segment.B.x)
				&&	out_point.y <=	::cho::max(segment.A.y, segment.B.y)
				) ? 1 : 0;
		}
		return collision;
	}

	// Calculate intersection of two lines. return 1 if found, 0 if not found or -1 on error
	template<typename _tCoord>
					::cho::error_t											segment_segment_intersect			
		( const ::cho::SLine2D<_tCoord>	& segment1
		, const ::cho::SLine2D<_tCoord>	& segment2
		, ::cho::SCoord2<_tCoord>		& out_point
		)
	{
		::cho::error_t																collision					= line_segment_intersect(segment1, segment2, out_point);
		if(1 == collision) {
			collision 
				= ( out_point.x >=	::cho::min(segment1.A.x, segment1.B.x)
				&&	out_point.y >=	::cho::min(segment1.A.y, segment1.B.y)
				&&	out_point.x <=	::cho::max(segment1.A.x, segment1.B.x)
				&&	out_point.y <=	::cho::max(segment1.A.y, segment1.B.y)
				) ? 1 : 0;
		}
		return collision;
	}
}


#endif // CHO_COLLISION_H_209384
