// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once. 
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "application.h"

#include "cho_bitmap_target.h"
#include "cho_bitmap_file.h"
#include "cho_grid_copy.h"
#include "cho_grid_scale.h"
#include "cho_bit_array_view.h"

#include "cho_app_impl.h"
#include "cho_matrix.h"

static constexpr	const uint32_t										ASCII_SCREEN_WIDTH							= 132	;
static constexpr	const uint32_t										ASCII_SCREEN_HEIGHT							= 50	;

CHO_DEFINE_APPLICATION_ENTRY_POINT(::SApplication);	

					::SApplication										* g_ApplicationInstance						= 0;

static				::cho::error_t										updateSizeDependentResources				(::SApplication& applicationInstance)											{ 
	const ::cho::SCoord2<uint32_t>												newSize										= applicationInstance.Framework.MainDisplay.Size; 
	::cho::updateSizeDependentTarget(applicationInstance.Framework.Offscreen, newSize);
	return 0;
}

// --- Cleanup application resources.
					::cho::error_t										cleanup										(::SApplication& applicationInstance)											{
	::cho::SDisplayPlatformDetail												& displayDetail								= applicationInstance.Framework.MainDisplay.PlatformDetail;
	if(displayDetail.WindowHandle) {
		error_if(0 == ::DestroyWindow(displayDetail.WindowHandle), "Not sure why would this fail.");
		error_if(errored(::cho::displayUpdate(applicationInstance.Framework.MainDisplay)), "Not sure why this would fail");
	}
	::UnregisterClass(displayDetail.WindowClassName, displayDetail.WindowClass.hInstance);
	g_ApplicationInstance													= 0;
	return 0;
}

