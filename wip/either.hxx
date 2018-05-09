#ifndef EITHER_HXX
#define EITHER_HXX

namespace engine_library{

template <typename L, typename R>
union EitherUnion
{
	EitherUnion(L l) : left(l)	{}
	EitherUnion(R r) : right(r) {}
	L left;
	R right;
};

template <typename L, typename R>
class either
{
public:
	EitherUnion<L,R> value;

	bool is_left;


	either( L l )
	:	value	(l),
		is_left	(true)
	{}

	either( R r )
	:	value	(r),
		is_left	(false)
	{}
};

}
#endif