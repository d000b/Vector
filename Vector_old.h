#pragma once

#include <memory.h>

#include "../BasicIterator/BasicIterator.h"
#include "../Memory/util.h"

namespace UltimaAPI
{
	template <typename type>  class Vector;
}

#define INCLUDE_INITIALIZER_LIST 0

#if defined(INCLUDE_INITIALIZER_LIST) && INCLUDE_INITIALIZER_LIST
#include <initializer_list>
#endif

template <typename __type__ = int>
class UltimaAPI::Vector
{
public:
	// Value
	using value = __type__;
	using rvalue = value&&;
	using pointer = value*;
	using reference = value&;

#if defined(INCLUDE_INITIALIZER_LIST) && INCLUDE_INITIALIZER_LIST
	// Initializer_list
	using list = std::initializer_list<value>;
	using list_rvalue = list&&;
	using list_pointer = list*;
	using list_reference = list&;
#endif

	// Vector
	using vector = Vector<value>;
	using vector_rvalue = vector&&;
	using vector_pointer = vector*;
	using vector_reference = vector&;
	// Iterator
	using iterator = BasicIterator<value>;
	using const_iterator = BasicIterator<const value>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
private:
	size_t	used;
	size_t	allocated;
	pointer	start;
public:
	static constexpr size_t npos = -1;
private:
	static constexpr double mul_alloc = 1.6487;

	static_assert(!INCLUDE_INITIALIZER_LIST, "It is not possible to use std::initializer_list at the current time(version)");
	static_assert(std::is_pod<value>::value, "In the current version it is not possible to work with non-POD type");

	/// <summary>
	///  TODO 
	/// https://en.cppreference.com/w/cpp/named_req/MoveAssignable
	/// https://en.cppreference.com/w/cpp/named_req/MoveInsertable
	/// https://en.cppreference.com/w/cpp/named_req/EmplaceConstructible
	/// </summary>


	/// <summary>
	/// allocated version
	/// The next step in the size of the data block.
	/// </summary>
	/// <returns>size_t</returns>
	decltype(auto) alloc_step()
	{
		return size_t(allocated * mul_alloc + 1);
	}
	/// <summary>
	/// used version
	/// The next step in the size of the data block.
	/// </summary>
	/// <returns>size_t</returns>
	decltype(auto) used_step()
	{
		return size_t(used * mul_alloc + 1);
	}
	/// <summary>
	/// index version
	/// The next step in the size of the data block.
	/// </summary>
	/// <returns>size_t</returns>
	decltype(auto) index_step(size_t idx)
	{
		return size_t(idx * mul_alloc + 1);
	}
	///	<summary>
	/// Increases the block for writing data if necessary
	/// </summary>
	/// <returns>void</returns>
	decltype(auto) check_allocate() noexcept
	{
		if (used >= allocated)
			allocate(used_step());
	}
	/// <summary>
	///	Creates in memory, changes, deletes block of elements - a vector, 
	///	the function should not be called without a shell. 
	///	To use this function, it should be called through the reserve(...) function.
	/// </summary>
	/// <returns>void</returns>
	/// <param name="al">count elements to alloc in memory</param>
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
				memcpy(block = new value[allocated = al], start, (used = used > al ? al : used) * size_value());
				delete[] start;
				start = block;
			}
		}
		else free();
	}
	/// <summary>
	///	The function checks if it is possible to insert elements without having to shift.
	/// </summary>
	/// <returns>true, if need moving data</returns>
	/// <param name="place">place index element</param>
	decltype(auto) insert_correct(size_t place)
	{
		bool ret = true;
		if (place > used)
		{
			used = place;
			ret = false;
		}
		check_allocate();
		return ret;
	}
	/// <summary>
	///	The function checks if it is possible to insert elements without having to shift.
	/// </summary>
	/// <returns>true, if need moving data</returns>
	/// <param name="place">place index elements</param>
	/// <param name="count">count elements</param>
	decltype(auto) insert_correct(size_t place, size_t count)
	{
		bool ret = true;
		if (place >= used)
			ret = false;
		auto next_used = place + count;
		if (next_used > used)
			used = next_used;
		check_allocate();
		return ret;
	}
