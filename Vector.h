#pragma once

#if __cplusplus == 202002L
//	C++20
#elif __cplusplus == 201703L
//	C++17
#elif __cplusplus == 201402L
//	C++14
#elif __cplusplus == 201103L
//	C++11
#elif __cplusplus == 199711L
// unknow
#endif

#include <memory.h>
#include <initializer_list>

#include "../BasicIterator/BasicIterator.h"
#include "../Allocator/Allocator.h"
#include "../Memory/util.h"

namespace UltimaAPI
{
	template <typename __type__ = int, class __locator__ = Allocator<int>>
	class Vector
	{
		using locator = __locator__;
	public:
		// Value
		using value = __type__;
		using rvalue = value&&;
		using pointer = value*;
		using reference = value&;
		using const_reference = const reference;
		// Initializer_list
		using list = std::initializer_list<value>;
		using list_rvalue = list&&;
		using list_pointer = list*;
		using list_reference = list&;
		using list_const_reference = const list_reference;
		// Vector
		using vector = Vector<value>;
		using vector_rvalue = vector&&;
		using vector_pointer = vector*;
		using vector_reference = vector&;
		using vector_const_reference = const vector_reference;
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
		static constexpr double mul_alloc = 1.6487; // (deleted info) is sqrt(e)
	
