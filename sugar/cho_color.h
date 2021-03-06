/// Copyright 2009-2017 - asm128
#include "cho_math.h"

#ifndef CHO_COLOR_H_29734982734
#define CHO_COLOR_H_29734982734

namespace cho
{
#pragma pack( push, 1 )
	// Stores RGBA color channels
	struct SColorRGBA {
									uint8_t			r = 0, g = 0, b = 0, a = 0xff; 

		constexpr									SColorRGBA		()																noexcept	= default;
		constexpr									SColorRGBA		(const SColorRGBA& other)										noexcept	= default;
		constexpr									SColorRGBA		(uint8_t r_, uint8_t g_, uint8_t b_, uint8_t a_=0xff)			noexcept	: b(b_), g(g_), r(r_), a(a_)																																										{}
		constexpr									SColorRGBA		(uint32_t other)												noexcept	: r((uint8_t)(((other & 0x000000FF) >> 0))), g((uint8_t)(((other & 0x0000FF00) >> 8))), b((uint8_t)(((other & 0x00FF0000) >> 16))), a((uint8_t)(((other & 0xFF000000) >> 24)))						{}

		constexpr					operator		uint32_t		()														const	noexcept	{ return (((uint32_t)a) << 24) | (((uint32_t)b) << 16) | (((uint32_t)g) << 8) | (((uint32_t)r) << 0);																								}
									SColorRGBA&		operator=		(const SColorRGBA& color)										noexcept	= default;
		constexpr					bool			operator ==		(uint32_t other)										const	noexcept	{ return other == *((const uint32_t*)this);																																							}
		constexpr					bool			operator ==		(const SColorRGBA& other)								const	noexcept	{ return b == other.b && g == other.g && r == other.r && a == other.a;																																}
		constexpr					bool			operator !=		(const SColorRGBA& other)								const	noexcept	{ return b != other.b || g != other.g || r != other.r || a != other.a;																																}
		constexpr					SColorRGBA		operator *		(const SColorRGBA& color)								const	noexcept	{ return SColorRGBA((uint8_t)::cho::clamp(r * (uint16_t)color.r, 0, 255)	, (uint8_t)::cho::clamp(g * (uint16_t)color.g, 0, 255)	, (uint8_t)::cho::clamp(b * (uint16_t)color.b, 0, 255)	, a);	}
		constexpr					SColorRGBA		operator +		(const SColorRGBA& color)								const	noexcept	{ return SColorRGBA((uint8_t)::cho::clamp(r + (uint16_t)color.r, 0, 255)	, (uint8_t)::cho::clamp(g + (uint16_t)color.g, 0, 255)	, (uint8_t)::cho::clamp(b + (uint16_t)color.b, 0, 255)	, a);	}
		constexpr					SColorRGBA		operator *		(double scalar)											const	noexcept	{ return SColorRGBA((uint8_t)::cho::clamp(r * scalar, 0.0 , 255.0)			, (uint8_t)::cho::clamp(g * scalar, 0.0 , 255.0 )		, (uint8_t)::cho::clamp(b * scalar, 0.0,  255.0 )		, a);	}
		constexpr					SColorRGBA		operator /		(double scalar)											const				{ return SColorRGBA((uint8_t)::cho::clamp(r / scalar, 0.0 , 255.0)			, (uint8_t)::cho::clamp(g / scalar, 0.0 , 255.0 )		, (uint8_t)::cho::clamp(b / scalar, 0.0,  255.0 )		, a);	}
	};	// struct

	// Stores BGRA color channels
	struct SColorBGRA {
									uint8_t			b = 0, g = 0, r = 0, a = 0xff; 

