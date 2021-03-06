#ifndef SMARTSHAREDPTR_HPP
#define SMARTSHAREDPTR_HPP

#include <vector>
#include <cstdint>
#include <algorithm>
#include <iostream>

namespace nstd {

template<typename T>
class SmartSharedPtr {
public:
    SmartSharedPtr() :
        data{nullptr},
        use_count_{0}{
    } // default

    ~SmartSharedPtr() {
        if (shared.empty() && use_count_ == 1) {
            delete data;
        } else if (use_count_ > 0) {
            for (auto ptr: shared) {
                auto it = std::remove_if(ptr->shared.begin(), ptr->shared.end(), 
                    [this](auto p)->bool{ return p == this; });
                ptr->shared.erase(it);
                ptr->use_count_--;
            }
        }
    } // destructor

    SmartSharedPtr(T data_in) :
        SmartSharedPtr() {
        data = new T;
        *data = data_in;
        use_count_ = 1;
    } // custom ctor

    SmartSharedPtr(T* data_in) :
        SmartSharedPtr() {
        data = data_in;
        use_count_ = 1;
    } // custom ctor

    SmartSharedPtr(const SmartSharedPtr& other) :
        SmartSharedPtr() {
        data = other.data;
        shared = other.shared;
        shared.push_back((SmartSharedPtr*)&other);
        use_count_ = other.use_count_ + 1;
        for (auto ptr: shared) {
            ptr->shared.push_back(this);
            ptr->use_count_++;
        }
    } // copy ctor from another shared ptr

    friend void swap_(SmartSharedPtr& first, SmartSharedPtr& second) {
        if (first.data == second.data) {
            return;
        }
        using std::swap;
        swap(first.shared, second.shared);
        swap(first.data, second.data);
        swap(first.use_count_, second.use_count_);

        for (auto ptr: first.shared) {
            std::replace_if(ptr->shared.begin(), ptr->shared.end(),
                 [first, second](auto p)->bool{ return p == &second; }, &first);
        }
        for (auto ptr: second.shared) {
            std::replace_if(ptr->shared.begin(), ptr->shared.end(),
                 [first, second](auto p)->bool{ return p == &first; }, &second);            
        }
    } // custom swap for safety

    SmartSharedPtr(SmartSharedPtr&& other) :
	    SmartSharedPtr() {
	    swap_(*this, other);
    } // Move ctor

    SmartSharedPtr& operator=(const SmartSharedPtr& rhs) {
        // dont use copy-swap idiom to avoid unecesary work in desctruction
        if (data == rhs.data) {
            return *this;
        }
        if (shared.empty() && use_count_ > 0) {
            delete data;
        } else if (!shared.empty()) {
            for (auto ptr: shared) {
                auto it = std::remove_if(ptr->shared.begin(), ptr->shared.end(),
                    [this](auto p)->bool{ return p == this; });
                ptr->shared.erase(it);
                ptr->use_count_--;
            }
        }
        use_count_ = rhs.use_count_ + 1;
        data = rhs.data;
        shared = rhs.shared;
        shared.push_back((SmartSharedPtr*)&rhs);
        for (auto ptr: shared) {
            ptr->shared.push_back(this);
            ptr->use_count_++;
        }
        return *this;
    } // assignement operator

    explicit operator bool() const noexcept{
        return data != nullptr;
    }

    T& operator*() const noexcept{
        return *data;
    }

    T* operator->() const noexcept{
        return data;
    }

    T* get() const noexcept {
        return data;
    }

    std::size_t use_count() const noexcept {
        return use_count_;
    }

    bool unique() const noexcept {
        return use_count_ == 1;
    }

    void reset() noexcept {
        if (use_count_ == 0) {
            return;
        } else if (shared.empty()){
            delete data;
        } else {
            for (auto ptr: shared) {
                auto it = std::remove_if(ptr->shared.begin(), ptr->shared.end(), 
                    [this](auto p)->bool{ return p == this; });
                ptr->shared.erase(it);
                ptr->use_count_--;
            }
            shared.clear();
        }
        use_count_ = 0;
        data = nullptr;
    }

    void reset_all() noexcept {
        if (use_count_ == 0) {
            return;
        } else if (!shared.empty()) {
            delete data;
            for (auto ptr: shared) {
                ptr->shared.clear();
                ptr->use_count_ = 0;
                ptr->data = nullptr;
            }
            shared.clear();
        }
        data = nullptr;
        use_count_ = 0;
    }

    void swap(SmartSharedPtr& other) noexcept {
        swap_(*this, other);
    }


private:
    std::vector<SmartSharedPtr*> shared;
    T* data;
    std::size_t use_count_;
};

template<typename T>
SmartSharedPtr<T>& make_shared(T& data_in) noexcept {
    return SmartSharedPtr(data_in);
}

template<typename T>
SmartSharedPtr<T>& make_shared(T* data_in) noexcept {
    return SmartSharedPtr(data_in);
}

template<typename T>
bool are_shared(const SmartSharedPtr<T>& first, const SmartSharedPtr<T>& second) noexcept {
    return first.get() == second.get();
}

template<typename T>
bool operator==(const SmartSharedPtr<T>& lhs, const SmartSharedPtr<T> rhs) {
    return *lhs == *rhs;
}

template<typename T>
bool operator!=(const SmartSharedPtr<T>& lhs, const SmartSharedPtr<T> rhs) {
    return *lhs != *rhs;
}

template<typename T>
bool operator<(const SmartSharedPtr<T>& lhs, const SmartSharedPtr<T> rhs) {
    return *lhs < *rhs;
}

template<typename T>
bool operator>(const SmartSharedPtr<T>& lhs, const SmartSharedPtr<T> rhs) {
    return *lhs > *rhs ;
}

template<typename T>
bool operator>=(const SmartSharedPtr<T>& lhs, const SmartSharedPtr<T> rhs) {
    return *lhs >= *rhs;
}

template<typename T>
bool operator<=(const SmartSharedPtr<T>& lhs, const SmartSharedPtr<T> rhs) {
    return return *lhs <= *rhs;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const SmartSharedPtr<T>& streamer) {
    return os << *streamer;
}

template<typename T>
std::istream& operator>>(std::istream& is, SmartSharedPtr<T>& streamer) {
    return is >> *streamer;
}

} // namespace nstd

#endif /* SMARTSHAREDPTR_HPP */