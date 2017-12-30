#include <cstdint>

#ifndef FTW_FRAMEINFO_H_928374654
#define FTW_FRAMEINFO_H_928374654

namespace cho
{
	struct SFrameSeconds {
				double															Total												;
				double															LastFrame											;
				double															LastFrameHalfSquared								;
		// Helper
		inline	void															UpdateFromTime										(double secondsToAdd)						noexcept	{ 
			Total																	+= LastFrame											= secondsToAdd; 
			LastFrameHalfSquared													=  secondsToAdd * secondsToAdd * 0.5;
		}
	};

	struct SFrameMicroseconds {
				uint64_t														Total												;
				uint64_t														LastFrame											;
		// Helper
		inline	void															UpdateFromTime										(uint64_t microsecondsToAdd)				noexcept	{ Total += LastFrame = microsecondsToAdd; }
	};

	struct SFrameInfo {
				uint64_t														FrameNumber											= 0;
				uint64_t														FramesPerSecond										= 0;
				uint64_t														FramesThisSecond									= 0;
				uint64_t														FrameStep											= 0;
				double															AverageFrameTime									= 0;
				SFrameMicroseconds												Microseconds										= {};
				SFrameSeconds													Seconds												= {};
		
				void															Frame												(uint64_t timeElapsedMicroseconds)						{	// Set last frame time and number.
					++FrameNumber;
					++FramesThisSecond;
					Microseconds	.UpdateFromTime(timeElapsedMicroseconds);
					Seconds			.UpdateFromTime(timeElapsedMicroseconds / 1000000.0);
					FrameStep														+= timeElapsedMicroseconds;
					while(FrameStep >= 1000000) {
						FrameStep														-= 1000000;
						FramesPerSecond													= FramesThisSecond;
						AverageFrameTime												= FramesThisSecond ? 1.0 / FramesThisSecond : 0;
					}
				}
	};
} // namespace

#endif // FTW_FRAMEINFO_H_928374654