		constexpr									SColorBGRA		()																noexcept	= default;
		constexpr									SColorBGRA		(const SColorRGBA& other)										noexcept	: b(other.b), g(other.g), r(other.r), a(other.a)																																										{}
		constexpr									SColorBGRA		(const SColorBGRA& other)										noexcept	= default;
		constexpr									SColorBGRA		(uint8_t b_, uint8_t g_, uint8_t r_, uint8_t a_=0xff)			noexcept	: b(b_), g(g_), r(r_), a(a_)																																										{}
		constexpr									SColorBGRA		(uint32_t other)												noexcept	: b((uint8_t)(((other & 0x000000FF) >> 0))), g((uint8_t)(((other & 0x0000FF00) >> 8))), r((uint8_t)(((other & 0x00FF0000) >> 16))), a((uint8_t)(((other & 0xFF000000) >> 24)))						{}

		constexpr					operator		SColorRGBA		()														const	noexcept	{ return {r, g, b, a};																																												}
		constexpr					operator		uint32_t		()														const	noexcept	{ return (((uint32_t)a) << 24) | (((uint32_t)r) << 16) | (((uint32_t)g) << 8) | (((uint32_t)b) << 0);																								}
									SColorBGRA&		operator=		(const SColorBGRA& color)										noexcept	= default;
		constexpr					bool			operator ==		(uint32_t other)										const	noexcept	{ return other == *((const uint32_t*)this);																																							}
		constexpr					bool			operator ==		(const SColorBGRA& other)								const	noexcept	{ return b == other.b && g == other.g && r == other.r && a == other.a;																																}
		constexpr					bool			operator !=		(const SColorBGRA& other)								const	noexcept	{ return b != other.b || g != other.g || r != other.r || a != other.a;																																}
		constexpr					SColorBGRA		operator *		(const SColorBGRA& color)								const	noexcept	{ return SColorBGRA((uint8_t)::cho::min(b * (uint16_t)color.b, 255)	, (uint8_t)::cho::min(g * (uint16_t)color.g, 255)	,(uint8_t)::cho::min(r * (uint16_t)color.r, 255), a);	}
		constexpr					SColorBGRA		operator +		(const SColorBGRA& color)								const	noexcept	{ return SColorBGRA((uint8_t)::cho::min(b + (uint16_t)color.b, 255)	, (uint8_t)::cho::min(g + (uint16_t)color.g, 255)	,(uint8_t)::cho::min(r + (uint16_t)color.r, 255), a);	}
		constexpr					SColorBGRA		operator *		(float scalar)											const	noexcept	{ return SColorBGRA((uint8_t)::cho::clamp(b * scalar, 0.0f, 255.0f)	, (uint8_t)::cho::clamp(g * scalar, 0.0f, 255.0f)	,(uint8_t)::cho::clamp(r * scalar, 0.0f, 255.0f), a);	}
		constexpr					SColorBGRA		operator /		(float scalar)											const				{ return SColorBGRA((uint8_t)::cho::clamp(b / scalar, 0.0f, 255.0f)	, (uint8_t)::cho::clamp(g / scalar, 0.0f, 255.0f)	,(uint8_t)::cho::clamp(r / scalar, 0.0f, 255.0f), a);	}
		constexpr					SColorBGRA		operator *		(double scalar)											const	noexcept	{ return SColorBGRA((uint8_t)::cho::clamp(b * scalar, 0.0,  255.0 )	, (uint8_t)::cho::clamp(g * scalar, 0.0 , 255.0 )	,(uint8_t)::cho::clamp(r * scalar, 0.0, 255.0),	a);		}
		constexpr					SColorBGRA		operator /		(double scalar)											const				{ return SColorBGRA((uint8_t)::cho::clamp(b / scalar, 0.0,  255.0 )	, (uint8_t)::cho::clamp(g / scalar, 0.0 , 255.0 )	,(uint8_t)::cho::clamp(r / scalar, 0.0, 255.0),	a);		}
									SColorBGRA&		operator *=		(float scalar)													noexcept	{ b = (uint8_t)::cho::clamp(b * scalar, 0.0f, 255.0f); g = (uint8_t)::cho::clamp(g * scalar, 0.0f, 255.0f); r = (uint8_t)::cho::clamp(r * scalar, 0.0f, 255.0f);	return *this; }
									SColorBGRA&		operator /=		(float scalar)																{ b = (uint8_t)::cho::clamp(b / scalar, 0.0f, 255.0f); g = (uint8_t)::cho::clamp(g / scalar, 0.0f, 255.0f); r = (uint8_t)::cho::clamp(r / scalar, 0.0f, 255.0f);	return *this; }
									SColorBGRA&		operator *=		(double scalar)													noexcept	{ b = (uint8_t)::cho::clamp(b * scalar, 0.0,  255.0 ); g = (uint8_t)::cho::clamp(g * scalar, 0.0 , 255.0 ); r = (uint8_t)::cho::clamp(r * scalar, 0.0, 255.0);		return *this; }
									SColorBGRA&		operator /=		(double scalar)																{ b = (uint8_t)::cho::clamp(b / scalar, 0.0,  255.0 ); g = (uint8_t)::cho::clamp(g / scalar, 0.0 , 255.0 ); r = (uint8_t)::cho::clamp(r / scalar, 0.0, 255.0);		return *this; }
	};	// struct

