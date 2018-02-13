#include "application.h"

					::cho::error_t										updateInput									(::SApplication& applicationInstance)											{ 
	::cho::SInput																& inputSystem								= applicationInstance.Framework.SystemInput;
	applicationInstance.ShipState.Firing									= inputSystem.KeyboardCurrent.KeyState[VK_SPACE		] != 0;
	applicationInstance.ShipState.Thrust									= inputSystem.KeyboardCurrent.KeyState[VK_SHIFT		] != 0;
	applicationInstance.ShipState.Brakes									= inputSystem.KeyboardCurrent.KeyState[VK_CONTROL	] != 0;

	applicationInstance.DirectionShip										= {};
	if(inputSystem.KeyboardCurrent.KeyState['W']) applicationInstance.DirectionShip.y += 1;
	if(inputSystem.KeyboardCurrent.KeyState['S']) applicationInstance.DirectionShip.y -= 1;
	if(inputSystem.KeyboardCurrent.KeyState['D']) applicationInstance.DirectionShip.x += 1;
	if(inputSystem.KeyboardCurrent.KeyState['A']) applicationInstance.DirectionShip.x -= 1;
	applicationInstance.DirectionShip.Normalize();
	return 0;
}

					::cho::error_t										updateParticles								(::SApplication& applicationInstance)											{ 
	::cho::SFramework															& framework									= applicationInstance.Framework;
	typedef	::SApplication::TParticleSystem										TParticleSystem;
	typedef	TParticleSystem::TParticleInstance									TParticleInstance;
	typedef	TParticleSystem::TIntegrator::TParticle								TParticle;
	TParticleSystem::TIntegrator												& particleIntegrator						= applicationInstance.ParticleSystem.Integrator;
	const float																	lastFrameSeconds							= (float)framework.FrameInfo.Seconds.LastFrame;
	ree_if(errored(particleIntegrator.Integrate(lastFrameSeconds, framework.FrameInfo.Seconds.LastFrameHalfSquared)), "Not sure why would this fail.");

	const float																	windDirection								= (float)sin(framework.FrameInfo.Seconds.Total/3.0f) * .025f;

	::cho::array_pod<TParticleInstance>											& particleInstances							= applicationInstance.ParticleSystem.Instances;
	::cho::clear
		( applicationInstance.StuffToDraw.ProjectilePaths
		, applicationInstance.StuffToDraw.CollisionPoints	
		, applicationInstance.StuffToDraw.Debris			
		, applicationInstance.StuffToDraw.Thrust			
		, applicationInstance.StuffToDraw.Stars				
		);
	for(uint32_t iParticle = 0; iParticle < particleInstances.size(); ++iParticle) {
		TParticleInstance															& particleInstance							= particleInstances[iParticle];
		int32_t																		physicsId									= particleInstance.ParticleIndex;
		TParticle																	& particleNext								= particleIntegrator.ParticleNext[physicsId];
		TParticle																	& particleCurrent							= particleIntegrator.Particle[physicsId];
		if(particleInstance.Type == PARTICLE_TYPE_LASER) {
			::SLaserToDraw	laserToDraw	= {physicsId, (int32_t)iParticle, ::cho::SLine2D<float>{particleCurrent.Position, particleNext.Position}};
			applicationInstance.StuffToDraw.ProjectilePaths.push_back(laserToDraw);
		}
		if( ((uint32_t)particleNext.Position.x) >= framework.Offscreen.View.width	()
		 || ((uint32_t)particleNext.Position.y) >= framework.Offscreen.View.height	()
		 || (particleInstance.TimeLived >= .125 && particleInstance.Type == PARTICLE_TYPE_SHIP_THRUST)
		 ) { // Remove the particle instance and related information.
			particleIntegrator.ParticleState[physicsId].Unused						= true;
			ree_if(errored(particleInstances.remove(iParticle)), "Not sure why would this fail.");
			--iParticle;
		}
		else {
			::SParticleToDraw particleToDraw = {physicsId, (int32_t)iParticle, particleInstance.TimeLived, particleCurrent.Position.Cast<int32_t>(), particleInstance.Lit};
				 if(particleInstance.Type == PARTICLE_TYPE_STAR			)	applicationInstance.StuffToDraw.Stars	.push_back(particleToDraw);
			else if(particleInstance.Type == PARTICLE_TYPE_SHIP_THRUST	)	applicationInstance.StuffToDraw.Thrust	.push_back(particleToDraw);
			else if(particleInstance.Type == PARTICLE_TYPE_DEBRIS		)	applicationInstance.StuffToDraw.Debris	.push_back(particleToDraw);
			particleInstance.TimeLived												+= lastFrameSeconds;
			particleCurrent															= particleNext;
		}
	}
	return 0;
}