public:
	/// <summary>
	/// Expands/controls the size/receives the value of the i element.
	/// </summary>
	/// <returns>reference</returns>
	/// <param name="i">Index</param>
	decltype(auto) at(size_t i)
	{
		if (i >= used)
		{
			used = i + 1;
			if (i >= allocated)
				allocate(index_step(i));
		}
		return reference(start[i]);
	}
	/// <summary>
	///	BEWARE out-from-range
	/// receives the value of the i element.
	/// </summary>
	/// <returns>reference value</returns>
	/// <param name="i">Index</param>
	decltype(auto) at(size_t i) const
	{
		return const reference(start[i]);
	}
	/// <summary>
	///	Inserting an element at the end of a data block.
	///	This function itself extends the data block if it is necessary.
	/// </summary>
	/// <returns>void</returns>
	/// <param name="val">element to push</param>
	decltype(auto) push_back(rvalue val) noexcept
	{
		check_allocate();
		start[used] = val;
		++used;
	}
	/// <summary>
	///	Inserting an elements at the end of a data block.
	///	This function itself extends the data block if it is necessary.
	/// </summary>
	/// <returns>void</returns>
	/// <param name="val">pointer elements to push</param>
	/// <param name="c">count elements</param>
	decltype(auto) push_back(pointer val, size_t c) noexcept
	{
		move_insert(used, val, c);
	}
	/// <summary>
	///	Deletes the last element from the data block.
	///	(As long as the data is not overwritten by new data and/or the block is not moved to a new location, the deleted information will still exist.)
	///	The size allocate of the block does not change.
	/// </summary>
	/// <returns>void</returns>
	decltype(auto) pop_back() noexcept
	{
		if (used > 0)
			--used;
	}
#if (0)
	/// <summary>
	/// 
	/// </summary>
	template <class... Args>
	decltype(auto) emplace(size_t pos, Args&&... args)
	{
		if (pos >= used)
		{
			used = pos + 1;
			if (pos >= allocated)
				allocate(index_step(pos));
		}
		reinterpret_cast<reference>(start[pos])(args);
	}
	/// <summary>
	/// 
	/// </summary>
	template <class... Args>
	decltype(auto) emplace_back(Args&&... args)
	{
		emplace(used, args);
	}
