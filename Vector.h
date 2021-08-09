#pragma once

#include <memory.h>
#include <initializer_list>

#include "../BasicIterator/BasicIterator.h"

namespace UltimaAPI
{
	template <typename type>  class Vector;
}

template <typename type>
class UltimaAPI::Vector
{
	double mul_alloc = 1.6487; // sqrt(e)
	
	__forceinline static constexpr const size_t	max_bytes()
	{
		return sizeof(pointer) - sizeof(container::use);
	}

	enum	config : __int8
	{
		bits_clear = 0,

		bit_init	= 1 << 0,
		bit_always_using_pointer = 1 << 1,

		bit_pointer	= 1 << 4,
		bit_needed_swap = 1 << 5,
	};
	struct	pointer
	{
		size_t used;
		size_t allocated;
		void* start, *last;
	};
	struct	container
	{
		__forceinline decltype(auto)	used()	{ return use; }
		__forceinline decltype(auto)	start()	{ return reinterpret_cast<type*>(&container); }
		__forceinline decltype(auto)	last()	{ return reinterpret_cast<type*>(&container) + use; }

		decltype(auto)	right()
		{ 
			if (use > 0)
				return reinterpret_cast<type*>(&container) + use - 1;
			else return reinterpret_cast<type*>(&container);
		}

		unsigned __int8	use;
		unsigned __int8	container[max_bytes()];
	};
	union 
	{
		pointer p;
		container c;
	};
	__int8 cfg;
public:
	using iterator = BasicIterator<type>;
	using const_iterator = BasicIterator<const type>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
private:
	static_assert(!(sizeof(Vector::pointer) - sizeof(Vector::container)), L"Unequal memory structure used");

	__forceinline static constexpr const size_t	max_elements()
	{
		return max_bytes() / sizeof(type);
	}

	decltype(auto) allocate(size_t al) noexcept
	{
		if (al)
		{
			if (swaped(al))
				pointer(al);
		}
		else free();
	}