template<typename _tCoord>
static inline	double		determinant					(const ::cho::SLine2D<_tCoord>& line)			noexcept { return ::cho::determinant((double)line.A.x, (double)line.A.y, (double)line.B.x, (double)line.B.y); }

// Calculate intersection of two lines. return 1 if found, 0 if not found or -1 on error
template<typename _tCoord>
::cho::error_t				line_line_intersect			
	( const ::cho::SLine2D<_tCoord>	& line1
	, const ::cho::SLine2D<_tCoord>	& line2
	, ::cho::SCoord2<_tCoord>		& out_point
	)
{
	double detL1 = ::determinant(line1);
	double detL2 = ::determinant(line2);

	double x1mx2 = line1.A.x - line1.B.x;
	double y1my2 = line1.A.y - line1.B.y;

	double x3mx4 = line2.A.x - line2.B.x;
	double y3my4 = line2.A.y - line2.B.y;

	double xnom  = ::cho::determinant(detL1, x1mx2, detL2, x3mx4);
	double ynom  = ::cho::determinant(detL1, y1my2, detL2, y3my4);
	double denom = ::cho::determinant(x1mx2, y1my2, x3mx4, y3my4);
	if(denom == 0.0) { // Lines don't seem to cross
		out_point.x = NAN;
		out_point.y = NAN;
		return 0;
	}
	out_point.x = (_tCoord)(xnom / denom);	
	out_point.y = (_tCoord)(ynom / denom);
	ree_if(!isfinite(out_point.x) 
		|| !isfinite(out_point.y)
		, "Probably a numerical issue");
	return 1; //All OK
}

