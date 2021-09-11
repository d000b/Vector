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

#define ALLOCATOR_VERSION_VECTOR // it is necessary for the BasicIterator to create a pattern
#include "../BasicIterator/BasicIterator.h"
#include "../Allocator/Allocator.h"


namespace UltimaAPI
{
	template <
		typename __type__ = int,
		class __container__ = ContainerSizer<__type__>,
		class __allocator__ = AllocatorLinear<__type__, __container__>
	>
	class Vector : public __allocator__
	{
	public:
		using allocator_inherited = __allocator__;
		using container_inherited =	typename allocator_inherited::container_inherited;
		using locate_inherited =	typename container_inherited::locate_inherited;

		using allocator =	allocator_inherited;
		using container =	container_inherited;
		using locator =		locate_inherited;
	public:
		// Value
		using value = __type__;
		using rvalue = value&&;
		using pointer = value*;
		using reference = value&;
		using refpointer = pointer&;
		using const_pointer = const pointer;
		using const_reference = const reference;
		// Initializer_list
		using list = std::initializer_list<value>;
		using list_rvalue = list&&;
		using list_pointer = list*;
		using list_reference = list&;
		using list_const_reference = const list_reference;
		// Vector
		using vector = Vector<value, container_inherited, allocator_inherited>;
		using vector_rvalue = vector&&;
		using vector_pointer = vector*;
		using vector_reference = vector&;
		using vector_const_reference = const vector_reference;
		// Iterator
		using iterator = BasicIterator<value>;
		using const_iterator = BasicIterator<const value>;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;
		
		static constexpr size_t npos = -1;

		static_assert(std::is_base_of<allocator, __allocator__>::value, "The inherited Allocator does not fit into the category");
		
