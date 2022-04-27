#ifndef ENGINE_LIBRARY_UTIL
#define ENGINE_LIBRARY_UTIL

#include <vector>
#include <utility>

namespace engineLibrary {
	template <class T, class E>
	class Result {
		public:
		// TODO: Result.check() -> value. what would it return if error?
		// TODO: CheckedResult? Result.orElse(callback(Result))?
		// vars	TODO: make private?
		T value;
		E error;
		// TODO: fully understand what std::forward does here
		Result(T value_, E error_) : value{std::forward<T>(value_)}, error{error_} {}
		~Result() = default;
		// copy
		Result(const Result &) = default;
		Result& operator= (const Result &) = default;
		// move construct
		Result(Result&& other) = default;
		// move assign
		Result& operator= (Result&& other) = default;
	};

	template<class T> using list = std::vector<T>;
}      // namespace engineLibrary
#endif // ENGINE_LIBRARY_UTIL