#endif
	/// <summary>
	///	HIGH TIME CONSUMPTION FUNCTION (memcpy)
	/// Inserting an item into a data block and moving other data if a collision occurred.
	/// </summary>
	/// <returns>void</returns>
	/// <param name="place">place index element</param>
	/// <param name="val">element to push</param>
	decltype(auto) move_insert(size_t place, rvalue val)
	{
		if (insert_correct(place))
			Memory::memcpy(start + place + 1, start + place, used - place);
		start[place] = val;
	}
	/// <summary>
	///	HIGH TIME CONSUMPTION FUNCTION (memcpy)
	/// Inserting an items into a data block and moving other data if a collision occurred.
	/// </summary>
	/// <returns>void</returns>
	/// <param name="place">place index element</param>
	/// <param name="val">pointer elements to push</param>
	/// <param name="count">counts elements</param>
	decltype(auto) move_insert(size_t place, pointer val, size_t count)
	{
		auto place_address = start + place;
		if (insert_correct(place, count))
			Memory::memcpy(place_address + count, place_address, used - place);
		Memory::memcpy(place_address, val, count * size_value());
	}
	/// <summary>
	///	HIGH TIME CONSUMPTION FUNCTION (memcpy)
	/// Inserting an items into a data block and moving other data if a collision occurred.
	/// </summary>
	/// <returns>void</returns>
	/// <param name="place">place index element</param>
	/// <param name="v"> TODO </param>
	__inline decltype(auto) move_insert(size_t place, vector_rvalue v)
	{
		move_insert(place, v.data(), v.size());
	}
	/// <summary>
	///	HIGH TIME CONSUMPTION FUNCTION (memcpy)
	/// Inserting an items into a data block and moving other data if a collision occurred.
	/// </summary>
	/// <returns>void</returns>
	/// <param name="v"> TODO </param>
	__inline decltype(auto) move_insert(vector_rvalue from)
	{
		move_insert(used, from);
	}
	/// <summary>
	///	HIGH TIME CONSUMPTION FUNCTION (memcpy)
	/// Inserting an items into a data block and moving other data if a collision occurred.
	/// </summary>
	/// <returns>void</returns>
	/// <param name="place">place index element</param>
	/// <param name="v"> TODO </param>
	__inline decltype(auto) move_insert(size_t place, vector_reference v)
	{
		move_insert(place, v.data(), v.size());
	}
	/// <summary>
	///	HIGH TIME CONSUMPTION FUNCTION (memcpy)
	/// Inserting an items into a data block and moving other data if a collision occurred.
	/// </summary>
	/// <returns>void</returns>
	/// <param name="v"> TODO </param>
	__inline decltype(auto) move_insert(vector_reference from)
	{
		move_insert(used, from);
	}
	/// <summary>
	/// Inserting an items into a data block.
	///	Without moving other data(Erases data) if a collision occurred.
	/// </summary>
	/// <returns>void</returns>
	/// <param name="place">place index element</param>
	/// <param name="val">element to push</param>
	decltype(auto) insert(size_t place, rvalue val) noexcept
	{
		insert_correct(place);
		start[place] = val;
	}
	/// <summary>
	/// Inserting an items into a data block.
	///	Without moving other data(Erases data) if a collision occurred.
	/// </summary>
	/// <returns>void</returns>
	/// <param name="place">place index element</param>
	/// <param name="val">pointer elements to push</param>
	/// <param name="count">counts elements</param>
	decltype(auto) insert(size_t place, pointer val, size_t count) noexcept
	{
		insert_correct(place, count);
		memcpy(start + place, val, count * size_value());
	}
	/// <summary>
	/// Inserting an items into a data block.
	///	Without moving other data(Erases data) if a collision occurred.
	/// </summary>
	/// <returns>void</returns>
	/// <param name="place">place index element</param>
	/// <param name="from"> TODO </param>
	__inline decltype(auto) insert(size_t place, vector_rvalue from) noexcept
	{
		insert(place, from.data(), from.size());
	}
	/// <summary>
	/// Inserting elements at the end of a data block.
	///	Without moving other data(Erases data) if a collision occurred.
	/// </summary>
	/// <returns>void</returns>
	/// <param name="l"> TODO </param>
	__inline decltype(auto) insert(vector_rvalue from) noexcept
	{
		insert(used, from);
	}
	/// <summary>
	/// Inserting an items into a data block.
	///	Without moving other data(Erases data) if a collision occurred.
	/// </summary>
	/// <returns>void</returns>
	/// <param name="place">place index element</param>
	/// <param name="from"> TODO </param>
	__inline decltype(auto) insert(size_t place, vector_reference from) noexcept
	{
		insert(place, from.data(), from.size());
	}
	/// <summary>
	/// Inserting elements at the end of a data block.
	///	Without moving other data(Erases data) if a collision occurred.
	/// </summary>
	/// <returns>void</returns>
	/// <param name="l"> TODO </param>
	__inline decltype(auto) insert(vector_reference from) noexcept
	{
		insert(used, from);
	}

