#ifndef ENGINE_LIBRARY_UTIL
#define ENGINE_LIBRARY_UTIL

namespace engineLibrary {
	template <class T, class E>
	class Result {
		public:
		// vars
		T value;
		E error;
		// bool conversion
		// operator bool() const { return !error; }; // TODO: allow implicit conversion from error?
		// big 6                                     // or pass error value in template or something
		// constructor, destructor
		// Result()  = default;
		Result(T value, E error) : value{value}, error{error} {}
		~Result() = default;
		// copy
		Result(const Result &) = default;
		Result& operator= (const Result &) = default;
		// move
		Result(Result &&) = default;
		Result& operator= (Result &&) = default;
	};
}      // namespace engineLibrary
#endif // ENGINE_LIBRARY_UTIL
