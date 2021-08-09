#pragma once

#include <memory.h>
#include <initializer_list>

#include "../BasicIterator/BasicIterator.h"
#include "../Memory/util.h"

namespace UltimaAPI
{
	template <typename type> class Vector;
}

template <
	typename __type__ = int, 
	bool throw_exception = false/* Throw out an exception */>
class UltimaAPI::Vector
{
public:
	using value = __type__;
	using rvalue = value&&;
	using pointer = value*;
	using reference = value&;

	using vector = Vector<value>;
	using vector_rvalue = vector&&;
	using vector_pointer = vector*;
	using vector_reference = vector&;

	using list = std::initializer_list<value>;
	using list_rvalue = list&&;
	using list_pointer = list*;
	using list_reference = list&;
	
	using iterator = BasicIterator<value>;
	using const_iterator = BasicIterator<const value>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
private:
	size_t	used;
	size_t	allocated;
	pointer	start;
public:
	constexpr auto npos = size_t(-1);
	constexpr auto mul_alloc = double(1.6487); // sqrt(e)
private:
	static_assert(std::is_pod<value>::value, "In the current version it is not possible to work with non-POD type");


	/// <summary>
	///	The function checks the necessity for extending the data block
	/// <returns>bool</returns>
	/// </summary>
	__forceinline decltype(auto) check_oversize_vector() noexcept
	{
		return used >= allocated;
	}
	/// <summary>
	///	Creates in memory, changes, deletes block of elements - a vector, 
	///	the function should not be called without a shell. 
	///	To use this function, it should be called through the reserve(...) function.
	/// <returns>void</returns>
	/// <param name="al">count elements to alloc in memory</param>
	/// </summary>
	decltype(auto) allocate(size_t al) noexcept(throw_exception)
	{
		if (al > max_size())
		{
			if (throw_exception)
				throw(); /// TODO
		}
		else if (al)
		{
			if (start == nullptr)
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
	/// <returns>true, if need moving data</returns>
	/// <param name="place">place index element</param>
	/// </summary>
	decltype(auto) insert_correct(size_t place) noexcept(throw_exception)
	{
		bool ret = true;
		if (place > used)
		{
			used = place;
			ret = false;
		}
		if (check_oversize_vector())
			allocate(used_step());
		return ret;
	}
	/// <summary>
	///	The function checks if it is possible to insert elements without having to shift.
	/// <returns>true, if need moving data</returns>
	/// <param name="place">place index elements</param>
	/// <param name="count">count elements</param>
	/// </summary>
	decltype(auto) insert_correct(size_t place, size_t count) noexcept(throw_exception)
	{
		bool ret = true;
		if (place > used)
			ret = false;
		auto next_used = place + index;
		if (next_used > used)
			used = next_used;
		if (check_oversize_vector())
			allocate(used_step());
		return ret;
	}
	/// <summary>
	///	Forward Sequence
	///	By comparing, searches for matching sequences in the data.
	/// <returns>
	///	Index of the first found element in the data block.
	///	If nothing was found will return npos
	///	</returns>
	/// <param name="idx">Start index for search in the data block</param>
	/// <param name="len">The length of the desired sequence</param>
	/// <param name="val">Sequence pointer to the data</param>
	/// </summary>
	decltype(auto) forward_sequence(size_t idx, size_t len, pointer val) noexcept(throw_exception)
	{
		for (size_t s = 0; s < len; s++)
			if (start[idx + s] != val[s])
				return npos;
		return idx;
	}
	/// <summary>
	///	Backward Sequence
	///	By comparing, searches for matching sequences in the data.
	/// <returns>
	///	Index of the first found element in the data block.
	///	If nothing was found will return npos
	///	</returns>
	/// <param name="idx">Start index for search in the data block</param>
	/// <param name="len">The length of the desired sequence</param>
	/// <param name="val">Sequence pointer to the data</param>
	/// </summary>
	decltype(auto) backward_sequence(size_t idx, size_t len, pointer val) noexcept(throw_exception)
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
	/// <summary>
	///	Inserting an element at the end of a data block.
	///	This function itself extends the data block if it is necessary.
	/// <returns>void</returns>
	/// <param name="val">element to push</param>
	/// </summary>
	decltype(auto) push_back(rvalue val) noexcept(throw_exception)
	{
		if (check_oversize_vector())
			allocate(alloc_step());
		start[used] = val;
		++used;
	}
	// <summary>
	/// Expands/controls the size/receives the value of the i element.
	/// <returns>rvalue</returns>
	/// <param name="i">Index</param>
	/// </summary>
	decltype(auto) at(size_t i) noexcept(throw_exception)
	{
		if (i >= allocated)
			allocate(i * mul_alloc + 1);
		if (i >= used)
			used = i + 1;
		return rvalue(start[i]);
	}
	/// <summary>
	///	Inserting an elements at the end of a data block.
	///	This function itself extends the data block if it is necessary.
	/// <returns>void</returns>
	/// <param name="val">pointer elements to push</param>
	/// <param name="c">count elements</param>
	/// </summary>
	decltype(auto) push_back(pointer val, size_t c) noexcept(throw_exception)
	{
		insert(used, val, c);
	}
	/// <summary>
	///	Deletes the last element from the data block.
	///	(As long as the data is not overwritten by new data and/or the block is not moved to a new location, the deleted information will still exist.)
	///	The size allocate of the block does not change.
	/// <returns>void</returns>
	/// </summary>
	decltype(auto) pop_back() noexcept(throw_exception)
	{
		if (used > 0)
			--used;
	}
	/// <summary>
	///	HIGH TIME CONSUMPTION FUNCTION (memcpy)
	/// Inserting an item into a data block and moving other data if a collision occurred.
	/// <returns>void</returns>
	/// <param name="place">place index element</param>
	/// <param name="val">element to push</param>
	/// </summary>
	decltype(auto) move_insert(size_t place, rvalue val) noexcept(throw_exception)
	{
		if (insert_correct(place))
		{
			auto place_address = start + place;
			Memory::memcpy(place_address + 1, place_address, used - place);
		}
		start[place] = val;
	}
	/// <summary>
	///	HIGH TIME CONSUMPTION FUNCTION (memcpy)
	/// Inserting an items into a data block and moving other data if a collision occurred.
	/// <returns>void</returns>
	/// <param name="place">place index element</param>
	/// <param name="val">pointer elements to push</param>
	/// <param name="count">counts elements</param>
	/// </summary>
	decltype(auto) move_insert(size_t place, pointer val, size_t count) noexcept(throw_exception)
	{
		if (val == nullptr)
		{
			if (throw_exception)
				throw(); /// TODO
		}
		else
		{
			auto place_address = start + place;
			if (insert_correct(place, count))
				Memory::memcpy(place_address + count, place_address, used - place);
			Memory::memcpy(place_address, val, count * size_value());
		}
	}
	/// <summary>
	/// Inserting an items into a data block.
	///	Without moving other data(Erases data) if a collision occurred.
	/// <returns>void</returns>
	/// <param name="place">place index element</param>
	/// <param name="val">element to push</param>
	/// </summary>
	decltype(auto) insert(size_t place, rvalue val) noexcept(throw_exception)
	{
		insert_correct(place);
		start[place] = val;
	}
	/// <summary>
	/// Inserting an items into a data block.
	///	Without moving other data(Erases data) if a collision occurred.
	/// <returns>void</returns>
	/// <param name="place">place index element</param>
	/// <param name="val">pointer elements to push</param>
	/// <param name="count">counts elements</param>
	/// </summary>
	decltype(auto) insert(size_t place, pointer val, size_t count) noexcept(throw_exception)
	{
		if (val == nullptr)
		{
			if (throw_exception)
				throw(); /// TODO
		}
		else
		{
			insert_correct(place, count);
			memcpy(start + place, val, count * size_value());
		}
	}
	/// <summary>
	/// Inserting an items into a data block.
	///	Without moving other data(Erases data) if a collision occurred.
	/// <returns>void</returns>
	/// <param name="place">place index element</param>
	/// <param name="list"> TODO </param>
	/// </summary>
	decltype(auto) insert(size_t place, list_rvalue from) noexcept(throw_exception)
	{
		insert(place, from.begin(), from.size());
	}
	/// <summary>
	/// Inserting elements at the end of a data block.
	///	Without moving other data(Erases data) if a collision occurred.
	/// <returns>void</returns>
	/// <param name="list"> TODO </param>
	/// </summary>
	decltype(auto) insert(list_rvalue from) noexcept(throw_exception)
	{
		insert(used, from);
	}
	/// <summary>
	/// Inserting elements at the end of a data block.
	///	Without moving other data(Erases data) if a collision occurred.
	/// <returns>void</returns>
	/// <param name="vector"> TODO </param>
	/// </summary>
	decltype(auto) insert(size_t place, vector_rvalue from) noexcept(throw_exception)
	{
		insert(used, from.data(), from.size());
	}
	/// <summary>
	/// Inserting elements at the end of a data block.
	///	Without moving other data(Erases data) if a collision occurred.
	/// <returns>void</returns>
	/// <param name="vector"> TODO </param>
	/// </summary>
	decltype(auto) insert(vector_rvalue from) noexcept(throw_exception)
	{
		insert(used, from);
	}
	/// <summary>
	/// Count items entered into the data block
	/// <returns>size_t</returns>
	/// </summary>
	decltype(auto) size() noexcept
	{
		return used;
	}
	/// <summary>
	/// Count items entered into the data block
	/// <returns>size_t</returns>
	/// </summary>
	decltype(auto) size() const noexcept
	{
		return used;
	}
	/// <summary>
	/// Maximum number of items that can be placed without the need to reallocate.
	/// <returns>size_t</returns>
	/// </summary>
	decltype(auto) capacity() noexcept
	{
		return allocated;
	}
	/// <summary>
	/// Maximum number of items that can be placed without the need to reallocate.
	/// <returns>size_t</returns>
	/// </summary>
	decltype(auto) capacity() const noexcept
	{
		return allocated;
	}
	/// <summary>
	/// allocated version
	/// The next step in the size of the data block.
	/// <returns>size_t</returns>
	/// </summary>
	decltype(auto) alloc_step() noexcept
	{
		return size_t(allocated * mul_alloc + 1);
	}
	/// <summary>
	/// used version
	/// The next step in the size of the data block.
	/// <returns>size_t</returns>
	/// </summary>
	decltype(auto) used_step() noexcept
	{
		return size_t(used * mul_alloc + 1);
	}
	/// <summary>
	/// Copies the contents of the data block.
	///	Allocating memory for the contents as in the original vector.
	/// <returns>void</returns>
	/// <param name="v">Vector<value>*</param>
	/// </summary>
	decltype(auto) copy(vector_pointer v) noexcept(throw_exception)
	{
		if (v == nullptr)
		{
			if (throw_exception)
				throw(); // TODO
		}
		else
		{
			v->allocate(allocated);
			if (used)
				memcpy(v->start, start, (v->used = used) * size_value());
		}
	}
	/// <summary>
	/// Copies the contents of the data block.
	///	Allocating memory for the contents as in the original vector.
	/// <returns>void</returns>
	/// <param name="v">Vector<value>*</param>
	/// </summary>
	decltype(auto) copy(vector_pointer v) const noexcept(throw_exception)
	{
		if (v == nullptr)
		{
			if (throw_exception)
				throw(); // TODO
		}
		else
		{
			v->allocate(allocated);
			if (used)
				memcpy(v->start, start, (v->used = used) * size_value());
		}
	}
	/// <summary>
	/// Clears the contents of the data block without erasing previous data.
	/// <returns>void</returns>
	/// </summary>
	decltype(auto) clear() noexcept(throw_exception)
	{
		used = 0;
	}
	/// <summary>
	/// Clears the contents of the data block without erasing previous data.
	/// <returns>void</returns>
	/// </summary>
	decltype(auto) clear() const noexcept(throw_exception)
	{
		used = 0;
	}
	/// <summary>
	/// Sets the standard value (zero) for the i element
	/// <returns>void</returns>
	/// <param name="i">Index to the element</param>
	/// </summary>
	decltype(auto) erase(size_t i) noexcept(throw_exception)
	{
		if (throw_exception)
			if (i >= used)
				throw(); /// TODO
		start[i] = 0;
	}
	/// <summary>
	/// Address for writing the next item.
	///		-(It is necessary to check the out of range!)
	///		-(Does not increase the current size of the data block!)
	///	Used in iterators.
	/// <returns>void</returns>
	/// </summary>
	decltype(auto) last() noexcept
	{
		return start + used;
	}
	/// <summary>
	/// Address for writing the next item.
	///		-(It is necessary to check the out of range!)
	///		-(Does not increase the current size of the data block!)
	///	Used in iterators.
	/// <returns>void</returns>
	/// </summary>
	decltype(auto) last() const noexcept
	{
		return start + used;
	}
	/// <summary>
	/// The cell for writing the first element. 
	///	It is necessary to check the out of range!
	///	Does not increase the current size of the data block!
	/// <returns>void</returns>
	/// </summary>
	decltype(auto) front() noexcept(throw_exception)
	{
		if (throw_exception)
		{
			if (start == nullptr)
				throw(); /// TODO
		}
		return rvalue(*start);
	}
	/// <summary>
	/// The cell for writing the first element. 
	///	It is necessary to check the out of range!
	///	Does not increase the current size of the data block!
	/// <returns>void</returns>
	/// </summary>
	decltype(auto) front() const noexcept(throw_exception)
	{
		if (throw_exception)
		{
			if (start == nullptr)
				throw(); /// TODO
		}
		return rvalue(*start);
	}
	/// <summary>
	/// The cell for writing the next element. 
	///	It is necessary to check the out of range!
	///	Does not increase the current size of the data block!
	/// <returns>void</returns>
	/// </summary>
	decltype(auto) back() noexcept(throw_exception)
	{
		if (throw_exception)
		{
			if (start == nullptr)
				throw(); /// TODO
		}
		return rvalue(*(start + used));
	}
	/// <summary>
	/// The cell for writing the next element. 
	///	It is necessary to check the out of range!
	///	Does not increase the current size of the data block!
	/// <returns>void</returns>
	/// </summary>
	decltype(auto) back() const noexcept(throw_exception)
	{
		if (throw_exception)
		{
			if (start == nullptr)
				throw(); /// TODO
		}
		return rvalue(*(start + used));
	}
	/// <summary>
	///	Data block
	/// <returns>pointer to the data block</returns>
	/// </summary>
	decltype(auto) data() noexcept
	{
		return start;
	}
	/// <summary>
	///	Data block
	/// <returns>pointer to the data block</returns>
	/// </summary>
	decltype(auto) data() const noexcept
	{
		return start;
	}
	/// <summary>
	///	Swaps the contents of the i and j elements
	/// <returns>void</returns>
	/// <param name="i">Index to the element</param>
	/// <param name="j">Index to the element</param>
	/// </summary>
	decltype(auto) swap(size_t i, size_t j) noexcept(throw_exception)
	{
		if (i == j)
			return;
		auto max = i > j ? i : j;
		if (throw_exception)
		{
			if (max >= used)
				throw(); /// TODO
		}
		insert_correct(max);
		std::swap(start[i], start[j]);
	}
	/// <summary>
	///	Swaps the contents Vectors
	/// <returns>void</returns>
	/// <param name="v">Vector<value> rvalue</param>
	/// </summary>
	decltype(auto) swap(vector_rvalue v) noexcept
	{
		std::swap(*this, v);
	}
	/// <summary>
	///	Swaps the contents const Vectors
	/// <returns>void</returns>
	/// <param name="v">Vector<value> rvalue</param>
	/// </summary>
	decltype(auto) swap(const vector_rvalue v) const noexcept
	{
		std::swap(*this, v);
	}
	/// <summary>
	///	Test to empty.
	/// <returns>bool</returns>
	/// </summary>
	decltype(auto) empty() noexcept
	{
		return used == 0;
	}
	/// <summary>
	///	Test to empty.
	/// <returns>bool</returns>
	/// </summary>
	decltype(auto) empty() const noexcept
	{
		return used == 0;
	}
	/// <summary>
	///	Resize data block. 
	///	It doesn't erase data when you resize, but it can happen if you reduce the size and it gets copied to a new location. 
	/// <returns>void</returns>
	/// <param name="sz">New data block's size</param>
	/// </summary>
	decltype(auto) resize(size_t sz) noexcept(throw_exception)
	{
		if ((used = sz) > allocated)
			allocate(used);
	}
	/// <summary>
	///	Frees up the data block.
	///	Makes the vector invalid.
	/// <returns>void</returns>
	/// </summary>
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
	/// <returns>void</returns>
	/// </summary>
	decltype(auto) free() const noexcept
	{
		allocated = used = 0;
		if (start)
			delete[] start;
		start = nullptr;
	}
	/// <summary>
	///	API for calling allocate(...)
	/// <returns>void</returns>
	/// <param name="sz">New size</param>
	/// </summary>
	decltype(auto) reserve(size_t sz) noexcept(throw_exception)
	{
		allocate(sz);
	}
	/// <summary>
	///	The maximum possible number of vector elements.
	///	Regardless of available memory.
	/// <returns>size_t</returns>
	/// </summary>
	constexpr decltype(auto) max_size() noexcept
	{
		return npos / size_value(); // npos has nothing to do with this expression, the only thing needed from it is size_t(-1)
	}
	/// <summary>
	///	The size of one vector element
	/// <returns>size_t</returns>
	/// </summary>
	constexpr decltype(auto) size_value() noexcept
	{
		return sizeof(value);
	}
	/// <summary>
	///	The size of vector
	/// <returns>size_t</returns>
	/// </summary>
	constexpr decltype(auto) size_vector() noexcept
	{
		return sizeof(vector);
	}
	/// <summary>
	///	Shrink vector to fit size
	/// <returns>void</returns>
	/// </summary>
	decltype(auto) shrink_to_fit() noexcept
	{
		if (used < allocated)
			allocate(used);
	}
	/// <summary>
	/// Forward iterator search
	///	Searches for the first matching value in the data block.
	///	If not found it returns the final iterator.
	/// <returns>iterator</returns>
	/// <param name="val">searching value in the data block</param>
	/// </summary>
	decltype(auto) iterator_find(value val) noexcept
	{
		auto b = begin();
		auto e = end();
		while (b != e)
		{
			if (b == val)
				return b;
			++b;
		}
		return e; // end();
	}
	/// <summary>
	/// Backward iterator search
	///	Searches for the first matching value in the data block.
	///	If not found it returns the final iterator.
	/// <returns>iterator</returns>
	/// <param name="val">searching value in the data block</param>
	/// </summary>
	decltype(auto) iterator_reverse_find(value val) noexcept
	{
		auto it = end();
		if (used > 0)
		{
			--it;
			auto b = begin();
			while (b != it)
			{
				if (it == val)
					return it;
				--it;
			}
		}
		return it;
	}
	/// <summary>
	/// Forward cell search
	///	Searches for the first matching value in the data block.
	///	If not found it returns a npos.
	/// <returns>Index</returns>
	/// <param name="val">searching value in the data block</param>
	/// </summary>
	decltype(auto) find(value val) noexcept
	{
		for (size_t i = 0; i < used; i++)
			if (start[i] == val)
				return i;
		return npos;
	}
	/// <summary>
	/// Backward cell search
	///	Searches for the first matching value in the data block.
	///	If not found it returns a npos.
	/// <returns>Index</returns>
	/// <param name="val">searching value in the data block</param>
	/// </summary>
	decltype(auto) reverse_find(value val) noexcept
	{
		if (used > 0)
			for (size_t i = used - 1; i != npos; i--)
				if (start[i] == val)
					return i;
		return npos;
	}
	/// <summary>
	/// Forward search for the iterator of a given sequence.
	///	Searches for the first matching iterator's value in the data block.
	///	If not found it returns a npos.
	/// <returns>Iterator</returns>
	/// <param name="val">searching value in the data block</param>
	/// </summary>
	decltype(auto) iterator_sequence_find(size_t len, pointer val, eSequence e) noexcept
	{
		if (auto _ = sequence_find(len, val, e); _ != npos)
			return iterator(start + _);
		return end();
	}
	/// <summary>
	/// Backward search for the iterator of a given sequence.
	///	Searches for the first matching iterator's value in the data block.
	///	If not found it returns a end iterator.
	/// <returns>Iterator</returns>
	/// <param name="val">searching value in the data block</param>
	/// </summary>
	decltype(auto) iterator_reverse_sequence_find(size_t len, pointer val, eSequence e) noexcept
	{
		if (auto _ = reverse_sequence_find(len, val, e); _ != npos)
			return iterator(start + _);
		return end();
	}
	/// <summary>
	/// Sequence values search. 
	///	Searches for the first matching values in the data block.
	///	If not found it returns a npos.
	/// <returns>Index</returns>
	/// <param name="val">searching value in the data block</param>
	/// </summary>
	decltype(auto) sequence_find(size_t len, pointer val, eSequence e) noexcept
	{
		switch (e)
		{
			default:
			case eSequence::seq_null:
				break;

			if (used >= len && len > 0)
			{
				case eSequence::seq_forward:
				{
					for (size_t i = 0; i <= used - len; i++)
						if (auto _ = forward_sequence(i, len, val); _ != npos)
							return _;
				}
				case eSequence::seq_backward:
				{
					for (size_t i = len - 1; i <= used; i++)
						if (auto _ = backward_sequence(i, len, val); _ != npos)
							return _;
				}
			}
		}
		return npos;
	}
	/// <summary>
	/// Sequence values reverse search. 
	///	Searches for the first matching values in the data block.
	///	If not found it returns a npos.
	/// <returns>Index</returns>
	/// <param name="val">searching value in the data block</param>
	/// </summary>
	decltype(auto) reverse_sequence_find(size_t len, pointer val, eSequence e) noexcept
	{
		switch (e)
		{
			default:
			case eSequence::seq_null:
				break;

			if (used >= len && len > 0)
			{
				case eSequence::seq_forward:
				{
					for (size_t i = used - len; i != npos; i--)
						if (auto _ = forward_sequence(i, len, val); _ != npos)
							return _;
				}
				case eSequence::seq_backward:
				{
					for (size_t i = used - 1; i != npos + len; i--)
						if (auto _ = backward_sequence(i, len, val); _ != npos)
							return _;
				}
			}
		}
		return npos;
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

	/// <summary>
	/// Operator for inserting values from the initialization_list.
	///	The old vector values in the data block will be forgotten (lost).
	/// <returns>void</returns>
	/// <param name="v"> TODO </param>
	/// </summary>
	decltype(auto) operator()(list_rvalue v) noexcept(throw_exception)
	{
		insert(v);
	}
	/// <summary>
	/// API calling free()
	/// <returns>void</returns>
	/// </summary>
	decltype(auto) operator~() noexcept
	{
		free();
	}
	/// <summary>
	/// API calling swap()
	/// <returns>void</returns>
	/// <param name="v">Vector to swap</param>
	/// </summary>
	decltype(auto) operator^=(vector_rvalue v) noexcept
	{
		swap(v);
	}
	/// <summary>
	/// insert value
	/// <returns>void</returns>
	/// <param name="v">push value</param>
	/// </summary>
	decltype(auto) operator+=(rvalue v) noexcept(throw_exception)
	{
		push_back(v);
	}
	/// <summary>
	/// insert values from other vector pointer.
	/// Not needing to check null-pointer
	/// <returns>void</returns>
	/// <param name="v">push values from pointer vector</param>
	/// </summary>
	decltype(auto) operator+=(vector_pointer v) noexcept(throw_exception)
	{
		if (v == nullptr)
		{
			if (throw_exception)
				throw(); /// TODO
		}
		else
		{
			insert(*(vector_rvalue*)p);
		}
	}
	/// <summary>
	/// insert values from other vector.
	/// <returns>void</returns>
	/// <param name="v">push values from vector</param>
	/// </summary>
	decltype(auto) operator+=(vector_rvalue v) noexcept(throw_exception)
	{
		insert(v);
	}
	/// <summary>
	/// insert values from other vector.
	/// <returns>void</returns>
	/// <param name="v">push values from vector</param>
	/// </summary>
	decltype(auto) operator+=(const vector_reference v) const noexcept(throw_exception)
	{
		insert(v); /// TODO Add function implementation for const vector_reference
	}
	/// API for call at(...)
	decltype(auto) operator[](size_t i) noexcept(throw_exception)
	{
		return at(i);
	}
	/// <summary>
	///	BEWARE out-from-range
	/// receives the value of the i element.
	/// <returns>rvalue</returns>
	/// <param name="i">Index</param>
	/// </summary>
	decltype(auto) operator[](size_t i) const noexcept
	{
		return rvalue(start[i]);
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
	///	CONSTRUCTOR initializer_list
	/// <param name="v"> TODO </param>
	/// </summary>
	Vector(list_rvalue v) noexcept(throw_exception)
	{
		this->operator()(v);
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
	/// <param name="sz">Count elements to allocate</param>
	/// </summary>
	Vector(size_t sz) noexcept(throw_exception)
	{
		allocated = used = 0;
		start = nullptr;
		allocate(sz);
	}
	/// <summary>
	///	CONSTRUCTOR insert from array
	/// <param name="sz">Count elements to allocate</param>
	/// <param name="ray">pointer to values</param>
	/// </summary>
	Vector(size_t sz, pointer ray) noexcept(throw_exception)
	{
		allocated = used = 0;
		start = nullptr;
		insert(0, ray, sz);
	}
	/// <summary>
	///	CONSTRUCTOR copy
	/// <param name="v">reference to vector</param>
	/// </summary>
	Vector(vector_rvalue v) noexcept(throw_exception)
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