// Calculate intersection of two lines. return 1 if found, 0 if not found or -1 on error
template<typename _tCoord>
::cho::error_t				line_segment_intersect			
	( const ::cho::SLine2D<_tCoord>	& line
	, const ::cho::SLine2D<_tCoord>	& segment
	, ::cho::SCoord2<_tCoord>		& out_point
	)
{
	::cho::error_t																collision									= line_line_intersect(line, segment, out_point);
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
::cho::error_t				segment_segment_intersect			
	( const ::cho::SLine2D<_tCoord>	& segment1
	, const ::cho::SLine2D<_tCoord>	& segment2
	, ::cho::SCoord2<_tCoord>		& out_point
	)
{
	::cho::error_t																collision									= line_segment_intersect(segment1, segment2, out_point);
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

template<typename _tParticleType>
static				::cho::error_t										addParticle														
	(	::PARTICLE_TYPE																		particleType
	,	::cho::array_pod<::cho::SParticleInstance<_tParticleType>>							& particleInstances
	,	::SApplication::TParticleSystem::TIntegrator										& particleIntegrator
	,	const ::cho::SCoord2<float>															& particlePosition
	,	const ::cho::SCoord2<float>															& particleDirection
	,	float																				speed
	,	const ::cho::array_view<::SApplication::TParticleSystem::TIntegrator::TParticle>	& particleDefinitions
	)														
{
	::cho::SParticleInstance<_tParticleType>									& newInstance													= particleInstances[::cho::addParticle(particleType, particleInstances, particleIntegrator, particleDefinitions[particleType])]; 
	::SApplication::TParticleSystem::TIntegrator::TParticle						& newParticle													= particleIntegrator.Particle[newInstance.ParticleIndex];
	newParticle.Position													= particlePosition; 
	::cho::SCoord2<float>														newDirection													= particleDirection;
	const float																	value															= .5;
	switch(particleType) {
	default							: break;
	case ::PARTICLE_TYPE_SHIP_THRUST:	
		newParticle.Position.y													+= rand() % 3 - 1;
		newDirection.Rotate(((rand() % 32767) / 32766.0f) * value - value / 2);
		newInstance.Lit															= 0 == (rand() % 2);
		break;
	case ::PARTICLE_TYPE_DEBRIS		:	
	case ::PARTICLE_TYPE_STAR		:	
		newInstance.Lit															= 0 == (rand() % 3);
		break;
	}
	newParticle.Forces.Velocity												= newDirection * speed;	//{ -, (float)((rand() % 31 * 4) - 15 * 4)};
	return 0;
}

					::cho::error_t										updateShip									(::SApplication & applicationInstance)
{
	::cho::SFramework															& framework									= applicationInstance.Framework;
	::cho::SFramework::TOffscreen												& offscreen									= framework.Offscreen;
	// update ship
	applicationInstance.CenterPositionShip									+= applicationInstance.DirectionShip * (float)(framework.FrameInfo.Seconds.LastFrame * 100) * 
		(applicationInstance.ShipState.Brakes ? .25f : (applicationInstance.ShipState.Thrust ? 2 : 1));
	applicationInstance.CenterPositionShip.x								= ::cho::clamp(applicationInstance.CenterPositionShip.x, .1f, (float)offscreen.View.metrics().x - 1);
	applicationInstance.CenterPositionShip.y								= ::cho::clamp(applicationInstance.CenterPositionShip.y, .1f, (float)offscreen.View.metrics().y - 1);
	return 0;
}

					::cho::error_t										updateSpawn									
	( ::SApplication																	& applicationInstance
	, const ::cho::array_view<::SApplication::TParticleSystem::TIntegrator::TParticle>	& particleDefinitions
	)
{
	::cho::SFramework															& framework									= applicationInstance.Framework;
	::cho::SFramework::TOffscreen												& offscreen									= applicationInstance.Framework.Offscreen;
	// Add some effect particles
	typedef	::SApplication::TParticleSystem										TParticleSystem;
	typedef	TParticleSystem::TParticleInstance									TParticleInstance;
	::cho::array_pod<TParticleInstance>											& particleInstances							= applicationInstance.ParticleSystem.Instances;
	TParticleSystem::TIntegrator												& particleIntegrator						= applicationInstance.ParticleSystem.Integrator;
	static double																delayThrust									= 0;
	static double																delayStar									= 0;
	static double																delayWeapon									= 0;
	delayThrust																+= framework.FrameInfo.Seconds.LastFrame;
	delayStar																+= framework.FrameInfo.Seconds.LastFrame;
	delayWeapon																+= framework.FrameInfo.Seconds.LastFrame;
	bool																		isTurbo										= applicationInstance.ShipState.Thrust;
	if(delayThrust > .01) {
		delayThrust																= 0;
		for(int32_t i = 0, particleCountToSpawn = 1 + rand() % 4; i < particleCountToSpawn; ++i) 
			::addParticle(PARTICLE_TYPE_SHIP_THRUST, particleInstances, particleIntegrator, applicationInstance.CenterPositionShip + applicationInstance.PSOffsetFromShipCenter.Cast<float>(), applicationInstance.DirectionShip * -1.0, (float)(rand() % 400) + (isTurbo ? 400 : 0), particleDefinitions);
	}
	if(delayStar > .1) {
		delayStar																= 0;
		::addParticle(PARTICLE_TYPE_STAR, particleInstances, particleIntegrator, {offscreen.View.metrics().x - 1.0f, (float)(rand() % offscreen.View.metrics().y)}, {-1, 0}, (float)(rand() % (isTurbo ? 400 : 75)) + 25, particleDefinitions);
	}

	if(applicationInstance.ShipState.Firing) {
		if(delayWeapon >= applicationInstance.Laser.Delay) {
			const ::cho::SCoord2<float>													textureShipMetrics					= applicationInstance.TextureShip.Processed.View.metrics().Cast<float>();
			const ::cho::SCoord2<float>													weaponParticleOffset				= {textureShipMetrics.x - (textureShipMetrics.x - applicationInstance.TextureCenters[GAME_TEXTURE_SHIP].x), -1};
			delayWeapon																= 0;
			::addParticle(PARTICLE_TYPE_LASER, particleInstances, particleIntegrator, applicationInstance.CenterPositionShip + weaponParticleOffset, {1, 0}, applicationInstance.Laser.Speed, particleDefinitions);
		}
	}
	return 0;
}

template<typename _tCoord>
					::cho::error_t										buildAABBSegments		
	( const ::cho::SCoord2<_tCoord> & center
	, const _tCoord					halfSizeBox
	,		::cho::SLine2D<_tCoord>	& left
	,		::cho::SLine2D<_tCoord>	& top
	,		::cho::SLine2D<_tCoord>	& right
	,		::cho::SLine2D<_tCoord>	& bottom
	) {
	left																	= {center + ::cho::SCoord2<_tCoord>{-halfSizeBox		, halfSizeBox - 1	}, center + ::cho::SCoord2<_tCoord>{-halfSizeBox		,-halfSizeBox		}};
	top																		= {center + ::cho::SCoord2<_tCoord>{ halfSizeBox - 1	, halfSizeBox - 1	}, center + ::cho::SCoord2<_tCoord>{-halfSizeBox		, halfSizeBox - 1	}};
	right																	= {center + ::cho::SCoord2<_tCoord>{ halfSizeBox - 1	, halfSizeBox - 1	}, center + ::cho::SCoord2<_tCoord>{ halfSizeBox - 1	,-halfSizeBox		}};
	bottom																	= {center + ::cho::SCoord2<_tCoord>{ halfSizeBox - 1	,-halfSizeBox		}, center + ::cho::SCoord2<_tCoord>{-halfSizeBox		,-halfSizeBox		}};
	return 0;
}

struct SAABBCache {	
					::cho::SLine2D<float>								RectangleSegments	[4]						= {};
					::cho::SCoord2<float>								CollisionPoints		[4]						= {};
					bool												Collision			[4]						= {};
};

static				::cho::error_t										updateLaserCollision
	( const ::cho::SLine2D<float>														& projectilePath
	, ::SAABBCache																		aabbCache
	, const cho::SCoord2<float>															& posXHair	
	, float																				halfSizeBox	
	, ::cho::array_pod<cho::SCoord2<float>>												& collisionPoints
	)
{ // Check powerup
	::buildAABBSegments(posXHair, halfSizeBox
		, aabbCache.RectangleSegments[0]
		, aabbCache.RectangleSegments[1]
		, aabbCache.RectangleSegments[2]
		, aabbCache.RectangleSegments[3]
		);
	::cho::error_t																result											= 0;
	for(uint32_t iSeg = 0; iSeg < 4; ++iSeg) {
		::cho::SCoord2<float>														& collision										= aabbCache.CollisionPoints[iSeg];
		if(1 == ::segment_segment_intersect(projectilePath, aabbCache.RectangleSegments[iSeg], collision)) {
			bool																		bFound											= false;
			for(uint32_t iS2 = 0; iS2 < iSeg; ++iS2) {
				if(collision == aabbCache.CollisionPoints[iS2]) {
					bFound																	= true;
					info_printf("Discarded collision point.");
					break;
				}
			}
			if(false == bFound) {
				result																	= 1;
				aabbCache.Collision	[iSeg]												= true;;
 				collisionPoints.push_back(collision);
			}
		}
	}
	return result;
}

					::cho::error_t										updateShots
	( ::SApplication																	& applicationInstance
	, const ::cho::array_view<::SApplication::TParticleSystem::TIntegrator::TParticle>	& particleDefinitions
	)
{ 
	applicationInstance.StuffToDraw.CollisionPoints.clear();
	::SAABBCache									aabbCache;
	for(uint32_t iProjectilePath = 0, projectilePathCount = applicationInstance.StuffToDraw.ProjectilePaths.size(); iProjectilePath < projectilePathCount; ++iProjectilePath) {
		const ::SLaserToDraw							& laserToDraw									= applicationInstance.StuffToDraw.ProjectilePaths[iProjectilePath];
		const ::cho::SLine2D<float>						& projectilePath								= laserToDraw.Segment;
		{ // Check powerup
			const cho::SCoord2<float>						& posXHair				= applicationInstance.CenterPositionPowerup;
			float											halfSizeBox				= (float)applicationInstance.TextureCenters[GAME_TEXTURE_POWERUP0].x / 4 * 3;
			::updateLaserCollision(projectilePath, aabbCache, posXHair, halfSizeBox, applicationInstance.StuffToDraw.CollisionPoints);
		}
		{ // Check enemy
			const cho::SCoord2<float>						& posXHair				= applicationInstance.CenterPositionEnemy;
			float											halfSizeBox				= (float)applicationInstance.TextureCenters[GAME_TEXTURE_ENEMY].x / 4 * 3;
			::updateLaserCollision(projectilePath, aabbCache, posXHair, halfSizeBox, applicationInstance.StuffToDraw.CollisionPoints);
			static constexpr const ::cho::SCoord2<float>								reference	= {1, 0};
			::cho::SCoord2<float>														vector;
			for(uint32_t iGhost = 0; iGhost < 5; ++iGhost) {
				vector																	= reference * (64 * sin(applicationInstance.Framework.FrameInfo.Seconds.Total));
				vector.Rotate(::cho::math_2pi / 5 * iGhost + applicationInstance.GhostTimer);
				::updateLaserCollision(projectilePath, aabbCache, posXHair + vector, halfSizeBox, applicationInstance.StuffToDraw.CollisionPoints);
			}
		}
	}

	typedef	::SApplication::TParticleSystem										TParticleSystem;
	typedef	TParticleSystem::TParticleInstance									TParticleInstance;
	::cho::array_pod<TParticleInstance>											& particleInstances							= applicationInstance.ParticleSystem.Instances;
	TParticleSystem::TIntegrator												& particleIntegrator						= applicationInstance.ParticleSystem.Integrator;
	for(uint32_t iCollision = 0, collisionCount = applicationInstance.StuffToDraw.CollisionPoints.size(); iCollision < collisionCount; ++iCollision)
		for(uint32_t i=0; i < 10; ++i) {
			::cho::SCoord2<float>	angle	= {(float)-(rand() % 20) - 10, (float)(rand() % 20 - 1 - 10)};
			angle.Normalize();
			::addParticle(PARTICLE_TYPE_DEBRIS, particleInstances, particleIntegrator, applicationInstance.StuffToDraw.CollisionPoints[iCollision], angle, (float)(rand() % 400) + 100, particleDefinitions);
		}
	return 0;
} 
					::cho::error_t										updateGUI									(::SApplication& applicationInstance)					{ 
	applicationInstance.LineOfFire											= false;
	applicationInstance.StuffToDraw.CollisionPoints.clear();
	{
		::SAABBCache																aabbCache;
		{
			const cho::SCoord2<float>													& posXHair				= applicationInstance.CenterPositionEnemy;
			float																		halfSizeBox				= (float)applicationInstance.TextureCenters[GAME_TEXTURE_ENEMY].x / 4 * 3;
			::cho::SLine2D<float>														projectilePath			= {applicationInstance.CenterPositionShip, applicationInstance.CenterPositionShip + ::cho::SCoord2<float>{10000, }};
			if(1 == ::updateLaserCollision(projectilePath, aabbCache, posXHair, halfSizeBox, applicationInstance.StuffToDraw.CollisionPoints))
				applicationInstance.LineOfFire											= true;
		}
		{
			const cho::SCoord2<float>						& posXHair				= applicationInstance.CenterPositionPowerup;
			float											halfSizeBox				= (float)applicationInstance.TextureCenters[GAME_TEXTURE_POWERUP0].x / 4 * 3;
			::cho::SLine2D<float>							projectilePath			= {applicationInstance.CenterPositionShip, applicationInstance.CenterPositionShip + ::cho::SCoord2<float>{10000, }};
			if(1 == ::updateLaserCollision(projectilePath, aabbCache, posXHair, halfSizeBox, applicationInstance.StuffToDraw.CollisionPoints))
				applicationInstance.LineOfFire											= true;
		}
	}
	for(uint32_t iProjectilePath = 0, projectilePathCount = applicationInstance.StuffToDraw.ProjectilePaths.size(); iProjectilePath < projectilePathCount; ++iProjectilePath) {
		const cho::SCoord2<float>						& posXHair				= applicationInstance.CenterPositionCrosshair;
		float											halfSizeBox				= 5.0f;
		::cho::SLine2D<float>							rectangleSegments[]		= 
			{ {posXHair + ::cho::SCoord2<float>{ halfSizeBox - 1, halfSizeBox - 1}, posXHair + ::cho::SCoord2<float>{ halfSizeBox - 1	,-halfSizeBox}}
			, {posXHair + ::cho::SCoord2<float>{-halfSizeBox	, halfSizeBox - 1}, posXHair + ::cho::SCoord2<float>{-halfSizeBox		,-halfSizeBox}}
			};

		const ::SLaserToDraw							& laserToDraw			= applicationInstance.StuffToDraw.ProjectilePaths[iProjectilePath];
		const ::cho::SLine2D<float>						& projectilePath		= laserToDraw.Segment;
		::cho::SCoord2<float>							collisions	[::cho::size(rectangleSegments)]	= {};
		for(uint32_t iSeg = 0; iSeg < ::cho::size(rectangleSegments); ++iSeg) {
			::cho::SCoord2<float>							& collision										= collisions		[iSeg];
			const ::cho::SLine2D<float>						& segSelected									= rectangleSegments	[iSeg]; 
			if(line_line_intersect(projectilePath, segSelected, collision)) {
				bool											bFound											= false;
				for(uint32_t iS2 = 0; iS2 < iSeg; ++iS2) {
					if(collision == collisions[iS2]) {
						bFound = true;
						info_printf("Discarded collision point.");
						break;
					}
				}
				if(false == bFound)
 					applicationInstance.StuffToDraw.CollisionPoints.push_back(collision);
			}
		}
	}
	return 0;
}
