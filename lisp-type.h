#pragma once

#include <typeinfo>
#include <cxxabi.h>
#include <string>
#include <variant>

namespace detail {
    template<typename T, typename>
    struct is_incomplete : ::std::true_type {};

    template<typename T>
    struct is_incomplete<T, decltype(sizeof(T))> : ::std::false_type {};
};

namespace std {
    template<typename T>
    struct is_incomplete : detail::is_incomplete<T, std::size_t>::type {};
}

struct type_traits;

typedef const type_traits* Type;

struct type_traits {
    template <typename U> friend struct type_traits_info;

#define implement(type, type_traits_func) virtual type type_traits_func() const = 0;
    implement(bool, is_void);
//    implement(bool, is_null_pointer);
    implement(bool, is_integral);
//    implement(bool, is_floating_point);
//    implement(bool, is_array);
//    implement(bool, is_enum);
//    implement(bool, is_union);
//    implement(bool, is_class);
    implement(bool, is_function);
    implement(bool, is_pointer);
    implement(bool, is_null_pointer);
//    implement(bool, is_lvalue_reference);
//    implement(bool, is_rvalue_reference);
//    implement(bool, is_member_object_pointer);
//    implement(bool, is_member_function_pointer);
//
//    implement(bool, is_fundamental);
//    implement(bool, is_arithmetic);
    implement(bool, is_scalar);
//    implement(bool, is_object);
//    implement(bool, is_compound);
//    implement(bool, is_reference);
//    implement(bool, is_member_pointer);
//
//    implement(bool, is_const);
//    implement(bool, is_volatile);
//    implement(bool, is_trivial);
//    implement(bool, is_trivially_copyable);
//    implement(bool, is_standard_layout);
//    implement(bool, is_pod);
//    implement(bool, is_literal_type);
//    implement(bool, has_unique_object_representations);
//    implement(bool, is_empty);
//    implement(bool, is_polymorphic);
//    implement(bool, is_abstract);
//    implement(bool, is_final);
//    implement(bool, is_aggregate);
//    implement(bool, is_signed);
//    implement(bool, is_unsigned);

    implement(bool, is_incomplete);
//
//    implement(size_t, alignment_of);
//    implement(size_t, rank);
//
    virtual bool before(const type_traits& type) const = 0;
    virtual bool operator==(const type_traits& type) const = 0;
    virtual bool operator!=(const type_traits& type) const = 0;

    virtual const size_t hash_code() const = 0;
    virtual const size_t size() const = 0;
    virtual std::string name() const = 0;
    virtual std::string pretty_name() const = 0;
//
//    virtual const type_traits& remove_cv() const = 0;
//    virtual const type_traits& remove_const() const = 0;
//    virtual const type_traits& remove_volatile() const = 0;
//    virtual const type_traits& add_cv() const = 0;
//    virtual const type_traits& add_const() const = 0;
//    virtual const type_traits& add_volatile() const = 0;
//    virtual const type_traits& remove_reference() const = 0;
//    virtual const type_traits& add_pointer() const = 0;
    virtual const type_traits* remove_pointer() const = 0;
//    virtual const type_traits& make_signed() const = 0;
//    virtual const type_traits& make_unsigned() const = 0;
//    virtual const type_traits& remove_extent() const = 0;
#undef implement
    type_traits() = default;

private:
    virtual const std::type_info& type_info() const = 0;

    type_traits& operator=(const type_traits&) = delete;
    type_traits(const type_traits&) = delete;
};

template <typename T>
struct type_traits_info : type_traits {
#define implement(type, type_traits_func) type type_traits_func() const { return std::type_traits_func<T>::value; }
#define implement_typename(type_traits_func, arg) auto type_traits_func() { return std::type_traits_func<T, arg>::value; }

    implement(bool, is_void);
//    implement(bool, is_null_pointer);
    implement(bool, is_integral);
//    implement(bool, is_floating_point);
//    implement(bool, is_array);
//    implement(bool, is_enum);
//    implement(bool, is_union);
//    implement(bool, is_class);
    implement(bool, is_function);
    implement(bool, is_pointer);
    implement(bool, is_null_pointer);
//    implement(bool, is_lvalue_reference);
//    implement(bool, is_rvalue_reference);
//    implement(bool, is_member_object_pointer);
//    implement(bool, is_member_function_pointer);
//
//    implement(bool, is_fundamental);
//    implement(bool, is_arithmetic);
    implement(bool, is_scalar);
//    implement(bool, is_object);
//    implement(bool, is_compound);
//    implement(bool, is_reference);
//    implement(bool, is_member_pointer);
//
//    implement(bool, is_const);
//    implement(bool, is_volatile);
//    implement(bool, is_trivial);
//    implement(bool, is_trivially_copyable);
//    implement(bool, is_standard_layout);
//    implement(bool, is_pod);
//    implement(bool, is_literal_type);
//    implement(bool, has_unique_object_representations);
//    implement(bool, is_empty);
//    implement(bool, is_polymorphic);
//    implement(bool, is_abstract);
//    implement(bool, is_final);
//    implement(bool, is_aggregate);
//    implement(bool, is_signed);
//    implement(bool, is_unsigned);

