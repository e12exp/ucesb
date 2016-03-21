#ifndef _REFPTR_H_
#define _REFPTR_H_

template<class T> class refptr
{
protected:
	T *ptr;
	int *refcount;
	
	void clear()
	{	
		if(!refcount)
			return;
		
		(*refcount)--;
		if(*refcount == 0)
		{
			delete refcount;
			refcount = NULL;
			
			if(ptr != NULL)
				delete ptr;
			ptr = NULL;
		}
	}
	
public:
	refptr() : ptr(NULL), refcount(NULL) {}
	
	refptr(const refptr<T> &other) : ptr(NULL), refcount(NULL)
	{
		operator=(other);
	}
	
	refptr<T>& operator=(const refptr<T> &other)
	{
		clear();
		ptr = other.ptr;
		refcount = other.refcount;
		if(refcount != NULL)
			(*refcount)++;
		
		return *this;
	}
	
	~refptr()
	{
		clear();
	}
	
	T* operator->() const
	{
		return ptr;
	}
	
	refptr<T>& operator=(const T* obj)
	{
		clear();
		refcount = new int;
		*refcount = 1;
		ptr = const_cast<T*>(obj);
		
		return *this;
	}
	
	operator T*()
	{
		return ptr;
	}
};

#endif
