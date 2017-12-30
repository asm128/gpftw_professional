/// Copyright 2017 - asm128
#ifndef FTW_AUTO_HANDLER_H_0239840239840238321
#define FTW_AUTO_HANDLER_H_0239840239840238321

namespace cho 
{
	template<typename _tHandle, _tHandle _vDefault>
	struct auto_handler { // I have no idea how to call this type so I'm really sorry.
		typedef				auto_handler<_tHandle, _vDefault>	TWrapper;
							_tHandle										Handle							= _vDefault;
		static constexpr	const _tHandle									VDefault						= _vDefault;

		inline constexpr													auto_handler					()										noexcept	= default;

		inline constexpr													auto_handler					(const _tHandle& other)					noexcept	= delete;
		inline constexpr													auto_handler					(const TWrapper& other)					noexcept	= delete;
		inline constexpr	TWrapper										operator=						(const _tHandle& other)					noexcept	= delete;

		inline constexpr	operator										const _tHandle&					()								const	noexcept	{ return Handle;		}
		inline constexpr	_tHandle*										operator&						()										noexcept	{ return &Handle;		}
		inline constexpr	const _tHandle*									operator&						()								const	noexcept	{ return &Handle;		}
	};
} // namespace

#endif // FTW_AUTO_HANDLER_H_0239840239840238321
