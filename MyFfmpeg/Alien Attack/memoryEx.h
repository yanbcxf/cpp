
#define _NOEXCEPT	throw ()

namespace std
{
	// TEMPLATE STRUCT _Get_deleter_pointer_type
	template<class _Val,
	class _Ty>
	struct _Get_deleter_pointer_type
		_GET_TYPE_OR_DEFAULT(pointer,
		_Val *);

	// TEMPLATE CLASS _Unique_ptr_base
	template<class _Ty,
	class _Dx,
		bool _Empty_deleter>
	class _Unique_ptr_base
	{	// stores pointer and deleter
	public:
		typedef typename remove_reference<_Dx>::type _Dx_noref;
		typedef typename _Get_deleter_pointer_type<_Ty, _Dx_noref>::type pointer;

		_Unique_ptr_base(pointer _Ptr, _Dx _Dt)
			: _Myptr(_Ptr), _Mydel(_Dt)
		{	// construct with pointer and deleter
		}

		_Unique_ptr_base(pointer _Ptr)
			: _Myptr(_Ptr)
		{	// construct with pointer and deleter
		}

		template<class _Ptr2,
		class _Dx2>
			_Unique_ptr_base(_Ptr2 _Ptr, _Dx2 _Dt)
			: _Myptr(_Ptr), _Mydel(_Dt)
		{	// construct with compatible pointer and deleter
		}

		template<class _Ptr2>
		_Unique_ptr_base(_Ptr2 _Ptr)
			: _Myptr(_Ptr)
		{	// construct with compatible pointer and deleter
		}

		_Dx_noref& get_deleter()
		{	// return reference to deleter
			return (_Mydel);
		}

		const _Dx_noref& get_deleter() const
		{	// return const reference to deleter
			return (_Mydel);
		}

		pointer _Myptr;	// the managed pointer
		_Dx _Mydel;		// the deleter
	};