	private:
		constexpr decltype(auto) get_data_pointer()
		{
			return refpointer(allocator::data);
		}
	public:
		/// <summary>
		/// Expands/controls the size/receives the value of the i element.
		/// </summary>
		/// <returns>reference</returns>
		/// <param name="i">Index</param>
		constexpr decltype(auto) at(size_t i)
		{
			if (i >= size())
			{
				locator::new_size(i + 1);
				allocator::check_allocate();
			}
			return allocator::access_to_element(i);
		}
		/// <summary>
		///	BEWARE out-from-range
		/// receives the value of the i element.
		/// </summary>
		/// <returns>reference value</returns>
		/// <param name="i">Index</param>
		constexpr decltype(auto) at(size_t i) const
		{
			return allocator::access_to_element(i);
		}
		/// <summary>
		///	Inserting an element at the end of a data block.
		///	This function itself extends the data block if it is necessary.
		/// </summary>
		/// <returns>void</returns>
		/// <param name="val">element to push</param>
		constexpr decltype(auto) push_back(rvalue val) noexcept
		{
			allocator::push_back_value(val);
		}
		/// <summary>
		///	Inserting an element at the end of a data block.
		///	This function itself extends the data block if it is necessary.
		/// </summary>
		/// <returns>void</returns>
		/// <param name="val">element to push</param>
		constexpr decltype(auto) push_back(const_reference val) noexcept
		{
			allocator::push_back_value(val);
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
			allocator::push_back_value(val, c);
		}
		/// <summary>
		///	Deletes the last element from the data block.
		///	(As long as the data is not overwritten by new data and/or the block is not moved to a new location, the deleted information will still exist.)
		///	The size allocate of the block does not change.
		/// </summary>
		/// <returns>void</returns>
		constexpr decltype(auto) pop_back() noexcept
		{
			allocator::pop_back();
		}
	#if (0)
		/// <summary>
		/// 
		/// </summary>
		template <class... Args>
		constexpr decltype(auto) emplace(size_t pos, Args&&... args)
		{
			if (pos >= size())
			{
				size() = pos + 1;
				if (pos >= allocator::capacity())
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
			emplace(size(), args);
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
			if (allocator::insert_correct(place))
			{
				auto place_used = place + 1;
				allocator::copy(data() + place_used, data() + place, size() - place_used);
			}
			allocator::access_to_element(place) = val;
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
			if (allocator::insert_correct(place))
			{
				auto place_used = place + 1;
				copy(data() + place_used, data() + place, size() - place_used);
			}
			allocator::access_to_element(place) = val;
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
				allocator::insert_correct(place, count);
				allocator::copy((data() + place), val, count);
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
			move_insert(size(), from);
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
			move_insert(size(), from);
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
			move_insert(size(), from);
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
			move_insert(size(), from);
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
			allocator::without_correct(place);
			allocator::access_to_element(place) = val;
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
				allocator::without_correct(place, count);
				allocator::copy(data() + place, val, count);
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
			insert(size(), from);
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
			insert(size(), from);
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
			insert(size(), from);
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
			insert(size(), from);
		}
		/// <summary>
		/// TODO
		/// </summary>
		/// <returns>const pointer to memory block</returns>
		constexpr __inline decltype(auto) data()
		{
			return const_pointer(allocator::data);
		}
		/// <summary>
		/// Count items entered into the data block
		/// </summary>
		/// <returns>size_t</returns>
		constexpr decltype(auto) size() noexcept
		{
			return locator::size();
		}
		/// <summary>
		/// Count items entered into the data block
		/// </summary>
		/// <returns>size_t</returns>
		constexpr decltype(auto) size() const noexcept
		{
			return locator::size();
		}
		/// <summary>
		/// Maximum number of items that can be placed without the need to reallocate.
		/// </summary>
		/// <returns>size_t</returns>
		constexpr decltype(auto) capacity() noexcept
		{
			return locator::capacity();
		}
		/// <summary>
		/// Maximum number of items that can be placed without the need to reallocate.
		/// </summary>
		/// <returns>size_t</returns>
		constexpr decltype(auto) capacity() const noexcept
		{
			return locator::capacity();
		}
		/// <summary>
		/// Copies the contents of the data block.
		///	Allocating memory for the contents as in the original vector.
		/// </summary>
		/// <returns>void</returns>
		/// <param name="v">Vector<value>*</param>
		constexpr decltype(auto) copy(vector_pointer v) noexcept
		{
			v->allocate(allocator::capacity());
			if (size())
				copy(v->get_data_pointer(), data(), v->new_size(size()));
		}
		/// <summary>
		/// Copies the contents of the data block.
		///	Allocating memory for the contents as in the original vector.
		/// </summary>
		/// <returns>void</returns>
		/// <param name="v">Vector<value>*</param>
		constexpr decltype(auto) copy(vector_pointer v) const noexcept
		{
			v->allocate(allocator::capacity());
			if (size())
				copy(v->get_data_pointer(), data(), v->new_size(size()));
		}
		/// <summary>
		/// Clears the contents of the data block without erasing previous data.
		/// </summary>
		/// <returns>void</returns>
		constexpr decltype(auto) clear() noexcept
		{
			locator::new_size(0);
		}
		/// <summary>
		/// Clears the contents of the data block without erasing previous data.
		/// </summary>
		/// <returns>void</returns>
		constexpr decltype(auto) clear() const noexcept
		{
			locator::new_size(0);
		}
		/// <summary>
		/// Sets the standard value (zero) for the i element
		/// </summary>
		/// <returns>void</returns>
		/// <param name="i">Index to the element</param>
		constexpr decltype(auto) erase(size_t i) noexcept
		{
			allocator::access_to_element(i) = value();
		}
		/// <summary>
		/// Address for writing the next item.
		///		-(It is necessary to check the out of range!)
		///		-(Does not increase the current size of the data block!)
		///	size() in iterators.
		/// </summary>
		/// <returns>void</returns>
		constexpr decltype(auto) last() noexcept
		{
			return locator::get_last();
		}
		/// <summary>
		/// Address for writing the next item.
		///		-(It is necessary to check the out of range!)
		///		-(Does not increase the current size of the data block!)
		///	size() in iterators.
		/// </summary>
		/// <returns>value&&</returns>
		constexpr decltype(auto) last() const noexcept
		{
			return locator::get_last();
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
			allocator::insert_correct(i > j ? i : j);
			std::swap(allocator::access_to_element(i), allocator::access_to_element(j));
		}
		constexpr decltype(auto) swap(size_t i, size_t j) const
		{
			if (i == j)
				return;
			allocator::insert_correct(i > j ? i : j);
			std::swap(allocator::access_to_element(i), allocator::access_to_element(j));
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
			return size() == 0;
		}
		/// <summary>
		///	Test to empty.
		/// </summary>
		/// <returns>bool</returns>
		constexpr decltype(auto) empty() const noexcept
		{
			return size() == 0;
		}
		/// <summary>
		///	Resize data block. 
		///	It doesn't erase data when you resize, but it can happen if you reduce the size and it gets copied to a new location. 
		/// </summary>
		/// <returns>void</returns>
		/// <param name="sz">New data block's size</param>
		constexpr decltype(auto) resize(size_t sz) noexcept
		{
			allocator::check_allocate(sz);
			locator::new_size(sz);
		}
		/// <summary>
		///	Frees up the data block.
		///	Makes the vector invalid.
		/// </summary>
		/// <returns>void</returns>
		constexpr decltype(auto) free() noexcept
		{
			allocator::deallocate_memory(0);
		}
		/// <summary>
		///	Frees up the data block.
		///	Makes the vector invalid.
		/// </summary>
		/// <returns>void</returns>
		constexpr decltype(auto) free() const noexcept
		{
			allocator::deallocate_memory(0);
		}
		/// <summary>
		/// TODO
		/// </summary>
		/// <returns>void</returns>
		/// <param name="sz">New size</param>
		constexpr decltype(auto) reserve(size_t sz) noexcept
		{
			allocator::allocate_memory(sz);
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
			return allocator::size_value();
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
			allocator::shrink_to_fit();
		}
		constexpr decltype(auto) move(vector_rvalue v)
		{
			allocator::new_size(v.size());			v.allocator::new_size(0);
			get_data_pointer() = v.data();			v.get_data_pointer() = nullptr;
			allocator::new_capacity(v.capacity());	v.allocator::new_capacity(0);
		}
	
		/// <summary>
		/// Iterators
		/// </summary>
	
		constexpr decltype(auto) begin() noexcept
		{
			return iterator(data());
		}
		constexpr decltype(auto) end() noexcept
		{
			return iterator(last());
		}
		constexpr decltype(auto) begin() const noexcept
		{
			return iterator(data());
		}
		constexpr decltype(auto) end() const noexcept
		{
			return iterator(last());
		}
		constexpr decltype(auto) cbegin() noexcept
		{
			return const_iterator(data());
		}
		constexpr decltype(auto) cend() noexcept
		{
			return const_iterator(last());
		}
		constexpr decltype(auto) cbegin() const noexcept
		{
			return const_iterator(data());
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
			if (data() == other.data())
				return true;
			const auto tmp(*this);
			for (size_t i = 0; i < size(); i++)
				if (tmp[i] != other[i])
					return false;
			return true;
		}
		constexpr decltype(auto) rough_parity(vector_const_reference other)
		{
			return size() == other.size() && allocator::capacity() == other.allocator::capacity();
		}
		constexpr decltype(auto) strict_equality(vector_const_reference other)
		{
			return rough_parity(other) && strict_equal_elements(other);
		}
	
		constexpr operator bool()
		{
			return data()() && (size() > 0);
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
			return size() < i;
		}
		constexpr __inline decltype(auto) operator>(size_t i) noexcept
		{
			return size() > i;
		}
		constexpr __inline decltype(auto) operator<=(size_t i) noexcept
		{
			return size() <= i;
		}
		constexpr __inline decltype(auto) operator>=(size_t i) noexcept
		{
			return size() >= i;
		}
		constexpr __inline decltype(auto) operator<(size_t i) const noexcept
		{
			return size() < i;
		}
		constexpr __inline decltype(auto) operator>(size_t i) const noexcept
		{
			return size() > i;
		}
		constexpr __inline decltype(auto) operator<=(size_t i) const noexcept
		{
			return size() <= i;
		}
		constexpr __inline decltype(auto) operator>=(size_t i) const noexcept
		{
			return size() >= i;
		}
		
		/// <summary>
		///	CONSTRUCTOR null
		/// </summary>
		constexpr Vector() noexcept = default;
		/// <summary>
		///	CONSTRUCTOR initializer_list
		/// </summary>
		/// <param name="v"> TODO </param>
		constexpr Vector(list_const_reference v) noexcept : Vector()
		{
			insert(v);
		}
		/// <summary>
		///	CONSTRUCTOR move
		/// </summary>
		/// <param name="v"> TODO </param>
		constexpr Vector(list_rvalue v) noexcept : Vector()
		{
			insert(v);
		}
		/// <summary>
		///	CONSTRUCTOR copy
		/// </summary>
		/// <param name="v">reference to vector</param>
		constexpr Vector(vector_const_reference v) noexcept : Vector()
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
		constexpr explicit Vector(size_t sz) noexcept : Vector()
		{
			allocator::allocate_memory(sz);
		}
		/// <summary>
		///	CONSTRUCTOR insert from array
		/// </summary>
		/// <param name="sz">Count elements to allocate</param>
		/// <param name="ray">pointer to values</param>
		constexpr explicit Vector(size_t sz, pointer ray) noexcept : Vector()
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