	typedef						uint16_t		SColor16;

	// Stores BGR color channels
	struct SColorBGR {
									uint8_t							b = 0, g = 0, r = 0; 

		constexpr					SColorBGR						()																noexcept	= default;
		constexpr					SColorBGR						(const SColorBGR& otherColorInt)								noexcept	= default;
		constexpr					SColorBGR						(uint8_t b_, uint8_t g_, uint8_t r_)							noexcept	: b(b_), g(g_), r(r_)																																													{}
		constexpr					SColorBGR						(const SColorBGRA& other)										noexcept	: b(other.b), g(other.g), r(other.r)																																									{}
		constexpr					SColorBGR						(const SColor16& other)											noexcept	: b((uint8_t)((other & 0x001F) / ((float)(0x1F))*255))	, g((uint8_t)(((other & 0x07E0) >>5) / ((float)(0x3F))*255)), r((uint8_t)(((other & 0xF800) >>11) / ((float)(0x1F))*255))						{}
		constexpr					SColorBGR						(uint32_t other)												noexcept	: b((uint8_t)(((other & 0x000000FF) >> 0)))				, g((uint8_t)((other & 0x0000FF00) >> 8))					, r((uint8_t)(((other & 0x00FF0000) >> 16)))										{}

		constexpr					operator		uint32_t		()														const	noexcept	{ return			0xFF000000 | (((uint32_t)r) << 16)	| (((uint32_t)g) << 8)							| (((uint32_t)b) << 0);																			}
		constexpr					operator		SColorBGRA		()														const	noexcept	{ return SColorBGRA(	0xFF000000 | (((uint32_t)r) << 16)	| (((uint32_t)g) << 8)							| (((uint32_t)b) << 0));																	}
		constexpr					operator		SColor16		()														const	noexcept	{ return (((uint16_t)(b * (1/255.0f) * 0x001F)) << 0)	| (((uint16_t)(g * (1/255.0f) * 0x003F)) << 5)	| (((uint16_t)(r * (1/255.0f) * 0x001F)) << 11 );												}
									SColorBGR&		operator=		(const SColorBGR& color)										noexcept	= default;
		constexpr					bool			operator ==		(const SColorBGRA& other)								const	noexcept	{ return r == other.r && g == other.g && b == other.b;																																					}
		constexpr					bool			operator ==		(const SColorBGR& other)								const	noexcept	{ return r == other.r && g == other.g && b == other.b;																																					}
		constexpr					bool			operator ==		(const SColor16& other)									const	noexcept	{ return operator == (SColorBGR(other));																																								}
		constexpr					bool			operator !=		(const SColorBGRA& other)								const	noexcept	{ return r != other.r || g != other.g || b != other.b;																																					}
		constexpr					bool			operator !=		(const SColorBGR& other)								const	noexcept	{ return r != other.r || g != other.g || b != other.b;																																					}
		constexpr					bool			operator !=		(const SColor16& other)									const	noexcept	{ return operator != (SColorBGR(other));																																								}
		constexpr					SColorBGR		operator *		(float scalar)											const	noexcept	{ return ::cho::SColorBGR((uint8_t)::cho::clamp(r * scalar, 0.0f, 255.0f),	(uint8_t)::cho::clamp(g * scalar, 0.0f, 255.0f),	(uint8_t)::cho::clamp(b * scalar, 0.0f, 255.0f));						}
		constexpr					SColorBGR		operator /		(float scalar)											const				{ return ::cho::SColorBGR((uint8_t)::cho::clamp(r / scalar, 0.0f, 255.0f),	(uint8_t)::cho::clamp(g / scalar, 0.0f, 255.0f),	(uint8_t)::cho::clamp(b / scalar, 0.0f, 255.0f));						}
		constexpr					SColorBGR		operator *		(double scalar)											const	noexcept	{ return ::cho::SColorBGR((uint8_t)::cho::clamp(r * scalar, 0.0,  255.0), 	(uint8_t)::cho::clamp(g * scalar, 0.0,  255.0),	(uint8_t)::cho::clamp(b * scalar, 0.0,  255.0));							}
		constexpr					SColorBGR		operator /		(double scalar)											const				{ return ::cho::SColorBGR((uint8_t)::cho::clamp(r / scalar, 0.0,  255.0), 	(uint8_t)::cho::clamp(g / scalar, 0.0,  255.0),	(uint8_t)::cho::clamp(b / scalar, 0.0,  255.0));							}
		constexpr					SColorBGR		operator *		(const SColorBGR& color)								const	noexcept	{ return ::cho::SColorBGR((uint8_t)::cho::clamp(r * (uint16_t)color.r, 0, 255), (uint8_t)::cho::clamp(g * (uint16_t)color.g, 0, 255), (uint8_t)::cho::clamp(b * (uint16_t)color.b, 0, 255));			}
		constexpr					SColorBGR		operator +		(const SColorBGR& color)								const	noexcept	{ return ::cho::SColorBGR((uint8_t)::cho::clamp(r + (uint16_t)color.r, 0, 255), (uint8_t)::cho::clamp(g + (uint16_t)color.g, 0, 255), (uint8_t)::cho::clamp(b + (uint16_t)color.b, 0, 255));			}
		constexpr					SColorBGRA		operator *		(const SColorBGRA& color)								const	noexcept	{ return ::cho::SColorBGRA((uint8_t)::cho::clamp(r * (uint16_t)color.r, 0, 255), (uint8_t)::cho::clamp(g * (uint16_t)color.g, 0, 255), (uint8_t)::cho::clamp(b * (uint16_t)color.b, 0, 255), color.a);	}
		constexpr					SColorBGRA		operator +		(const SColorBGRA& color)								const	noexcept	{ return ::cho::SColorBGRA((uint8_t)::cho::clamp(r + (uint16_t)color.r, 0, 255), (uint8_t)::cho::clamp(g + (uint16_t)color.g, 0, 255), (uint8_t)::cho::clamp(b + (uint16_t)color.b, 0, 255), color.a);	}
	};	// struct