// Vertex coordinates for cube faces
static constexpr const ::cho::STriangle3D<float>						geometryCube	[12]						= 
	{ {{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}	// Right	- first			?? I have no idea if this is correct lol
	, {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}}	// Right	- second		?? I have no idea if this is correct lol

	, {{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}	// Back		- first			?? I have no idea if this is correct lol
	, {{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}}	// Back		- second		?? I have no idea if this is correct lol

	, {{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}	// Bottom	- first			?? I have no idea if this is correct lol
	, {{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}	// Bottom	- second		?? I have no idea if this is correct lol

	, {{1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}	// Left		- first
	, {{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}}	// Left		- second

	, {{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}}	// Front	- first
	, {{1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}}	// Front	- second

	, {{1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}}	// Top		- first
	, {{1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}}	// Top		- second
	};

					::cho::error_t										mainWindowCreate							(::cho::SDisplay& mainWindow, HINSTANCE hInstance);
					::cho::error_t										setup										(::SApplication& applicationInstance)											{ 
	g_ApplicationInstance													= &applicationInstance;
	::cho::SDisplay																& mainWindow								= applicationInstance.Framework.MainDisplay;
	error_if(errored(::mainWindowCreate(mainWindow, applicationInstance.Framework.RuntimeValues.PlatformDetail.EntryPointArgs.hInstance)), "Failed to create main window why?????!?!?!?!?");
	static constexpr	const char												bmpFileName2	[]							= "Codepage-437-24.bmp";
	error_if(errored(::cho::bmpFileLoad((::cho::view_const_string)bmpFileName2, applicationInstance.TextureFont				)), "Failed to load bitmap from file: %s.", bmpFileName2);
	const ::cho::SCoord2<uint32_t>												& textureFontMetrics						= applicationInstance.TextureFont.View.metrics();
	applicationInstance.TextureFontMonochrome.resize(textureFontMetrics);
	for(uint32_t y = 0, yMax = textureFontMetrics.y; y < yMax; ++y)
	for(uint32_t x = 0, xMax = textureFontMetrics.x; x < xMax; ++x)
		applicationInstance.TextureFontMonochrome.View[y * textureFontMetrics.x + x]	
		=	0 != applicationInstance.TextureFont.View[y][x].r
		||	0 != applicationInstance.TextureFont.View[y][x].g
		||	0 != applicationInstance.TextureFont.View[y][x].b
		;

	static constexpr const ::cho::SCoord3<float>								cubeCenter									= {0.5f, 0.5f, 0.5f};
	for(uint32_t iTriangle = 0; iTriangle < 12; ++iTriangle) {
		::cho::STriangle3D<float>													& transformedTriangle						= applicationInstance.CubePositions[iTriangle];
		transformedTriangle														= geometryCube[iTriangle];
		transformedTriangle.A													-= cubeCenter;
		transformedTriangle.B													-= cubeCenter;
		transformedTriangle.C													-= cubeCenter;
	}
	ree_if	(errored(::updateSizeDependentResources	(applicationInstance)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");
	return 0;
}

					::cho::error_t										update										(::SApplication& applicationInstance, bool systemRequestedExit)					{ 
	retval_info_if(1, systemRequestedExit, "Exiting because the runtime asked for close. We could also ignore this value and just continue execution if we don't want to exit.");
	::cho::error_t																frameworkResult								= ::cho::updateFramework(applicationInstance.Framework);
	ree_if(errored(frameworkResult), "Unknown error.");
	rvi_if(1, frameworkResult == 1, "Framework requested close. Terminating execution.");
	ree_if(errored(::updateSizeDependentResources	(applicationInstance)), "Cannot update offscreen and this could cause an invalid memory access later on.");
	//-----------------------------
	::cho::STimer																& timer										= applicationInstance.Framework.Timer;
	::cho::SDisplay																& mainWindow								= applicationInstance.Framework.MainDisplay;
	char																		buffer		[256]							= {};
	sprintf_s(buffer, "[%u x %u]. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
	::HWND																		windowHandle								= mainWindow.PlatformDetail.WindowHandle;
	SetWindowText(windowHandle, buffer);
	return 0;
}

template<size_t _sizeString>
static				::cho::error_t										textCalcSizeLine							(const ::cho::SCoord2<int32_t>& sizeCharCell, const char (&text0)[_sizeString] )	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	return (::cho::error_t)(sizeCharCell.x * ::cho::size(text0) - 1);
}

static				::cho::error_t										textDrawFixedSize							(::cho::grid_view<::cho::SColorBGRA>& bmpTarget, const ::cho::bit_array_view<uint32_t>& viewTextureFont, const ::cho::SCoord2<uint32_t> & viewMetrics, uint32_t characterCellsX, int32_t dstOffsetY, const ::cho::SCoord2<int32_t>& sizeCharCell, const ::cho::view_const_string& text0, const ::cho::SCoord2<int32_t> dstTextOffset, const ::cho::SColorBGRA& color)	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::cho::array_pod<::cho::SCoord2<uint32_t>>								dstCoords;
	for(int32_t iChar = 0, charCount = (int32_t)text0.size(); iChar < charCount; ++iChar) {
		int32_t																	coordTableX										= text0[iChar] % characterCellsX;
		int32_t																	coordTableY										= text0[iChar] / characterCellsX;
		const ::cho::SCoord2<int32_t>											coordCharTable									= {coordTableX * sizeCharCell.x, coordTableY * sizeCharCell.y};
		const ::cho::SCoord2<int32_t>											dstOffset1										= {sizeCharCell.x * iChar, dstOffsetY};
		const ::cho::SRectangle2D<int32_t>										srcRect0										= ::cho::SRectangle2D<int32_t>{{coordCharTable.x, (int32_t)viewMetrics.y - sizeCharCell.y - coordCharTable.y}, sizeCharCell};
		error_if(errored(::cho::grid_copy_alpha_bit(bmpTarget, viewTextureFont, dstTextOffset + dstOffset1, viewMetrics, ::cho::SColorBGRA{0, 0xFF, 0xFF, 0xFF}, srcRect0)), "I believe this never fails.");
		color;
	}
	return 0;
}

template<size_t _sizeString>
static				::cho::error_t										textDrawAlignedFixedSize					(::cho::grid_view<::cho::SColorBGRA>& targetView, const ::cho::bit_array_view<uint32_t>& fontAtlas, const ::cho::SCoord2<uint32_t> & viewMetrics, uint32_t lineOffset, const ::cho::SCoord2<uint32_t>& targetSize, const ::cho::SCoord2<int32_t>& sizeCharCell, const char (&text0)[_sizeString], const ::cho::SColorBGRA& color)	{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	const ::cho::SCoord2<int32_t>												dstTextOffset								= {(int32_t)targetSize.x / 2 - (int32_t)textCalcSizeLine(sizeCharCell, text0) / 2, };
	uint32_t																	dstOffsetY									= (int32_t)(targetSize.y - lineOffset * sizeCharCell.y - sizeCharCell.y);
	return ::textDrawFixedSize(targetView, fontAtlas, viewMetrics, 32, dstOffsetY, sizeCharCell, {text0, ::cho::size(text0) -1}, dstTextOffset, color);
}

static constexpr const ::cho::SCoord3<float>						geometryCubeNormals	[12]						= 
	{ {0.0f, 0.0f, -1.0f}	// Right	- first			?? I have no idea if this is correct lol
	, {0.0f, 0.0f, -1.0f}	// Right	- second		?? I have no idea if this is correct lol

	, {-1.0f, 0.0f, 0.0f}	// Back		- first			?? I have no idea if this is correct lol
	, {-1.0f, 0.0f, 0.0f}	// Back		- second		?? I have no idea if this is correct lol

	, {0.0f, -1.0f, 0.0f}	// Bottom	- first			?? I have no idea if this is correct lol
	, {0.0f, -1.0f, 0.0f}	// Bottom	- second		?? I have no idea if this is correct lol

	, {0.0f, 0.0f, 1.0f}	// Left		- first
	, {0.0f, 0.0f, 1.0f}	// Left		- second

	, {1.0f, 0.0f, 0.0f}	// Front	- first
	, {1.0f, 0.0f, 0.0f}	// Front	- second

	, {0.0f, 1.0f, 0.0f}	// Top		- first
	, {0.0f, 1.0f, 0.0f}	// Top		- second
	};

struct SCamera {
						::cho::SCoord3<float>								Position, Target;
};

					::cho::error_t										draw										(::SApplication& applicationInstance)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::cho::SFramework															& framework									= applicationInstance.Framework;
	::cho::SFramework::TOffscreen												& offscreen									= framework.Offscreen;
	const ::cho::SCoord2<uint32_t>												& offscreenMetrics							= offscreen.View.metrics();
	::memset(offscreen.Texels.begin(), 0, sizeof(::cho::SFramework::TOffscreen::TTexel) * offscreen.Texels.size());	// Clear target.
	//------------------------------------------------
	::cho::array_pod<::cho::STriangle3D<float>>									triangle3dList								= {};
	::cho::array_pod<::cho::SColorBGRA>											triangle3dColorList							= {};
	triangle3dList.resize(12);
	triangle3dColorList.resize(12);
	::cho::SMatrix4<float>														projection									= {};
	::cho::SMatrix4<float>														viewMatrix									= {};
	projection.Identity();
	::cho::SFrameInfo															& frameInfo									= framework.FrameInfo;
	const ::cho::SCoord3<float>													tilt										= {10, };	// ? cam't remember what is this. Radians? Eulers?
	const ::cho::SCoord3<float>													rotation									= {0, (float)frameInfo.FrameNumber / 100, 0};

	float																		fFar										= 1000.0f
		,																		fNear										= 0.01f
		;

	static constexpr const ::cho::SCoord3<float>								cameraUp									= {0, 1, 0};	// ? cam't remember what is this. Radians? Eulers?
	::SCamera																	camera										= {{10, 5, 0}, {}};
	::cho::SCoord3<float>														lightPos									= {10, 5, 0};
	static float																cameraRotation								= 0;
	cameraRotation															+= (float)framework.Input.MouseCurrent.Deltas.x / 5.0f;
	//camera.Position	.RotateY(cameraRotation);
	camera.Position	.RotateY(frameInfo.Microseconds.Total / 1000000.0f);
	lightPos		.RotateY(frameInfo.Microseconds.Total /  500000.0f * -2);
	viewMatrix.LookAt(camera.Position, camera.Target, cameraUp);
	projection.FieldOfView(.25 * ::cho::math_pi, offscreenMetrics.x / (double)offscreenMetrics.y, fNear, fFar );
	projection																= viewMatrix * projection;
	lightPos.Normalize();

	::cho::SMatrix4<float>														viewport									= {};
	viewport._11															= 2.0f / offscreenMetrics.x;
	viewport._22															= 2.0f / offscreenMetrics.y;
	viewport._33															= 1.0f / (fFar - fNear);
	viewport._43															= - fNear * ( 1.0f / (fFar - fNear) );
	viewport._44															= 1.0f;
	projection																= projection * viewport.GetInverse();
	for(uint32_t iTriangle = 0; iTriangle < 12; ++iTriangle) {
		::cho::STriangle3D<float>													& transformedTriangle						= triangle3dList[iTriangle];
		transformedTriangle														= applicationInstance.CubePositions[iTriangle];
		::cho::transform(transformedTriangle, projection);
	}
	::cho::array_pod<::cho::STriangle2D<int32_t>>								triangle2dList								= {};
	triangle2dList.resize(12);
	const ::cho::SCoord2<int32_t>												screenCenter								= {(int32_t)offscreenMetrics.x / 2, (int32_t)offscreenMetrics.y / 2};
	for(uint32_t iTriangle = 0; iTriangle < 12; ++iTriangle) { // Maybe the scale
		::cho::STriangle3D<float>													& transformedTriangle3D						= triangle3dList[iTriangle];
		::cho::STriangle2D<int32_t>													& transformedTriangle2D						= triangle2dList[iTriangle];
		transformedTriangle2D.A													= {(int32_t)transformedTriangle3D.A.x, (int32_t)transformedTriangle3D.A.y};
		transformedTriangle2D.B													= {(int32_t)transformedTriangle3D.B.x, (int32_t)transformedTriangle3D.B.y};
		transformedTriangle2D.C													= {(int32_t)transformedTriangle3D.C.x, (int32_t)transformedTriangle3D.C.y};
		::cho::translate(transformedTriangle2D, screenCenter);
	}

	for(uint32_t iTriangle = 0; iTriangle < 12; ++iTriangle) {
		double																		lightFactor									= geometryCubeNormals[iTriangle].Dot(lightPos);
		triangle3dColorList[iTriangle]											= ::cho::BLUE * lightFactor;
	}
	::cho::array_pod<::cho::SCoord2<int32_t>>										trianglePixelCoords;
	::cho::array_pod<::cho::SCoord2<int32_t>>										wireframePixelCoords;
	::cho::SCoord3<float> cameraFront = (camera.Target - camera.Position).Normalize();
	for(uint32_t iTriangle = 0; iTriangle < 12; ++iTriangle) {
		double																		lightFactor									= geometryCubeNormals[iTriangle].Dot(cameraFront);
		if(lightFactor > 0)
			continue;
		error_if(errored(::cho::drawTriangle(offscreen.View, triangle3dColorList[iTriangle], triangle2dList[iTriangle])), "Not sure if these functions could ever fail");
		//::cho::drawLine(offscreen.View, (::cho::SColorBGRA)::cho::GREEN, ::cho::SLine2D<int32_t>{triangle2dList[iTriangle].A, triangle2dList[iTriangle].B});
		//::cho::drawLine(offscreen.View, (::cho::SColorBGRA)::cho::GREEN, ::cho::SLine2D<int32_t>{triangle2dList[iTriangle].B, triangle2dList[iTriangle].C});
		//::cho::drawLine(offscreen.View, (::cho::SColorBGRA)::cho::GREEN, ::cho::SLine2D<int32_t>{triangle2dList[iTriangle].C, triangle2dList[iTriangle].A});
		//trianglePixelCoords.clear();
		//error_if(errored(::cho::drawTriangle(offscreenMetrics, triangle2dList[iTriangle], trianglePixelCoords)), "Not sure if these functions could ever fail");
		//for(uint32_t iCoord = 0; iCoord < trianglePixelCoords.size(); ++iCoord)
		//	::cho::drawPixelLight(offscreen.View, trianglePixelCoords[iCoord], (::cho::SColorBGRA)::cho::BLUE, 0.05f, 2.5);
		::cho::drawLine(offscreenMetrics, ::cho::SLine2D<int32_t>{triangle2dList[iTriangle].A, triangle2dList[iTriangle].B}, wireframePixelCoords);
		::cho::drawLine(offscreenMetrics, ::cho::SLine2D<int32_t>{triangle2dList[iTriangle].B, triangle2dList[iTriangle].C}, wireframePixelCoords);
		::cho::drawLine(offscreenMetrics, ::cho::SLine2D<int32_t>{triangle2dList[iTriangle].C, triangle2dList[iTriangle].A}, wireframePixelCoords);
	}
	for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord)
		::cho::drawPixelLight(offscreen.View, wireframePixelCoords[iCoord], (::cho::SColorBGRA)::cho::GREEN, 0.05f, 2.5);

	//------------------------------------------------
	static constexpr const ::cho::SCoord2<int32_t>								sizeCharCell								= {9, 16};
	uint32_t																	lineOffset									= 0;
	static constexpr const char													textLine0	[]								= "W: Up, S: Down, A: Left, D: Right";
	static constexpr const char													textLine1	[]								= "T: Shoot. Y: Thrust. U: Handbrake.";
	static constexpr const char													textLine2	[]								= "Press ESC to exit.";
	::cho::grid_view<::cho::SColorBGRA>											& offscreenView								= applicationInstance.Framework.Offscreen.View;
	const ::cho::grid_view<::cho::SColorBGRA>									& fontAtlasView								= applicationInstance.TextureFont.View;
	::textDrawAlignedFixedSize(offscreenView, applicationInstance.TextureFontMonochrome.View, fontAtlasView.metrics(), lineOffset, offscreenMetrics, sizeCharCell, textLine0, ::cho::SColorBGRA{0, framework.FrameInfo.FrameNumber % 0xFF, 0xFFU, 0xFFU});	++lineOffset;
	::textDrawAlignedFixedSize(offscreenView, applicationInstance.TextureFontMonochrome.View, fontAtlasView.metrics(), lineOffset, offscreenMetrics, sizeCharCell, textLine1, ::cho::SColorBGRA{framework.FrameInfo.FrameNumber % 0xFFU, 0xFFU, 0, 0xFFU});	++lineOffset;
	::textDrawAlignedFixedSize(offscreenView, applicationInstance.TextureFontMonochrome.View, fontAtlasView.metrics(), lineOffset = offscreenMetrics.y / 16 - 1, offscreenMetrics, sizeCharCell, textLine2, ::cho::SColorBGRA{0, framework.FrameInfo.FrameNumber % 0xFFU, 0, 0xFFU});	--lineOffset;
	return 0;																																																
}
	