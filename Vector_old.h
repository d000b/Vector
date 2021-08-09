#pragma once

#include <memory.h>
#include <initializer_list>

#include "../BasicIterator/BasicIterator.h"
#include "../Memory/util.h"

namespace UltimaAPI
{
	template <typename type>  class Vector;
}

template <typename __type__ = int>
class UltimaAPI::Vector
{
public:
	using value = __type__;
	using pointer = value*;
	using refvalue = value&;
private:
	double mul_alloc = 1.6487; // sqrt(e)

	size_t	used;
	size_t	allocated;
	pointer	start;
public:
	static const size_t npos = -1;

	using iterator = BasicIterator<value>;
	using const_iterator = BasicIterator<const value>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
private:
	decltype(auto) allocate(size_t al) noexcept
	{
		if (al)
		{
			if (!start)
				start = new value[allocated = al];
			else if (al == allocated); // maybe adding code to do something!
			else
			{
				value* block;
				memcpy(block = new value[allocated = al], start, (used = used > al ? al : used) * sizeof(value));
				delete[] start;
				start = block;
			}
		}
		else free();
	}
	decltype(auto) insert_correct(size_t place)
	{
		bool ret = true;
		if (place > used)
		{
			used = place;
			ret = false;
		}
		if (used >= allocated)
			allocate(used * mul_alloc + 1);
		return ret;
	}
	decltype(auto) insert_correct(size_t place, size_t count)
	{
		bool ret = true;
		if (place + count > used)
		{
			used = place + count;
			ret = false;
		}
		if (used >= allocated)
			allocate(used * mul_alloc + 1);
		return ret;
	}

	decltype(auto) forward_sequence(size_t idx, size_t len, pointer val)
	{
		for (size_t s = 0; s < len; s++)
			if (start[idx + s] != val[s])
				return npos;
		return idx;
	}
	decltype(auto) backward_sequence(size_t idx, size_t len, pointer val)
	{
		for (size_t s = len - 1; s != npos; s--)
			if (start[idx + s] != val[s])
				return npos;
		return idx;
	}
public:
	enum eSequence
	{
		seq_null,
		seq_forward,
		seq_backward
	};
public:
	decltype(auto) push_back(value val) noexcept
	{
		if (used >= allocated)
			allocate(alloc_step());
		start[used] = val;
		++used;
	}
	decltype(auto) push_back(pointer val, size_t c) noexcept
	{
		move_insert(used, val, c);
	}
	decltype(auto) pop_back() noexcept
	{
		if (used > 0)
			--used;
	}
	
	decltype(auto) move_insert(size_t place, value val)
	{
		if (insert_correct(place))
			Memory::memcpy(start + place + 1, start + place, used - place);
		start[place] = val;
	}
	decltype(auto) move_insert(size_t place, pointer val, size_t count)
	{
		if (insert_correct(place, count))
			Memory::memcpy(start + place + count, start + place, used - place);
		Memory::memcpy(start + place, val, count * sizeof(value));
	}
	decltype(auto) insert(size_t place, value val) noexcept
	{
		insert_correct(place);
		start[place] = val;
	}
	decltype(auto) insert(size_t place, pointer val, size_t count) noexcept
	{
		insert_correct(place, count);
		memcpy(start + place, val, count * sizeof(value));
	}
	decltype(auto) insert(size_t place, std::initializer_list<value> list) noexcept
	{
		insert_correct(place, list.size());
		memcpy(start + place, list.begin(), list.size() * sizeof(value));
	}
	decltype(auto) insert(std::initializer_list<value> list) noexcept
	{
		insert_correct(used, list.size());
		memcpy(start + used, list.begin(), list.size() * sizeof(value));
	}
	decltype(auto) size() noexcept
	{
		return used;
	}
	decltype(auto) size() const noexcept
	{
		return used;
	}
	decltype(auto) capacity() noexcept
	{
		return allocated;
	}
	decltype(auto) capacity() const noexcept
	{
		return allocated;
	}
	decltype(auto) alloc_step()
	{
		return size_t(allocated * mul_alloc + 1);
	}
	decltype(auto) copy(Vector<value>* v) noexcept
	{
		v->allocate(allocated);
		if (used)
			memcpy(v->start, start, (v->used = used) * sizeof(value));
	}
	decltype(auto) copy(Vector<value>* v) const noexcept
	{
		v->allocate(allocated);
		if (used)
			memcpy(v->start, start, (v->used = used) * sizeof(value));
	}
	decltype(auto) clear() noexcept
	{
		used = 0;
	}
	decltype(auto) clear() const noexcept
	{
		used = 0;
	}
	decltype(auto) erase(size_t i) noexcept
	{
		start[i] = 0; // ??
		// memcpy(start + i, )
	}
	decltype(auto) last() noexcept
	{
		return start + used;
	}
	decltype(auto) last() const noexcept
	{
		return start + used;
	}
	decltype(auto) back() noexcept
	{
		return *(start + used);
	}
	decltype(auto) back() const noexcept
	{
		return *(start + used);
	}
	decltype(auto) data() noexcept
	{
		return start;
	}
	decltype(auto) data() const noexcept
	{
		return start;
	}
	decltype(auto) swap(size_t i, size_t j)
	{
		if (i == j)
			return;
		insert_correct(i > j ? i : j);
		std::swap(start[i], start[j]);
	}
	decltype(auto) swap(Vector<value>&& v) noexcept
	{
		std::swap(*this, v);
	}
	decltype(auto) swap(const Vector<value>&& v) const noexcept
	{
		std::swap(*this, v);
	}
	decltype(auto) empty() noexcept
	{
		return used == 0;
	}
	decltype(auto) empty() const noexcept
	{
		return used == 0;
	}
	decltype(auto) resize(size_t sz) noexcept
	{
		if ((used = sz) > allocated)
			allocate(used);
	}
	decltype(auto) free() noexcept
	{
		allocated = used = 0;
		if (start)
			delete[] start;
		start = nullptr;
	}
	decltype(auto) free() const noexcept
	{
		allocated = used = 0;
		if (start)
			delete[] start;
		start = nullptr;
	}
	decltype(auto) reserve(size_t sz) noexcept
	{
		allocate(sz);
	}
	decltype(auto) rate(double val) noexcept
	{
		return double&(mul_alloc = val);
	}
	decltype(auto) rate() noexcept
	{
		return double&(mul_alloc);
	}
	decltype(auto) max_size() noexcept
	{
		return (1 << (8 * sizeof(allocated))) / sizeof(value);
	}
	decltype(auto) size_of() noexcept
	{
		return sizeof(value);
	}
	decltype(auto) shrink_to_fit() noexcept
	{
		if (used < allocated)
			allocate(used);
	}