#if defined(INCLUDE_INITIALIZER_LIST) && INCLUDE_INITIALIZER_LIST
	/// <summary>
	///	HIGH TIME CONSUMPTION FUNCTION (memcpy)
	/// Inserting an items into a data block and moving other data if a collision occurred.
	/// </summary>
	/// <returns>void</returns>
	/// <param name="place">place index element</param>
	/// <param name="l"> TODO </param>
	__inline decltype(auto) move_insert(size_t place, list_rvalue l)
	{
		move_insert(place, l.begin(), l.size());
	}
	/// <summary>
	///	HIGH TIME CONSUMPTION FUNCTION (memcpy)
	/// Inserting an items into a data block and moving other data if a collision occurred.
	/// </summary>
	/// <returns>void</returns>
	/// <param name="l"> TODO </param>
	__inline decltype(auto) move_insert(list_rvalue from)
	{
		move_insert(used, from);
	}
	/// <summary>
	///	HIGH TIME CONSUMPTION FUNCTION (memcpy)
	/// Inserting an items into a data block and moving other data if a collision occurred.
	/// </summary>
	/// <returns>void</returns>
	/// <param name="place">place index element</param>
	/// <param name="l"> TODO </param>
	__inline decltype(auto) move_insert(size_t place, list_reference l)
	{
		move_insert(place, l.begin(), l.size());
	}
	/// <summary>
	///	HIGH TIME CONSUMPTION FUNCTION (memcpy)
	/// Inserting an items into a data block and moving other data if a collision occurred.
	/// </summary>
	/// <returns>void</returns>
	/// <param name="l"> TODO </param>
	__inline decltype(auto) move_insert(list_reference from)
	{
		move_insert(used, from);
	}
	/// <summary>
	/// Inserting an items into a data block.
	///	Without moving other data(Erases data) if a collision occurred.
	/// </summary>
	/// <returns>void</returns>
	/// <param name="place">place index element</param>
	/// <param name="from"> TODO </param>
	__inline decltype(auto) insert(size_t place, list_rvalue from) noexcept
	{
		insert(place, from.begin(), from.size());
	}
	/// <summary>
	/// Inserting elements at the end of a data block.
	///	Without moving other data(Erases data) if a collision occurred.
	/// </summary>
	/// <returns>void</returns>
	/// <param name="list"> TODO </param>
	__inline decltype(auto) insert(list_rvalue from) noexcept
	{
		insert(used, from);
	}
	/// <summary>
	/// Inserting an items into a data block.
	///	Without moving other data(Erases data) if a collision occurred.
	/// </summary>
	/// <returns>void</returns>
	/// <param name="place">place index element</param>
	/// <param name="from"> TODO </param>
	__inline decltype(auto) insert(size_t place, list_reference from) noexcept
	{
		insert(place, from.begin(), from.size()); // FIXME
	}
	/// <summary>
	/// Inserting elements at the end of a data block.
	///	Without moving other data(Erases data) if a collision occurred.
	/// </summary>
	/// <returns>void</returns>
	/// <param name="list"> TODO </param>
	__inline decltype(auto) insert(list_reference from) noexcept
	{
		insert(used, from);
	}