	// TEMPLATE CLASS unique_ptr SCALAR
	template<class _Ty,
	class _Dx>	// = default_delete<_Ty>
	class unique_ptr
		: private _Unique_ptr_base<_Ty, _Dx,
		is_empty<_Dx>::value
		|| is_same<default_delete<_Ty>, _Dx>::value>
	{	// non-copyable pointer to an object
	public:
		typedef unique_ptr<_Ty, _Dx> _Myt;
		typedef _Unique_ptr_base<_Ty, _Dx,
			is_empty<_Dx>::value
			|| is_same<default_delete<_Ty>, _Dx>::value> _Mybase;
		typedef typename _Mybase::pointer pointer;
		typedef _Ty element_type;
		typedef _Dx deleter_type;

		using _Mybase::get_deleter;

		unique_ptr() _NOEXCEPT
			: _Mybase(pointer())
		{	// default construct
			static_assert(!is_pointer<_Dx>::value,
				"unique_ptr constructed with null deleter pointer");
		}

		unique_ptr(nullptr_t) _NOEXCEPT
			: _Mybase(pointer())
		{	// null pointer construct
			static_assert(!is_pointer<_Dx>::value,
				"unique_ptr constructed with null deleter pointer");
		}

		_Myt& operator=(nullptr_t) _NOEXCEPT
		{	// assign a null pointer
			reset();
			return (*this);
		}

		explicit unique_ptr(pointer _Ptr) _NOEXCEPT
			: _Mybase(_Ptr)
		{	// construct with pointer
			static_assert(!is_pointer<_Dx>::value,
				"unique_ptr constructed with null deleter pointer");
		}

		unique_ptr(pointer _Ptr,
			typename _If<is_reference<_Dx>::value, _Dx,
			const typename remove_reference<_Dx>::type&>::type _Dt) _NOEXCEPT
			: _Mybase(_Ptr, _Dt)
		{	// construct with pointer and (maybe const) deleter&
		}

		unique_ptr(pointer _Ptr,
			typename remove_reference<_Dx>::type&& _Dt) _NOEXCEPT
			: _Mybase(_Ptr, _STD move(_Dt))
		{	// construct by moving deleter
			static_assert(!is_reference<_Dx>::value,
				"unique_ptr constructed with reference to rvalue deleter");
		}

		unique_ptr(unique_ptr&& _Right) _NOEXCEPT
			: _Mybase(_Right.release(),
			_STD forward<_Dx>(_Right.get_deleter()))
		{	// construct by moving _Right
		}

		template<class _Ty2,
		class _Dx2,
		class = typename enable_if<!is_array<_Ty2>::value
			&& is_convertible<typename unique_ptr<_Ty2, _Dx2>::pointer,
			pointer>::value
			&& ((is_reference<_Dx>::value && is_same<_Dx, _Dx2>::value)
			|| (!is_reference<_Dx>::value
			&& is_convertible<_Dx2, _Dx>::value)),
			void>::type>
			unique_ptr(unique_ptr<_Ty2, _Dx2>&& _Right) _NOEXCEPT
			: _Mybase(_Right.release(),
			_STD forward<_Dx2>(_Right.get_deleter()))
		{	// construct by moving _Right
		}

		template<class _Ty2,
		class = typename enable_if<is_convertible<_Ty2 *, _Ty *>::value
			&& is_same<_Dx, default_delete<_Ty> >::value,
			void>::type>
			unique_ptr(auto_ptr<_Ty2>&& _Right) _NOEXCEPT
			: _Mybase(_Right.release())
		{	// construct by moving _Right
		}

		template<class _Ty2,
		class _Dx2>
			typename enable_if<!is_array<_Ty2>::value
			&& is_convertible<typename unique_ptr<_Ty2, _Dx2>::pointer,
			pointer>::value,
			_Myt&>::type
			operator=(unique_ptr<_Ty2, _Dx2>&& _Right) _NOEXCEPT
		{	// assign by moving _Right
			reset(_Right.release());
			this->get_deleter() = _STD forward<_Dx2>(_Right.get_deleter());
			return (*this);
		}

		_Myt& operator=(_Myt&& _Right) _NOEXCEPT
		{	// assign by moving _Right
			if (this != &_Right)
			{	// different, do the move
				reset(_Right.release());
				this->get_deleter() = _STD forward<_Dx>(_Right.get_deleter());
			}
			return (*this);
		}

		void swap(_Myt& _Right) _NOEXCEPT
		{	// swap elements
			_Swap_adl(this->_Myptr, _Right._Myptr);
			_Swap_adl(this->get_deleter(),
				_Right.get_deleter());
		}

		~unique_ptr() _NOEXCEPT
		{	// destroy the object
			if (this->_Myptr != pointer())
				this->get_deleter()(this->_Myptr);
		}

		typename add_reference<_Ty>::type operator*() const
		{	// return reference to object
			return (*this->_Myptr);
		}

		pointer operator->() const _NOEXCEPT
		{	// return pointer to class object
			return (_STD pointer_traits<pointer>::pointer_to(**this));
		}

		pointer get() const _NOEXCEPT
		{	// return pointer to object
			return (this->_Myptr);
		}

		explicit operator bool() const _NOEXCEPT
		{	// test for non-null pointer
			return (this->_Myptr != pointer());
		}

		pointer release() _NOEXCEPT
		{	// yield ownership of pointer
			pointer _Ans = this->_Myptr;
			this->_Myptr = pointer();
			return (_Ans);
		}

		void reset(pointer _Ptr = pointer()) _NOEXCEPT
		{	// establish new pointer
			pointer _Old = this->_Myptr;
			this->_Myptr = _Ptr;
			if (_Old != pointer())
				this->get_deleter()(_Old);
		}

		unique_ptr(const _Myt&) = delete;
		_Myt& operator=(const _Myt&) = delete;
	};
}