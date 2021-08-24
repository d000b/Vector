#pragma once

namespace UltimaAPI
{
	template <typename type> class MultiVector;
}


template <typename __type__>
class UltimaAPI::MultiVector
{
public:
	using value = __type__;
	using rvalue = value&&;
	using pointer = value*;
	using reference = value&;
protected:

public:

};
