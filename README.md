This is my Not-STL, where I make slight modifications to STL methods that are suited to my liking. \

Fibonacci Priority Queue:
- Default ctor: O(1) \
- Copy ctor: O(n) \
- Move ctor: O(n)\
- Overloaded assignment: O(n)\
- push: O(1)\
- top: O(1)\
- size: O(1)\
- empty: O(1)\
- decrease_key: O(1)\
- - Pass in a value in the priority too, and the "lesser" value you want it to be promoted to.\


SmartSharedPointer: An RAII wrapper, with some additional functionality
- Default ctor: constructs and empty shared pointer with no use count, and nullptr data\
- Ctor 1: constructs a shared pointer from value T\
- Ctor 2: constructs a shared pointer from dynamic pointer T\
- Copy Ctor, Move Ctor, assignment: ame behavior as std::shared_ptr\
- reset, swap, use_count, unique, operators bool, ->, *: same behavior as std::shared_ptr\
- reset_all: resets all use cases of this shared pointer, and deallocates data. All data set to nullptr\