	decltype(auto) swaped(size_t al)
	{
		if (cfg & config::bit_pointer)
		{
			if (max_elements() >= al && p.start)
			{
				void* block;
				memcpy(c.start(), block = p.start, (c.use = p.used > al ? al : p.used) * sizeof(type));
				delete[] block;
				cfg &= ~config::bit_pointer;
				return false;
			}
			else return true;
		}
		else
		{
			if (max_elements() < al)
			{
				void* block;
				p.start = memcpy(block = new type[p.allocated = al], c.start(), c.use * sizeof(type));
				reinterpret_cast<type*&>(p.last) = reinterpret_cast<type*>(p.start) + (p.used = c.use);
				cfg |= config::bit_pointer;
				return false;
			}
			else return true;
		}
	}
	decltype(auto) pointer(size_t al)
	{
		if (!p.start)
		{
			p.last = p.start = new type[p.allocated = al];
		}
		else if (al == p.allocated); // maybe adding code to do something!
		else
		{
			void* block;
			memcpy(block = new type[p.allocated = al], p.start, (p.used = p.used > al ? al : p.used) * sizeof(type));
			delete[] p.start;
			reinterpret_cast<type*&>(p.last) = reinterpret_cast<type*>(p.start = block) + p.used;
		}
		cfg |= config::bit_pointer;
	}
public:
	decltype(auto) push_back(type val) noexcept
	{
		if (cfg & config::bit_pointer)
		{  
			if (p.used >= p.allocated)
				allocate(p.allocated * mul_alloc + 1);
		}
		else if (c.use >= max_elements())
			allocate(c.use * mul_alloc + 1);

		if (cfg & config::bit_pointer)
		{
			if (p.used > 0)
				++reinterpret_cast<type*&>(p.last);
			*reinterpret_cast<type*>(p.last) = val;
			++p.used;
		}
		else
		{
			*reinterpret_cast<type*>(c.last()) = val;
			++c.use;
		}
	}
	decltype(auto) pop_back() noexcept
	{
		if (cfg & config::bit_pointer)
		{
			if (p.used > 0)
			{
				--reinterpret_cast<type*&>(p.last);
				--p.used;
			}
		}
		else if (c.use > 0)
			--c.use;
	}
	decltype(auto) insert(size_t place, type val) noexcept
	{
		if (cfg & config::bit_pointer)
		{
			if (place >= p.allocated)
			{
				allocate((p.used = place) * mul_alloc + 1);
				*p.last = val;
			}
			else if (place > p.used)
			{
				*(p.last = p.start + (p.used = place)) = val;
			}
			else p.start[place] = val;
		}
		else
		{
			if (place >= max_elements())
			{
				allocate((p.used = place) * mul_alloc + 1);
				*p.last = val;
			}
			else if (place > c.use)
			{
				c.use = place;
				*c.last() = val;
			}
			else c.start()[place] = val;
		}
	}
	decltype(auto) insert(size_t place, type* val, size_t count) noexcept
	{
		if (cfg & config::bit_pointer)
		{
			if (place + count >= p.allocated)
				allocate((p.used = place + count) * mul_alloc + 1);
		}
		else if (place + count >= max_elements())
			allocate((p.used = place + count) * mul_alloc + 1);

		if (cfg & config::bit_pointer)
		{
			memcpy(reinterpret_cast<type*&>(p.start) + place, val, count * sizeof(type));
			p.last = reinterpret_cast<type*>(p.start) + p.used;
		}
		else
		{
			memcpy(p.start() + place, val, count * sizeof(type));
			if (place + count > c.use)
				c.use = place + count;
		}
	}
	decltype(auto) size() noexcept
	{
		if (cfg & config::bit_pointer)
			return p.used;
		else return size_t(c.use);
	}
	decltype(auto) copy(Vector<type>* v) noexcept
	{
		if (cfg & config::bit_pointer)
		{
			v->allocate(p.allocated);
			if (p.used)
				memcpy(v->p.start, p.start, (v->p.used = p.used) * sizeof(type));
		}
		else if (c.use)
			memcpy(v->c.start(), c.start(), (v->c.use = c.use) * sizeof(type));
	}
	decltype(auto) clear() noexcept
	{
		if (cfg & config::bit_pointer)
		{
			p.last = p.start;
			p.used = 0;
		}
		else c.use = 0;
	}
	decltype(auto) back() noexcept
	{
		if (cfg & config::bit_pointer)
			return *reinterpret_cast<type*>(p.last);
		else return *c.last();
	}
	decltype(auto) capacity() noexcept
	{
		if (cfg & config::bit_pointer)
			return p.allocated;
		else max_elements();
	}
	decltype(auto) data() noexcept
	{
		if (cfg & config::bit_pointer)
			return reinterpret_cast<type*>(p.start);
		else return c.start();
	}
	decltype(auto) swap(Vector<type>& v) noexcept
	{
		std::swap(*this, v);
	}
	decltype(auto) empty() noexcept
	{
		if (cfg & config::bit_pointer)
			return p.used == 0;
		else return c.use == 0;
	}
	decltype(auto) resize(size_t sz) noexcept
	{
		if (!(cfg & config::bit_pointer) && sz > max_elements())
			allocate(sz);

		if (cfg & config::bit_pointer)
		{
			if ((p.used = sz) <= p.allocated)
				reinterpret_cast<type*&>(p.last) = reinterpret_cast<type*>(p.start) + p.used;
			else allocate(p.used);
		}
		else c.use = sz;
	}
	decltype(auto) free() noexcept
	{
		p.allocated = p.used = 0;
		if (cfg & config::bit_pointer && p.start)
			delete[] p.start;
		p.last = p.start = nullptr;
	}
	decltype(auto) reserve(size_t sz) noexcept
	{
		allocate(sz);
	}
	decltype(auto) rate(double val) noexcept
	{
		return double&&(mul_alloc = val);
	}
	decltype(auto) rate() noexcept
	{
		return double&&(mul_alloc);
	}
	decltype(auto) max_size() noexcept
	{
		return (1 << (8 * sizeof(p.allocated))) / sizeof(type);
	}
	decltype(auto) size_of() noexcept
	{
		return sizeof(type);
	}
	decltype(auto) shrink_to_fit() noexcept
	{
		if (cfg & config::bit_pointer && p.used < p.allocated)
			allocate(p.used);
	}

	decltype(auto) begin() noexcept
	{
		if (cfg & config::bit_pointer)
			return iterator(reinterpret_cast<type*>(p.start));
		else return iterator(c.start());
	}
	decltype(auto) end() noexcept
	{
		if (cfg & config::bit_pointer)
			return iterator(reinterpret_cast<type*>(p.start) + p.used);
		else return iterator(c.last());
	}
	decltype(auto) cbegin() const noexcept
	{
		if (cfg & config::bit_pointer)
			return const_iterator(reinterpret_cast<type*>(p.start));
		else return const_iterator(c.start());
	}
	decltype(auto) cend() const noexcept
	{
		if (cfg & config::bit_pointer)
			return const_iterator(reinterpret_cast<type*>(p.start) + p.used);
		else return const_iterator(c.last());
	}
	decltype(auto) rbegin() noexcept
	{
		return reverse_iterator(end());
	}
	decltype(auto) rend() noexcept
	{
		return reverse_iterator(begin());
	}
	decltype(auto) crbegin() const noexcept
	{
		return const_reverse_iterator(cend());
	}
	decltype(auto) crend() const noexcept
	{
		return const_reverse_iterator(cbegin());
	}