#endif

	/// <summary>
	/// Count items entered into the data block
	/// </summary>
	/// <returns>size_t</returns>
	decltype(auto) size() noexcept
	{
		return used;
	}
	/// <summary>
	/// Count items entered into the data block
	/// </summary>
	/// <returns>size_t</returns>
	decltype(auto) size() const noexcept
	{
		return used;
	}
	/// <summary>
	/// Maximum number of items that can be placed without the need to reallocate.
	/// </summary>
	/// <returns>size_t</returns>
	decltype(auto) capacity() noexcept
	{
		return allocated;
	}
	/// <summary>
	/// Maximum number of items that can be placed without the need to reallocate.
	/// </summary>
	/// <returns>size_t</returns>
	decltype(auto) capacity() const noexcept
	{
		return allocated;
	}
	/// <summary>
	/// Copies the contents of the data block.
	///	Allocating memory for the contents as in the original vector.
	/// </summary>
	/// <returns>void</returns>
	/// <param name="v">Vector<value>*</param>
	decltype(auto) copy(vector_pointer v) noexcept
	{
		v->allocate(allocated);
		if (used)
			memcpy(v->start, start, (v->used = used) * size_value());
	}
	/// <summary>
	/// Copies the contents of the data block.
	///	Allocating memory for the contents as in the original vector.
	/// </summary>
	/// <returns>void</returns>
	/// <param name="v">Vector<value>*</param>
	decltype(auto) copy(vector_pointer v) const noexcept
	{
		v->allocate(allocated);
		if (used)
			memcpy(v->start, start, (v->used = used) * size_value());
	}
	/// <summary>
	/// Clears the contents of the data block without erasing previous data.
	/// </summary>
	/// <returns>void</returns>
	decltype(auto) clear() noexcept
	{
		used = 0;
	}
	/// <summary>
	/// Clears the contents of the data block without erasing previous data.
	/// </summary>
	/// <returns>void</returns>
	decltype(auto) clear() const noexcept
	{
		used = 0;
	}
	/// <summary>
	/// Sets the standard value (zero) for the i element
	/// </summary>
	/// <returns>void</returns>
	/// <param name="i">Index to the element</param>
	decltype(auto) erase(size_t i) noexcept
	{
		start[i] = 0; // ?? memcpy(start + i, )
	}
	/// <summary>
	/// Address for writing the next item.
	///		-(It is necessary to check the out of range!)
	///		-(Does not increase the current size of the data block!)
	///	Used in iterators.
	/// </summary>
	/// <returns>void</returns>
	decltype(auto) last() noexcept
	{
		return start + used;
	}
	/// <summary>
	/// Address for writing the next item.
	///		-(It is necessary to check the out of range!)
	///		-(Does not increase the current size of the data block!)
	///	Used in iterators.
	/// </summary>
	/// <returns>value&&</returns>
	decltype(auto) last() const noexcept
	{
		return start + used;
	}
	/// <summary>
	/// The cell for writing the first element. 
	///	It is necessary to check the out of range!
	///	Does not increase the current size of the data block!
	/// </summary>
	/// <returns>rvalue</returns>
	decltype(auto) front() noexcept
	{
		return rvalue(*start);
	}
	/// <summary>
	/// The cell for writing the first element. 
	///	It is necessary to check the out of range!
	///	Does not increase the current size of the data block!
	/// </summary>
	/// <returns>rvalue</returns>
	decltype(auto) front() const noexcept
	{
		return rvalue(*start);
	}
	/// <summary>
	/// The cell for writing the next element. 
	///	It is necessary to check the out of range!
	///	Does not increase the current size of the data block!
	/// </summary>
	/// <returns>rvalue</returns>
	decltype(auto) back() noexcept
	{
		return rvalue(*(start + used));
	}
	/// <summary>
	/// The cell for writing the next element. 
	///	It is necessary to check the out of range!
	///	Does not increase the current size of the data block!
	/// </summary>
	/// <returns>rvalue</returns>
	decltype(auto) back() const noexcept
	{
		return rvalue(*(start + used));
	}
	/// <summary>
	///	Data block
	/// </summary>
	/// <returns>pointer to the data block</returns>
	decltype(auto) data() noexcept
	{
		return start;
	}
	/// <summary>
	///	Data block
	/// </summary>
	/// <returns>pointer to the data block</returns>
	decltype(auto) data() const noexcept
	{
		return start;
	}
	/// <summary>
	///	Swaps the contents of the i and j elements
	/// </summary>
	/// <returns>void</returns>
	/// <param name="i">Index to the element</param>
	/// <param name="j">Index to the element</param>
	decltype(auto) swap(size_t i, size_t j)
	{
		if (i == j)
			return;
		insert_correct(i > j ? i : j);
		std::swap(start[i], start[j]);
	}
	/// <summary>
	///	Swaps the contents Vectors
	/// </summary>
	/// <returns>void</returns>
	/// <param name="v">Vector<value> rvalue</param>
	decltype(auto) swap(vector_rvalue v) noexcept
	{
		std::swap(*this, v);
	}
	/// <summary>
	///	Swaps the contents const Vectors
	/// </summary>
	/// <returns>void</returns>
	/// <param name="v">Vector<value> rvalue</param>
	decltype(auto) swap(const vector_rvalue v) const noexcept
	{
		std::swap(*this, v);
	}
	/// <summary>
	///	Test to empty.
	/// </summary>
	/// <returns>bool</returns>
	decltype(auto) empty() noexcept
	{
		return used == 0;
	}
	/// <summary>
	///	Test to empty.
	/// </summary>
	/// <returns>bool</returns>
	decltype(auto) empty() const noexcept
	{
		return used == 0;
	}
	/// <summary>
	///	Resize data block. 
	///	It doesn't erase data when you resize, but it can happen if you reduce the size and it gets copied to a new location. 
	/// </summary>
	/// <returns>void</returns>
	/// <param name="sz">New data block's size</param>
	decltype(auto) resize(size_t sz) noexcept
	{
		if ((used = sz) > allocated)
			allocate(used);
	}
	/// <summary>
	///	Frees up the data block.
	///	Makes the vector invalid.
	/// </summary>
	/// <returns>void</returns>
	decltype(auto) free() noexcept
	{
		allocated = used = 0;
		if (start)
			delete[] start;
		start = nullptr;
	}
	/// <summary>
	///	Frees up the data block.
	///	Makes the vector invalid.
	/// </summary>
	/// <returns>void</returns>
	decltype(auto) free() const noexcept
	{
		allocated = used = 0;
		if (start)
			delete[] start;
		start = nullptr;
	}
	/// <summary>
	/// TODO
	/// </summary>
	/// <returns>void</returns>
	/// <param name="sz">New size</param>
	decltype(auto) reserve(size_t sz) noexcept
	{
		allocate(sz);
	}
	/// <summary>
	///	The maximum possible number of vector elements.
	///	Regardless of available memory.
	/// </summary>
	/// <returns>size_t</returns>
	constexpr decltype(auto) max_size() noexcept
	{
		return npos / size_value(); // npos has nothing to do with this expression, the only thing needed from it is size_t(-1)
	}
	/// <summary>
	///	The size of one vector element
	/// </summary>
	/// <returns>size_t</returns>
	constexpr decltype(auto) size_value() noexcept
	{
		return sizeof(value);
	}
	/// <summary>
	///	The size of vector
	/// </summary>
	/// <returns>size_t</returns>
	constexpr decltype(auto) size_vector() noexcept
	{
		return sizeof(vector);
	}
	/// <summary>
	///	Shrink vector to fit size
	/// </summary>
	/// <returns>void</returns>
	decltype(auto) shrink_to_fit() noexcept
	{
		if (used < allocated)
			allocate(used);
	}
	
	/// <summary>
	/// Iterators
	/// </summary>

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