    implement(bool, is_incomplete);
//
//    implement(size_t, alignment_of);
//    implement(size_t, rank);
//
//    template<unsigned N = 0>
//    implement_typename(extent, N);
//
//    template<typename U>
//    implement_typename(is_same, U);
//
//    template<typename U>
//    implement_typename(is_base_of, U);
//
//    template<typename U>
//    implement_typename(is_convertible, U);
//
//    template<typename ...Args>
//    implement_typename(is_invocable, Args...);
//
//    template<typename ...Args>
//    implement_typename(is_invocable_r, Args...);
//
//    template<typename ...Args>
//    implement_typename(is_nothrow_invocable, Args...);
//
//    template<typename ...Args>
//    implement_typename(is_nothrow_invocable_r, Args...);
//
//    const type_traits& remove_cv() const override {
//        static const type_traits& type = type_traits_info<typename std::remove_cv<T>::type>();
//        return type;
//    }
//
//    const type_traits& remove_const() const override {
//        static const type_traits& type = type_traits_info<typename std::remove_const<T>::type>();
//        return type;
//    }
//
//    const type_traits& remove_volatile() const override {
//        static const type_traits& type = type_traits_info<typename std::remove_volatile<T>::type>();
//        return type;
//    }
//
//    const type_traits& add_cv() const override {
//        static const type_traits& type = type_traits_info<typename std::add_cv<T>::type>();
//        return type;
//    }
//
//    const type_traits& add_const() const override {
//        static const type_traits& type = type_traits_info<typename std::add_const<T>::type>();
//        return type;
//    }
//
//    const type_traits& add_volatile() const override {
//        static const type_traits& type = type_traits_info<typename std::add_volatile<T>::type>();
//        return type;
//    }
//
//    const type_traits& remove_reference() const override {
//        static const type_traits& type = type_traits_info<typename std::remove_reference<T>::type>();
//        return type;
//    }
//
//    const type_traits& add_pointer() const override {
//        static const type_traits& type = type_traits_info<typename std::add_pointer<T>::type>();
//        return type;
//    }
//
    Type remove_pointer() const override {
        if constexpr (std::is_incomplete<std::remove_pointer_t<T>>::value) {
            return this;
        } else {
            static const type_traits& type = type_traits_info<std::remove_pointer_t<T>>();
            return &type;
        }
    }
//
//    const type_traits& make_signed() const override {
//        if constexpr (std::is_integral<T>::value) {
//            static const type_traits& type = type_traits_info<typename std::make_signed<T>::type>();
//            return type;
//        } else {
//            return *this;
//        }
//    }

//    const type_traits& make_unsigned() const override {
//        if constexpr (std::is_integral<T>::value) {
//            static const type_traits& type = type_traits_info<typename std::make_unsigned<T>::type>();
//            return type;
//        } else {
//            return *this;
//        }
//    }

//    const type_traits& remove_extent() const override {
//        if constexpr (std::is_array<T>::value) {
//            static const type_traits& type =  type_traits_info<typename std::remove_extent<T>::type>();
//            return type;
//        } else {
//            return *this;
//        }
//    }

public:
    bool before(const type_traits& type) const final {
        return typeid(T).before(type.type_info());
    }

    bool operator==(const type_traits& type) const final {
        return typeid(T) == type.type_info();
    }

    bool operator!=(const type_traits& type) const final {
        return typeid(T) != type.type_info();
    }

    const size_t hash_code() const final {
        return typeid(T).hash_code();
    }

    const size_t size() const final {
        if constexpr (std::is_void<T>::value || std::is_function<T>::value) {
            return 0;
        } else {
            return sizeof(T);
        }
    }

    std::string name() const final {
        return typeid(T).name();
    }

    std::string pretty_name() const final {
        auto name = typeid(T).name();

        int status = -4;

        char *res = abi::__cxa_demangle(name, nullptr, nullptr, &status);

        std::string ret_val((status == 0) ? res : name);

        free(res);

        return ret_val;
    }

#undef implement
#undef implement_typename
    type_traits_info() = default;
private:
    const std::type_info& type_info() const final {
        return typeid(T);
    }

    type_traits_info& operator=(const type_traits_info&) = delete;
    type_traits_info(const type_traits_info&) = delete;
};

template <typename T>
static inline Type get_type() {
    static auto const& type = type_traits_info<T>();
    return &type;
}