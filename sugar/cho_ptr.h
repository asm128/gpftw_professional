#include "cho_sync.h"
#include "cho_log.h"

#ifndef CHO_PTR_H_0297349827348923
#define CHO_PTR_H_0297349827348923

namespace cho 
{
	template<typename _tInstance>
	struct cho_ref {
		typedef		_tInstance									TInstance;

					_tInstance									* Instance;
					refcount_t									References;
	};
	
	template<typename _tNCO>
						::cho::cho_ref<_tNCO> *				ref_acquire							(::cho::cho_ref<_tNCO>* cho_reference)									{
		if(cho_reference)
			cho_sync_increment(cho_reference->References);
		return cho_reference;
	};

	template<typename _tNCO>
						::cho::error_t						ref_release							(::cho::cho_ref<_tNCO>** cho_reference)									{
		typedef	::cho::cho_ref<_tNCO>								TRef;
		TRef														* oldRef							= *cho_reference;
		*cho_reference											= 0;
		if(oldRef)
			switch(cho_sync_decrement(oldRef->References)) {
			case -1: error_printf("Reference count error!"); return -1;
			case  0:
				if(oldRef->Instance)
					oldRef->Instance->~_tNCO();
				else
					error_printf("Instance is NULL! At the point of writing this code it wouldn't make any sense.");
				::cho::cho_free(oldRef->Instance);
				::cho::cho_free(oldRef);
				break;
			}
		return 0;
	};

	template<typename _tOBJ>
						_tOBJ *								ref_allocate						(::cho::cho_ref<_tOBJ>** cho_reference)									{
		typedef	::cho::cho_ref<_tOBJ>								TRef;
		TRef														* newRef							= (TRef*)::cho::cho_malloc(sizeof(TRef));
		retnul_error_if(0 == newRef, "Failed to allocate reference! Out of memory?");
		if(0 == (newRef->Instance = (_tOBJ*)::cho::cho_malloc(sizeof(_tOBJ)))) {
			::cho::cho_free(newRef);
			error_printf("Failed to allocate instance! Out of memory?");
			return 0;
		}
		newRef->References										= 1;
		TRef														* oldRef							= *cho_reference;
		*cho_reference											= newRef;
		::cho::ref_release(&oldRef);
		return (*cho_reference)->Instance;
	};

	template<typename _tOBJ, typename... _tArgs>
						_tOBJ *								ref_create							(::cho::cho_ref<_tOBJ>** cho_reference, _tArgs&&... argsConstructor)	{
		typedef	::cho::cho_ref<_tOBJ>								TRef;
		TRef														* newRef							= 0;
		retnul_error_if(0 == ::cho::ref_allocate(&newRef), "Failed to allocate reference");
		new (newRef->Instance) _tOBJ{argsConstructor...};

		TRef														* oldRef							= *cho_reference;
		*cho_reference											= newRef;
		::cho::ref_release(&oldRef);
		return (*cho_reference)->Instance;
	};

	template<typename _tNCO>
	class ptr_nco {
	protected:
							::cho::cho_ref<_tNCO>				* Reference							= 0;
	public:
		typedef				::cho::ptr_nco<_tNCO>				TNCOPtr;
		typedef				::cho::cho_ref<_tNCO>				TRef;

		inline													~ptr_nco							()															noexcept	{ ::cho::ref_release(&Reference);													}
		inline constexpr										ptr_nco								()															noexcept	= default;
		inline													ptr_nco								(const TNCOPtr& other)										noexcept	{ Reference = ::cho::ref_acquire(other.Reference);									}
		inline constexpr										ptr_nco								(TNCOPtr&& other)											noexcept	{ Reference = other.Reference; other.Reference = 0;									}
		inline													ptr_nco								(TRef* other)												noexcept	{ Reference = other;																}

		inline constexpr	operator							_tNCO	*							()															noexcept	{ return Reference ? Reference->Instance : 0;										}

		inline constexpr	bool								operator==							(const TNCOPtr& other)								const	noexcept	{ return Reference == other.Reference;												}
		inline constexpr	bool								operator!=							(const TNCOPtr& other)								const	noexcept	{ return !operator==(other);														}

		inline constexpr	TNCOPtr								operator =							(const TNCOPtr& other)										noexcept	{ TRef* oldInstance = Reference; Reference = ::cho::ref_acquire(other.Reference);	::cho::ref_release(&oldInstance); return *this; }
		inline constexpr	TNCOPtr								operator =							(TNCOPtr&& other)											noexcept	{ TRef* oldInstance = Reference; Reference = other.Reference; other.Reference = 0;	::cho::ref_release(&oldInstance); return *this; }
		inline constexpr	TNCOPtr								operator =							(TRef* other)												noexcept	{ TRef* oldInstance = Reference; Reference = other;									::cho::ref_release(&oldInstance); return *this; }

		inline				_tNCO*								operator->							()															noexcept	{ return Reference->Instance; }
		inline				const _tNCO*						operator->							()													const	noexcept	{ return Reference->Instance; }

		inline				TRef**								operator &							()															noexcept	{ return &Reference;	}

		inline constexpr	const TRef*							get_ref								()													const	noexcept	{ return Reference;	}

		template<typename _tNCOOther>
		inline				_tNCO*								as									(_tNCOOther** other)										noexcept	{ return *other = (Reference ? (_tNCOOther*)Reference->Instance : 0);				}

		template<typename _tNCOOther>
		inline				_tNCO*								as									(TNCOPtr& other)											noexcept	{ other = ::cho::ref_acquire(Reference); return 0;									}
	};
	
	template<typename _tOBJ>
	class ptr_obj : public ::cho::ptr_nco<_tOBJ> {
	public:
		typedef				::cho::ptr_nco<_tOBJ>				TNCOPtr;
		typedef				::cho::ptr_obj<_tOBJ>				TOBJPtr;
		typedef				::cho::cho_ref<_tOBJ>				TRef;
		using				TNCOPtr								::Reference;

		inline				const _tOBJ*						operator->							()													const	noexcept	{ return Reference;																	}
		inline				_tOBJ*								operator->							()															noexcept	{ return (0 == Reference) ? ::cho::ref_create(&Reference) : Reference->Instance;	}

		template<typename... _tArgsConstructor>
		inline				_tOBJ*								create								(_tArgsConstructor&&... argsConstructor)					noexcept	{ return ::cho::ref_create(&Reference, argsConstructor...);							}
		inline				_tOBJ*								allocate							()															noexcept	{ return ::cho::ref_allocate(&Reference);											}
	};

	template<typename _tPOD>
	class ptr_pod : public ::cho::ptr_nco<_tPOD> {
	public:
		typedef				::cho::ptr_nco<_tPOD>				TNCOPtr;
		typedef				::cho::ptr_pod<_tPOD>				TPODPtr;
		typedef				::cho::cho_ref<_tPOD>				TRef;
		using				TNCOPtr								::Reference;

		inline				const _tPOD*						operator->							()													const	noexcept	{ return Reference->Instance;														}
		inline				_tPOD*								operator->							()															noexcept	{ return (0 == Reference) ? ::cho::ref_allocate(&Reference) : Reference->Instance;	}

		template<typename... _tArgsConstructor>
		inline				_tPOD*								create								(_tArgsConstructor&&... argsConstructor)					noexcept	{ return ::cho::ref_create(&Reference, argsConstructor...);							}
		inline				_tPOD*								allocate							()															noexcept	{ return ::cho::ref_allocate(&Reference);											}
	};
} // namespace

#endif // CHO_PTR_H_0297349827348923