	decltype(auto) begin() noexcept
	{
		return iterator(start);
	}
	decltype(auto) end() noexcept
	{
		return iterator(last());
	}
	decltype(auto) cbegin() const noexcept
	{
		return const_iterator(start);
	}
	decltype(auto) cend() const noexcept
	{
		return const_iterator(last());
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

	decltype(auto) operator()(std::initializer_list<value> v) noexcept
	{
		start = nullptr;
		allocate(used = v.size());
		memcpy(start, v.begin(), used * sizeof(value));
	}
	decltype(auto) operator~() noexcept
	{
		free();
	}
	decltype(auto) operator^=(Vector& v) noexcept
	{
		swap(v);
	}
	decltype(auto) operator+=(value c) noexcept
	{
		push_back(c);
	}
	decltype(auto) operator+=(Vector v) noexcept
	{
		insert(used, v.start, v.used);
	}
	decltype(auto) operator+=(Vector& v) noexcept
	{
		insert(used, v.start, v.used);
	}
	decltype(auto) operator+=(Vector&& v) noexcept
	{
		insert(used, v.start, v.used);
	}
	decltype(auto) operator+=(const Vector<value> v) const  noexcept
	{
		insert(used, v.start, v.used);
	}
	decltype(auto) operator+=(const Vector<value>& v) const noexcept
	{
		insert(used, v.start, v.used);
	}
	decltype(auto) operator+=(const Vector<value>&& v) const noexcept
	{
		insert(used, v.start, v.used);
	}
	decltype(auto) operator[](size_t i) noexcept
	{
		if (i >= allocated)
			allocate(i * mul_alloc + 1);
		if (i >= used)
			used = i + 1;
		return refvalue(start[i]);
	}
	decltype(auto) operator[](size_t i) const noexcept
	{
		return refvalue(start[i]);
	}

	__inline decltype(auto) operator<(size_t i) const noexcept
	{
		return used < i;
	}
	__inline decltype(auto) operator>(size_t i) const noexcept
	{
		return used > i;
	}
	__inline decltype(auto) operator<=(size_t i) const noexcept
	{
		return used <= i;
	}
	__inline decltype(auto) operator>=(size_t i) const noexcept
	{
		return used >= i;
	}
	
	Vector(std::initializer_list<value> v) noexcept
	{
		this->operator()(v);
	}
	Vector() noexcept
	{
		start = nullptr;
		allocated = used = 0;
	}
	Vector(size_t sz) noexcept
	{
		allocated = used = 0;
		start = nullptr;
		allocate(sz);
	}
	Vector(size_t sz, pointer ray) noexcept
	{
		allocated = used = 0;
		start = nullptr;
		insert(0, ray, sz);
	}
	Vector(Vector<value>& v) noexcept
	{
		v.copy(this);
	}

	~Vector() noexcept
	{
		free();
	}
};