	// Stores RGBA floating point color channels
	struct SColorFloat {	
									float			r = 0, g = 0, b = 0, a = 0;		// store the color values as floating point ranged in the values (0,1)

		constexpr									SColorFloat		()																noexcept	= default;
		constexpr									SColorFloat		(const SColorFloat& color)										noexcept	= default;
		constexpr									SColorFloat		(float _r, float _g, float _b, float _a=1.0f)					noexcept	: r(_r) ,g(_g) ,b(_b) ,a(_a)																																										{}
		constexpr									SColorFloat		(uint16_t Color)												noexcept	: SColorFloat(SColorBGR(Color))																																										{}
		constexpr									SColorFloat		(uint32_t Color)												noexcept	: r(((Color & 0x00FF0000L)>>16)	*(1/255.0f)), g(((Color & 0x0000FF00L)>>8)*(1/255.0f)), b(((Color & 0x000000FFL)>>0)*(1/255.0f)), a(((Color & 0xFF000000L)>>24)*(1/255.0f))							{}
		constexpr									SColorFloat		(SColorBGRA Color)												noexcept	: r(Color.r * (1/255.0f)), g(Color.g * (1/255.0f)), b(Color.b * (1/255.0f)), a(Color.a * (1/255.0f))																								{}
		constexpr									SColorFloat		(SColorBGR Color)												noexcept	: r(Color.r	* (1/255.0f)), g(Color.g * (1/255.0f)), b(Color.b * (1/255.0f)), a(1.0f)																												{}
		constexpr									SColorFloat		(const float* rgbaColor)													: r(rgbaColor[0]), g(rgbaColor[1]), b(rgbaColor[2]), a(rgbaColor[3])																																{}		