#if defined(INCLUDE_INITIALIZER_LIST) && INCLUDE_INITIALIZER_LIST
	/// <summary>
	/// Operator for inserting values from the initialization_list.
	///	The old vector values in the data block will be forgotten (lost).
	/// </summary>
	/// <returns>void</returns>
	/// <param name="v"> TODO </param>
	decltype(auto) operator()(list_rvalue v) noexcept
	{
		insert(v);
	}
	/// <summary>
	/// Operator for inserting values from the initialization_list.
	///	The old vector values in the data block will be forgotten (lost).
	/// </summary>
	/// <returns>void</returns>
	/// <param name="v"> TODO </param>
	decltype(auto) operator()(list_reference v) noexcept
	{
		insert(v);
	}
#endif

	/// <summary>
	/// API calling free()
	/// </summary>
	/// <returns>void</returns>
	decltype(auto) operator~() noexcept
	{
		free();
	}
	/// <summary>
	/// API calling swap()
	/// </summary>
	/// <returns>void</returns>
	/// <param name="v">Vector to swap</param>
	decltype(auto) operator^=(vector_rvalue v) noexcept
	{
		swap(v);
	}
	/// <summary>
	/// insert value
	/// </summary>
	/// <returns>void</returns>
	/// <param name="v">push value</param>
	decltype(auto) operator+=(value c) noexcept
	{
		push_back(c);
	}
	/// <summary>
	/// insert values from other vector.
	/// </summary>
	/// <returns>void</returns>
	/// <param name="v">push values from vector</param>
	decltype(auto) operator+=(vector_rvalue v) noexcept
	{
		insert(v);
	}
	/// <summary>
	/// insert values from other vector pointer.
	/// Not needing to check null-pointer
	/// </summary>
	/// <returns>void</returns>
	/// <param name="v">push values from pointer vector</param>
	decltype(auto) operator+=(vector_pointer v) noexcept
	{
		if (v)
			insert(*v);
	}
	/// <summary>
	/// insert values from other vector.
	/// </summary>
	/// <returns>void</returns>
	/// <param name="v">push values from vector</param>
	decltype(auto) operator+=(const vector_rvalue v) const noexcept
	{
		insert(v);
	}
	/// <summary>
	/// API calling at(...)
	/// </summary>
	/// <param name="i">Index</param>
	/// <returns>reference</returns>
	decltype(auto) operator[](size_t i) noexcept
	{
		return at(i);
	}
	/// <summary>
	/// API calling at(...)
	/// </summary>
	/// <param name="i">Index</param>
	/// <returns>const reference value</returns>
	decltype(auto) operator[](size_t i) const noexcept
	{
		return at(i);
	}

	__inline decltype(auto) operator<(size_t i) noexcept
	{
		return used < i;
	}
	__inline decltype(auto) operator>(size_t i) noexcept
	{
		return used > i;
	}
	__inline decltype(auto) operator<=(size_t i) noexcept
	{
		return used <= i;
	}
	__inline decltype(auto) operator>=(size_t i) noexcept
	{
		return used >= i;
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
	
	/// <summary>
	///	CONSTRUCTOR null
	/// </summary>
	Vector() noexcept
	{
		start = nullptr;
		allocated = used = 0;
	}
	/// <summary>
	///	CONSTRUCTOR reserve
	/// </summary>
	/// <param name="sz">Count elements to allocate</param>
	Vector(size_t sz) : Vector()
	{
		allocate(sz);
	}
	/// <summary>
	///	CONSTRUCTOR insert from array
	/// </summary>
	/// <param name="sz">Count elements to allocate</param>
	/// <param name="ray">pointer to values</param>
	Vector(size_t sz, pointer ray) : Vector()
	{
		insert(used, ray, sz);
	}

#if defined(INCLUDE_INITIALIZER_LIST) && INCLUDE_INITIALIZER_LIST
	/// <summary>
	///	CONSTRUCTOR initializer_list
	/// </summary>
	/// <param name="v"> TODO </param>
	Vector(list_reference v) : Vector()
	{
		insert(v);
	}
	/// <summary>
	///	CONSTRUCTOR initializer_list
	/// </summary>
	/// <param name="v"> TODO </param>
	Vector(list_rvalue v) : Vector()
	{
		insert(v);
	}
#endif

	/// <summary>
	///	CONSTRUCTOR copy
	/// </summary>
	/// <param name="v">reference to vector</param>
	Vector(vector_reference v) : Vector()
	{
		v.copy(this);
	}
	/// <summary>
	///	CONSTRUCTOR copy
	/// </summary>
	/// <param name="v">reference to vector</param>
	Vector(vector_rvalue v) noexcept
	{
		v.copy(this);
	}
	/// <summary>
	///	DESTRUCTOR
	/// </summary>
	~Vector() noexcept
	{
		free();
	}
};