	//	static_assert(std::is_trivially_copyable<value>::value, "In the current version it is not possible to work with non-POD type");
	
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
		constexpr decltype(auto) alloc_step()
		{
			return size_t(allocated * mul_alloc + 1);
		}
		/// <summary>
		/// used version
		/// The next step in the size of the data block.
		/// </summary>
		/// <returns>size_t</returns>
		constexpr decltype(auto) used_step()
		{
			return size_t(used * mul_alloc + 1);
		}
		/// <summary>
		/// index version
		/// The next step in the size of the data block.
		/// </summary>
		/// <returns>size_t</returns>
		constexpr decltype(auto) index_step(size_t idx)
		{
			return size_t(idx * mul_alloc + 1);
		}
		///	<summary>
		/// Increases the block for writing data if necessary
		/// </summary>
		/// <returns>void</returns>
		constexpr decltype(auto) check_allocate() noexcept
		{
			if (used >= allocated)
				allocate(used_step());
		}
		///	<summary>
		/// Increases the block for writing data if necessary
		/// </summary>
		/// <returns>void</returns>
		constexpr decltype(auto) check_allocate(size_t use) noexcept
		{
			if (use >= allocated)
				allocate(index_step(use));
		}
		///	Creates in memory, changes, deletes block of elements - a vector, 
		///	the function should not be called without a shell. 
		///	To use this function, it should be called through the reserve(...) function.
		/// </summary>
		/// <returns>void</returns>
		/// <param name="al">count elements to alloc in memory</param>
		constexpr decltype(auto) allocate(size_t al) noexcept
		{
			if (al)
			{
				if (!start)
					start = new value[allocated = al];
				else if (al == allocated); // maybe adding code to do something!
				else movecopy(al);
			}
			else free();
		}
		/// <summary>
		///	TODO
		/// </summary>
		/// <returns>void</returns>
		/// <param name="al">count elements to alloc in memory</param>
		constexpr decltype(auto) allocate_copy(size_t al, size_t place, size_t count, bool need) noexcept
		{
			if (al > allocated)
			{
				if (!start)
					start = new value[allocated = al];
				else if (al == allocated); // maybe adding code to do something!
				else copy_free(new value[allocated = al], place, count, need);
			}
			else if (need)
			{
				auto placed = start + place;
				copy(placed + count, placed, used - place);
			}
		}
		/// <summary>
		///	TODO
		/// </summary>
		/// <param name="place">place index elements</param>
		/// <param name="count">count elements</param>
		constexpr decltype(auto) without_correct(size_t place, size_t count = 1)
		{
			auto next_used = place + count;
			check_allocate(next_used);
			if (next_used > used)
				used = next_used;
		}
		/// <summary>
		///	The function checks if it is possible to insert elements without having to shift.
		/// </summary>
		/// <returns>true, if need moving data</returns>
		/// <param name="place">place index elements</param>
		/// <param name="count">count elements</param>
		constexpr decltype(auto) insert_correct(size_t place, size_t count = 1)
		{
			/// [0; place]		(copy as in allocate memory)
			/// [place, used]	(copy with a shift relative to the current position by count)
			/// [place; place + count]	(Then return from the function and fill in the missing part)
			bool correct = false;
			size_t new_used = place;
			if (place < used)
			{
				new_used = used;
				correct = true;
			}
			new_used += count;
			allocate_copy(index_step(new_used), place, count, correct);
			used = new_used;
		}
		/// <summary>
		///	A shell to call std::copy. for minimal changes from memcpy.
		/// </summary>
		/// <param name="dest">To copy(new block memory)</param>
		/// <param name="src">From copy(old block memory)</param>
		/// <param name="count">count elements to copy</param>
		__inline constexpr decltype(auto) copy(pointer dest, const pointer src, size_t count)
		{
			std::copy(src, src + count, dest);
		}
		///	Copying to a new data block. Deleting the old data block.Returns the pointer to the new data block.
		/// </summary>
		/// <param name="dest">To copy(new block memory)</param>
		/// <param name="src">From copy(old block memory)</param>
		/// <param name="count">count elements to copy</param>
		__inline constexpr decltype(auto) copy_free(pointer dest, const pointer src, size_t count)
		{
			copy(dest, src, count);
			delete[] src;
			return dest;
		}
		/// <summary>
		///	A special version of the function for insert_correct(...), it is called only in allocate_copy(...). 
		/// (changes are possible in the future, please notify me if you have them).  
		/// </summary>
		/// <param name="dest">To copy(new block memory)</param>
		/// <param name="place">position for inserting new values</param>
		/// <param name="count">count elements to copy</param>
		/// <param name="need">whether it is necessary to copy from the original vector the old part (this variable should give true when place less used)</param>
		__inline constexpr decltype(auto) copy_free(pointer dest, size_t place, size_t count, bool need)
		{
			copy(dest, start, place);
			if (need)
				copy(dest + place + count, start + place, used - place);
			delete[] start;
			start = dest;
		}
		/// <summary>
		///	TODO
		/// </summary>
		/// <param name="al">New block size</param>
		__inline constexpr decltype(auto) movecopy(size_t al)
		{
			start = copy_free(new value[allocated = al], start, (used = used > al ? al : used));
		}
	public:
		/// <summary>
		/// Expands/controls the size/receives the value of the i element.
		/// </summary>
		/// <returns>reference</returns>
		/// <param name="i">Index</param>
		constexpr decltype(auto) at(size_t i)
		{
			if (i >= used)
			{
				used = i + 1;
				check_allocate();
			}
			return reference(start[i]);
		}
		/// <summary>
		///	BEWARE out-from-range
		/// receives the value of the i element.
		/// </summary>
		/// <returns>reference value</returns>
		/// <param name="i">Index</param>
		constexpr decltype(auto) at(size_t i) const
		{
			return const_reference(start[i]);
		}
		/// <summary>
		///	Inserting an element at the end of a data block.
		///	This function itself extends the data block if it is necessary.
		/// </summary>
		/// <returns>void</returns>
		/// <param name="val">element to push</param>
		constexpr decltype(auto) push_back(rvalue val) noexcept
		{
			check_allocate();
			start[used] = val;
			++used;
		}
		/// <summary>
		///	Inserting an element at the end of a data block.
		///	This function itself extends the data block if it is necessary.
		/// </summary>
		/// <returns>void</returns>
		/// <param name="val">element to push</param>
		constexpr decltype(auto) push_back(const_reference val) noexcept
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
		constexpr decltype(auto) push_back(pointer val, size_t c) noexcept
		{
			insert(used, val, c);
		}
		/// <summary>
		///	Deletes the last element from the data block.
		///	(As long as the data is not overwritten by new data and/or the block is not moved to a new location, the deleted information will still exist.)
		///	The size allocate of the block does not change.
		/// </summary>
		/// <returns>void</returns>
		constexpr decltype(auto) pop_back() noexcept
		{
			if (used > 0)
				--used;
		}
	#if (0)
		/// <summary>
		/// 
		/// </summary>
		template <class... Args>
		constexpr decltype(auto) emplace(size_t pos, Args&&... args)
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
		constexpr decltype(auto) emplace_back(Args&&... args)
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
		constexpr decltype(auto) move_insert(size_t place, rvalue val)
		{
			if (insert_correct(place))
			{
				auto place_used = place + 1;
				copy(start + place_used, start + place, used - place_used);
			}
			start[place] = val;
		}
		/// <summary>
		///	HIGH TIME CONSUMPTION FUNCTION (memcpy)
		/// Inserting an item into a data block and moving other data if a collision occurred.
		/// </summary>
		/// <returns>void</returns>
		/// <param name="place">place index element</param>
		/// <param name="val">element to push</param>
		constexpr decltype(auto) move_insert(size_t place, const_reference val)
		{
			if (insert_correct(place))
			{
				auto place_used = place + 1;
				copy(start + place_used, start + place, used - place_used);
			}
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
		constexpr decltype(auto) move_insert(size_t place, pointer val, size_t count)
		{
			if (count > 0)
			{
				insert_correct(place, count);
				copy((start + place), val, count);
			}
		}
		/// <summary>
		///	HIGH TIME CONSUMPTION FUNCTION (memcpy)
		/// Inserting an items into a data block and moving other data if a collision occurred.
		/// </summary>
		/// <returns>void</returns>
		/// <param name="place">place index element</param>
		/// <param name="v"> TODO </param>
		constexpr __inline decltype(auto) move_insert(size_t place, vector_rvalue v)
		{
			move_insert(place, v.data(), v.size());
		}
		/// <summary>
		///	HIGH TIME CONSUMPTION FUNCTION (memcpy)
		/// Inserting an items into a data block and moving other data if a collision occurred.
		/// </summary>
		/// <returns>void</returns>
		/// <param name="v"> TODO </param>
		constexpr __inline decltype(auto) move_insert(vector_rvalue from)
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
		constexpr __inline decltype(auto) move_insert(size_t place, vector_reference v)
		{
			move_insert(place, v.data(), v.size());
		}
		/// <summary>
		///	HIGH TIME CONSUMPTION FUNCTION (memcpy)
		/// Inserting an items into a data block and moving other data if a collision occurred.
		/// </summary>
		/// <returns>void</returns>
		/// <param name="v"> TODO </param>
		constexpr __inline decltype(auto) move_insert(vector_reference from)
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
		constexpr __inline decltype(auto) move_insert(size_t place, list_rvalue l)
		{
			move_insert(place, const_cast<pointer>(l.begin()), l.size());
		}
		/// <summary>
		///	HIGH TIME CONSUMPTION FUNCTION (memcpy)
		/// Inserting an items into a data block and moving other data if a collision occurred.
		/// </summary>
		/// <returns>void</returns>
		/// <param name="l"> TODO </param>
		constexpr __inline decltype(auto) move_insert(list_rvalue from)
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
		constexpr __inline decltype(auto) move_insert(size_t place, list_reference l)
		{
			move_insert(place, const_cast<pointer>(l.begin()), l.size());
		}
		/// <summary>
		///	HIGH TIME CONSUMPTION FUNCTION (memcpy)
		/// Inserting an items into a data block and moving other data if a collision occurred.
		/// </summary>
		/// <returns>void</returns>
		/// <param name="l"> TODO </param>
		constexpr __inline decltype(auto) move_insert(list_reference from)
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
		constexpr decltype(auto) insert(size_t place, rvalue val) noexcept
		{
			without_correct(place);
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
		constexpr decltype(auto) insert(size_t place, pointer val, size_t count) noexcept
		{
			if (count > 0)
			{
				without_correct(place, count);
				copy(start + place, val, count);
			}
		}
		/// <summary>
		/// Inserting an items into a data block.
		///	Without moving other data(Erases data) if a collision occurred.
		/// </summary>
		/// <returns>void</returns>
		/// <param name="place">place index element</param>
		/// <param name="from"> TODO </param>
		constexpr __inline decltype(auto) insert(size_t place, vector_rvalue from) noexcept
		{
			insert(place, from.data(), from.size());
		}
		/// <summary>
		/// Inserting elements at the end of a data block.
		///	Without moving other data(Erases data) if a collision occurred.
		/// </summary>
		/// <returns>void</returns>
		/// <param name="l"> TODO </param>
		constexpr __inline decltype(auto) insert(vector_rvalue from) noexcept
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
		constexpr __inline decltype(auto) insert(size_t place, vector_reference from) noexcept
		{
			insert(place, from.data(), from.size());
		}
		/// <summary>
		/// Inserting elements at the end of a data block.
		///	Without moving other data(Erases data) if a collision occurred.
		/// </summary>
		/// <returns>void</returns>
		/// <param name="l"> TODO </param>
		constexpr __inline decltype(auto) insert(vector_reference from) noexcept
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
		constexpr __inline decltype(auto) insert(size_t place, list_rvalue from) noexcept
		{
			insert(place, from.begin(), from.size());
		}
		/// <summary>
		/// Inserting elements at the end of a data block.
		///	Without moving other data(Erases data) if a collision occurred.
		/// </summary>
		/// <returns>void</returns>
		/// <param name="list"> TODO </param>
		constexpr __inline decltype(auto) insert(list_rvalue from) noexcept
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
		constexpr __inline decltype(auto) insert(size_t place, list_reference from) noexcept
		{
			insert(place, const_cast<pointer>(from.begin()), from.size());
		}
		/// <summary>
		/// Inserting elements at the end of a data block.
		///	Without moving other data(Erases data) if a collision occurred.
		/// </summary>
		/// <returns>void</returns>
		/// <param name="list"> TODO </param>
		constexpr __inline decltype(auto) insert(list_reference from) noexcept
		{
			insert(used, from);
		}
		/// <summary>
		/// Count items entered into the data block
		/// </summary>
		/// <returns>size_t</returns>
		constexpr decltype(auto) size() noexcept
		{
			return used;
		}
		/// <summary>
		/// Count items entered into the data block
		/// </summary>
		/// <returns>size_t</returns>
		constexpr decltype(auto) size() const noexcept
		{
			return used;
		}
		/// <summary>
		/// Maximum number of items that can be placed without the need to reallocate.
		/// </summary>
		/// <returns>size_t</returns>
		constexpr decltype(auto) capacity() noexcept
		{
			return allocated;
		}
		/// <summary>
		/// Maximum number of items that can be placed without the need to reallocate.
		/// </summary>
		/// <returns>size_t</returns>
		constexpr decltype(auto) capacity() const noexcept
		{
			return allocated;
		}
		/// <summary>
		/// Copies the contents of the data block.
		///	Allocating memory for the contents as in the original vector.
		/// </summary>
		/// <returns>void</returns>
		/// <param name="v">Vector<value>*</param>
		constexpr decltype(auto) copy(vector_pointer v) noexcept
		{
			v->allocate(allocated);
			if (used)
				copy(v->start, start, v->used = used);
		}
		/// <summary>
		/// Copies the contents of the data block.
		///	Allocating memory for the contents as in the original vector.
		/// </summary>
		/// <returns>void</returns>
		/// <param name="v">Vector<value>*</param>
		constexpr decltype(auto) copy(vector_pointer v) const noexcept
		{
			v->allocate(allocated);
			if (used)
				copy(v->start, start, v->used = used);
		}
		/// <summary>
		/// Clears the contents of the data block without erasing previous data.
		/// </summary>
		/// <returns>void</returns>
		constexpr decltype(auto) clear() noexcept
		{
			used = 0;
		}
		/// <summary>
		/// Clears the contents of the data block without erasing previous data.
		/// </summary>
		/// <returns>void</returns>
		constexpr decltype(auto) clear() const noexcept
		{
			used = 0;
		}
		/// <summary>
		/// Sets the standard value (zero) for the i element
		/// </summary>
		/// <returns>void</returns>
		/// <param name="i">Index to the element</param>
		constexpr decltype(auto) erase(size_t i) noexcept
		{
			start[i] = value();
		}
		/// <summary>
		/// Address for writing the next item.
		///		-(It is necessary to check the out of range!)
		///		-(Does not increase the current size of the data block!)
		///	Used in iterators.
		/// </summary>
		/// <returns>void</returns>
		constexpr decltype(auto) last() noexcept
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
		constexpr decltype(auto) last() const noexcept
		{
			return start + used;
		}
		/// <summary>
		/// The cell for writing the first element. 
		///	It is necessary to check the out of range!
		///	Does not increase the current size of the data block!
		/// </summary>
		/// <returns>rvalue</returns>
		constexpr decltype(auto) front() noexcept
		{
			return rvalue(*start);
		}
		/// <summary>
		/// The cell for writing the first element. 
		///	It is necessary to check the out of range!
		///	Does not increase the current size of the data block!
		/// </summary>
		/// <returns>rvalue</returns>
		constexpr decltype(auto) front() const noexcept
		{
			return rvalue(*start);
		}
		/// <summary>
		/// The cell for writing the next element. 
		///	It is necessary to check the out of range!
		///	Does not increase the current size of the data block!
		/// </summary>
		/// <returns>rvalue</returns>
		constexpr decltype(auto) back() noexcept
		{
			return rvalue(*(start + used));
		}
		/// <summary>
		/// The cell for writing the next element. 
		///	It is necessary to check the out of range!
		///	Does not increase the current size of the data block!
		/// </summary>
		/// <returns>rvalue</returns>
		constexpr decltype(auto) back() const noexcept
		{
			return rvalue(*(start + used));
		}
		/// <summary>
		///	Data block
		/// </summary>
		/// <returns>pointer to the data block</returns>
		constexpr decltype(auto) data() noexcept
		{
			return start;
		}
		/// <summary>
		///	Data block
		/// </summary>
		/// <returns>pointer to the data block</returns>
		constexpr decltype(auto) data() const noexcept
		{
			return start;
		}
		/// <summary>
		///	Swaps the contents of the i and j elements
		/// </summary>
		/// <returns>void</returns>
		/// <param name="i">Index to the element</param>
		/// <param name="j">Index to the element</param>
		constexpr decltype(auto) swap(size_t i, size_t j)
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
		constexpr decltype(auto) swap(vector_rvalue v) noexcept
		{
			std::swap(*this, v);
		}
		/// <summary>
		///	Swaps the contents Vectors
		/// </summary>
		/// <returns>void</returns>
		/// <param name="v">Vector<value>&</param>
		constexpr decltype(auto) swap(vector_reference v) noexcept
		{
			std::swap(*this, v);
		}
		/// <summary>
		///	Swaps the contents const Vectors
		/// </summary>
		/// <returns>void</returns>
		/// <param name="v">Vector<value>&&</param>
		constexpr decltype(auto) swap(const vector_rvalue v) const noexcept
		{
			std::swap(*this, v);
		}
		/// <summary>
		///	Swaps the contents const Vectors
		/// </summary>
		/// <returns>void</returns>
		/// <param name="v">Vector<value>&</param>
		constexpr decltype(auto) swap(const vector_reference v) const noexcept
		{
			std::swap(*this, v);
		}
		/// <summary>
		///	Test to empty.
		/// </summary>
		/// <returns>bool</returns>
		constexpr decltype(auto) empty() noexcept
		{
			return used == 0;
		}
		/// <summary>
		///	Test to empty.
		/// </summary>
		/// <returns>bool</returns>
		constexpr decltype(auto) empty() const noexcept
		{
			return used == 0;
		}
		/// <summary>
		///	Resize data block. 
		///	It doesn't erase data when you resize, but it can happen if you reduce the size and it gets copied to a new location. 
		/// </summary>
		/// <returns>void</returns>
		/// <param name="sz">New data block's size</param>
		constexpr decltype(auto) resize(size_t sz) noexcept
		{
			if ((used = sz) > allocated)
				allocate(used);
		}
		/// <summary>
		///	Frees up the data block.
		///	Makes the vector invalid.
		/// </summary>
		/// <returns>void</returns>
		constexpr decltype(auto) free() noexcept
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
		constexpr decltype(auto) free() const noexcept
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
		constexpr decltype(auto) reserve(size_t sz) noexcept
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
		constexpr decltype(auto) shrink_to_fit() noexcept
		{
			if (used < allocated)
				allocate(used);
		}
		
		constexpr decltype(auto) move(vector_rvalue v)
		{
			used = v.used;				v.used = 0;
			start = v.start;			v.start = nullptr;
			allocated = v.allocated;	v.allocated = 0;
		}
	
		/// <summary>
		/// Iterators
		/// </summary>
	
		constexpr decltype(auto) begin() noexcept
		{
			return iterator(start);
		}
		constexpr decltype(auto) end() noexcept
		{
			return iterator(last());
		}
		constexpr decltype(auto) begin() const noexcept
		{
			return iterator(start);
		}
		constexpr decltype(auto) end() const noexcept
		{
			return iterator(last());
		}
		constexpr decltype(auto) cbegin() noexcept
		{
			return const_iterator(start);
		}
		constexpr decltype(auto) cend() noexcept
		{
			return const_iterator(last());
		}
		constexpr decltype(auto) cbegin() const noexcept
		{
			return const_iterator(start);
		}
		constexpr decltype(auto) cend() const noexcept
		{
			return const_iterator(last());
		}
		constexpr decltype(auto) rbegin() noexcept
		{
			return reverse_iterator(end());
		}
		constexpr decltype(auto) rend() noexcept
		{
			return reverse_iterator(begin());
		}
		constexpr decltype(auto) rbegin() const noexcept
		{
			return reverse_iterator(end());
		}
		constexpr decltype(auto) rend() const noexcept
		{
			return reverse_iterator(begin());
		}
		constexpr decltype(auto) crbegin() noexcept
		{
			return const_reverse_iterator(cend());
		}
		constexpr decltype(auto) crend() noexcept
		{
			return const_reverse_iterator(cbegin());
		}
		constexpr decltype(auto) crbegin() const noexcept
		{
			return const_reverse_iterator(cend());
		}
		constexpr decltype(auto) crend() const noexcept
		{
			return const_reverse_iterator(cbegin());
		}
	
	
		constexpr decltype(auto) strict_equal_elements(vector_const_reference other)
		{
			if (start == other.start)
				return true;
			const auto tmp(*this);
			for (size_t i = 0; i < used; i++)
				if (tmp[i] != other[i])
					return false;
			return true;
		}
		constexpr decltype(auto) rough_parity(vector_const_reference other)
		{
			return used == other.used && allocated == other.allocated;
		}
		constexpr decltype(auto) strict_equality(vector_const_reference other)
		{
			return rough_parity(other) && strict_equal_elements(other);
		}
	
		constexpr operator bool()
		{
			return data() && (size() > 0);
		}
	
		constexpr decltype(auto) operator=(vector_rvalue v)
		{
			return vector(v);
		}
		constexpr decltype(auto) operator=(vector_reference v)
		{
			return vector(v);
		}
		/// <summary>
		/// Operator for inserting values from the initialization_list.
		///	The old vector values in the data block will be forgotten (lost).
		/// </summary>
		/// <returns>void</returns>
		/// <param name="v"> TODO </param>
		constexpr decltype(auto) operator()(list_rvalue v) noexcept
		{
			insert(v);
		}
		/// <summary>
		/// Operator for inserting values from the initialization_list.
		///	The old vector values in the data block will be forgotten (lost).
		/// </summary>
		/// <returns>void</returns>
		/// <param name="v"> TODO </param>
		constexpr decltype(auto) operator()(list_reference v) noexcept
		{
			insert(v);
		}
		/// <summary>
		/// API calling free()
		/// </summary>
		/// <returns>void</returns>
		constexpr decltype(auto) operator~() noexcept
		{
			free();
		}
		/// <summary>
		/// API calling swap()
		/// </summary>
		/// <returns>void</returns>
		/// <param name="v">Vector to swap</param>
		constexpr decltype(auto) operator^=(vector_rvalue v) noexcept
		{
			swap(v);
		}
		/// <summary>
		/// insert value
		/// </summary>
		/// <returns>void</returns>
		/// <param name="v">push value</param>
		constexpr decltype(auto) operator+=(rvalue c) noexcept
		{
			push_back(c);
		}
		/// <summary>
		/// insert value
		/// </summary>
		/// <returns>void</returns>
		/// <param name="v">push value</param>
		constexpr decltype(auto) operator+=(reference c) noexcept
		{
			push_back(c);
		}
		/// <summary>
		/// insert values from other vector.
		/// </summary>
		/// <returns>void</returns>
		/// <param name="v">push values from vector</param>
		constexpr decltype(auto) operator+=(vector_rvalue v) noexcept
		{
			insert(v);
		}
		/// <summary>
		/// insert values from other vector.
		/// </summary>
		/// <returns>void</returns>
		/// <param name="v">push values from vector</param>
		constexpr decltype(auto) operator+=(vector_reference v) noexcept
		{
			insert(v);
		}
		/// <summary>
		/// insert values from other vector pointer.
		/// Not needing to check null-pointer
		/// </summary>
		/// <returns>void</returns>
		/// <param name="v">push values from pointer vector</param>
		constexpr decltype(auto) operator+=(vector_pointer v) noexcept
		{
			if (v)
				insert(*v);
		}
		/// <summary>
		/// insert values from other vector.
		/// </summary>
		/// <returns>void</returns>
		/// <param name="v">push values from vector</param>
		constexpr decltype(auto) operator+=(const vector_rvalue v) const noexcept
		{
			insert(v);
		}
		/// <summary>
		/// API calling at(...)
		/// </summary>
		/// <param name="i">Index</param>
		/// <returns>reference</returns>
		constexpr decltype(auto) operator[](size_t i) noexcept
		{
			return at(i);
		}
		/// <summary>
		/// API calling at(...)
		/// </summary>
		/// <param name="i">Index</param>
		/// <returns>const reference value</returns>
		constexpr decltype(auto) operator[](size_t i) const noexcept
		{
			return at(i);
		}
	
		constexpr __inline decltype(auto) operator!=(vector_const_reference v)
		{
			return !operator==(v);
		}
		constexpr __inline decltype(auto) operator==(vector_const_reference v)
		{
			return rough_parity(v);
		}
	
		constexpr __inline decltype(auto) operator<(size_t i) noexcept
		{
			return used < i;
		}
		constexpr __inline decltype(auto) operator>(size_t i) noexcept
		{
			return used > i;
		}
		constexpr __inline decltype(auto) operator<=(size_t i) noexcept
		{
			return used <= i;
		}
		constexpr __inline decltype(auto) operator>=(size_t i) noexcept
		{
			return used >= i;
		}
		constexpr __inline decltype(auto) operator<(size_t i) const noexcept
		{
			return used < i;
		}
		constexpr __inline decltype(auto) operator>(size_t i) const noexcept
		{
			return used > i;
		}
		constexpr __inline decltype(auto) operator<=(size_t i) const noexcept
		{
			return used <= i;
		}
		constexpr __inline decltype(auto) operator>=(size_t i) const noexcept
		{
			return used >= i;
		}
		
		/// <summary>
		///	CONSTRUCTOR null
		/// </summary>
		constexpr Vector() noexcept
		{
			start = nullptr;
			allocated = used = 0;
		}
		/// <summary>
		///	CONSTRUCTOR initializer_list
		/// </summary>
		/// <param name="v"> TODO </param>
		constexpr Vector(list_const_reference v) : Vector()
		{
			insert(v);
		}
		/// <summary>
		///	CONSTRUCTOR move
		/// </summary>
		/// <param name="v"> TODO </param>
		constexpr Vector(list_rvalue v) : Vector()
		{
			insert(v);
		}
		/// <summary>
		///	CONSTRUCTOR copy
		/// </summary>
		/// <param name="v">reference to vector</param>
		constexpr Vector(vector_const_reference v) : Vector()
		{
			v.copy(this);
		}
		/// <summary>
		///	CONSTRUCTOR copy
		/// </summary>
		/// <param name="v">reference to vector</param>
		constexpr Vector(vector_rvalue v) noexcept
		{
			move(v);
		}
		/// <summary>
		///	CONSTRUCTOR reserve
		/// </summary>
		/// <param name="sz">Count elements to allocate</param>
		constexpr explicit Vector(size_t sz) : Vector()
		{
			allocate(sz);
		}
		/// <summary>
		///	CONSTRUCTOR insert from array
		/// </summary>
		/// <param name="sz">Count elements to allocate</param>
		/// <param name="ray">pointer to values</param>
		constexpr explicit Vector(size_t sz, pointer ray) : Vector()
		{
			push_back(ray, sz);
		}
		/// <summary>
		///	DESTRUCTOR
		/// </summary>
		~Vector() noexcept
		{
			free();
		}
	};
}