									SColorFloat&	operator=		(const SColorFloat& color)													= default;
		constexpr					operator		uint32_t		()														const	noexcept	{ return (((uint32_t)(a * 255.0f)) << 24) | (((uint32_t)(r * 255.0f)) << 16) | (((uint32_t)(g * 255.0f)) << 8) | ((uint32_t)(b*255.0f));															}

		// For some reason this cast returns a different result if the error_printf() line is enabled. This is noticed in release builds where this operator turns a literal RED into YELLOW. 
		// At first I thought that this could be floating point precision optimizations causing the green channel to underflow. However, 
		// it is extremely unlikely that literal zeros in (g) turn into -0.00392f or less due to such optimizations. 
		// What comes to my mind is that optimizations could be messing up somehow and folding the code incorrectly.
		// Removing constexpr doesn't seem to affect this behavior, but it's still possible that optimizations are still resolving this in compile time because of its simplicity, 
		// so the problem certainly affects the compile time and it's also possible that it doesn't happen in runtime as debug builds perform the operation just fine.
		constexpr					operator		SColorBGRA		()														const	noexcept	{ 
			//error_printf("Color Cast:{r:%f, g:%f, b:%f, a:%f}", (float)r, (float)g, (float)b, (float)a);
			return SColorBGRA((uint8_t)(b * 255), (uint8_t)(g * 255), (uint8_t)(r * 255), (uint8_t)(a * 255));																								
		}
		constexpr					operator		SColorBGR		()														const	noexcept	{ return SColorBGR ((uint8_t)(b * 255), (uint8_t)(g * 255), (uint8_t)(r * 255));																													}
		constexpr					operator		SColor16		()														const	noexcept	{ return ((((uint16_t)(r * 0x1F)) << 11) | (((uint16_t)(g * 0x3F)) << 5) | (((uint16_t)(b * 0x1F)) << 0));																							}
		constexpr					operator		const float*	()														const	noexcept	{ return &r;																																														}
		constexpr					bool			operator ==		(const SColorFloat& color)								const	noexcept	{ return ((SColorBGR)color) == ((SColorBGR)*this);																																					}
		constexpr					bool			operator !=		(const SColorFloat& color)								const	noexcept	{ return ((SColorBGR)color) != ((SColorBGR)*this);																																					}
									SColorFloat&	operator +=		(const SColorFloat& color)										noexcept	{ r = r + color.r;					g = g + color.g;				b = b + color.b;				return Clamp();																					}
									SColorFloat&	operator *=		(const SColorFloat& color)										noexcept	{ r = r * color.r;					g = g * color.g;				b = b * color.b;				return Clamp();																					}
									SColorFloat&	operator *=		(double scalar)													noexcept	{ r = (float)(r * scalar);			g = (float)(g * scalar);		b = (float)(b * scalar);		return Clamp();																					}
									SColorFloat&	operator /=		(double scalar)																{ r = (float)(r / scalar);			g = (float)(g / scalar);		b = (float)(b / scalar);		return Clamp();																					}
									SColorFloat&	operator *=		(const SColorBGRA& color)										noexcept	{ r = r * (color.r * (1/255.0f));	g = g * (color.g * (1/255.0f));	b = b * (color.b * (1/255.0f));	return Clamp();																					}
									SColorFloat&	operator +=		(const SColorBGRA& color)										noexcept	{ r = r + (color.r * (1/255.0f));	g = g + (color.g * (1/255.0f));	b = b + (color.b * (1/255.0f));	return Clamp();																					}
									SColorFloat&	operator *=		(const SColorBGR& color)										noexcept	{ r = r * (color.r * (1/255.0f));	g = g * (color.g * (1/255.0f));	b = b * (color.b * (1/255.0f));	return Clamp();																					}
									SColorFloat&	operator +=		(const SColorBGR& color)										noexcept	{ r = r + (color.r * (1/255.0f));	g = g + (color.g * (1/255.0f));	b = b + (color.b * (1/255.0f));	return Clamp();																					}
									SColorFloat		operator *		(const SColorBGRA& color)								const	noexcept	{ return ::cho::SColorFloat{r * (color.r * (1/255.0f)), g * (color.g * (1/255.0f)), b * (color.b * (1/255.0f)), a}.Clamp();																			}
									SColorFloat		operator +		(const SColorBGRA& color)								const	noexcept	{ return ::cho::SColorFloat{r + (color.r * (1/255.0f)), g + (color.g * (1/255.0f)), b + (color.b * (1/255.0f)), a}.Clamp();																			}
									SColorFloat		operator *		(const SColorBGR& color)								const	noexcept	{ return ::cho::SColorFloat{r * (color.r * (1/255.0f)), g * (color.g * (1/255.0f)), b * (color.b * (1/255.0f)), a}.Clamp();																			}
									SColorFloat		operator +		(const SColorBGR& color)								const	noexcept	{ return ::cho::SColorFloat{r + (color.r * (1/255.0f)), g + (color.g * (1/255.0f)), b + (color.b * (1/255.0f)), a}.Clamp();																			}
		constexpr					SColorFloat		operator *		(const SColorFloat& color)								const	noexcept	{ return ::cho::SColorFloat{::cho::clamp(r * color.r, 0.0f, 1.0f),	::cho::clamp(g * color.g, 0.0f, 1.0f),	::cho::clamp(b * color.b, 0.0f, 1.0f)};													}
		constexpr					SColorFloat		operator +		(const SColorFloat& color)								const	noexcept	{ return ::cho::SColorFloat{::cho::clamp(r + color.r, 0.0f, 1.0f),	::cho::clamp(g + color.g, 0.0f, 1.0f),	::cho::clamp(b + color.b, 0.0f, 1.0f)};													}
		constexpr					SColorFloat		operator *		(double scalar)											const	noexcept	{ return ::cho::SColorFloat{(float)::cho::clamp(r * scalar, 0.0, 1.0), (float)::cho::clamp(g * scalar, 0.0, 1.0), (float)::cho::clamp(b * scalar, 0.0, 1.0)};										}
		constexpr					SColorFloat		operator /		(double scalar)											const				{ return ::cho::SColorFloat{(float)::cho::clamp(r / scalar, 0.0, 1.0), (float)::cho::clamp(g / scalar, 0.0, 1.0), (float)::cho::clamp(b / scalar, 0.0, 1.0)};										}
									SColorFloat&	operator *=		(float scalar)													noexcept	{ r = r * scalar;					g = g * scalar;					b = b * scalar;					return Clamp();																					}
									SColorFloat&	operator /=		(float scalar)																{ r = r / scalar;					g = g / scalar;					b = b / scalar;					return Clamp();																					}
		constexpr					SColorFloat		operator *		(float scalar)											const	noexcept	{ return ::cho::SColorFloat(::cho::clamp(r * scalar, 0.0f, 1.0f),	::cho::clamp(g * scalar, 0.0f, 1.0f),	::cho::clamp(b * scalar, 0.0f, 1.0f));													}
		constexpr					SColorFloat		operator /		(float scalar)											const				{ return ::cho::SColorFloat(::cho::clamp(r / scalar, 0.0f, 1.0f),	::cho::clamp(g / scalar, 0.0f, 1.0f),	::cho::clamp(b / scalar, 0.0f, 1.0f));													}