	decltype(auto) operator()(std::initializer_list<type> v) noexcept
	{
		if (v.size() > max_elements())
		{
			p.start = nullptr;
			allocate(p.used = v.size());
			memcpy(p.start, v.begin(), p.used * sizeof(type));
		}
		else memcpy(c.start(), v.begin(), (c.use = v.size()) * sizeof(type));
	}
	decltype(auto) operator~() noexcept
	{
		free();
	}
	decltype(auto) operator^=(Vector<type>& v) noexcept
	{
		swap(v);
	}
	decltype(auto) operator+=(type c) noexcept
	{
		push_back(c);
	}
	decltype(auto) operator+=(Vector<type> v) noexcept
	{
		void* s;
		size_t t;
		if (v.cfg & config::bit_pointer)
		{
			t = v.p.used;
			s = v.p.start;
		}
		else
		{
			t = v.c.use;
			s = v.c.start();
		}

		if (cfg & config::bit_pointer)
			insert(p.used, s, t);
		else insert(c.use, s, t);
	}
	decltype(auto) operator+=(Vector<type>& v) noexcept
	{
		void* s;
		size_t t;
		if (v.cfg & config::bit_pointer)
		{
			t = v.p.used;
			s = v.p.start;
		}
		else
		{
			t = v.c.use;
			s = v.c.start();
		}

		if (cfg & config::bit_pointer)
			insert(p.used, s, t);
		else insert(c.use, s, t);
	}
	decltype(auto) operator+=(Vector<type>&& v) noexcept
	{
		void* s;
		size_t t;
		if (v.cfg & config::bit_pointer)
		{
			t = v.p.used;
			s = v.p.start;
		}
		else
		{
			t = v.c.use;
			s = v.c.start();
		}

		if (cfg & config::bit_pointer)
			insert(p.used, s, t);
		else insert(c.use, s, t);
	}
	decltype(auto) operator+=(const Vector<type> v) const  noexcept
	{
		void* s;
		size_t t;
		if (v.cfg & config::bit_pointer)
		{
			t = v.p.used;
			s = v.p.start;
		}
		else
		{
			t = v.c.use;
			s = v.c.start();
		}

		if (cfg & config::bit_pointer)
			insert(p.used, s, t);
		else insert(c.use, s, t);
	}
	decltype(auto) operator+=(const Vector<type>& v) const noexcept
	{
		void* s;
		size_t t;
		if (v.cfg & config::bit_pointer)
		{
			t = v.p.used;
			s = v.p.start;
		}
		else
		{
			t = v.c.use;
			s = v.c.start();
		}

		if (cfg & config::bit_pointer)
			insert(p.used, s, t);
		else insert(c.use, s, t);
	}
	decltype(auto) operator+=(const Vector<type>&& v) const noexcept
	{
		void* s;
		size_t t;
		if (v.cfg & config::bit_pointer)
		{
			t = v.p.used;
			s = v.p.start;
		}
		else
		{
			t = v.c.use;
			s = v.c.start();
		}

		if (cfg & config::bit_pointer)
			insert(p.used, s, t);
		else insert(c.use, s, t);
	}
	decltype(auto) operator[](size_t i) noexcept
	{
		if (cfg & config::bit_pointer && i >= p.allocated || !(cfg & config::bit_pointer) && i >= max_elements())
			allocate(i * mul_alloc + 1);
		if (cfg & config::bit_pointer)
		{
			if (i >= p.used)
			{
				p.used = i + 1;
				p.last = reinterpret_cast<type*&>(p.start) + p.used;
			}
			return reinterpret_cast<type*&>(p.start)[i];
		}
		else
		{
			c.use = i >= c.use ? i + 1 : c.use;
			return c.start()[i];
		}
	}

	Vector(std::initializer_list<type> v) noexcept
	{
		cfg = config::bit_init;
		if (!max_elements())
			cfg |= config::bit_always_using_pointer | config::bit_pointer;
		this->operator()(v);
	}
	Vector() noexcept
	{
		cfg = config::bits_clear;
		if (!max_elements())
			cfg |= config::bit_always_using_pointer | config::bit_pointer;
		p.last = p.start = nullptr;
		p.allocated = p.used = 0;
	}
	Vector(size_t sz) noexcept
	{
		p.used = 0;
		p.start = nullptr;
		cfg = config::bit_init;
		if (!max_elements())
			cfg |= config::bit_always_using_pointer | config::bit_pointer;
		allocate(sz);
	}
	Vector(size_t sz, type* ray) noexcept
	{
		p.used = 0;
		p.start = nullptr;
		cfg = config::bit_init;
		if (!max_elements())
			cfg |= config::bit_always_using_pointer | config::bit_pointer;
		insert(0, ray, sz);
	}
	Vector(Vector<type>& v) noexcept
	{
		cfg = config::bit_init;
		v.copy(this);
	}

	~Vector() noexcept
	{
		free();
	}
};
