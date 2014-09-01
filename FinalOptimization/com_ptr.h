#pragma once

template <class T>
class com_ptr
{
private:
	T *inst;

	static void release(T *i)
	{
		if(i)
			i->Release();
	}

	static void acquire(T *i)
	{
		if(i)
			i->AddRef();
	}

public:
	com_ptr(T *ptr = 0)
		: inst(ptr) 
	{
	}

	com_ptr(const com_ptr& ptr)
		: inst(ptr.inst)
	{
		acquire(inst);
	}

	~com_ptr()
	{
		release(inst);
	}

	com_ptr& operator = (const com_ptr& ptr)
	{
		acquire(ptr.inst);
		release(inst);
		inst = ptr.inst;
		return *this;
	}

	T** operator & ()
	{
		return &inst;
	}

	T* operator -> () { return inst; }
	const T* operator -> () const { return inst; }
	operator T* () { return inst; }
	operator const T* () const { return inst; }

	T* const& get() const { return inst; }
};