									SColorFloat&	Clamp			()																noexcept	{ r = ::cho::clamp(r, 0.0f, 1.0f); g = ::cho::clamp(g, 0.0f, 1.0f); b = ::cho::clamp(b, 0.0f, 1.0f); return *this;																				}
	};	// struct

	static constexpr	const ::cho::SColorFloat	BLACK			= {0.0f, 0.0f, 0.0f, 1.0f					};
	static constexpr	const ::cho::SColorFloat	WHITE			= {1.0f, 1.0f, 1.0f, 1.0f					};
	static constexpr	const ::cho::SColorFloat	RED				= {1.0f, 0.0f, 0.0f, 1.0f					};
	static constexpr	const ::cho::SColorFloat	GREEN			= {0.0f, 1.0f, 0.0f, 1.0f					};
	static constexpr	const ::cho::SColorFloat	BLUE			= {0.0f, 0.0f, 1.0f, 1.0f					};
	static constexpr	const ::cho::SColorFloat	YELLOW			= {1.0f, 1.0f, 0.0f, 1.0f					};
	static constexpr	const ::cho::SColorFloat	MAGENTA			= {1.0f, 0.0f, 1.0f, 1.0f					};
	static constexpr	const ::cho::SColorFloat	CYAN			= {0.0f, 1.0f, 1.0f, 1.0f					};
	static constexpr	const ::cho::SColorFloat	ORANGE			= {1.0f, 0.647f, 0.0f, 1.0f					};
	static constexpr	const ::cho::SColorFloat	BROWN			= 0xFF964B00;	
	static constexpr	const ::cho::SColorFloat	GRAY			= {0.5f, 0.5f, 0.5f, 1.0f					};
	static constexpr	const ::cho::SColorFloat	DARKGRAY		= {0.25f, 0.25f, 0.25f, 1.0f				};
	static constexpr	const ::cho::SColorFloat	DARKRED			= {0.5f, 0.0f, 0.0f, 1.0f					};
	static constexpr	const ::cho::SColorFloat	DARKGREEN		= {0.0f, 0.5f, 0.0f, 1.0f					};
	static constexpr	const ::cho::SColorFloat	DARKBLUE		= {0.0f, 0.0f, 0.5f, 1.0f					};
	static constexpr	const ::cho::SColorFloat	DARKYELLOW		= {0.5f, 0.5f, 0.0f, 1.0f					};
	static constexpr	const ::cho::SColorFloat	DARKMAGENTA		= {0.5f, 0.0f, 0.5f, 1.0f					};
	static constexpr	const ::cho::SColorFloat	DARKCYAN		= {0.0f, 0.5f, 0.5f, 1.0f					};
	static constexpr	const ::cho::SColorFloat	DARKORANGE		= {1.0f, 0.5490196078431373f, 0.0f, 1.0f	};
	static constexpr	const ::cho::SColorFloat	LIGHTGRAY		= {0.75f, 0.75f, 0.75f, 1.0f				};
	static constexpr	const ::cho::SColorFloat	LIGHTRED		= {1.0f, 0.25f, 0.25f, 1.0f					};
	static constexpr	const ::cho::SColorFloat	LIGHTGREEN		= {0.25f, 1.0f, 0.25f, 1.0f					};
	static constexpr	const ::cho::SColorFloat	LIGHTBLUE		= {0.25f, 0.25f, 1.0f, 1.0f					};
	static constexpr	const ::cho::SColorFloat	LIGHTYELLOW		= {1.0f, 1.0f, 0.25f, 1.0f					};
	static constexpr	const ::cho::SColorFloat	LIGHTMAGENTA	= {1.0f, 0.25f, 1.0f, 1.0f					};
	static constexpr	const ::cho::SColorFloat	LIGHTCYAN		= {0.25f, 1.0f, 1.0f, 1.0f					};
	static constexpr	const ::cho::SColorFloat	LIGHTORANGE		= {1.0f, 0.780f, 0.25f, 1.0f				};
#pragma pack( pop )
} // namespace


#endif // CHO_COLOR_H_29734982734
