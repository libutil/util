//
// Created in February 2020
//

#ifndef HEADER_GUARD__c49fd3f8_2085_4b0c_849f_13589592a242__referable_unique_hpp
#define HEADER_GUARD__c49fd3f8_2085_4b0c_849f_13589592a242__referable_unique_hpp

#include "variable_util_includes.h"

/**
 * The primary class template of referable_unique.
 * @tparam T Type
 * @tparam 15 anonymous type template parameters with default type void for future use.
 */
template<
        typename T,
        typename = void,
        typename = void,
        typename = void,
        typename = void,
        typename = void,
        typename = void,
        typename = void,
        typename = void,
        typename = void,
        typename = void,
        typename = void,
        typename = void,
        typename = void,
        typename = void,
        typename = void,
        typename = void
>
class referable_unique final
{
    static_assert(
            std::is_const<T>::value,
            "referable_unique requires a non-const content type"
    );
};

/**
 * Partial specification aimed to confirm
 * the first type parameter is non-const.
 * @tparam T Type
 */
template<typename T>
class referable_unique<const T>
{
    static_assert(
            !std::is_const<const T>::value,
            "referable_unique requires a non-const content type"
    );
};


/**
 * @tparam T non-const content type. It is guaranteed by std::enable_if_t<!std::is_const<T>::value, T>
 */
template<typename T>
class referable_unique<
        T, std::enable_if_t<
                (!std::is_const<T>::value) &&
                (!type_util::is_class_template_instance<T, std::atomic>::value)
        >
> final
{
private:
    static_assert(std::is_const_v<T> == 0, "referable_unique requires a non-const content type");
    
    friend class referable_unique<T, std::shared_timed_mutex>;
    
    ///std::shared_ptr applied as unique pointer aimed to use std::weak_ptr
    std::shared_ptr<T> content_shared_ptr;
    //std::shared_ptr<std::atomic_uint64_t> content_weak_ptr_count;
    
    ///Data Class
    class Container
    {
    private:
        /**
         * Disable copy constructor and move constructor
         * because not only these constructors are unnecessary
         * but also mutex members of this class are NOT copyable or movable
         */
        inline explicit Container(const Container &) = delete;
        
        inline explicit Container(Container &&) = delete;
    
    public:
        std::any content_id, content_label;
        //todo:consider to use another data structure to store tag
        /**
         * content_guard: guarantee thread safety of the content
         * state_holder:  UNUSED state holder of this container
         */
        std::shared_timed_mutex content_guard, state_holder;
        
        ///Default constructor
        inline explicit Container(
                const std::any &id = std::any(), const std::any &label = std::any()
        ) noexcept :
                content_id(id), content_label(label),
                content_guard(), state_holder()
        {}
    };
    
    std::shared_ptr<Container> container;
    
    /**
     * Disable unnecessary default constructor
     */
    inline explicit referable_unique() = delete;
    
    /**
     * Disable copy constructor
     */
    inline explicit referable_unique(
            const referable_unique<T, std::shared_timed_mutex> &
    ) = delete;
    
    /**
     * 禁用拷贝赋值运算符
     * @return lvalue reference of current referable_unique
     */
    inline referable_unique<T, std::shared_timed_mutex> &operator=(
            const referable_unique<T, std::shared_timed_mutex> &
    ) = delete;
    
    /**
     * 禁用拷贝赋值运算符（const重载版本）
     * This declaration is redundant because const instance can only call const method.
     * Even this declaration removed,const instances are unable to call the non-const version.
     * @return const lvalue reference of current referable_unique
     */
    inline const referable_unique<T, std::shared_timed_mutex> &operator=(
            const referable_unique<T, std::shared_timed_mutex> &
    ) const = delete;
    
    /**
     * 禁用移动赋值运算符
     * @return const lvalue reference of current referable_unique
     */
    inline const referable_unique<T, std::shared_timed_mutex> &&operator=(
            referable_unique<T, std::shared_timed_mutex> &&
    ) const = delete;

public:
    /**
     * Move constructor
     * @param original_referable_unique 原referable_unique<T>
     */
    inline explicit referable_unique(
            referable_unique<T, std::shared_timed_mutex> &&original_referable_unique
    ) noexcept :
            container(std::move(original_referable_unique.container)),
            content_shared_ptr(
                    std::move<std::shared_ptr<T>>(
                            original_referable_unique.content_shared_ptr
                    )
            )
    {}
    
    /**
     * Move constructor
     * @tparam original_referable_unique_t
     * @param original_referable_unique
     */
    template<typename original_referable_unique_t>
    inline explicit referable_unique(
            referable_unique<
                    original_referable_unique_t, std::shared_timed_mutex
            > &&original_referable_unique
    ) noexcept :
            container(std::move(original_referable_unique.container)),
            content_shared_ptr(
                    std::move<
                            std::shared_ptr<original_referable_unique_t>
                    >(original_referable_unique.content_shared_ptr)
            )
    {}
    
    /**
     * Commonly used constructor
     * @tparam unique_ptr_t 参数unique_ptr所管理的类型
     * @param unique_ptr
     */
    template<typename unique_ptr_t>
    inline explicit referable_unique(
            std::unique_ptr<unique_ptr_t> &&unique_ptr,
            const std::any &id = std::any(), const std::any &label = std::any()
    ) noexcept :
            container(std::make_shared<Container>(id, label)),
            content_shared_ptr(
                    std::forward<std::unique_ptr<unique_ptr_t>>(unique_ptr)
            )
    {}
    
    /**
     * Commonly used constructor
     * @tparam shared_ptr_t 参数shared_ptr所管理的类型
     * @param shared_ptr 共享所有权智能指针
     */
    template<typename shared_ptr_t>
    inline explicit referable_unique(
            std::shared_ptr<shared_ptr_t> &&shared_ptr,
            const std::any &id = std::any(), const std::any &label = std::any()
    ) noexcept :
            container(std::make_shared<Container>(id, label)),
            content_shared_ptr(
                    std::forward<std::shared_ptr<shared_ptr_t>>(shared_ptr)
            )
    {}
    
    /**
     * Available constructor
     * @tparam raw_pointer_t 裸指针的类型
     * @param content_raw_pointer 指向内容的裸指针的右值引用
     */
    template<typename raw_pointer_t>
    inline explicit referable_unique(
            raw_pointer_t *&&content_raw_pointer,
            const std::any &id = std::any(), const std::any &label = std::any()
    ) noexcept :
            container(std::make_shared<Container>(id, label)),
            content_shared_ptr(content_raw_pointer)
    {}
    
    /**
     * Available constructor
     * @tparam raw_pointer_t 裸指针的类型
     * @param content_raw_pointer 指向内容的裸指针的左值引用，构造后将该指针置为nullptr
     */
    template<typename raw_pointer_t>
    inline explicit referable_unique(
            raw_pointer_t *&content_raw_pointer,
            const std::any &id = std::any(), const std::any &label = std::any()
    ) noexcept :
            container(std::make_shared<Container>(id, label)),
            content_shared_ptr(content_raw_pointer)
    {
        content_raw_pointer = nullptr;
    }
    
    inline operator bool() const noexcept
    {
        return (container && content_shared_ptr);
    }
    
    class const_view final
    {
    private:
        friend class referable_unique<T, std::shared_timed_mutex>;
        
        /**
         * These std::shared_ptr members are not empty only when
         * external users never use move constructor
         * or always use move constructor legally.
         * For example,users never access moved instance.
         * This feature is guaranteed during initialization.
         */
        std::shared_ptr<T> content_shared_ptr;
        std::shared_ptr<Container> container_shared_ptr;
        
        std::shared_lock<std::shared_timed_mutex> content_shared_lock;
        
        ///Disable default constructor
        inline explicit const_view() = delete;
        
        ///Disable copy constructor
        inline explicit const_view(const const_view &) = delete;
        
        inline const const_view &operator=(const const_view &) = delete;
        
        inline const const_view &operator=(const_view &&) = delete;
        
        ///Constructor used by weak_ptr
        inline explicit const_view(
                std::shared_ptr<T> &&content,
                std::shared_ptr<Container> &&container,
                std::shared_lock<std::shared_timed_mutex> &&content_lock
        ) noexcept :
                content_shared_ptr(std::forward<std::shared_ptr<T>>(content)),
                container_shared_ptr(std::forward<std::shared_ptr<Container>>(container)),
                content_shared_lock(
                        std::forward<std::shared_lock<std::shared_timed_mutex>>(content_lock)
                )
        {}
    
    public:
        /// Move constructor
        inline explicit const_view(const_view &&original) :
                content_shared_ptr(std::move(original.content_shared_ptr)),
                container_shared_ptr(std::move(original.container_shared_ptr)),
                content_shared_lock(std::move(original.content_shared_lock))
        {}
        
        ///is this view valid
        inline operator bool() const
        {
            return (
                    (bool) container_shared_ptr &&
                    (bool) content_shared_ptr &&
                    (bool) content_shared_lock
            );
        }
        
        inline const T &operator*()
        {
            return *content_shared_ptr;
        }
        
        inline const T *operator->()
        {
            return content_shared_ptr.get();
        }
    };
    
    class view final
    {
    private:
        friend class referable_unique<T, std::shared_timed_mutex>;
        
        /**
         * These std::shared_ptr members are not empty only when
         * external users never use move constructor
         * or always use move constructor legally.
         * For example,users never access moved instance.
         * This feature is guaranteed during initialization.
         */
        std::shared_ptr<T> content_shared_ptr;
        std::shared_ptr<Container> container_shared_ptr;
        
        std::unique_lock<std::shared_timed_mutex> content_unique_lock;
        
        ///Disable default constructor
        inline explicit view() = delete;
        
        ///Disable copy constructor
        inline explicit view(const view &) = delete;
        
        inline const view &operator=(const view &) = delete;
        
        inline const view &operator=(view &&) = delete;
        
        ///Constructor used by weak_ptr
        inline explicit view(
                std::shared_ptr<T> &&content, std::shared_ptr<Container> &&container,
                std::unique_lock<std::shared_timed_mutex> &&content_lock
        ) noexcept :
                content_shared_ptr(std::forward<std::shared_ptr<T>>(content)),
                container_shared_ptr(std::forward<std::shared_ptr<Container>>(container)),
                content_unique_lock(
                        std::forward<std::unique_lock<std::shared_timed_mutex>>(content_lock)
                )
        {}
    
    public:
        /// Move constructor
        inline explicit view(view &&original) :
                content_shared_ptr(std::move(original.content_shared_ptr)),
                container_shared_ptr(std::move(original.container_shared_ptr)),
                content_unique_lock(std::move(original.content_unique_lock))
        {}
        
        ///is this view valid
        inline operator bool() const
        {
            return (
                    (bool) container_shared_ptr &&
                    (bool) content_shared_ptr &&
                    (bool) content_unique_lock
            );
        }
        
        inline T &operator*()
        {
            return *content_shared_ptr;
        }
        
        inline T *operator->()
        {
            return content_shared_ptr.get();
        }
        
        inline T &operator=(const T &t)
        {
            *content_shared_ptr = t;
            return *content_shared_ptr;
        }
    };
    
    class weak_ptr final
    {
    private:
        std::weak_ptr<T> content_weak_ptr;
        std::weak_ptr<Container> container_weak_ptr;
        
        /**
         * Disable default constructor
         */
        inline explicit weak_ptr() = delete;
        
        /**
         * Disable unnecessary move constructor
         */
        inline explicit weak_ptr(weak_ptr &&) = delete;
        
        /**
         * 禁用移动赋值运算符
         * @return lvalue reference of current object
         */
        inline weak_ptr &operator=(weak_ptr &&) = delete;
    
    public:
        /**
         * Commonly used constructor
         * @tparam referable_unique_t referable_unique content type
         * @param referable_unique referable_unique<referable_unique_t>
         */
        template<typename referable_unique_t>
        inline explicit weak_ptr(
                const referable_unique<
                        referable_unique_t, std::shared_timed_mutex
                > &referable_unique
        ) noexcept:
                content_weak_ptr(referable_unique.content_shared_ptr),
                container_weak_ptr(referable_unique.container)
        {}
        
        /**
         * Copy constructor
         * @param another 另一weak_ptr
         */
        inline explicit weak_ptr(const weak_ptr &another) noexcept :
                content_weak_ptr(another.content_weak_ptr),
                container_weak_ptr(another.container_weak_ptr)
        {}
        
        /**
         * Copy constructor
         * @tparam original_type 原类型
         * @param another 另一weak_ptr
         */
        template<typename original_type>
        inline explicit weak_ptr(
                const typename referable_unique<
                        original_type, std::shared_timed_mutex
                >::weak_ptr &another
        ) noexcept :
                content_weak_ptr(another.content_weak_ptr),
                container_weak_ptr(another.container_weak_ptr)
        {}
        
        /**
         * 拷贝赋值运算符
         * @tparam original_type 原类型
         * @param another 另一weak_ptr
         * @return 当前weak_ptr的左值引用
         */
        template<typename original_type>
        inline weak_ptr &operator=(
                const typename referable_unique<
                        original_type, std::shared_timed_mutex
                >::weak_ptr &another
        ) noexcept
        {
            this->content_weak_ptr = another.content_weak_ptr;
            this->container_weak_ptr = another.container_weak_ptr;
            return *this;
        }
        
        /**
         * 禁用const版本的拷贝赋值运算符
         * This declaration is redundant because const instance can only call const method.
         * Even this declaration removed,const instances are unable to call the non-const version.
         * @tparam original_type
         * @return const lvalue reference of current weak_ptr
         */
        template<typename original_type>
        inline const weak_ptr &operator=(
                const typename referable_unique<original_type, std::shared_timed_mutex>::weak_ptr &
        ) const noexcept = delete;
        
        inline operator bool() noexcept
        {
            return !(content_weak_ptr.expired() || container_weak_ptr.expired());
        }
        
        inline std::optional<const_view> get_const_view()
        {
            std::shared_ptr<T> content_shared_pointer(this->content_weak_ptr);
            std::shared_ptr<Container> container_shared_ptr(this->container_weak_ptr);
            /// in constructor of these shared pointers
            /// exception std::bad_weak_ptr will be thrown
            /// when content has expired or container has expired
            return std::optional<const_view>(
                    const_view(
                            std::move(content_shared_pointer),
                            std::move(container_shared_ptr),
                            std::move<std::shared_lock<std::shared_timed_mutex>>(
                                    std::shared_lock<std::shared_timed_mutex>(
                                            container_shared_ptr->content_guard
                                    )
                            )
                    )
            );
        }
        
        template<class Rep, class Period>
        inline std::optional<const_view> get_const_view(
                const std::chrono::duration<Rep, Period> &timeout_duration
        )
        {
            std::shared_ptr<T> content_shared_pointer(this->content_weak_ptr);
            std::shared_ptr<Container> container_shared_ptr(this->container_weak_ptr);
            /// in constructor of these shared pointers
            /// exception std::bad_weak_ptr will be thrown
            /// when content has expired or container has expired
            if (std::shared_lock<std::shared_timed_mutex> content_guard_lock(
                        container_shared_ptr->content_guard, timeout_duration
                );content_guard_lock)
            {
                return std::optional<const_view>(
                        const_view(
                                std::move(content_shared_pointer),
                                std::move(container_shared_ptr),
                                std::move(content_guard_lock)
                        )
                );
            }
            else return std::optional<const_view>();
        }
        
        inline std::optional<view> get_view()
        {
            std::shared_ptr<T> content_shared_pointer(this->content_weak_ptr);
            std::shared_ptr<Container> container_shared_ptr(this->container_weak_ptr);
            /// in constructor of these shared pointers
            /// exception std::bad_weak_ptr will be thrown
            /// when content has expired or container has expired
            return std::optional<view>(
                    view(
                            std::move(content_shared_pointer),
                            std::move(container_shared_ptr),
                            std::move<std::unique_lock<std::shared_timed_mutex>>(
                                    std::unique_lock<std::shared_timed_mutex>(
                                            container_shared_ptr->content_guard
                                    )
                            )
                    )
            );
        }
        
        template<class Rep, class Period>
        inline std::optional<view> get_view(
                const std::chrono::duration<Rep, Period> &timeout_duration
        )
        {
            std::shared_ptr<T> content_shared_pointer(this->content_weak_ptr);
            std::shared_ptr<Container> container_shared_ptr(this->container_weak_ptr);
            /// in constructor of these shared pointers
            /// exception std::bad_weak_ptr will be thrown
            /// when content has expired or container has expired
            if (std::unique_lock<std::shared_timed_mutex> content_guard_lock(
                        container_shared_ptr->content_guard, timeout_duration
                );content_guard_lock)
            {
                return std::optional<view>(
                        view(
                                std::move(content_shared_pointer),
                                std::move(container_shared_ptr),
                                std::move(content_guard_lock)
                        )
                );
            }
            else return std::optional<const_view>();
        }
    };
    
    inline std::optional<view> operator*() noexcept
    {
        /// in constructor of these shared pointers
        /// exception std::bad_weak_ptr will be thrown
        /// when content has expired or container has expired
        return std::make_optional<view>(
                std::move(content_shared_ptr),
                std::move(container),
                std::unique_lock<std::shared_timed_mutex>(
                        container->content_guard
                )
        );
    }
    
    inline std::optional<const_view> operator*() const noexcept
    {
        /// in constructor of these shared pointers
        /// exception std::bad_weak_ptr will be thrown
        /// when content has expired or container has expired
        return std::make_optional<const_view>(
                std::move(content_shared_ptr),
                std::move(container),
                std::shared_lock<std::shared_timed_mutex>(
                        container->content_guard
                )
        );
    }
    
    inline T *operator->() noexcept
    {
        return this->content_shared_ptr.get();
    }
    
    inline const T *operator->() const noexcept
    {
        return this->content_shared_ptr.get();
    }
};

/**
 * Partial specification for std::atomic<T>
 * @tparam T non-const content type. It is guaranteed by std::enable_if_t<!std::is_const<T>::value, T>
 */
template<typename T>
class referable_unique<
        std::atomic<T>, std::enable_if_t<(!std::is_const<T>::value) && std::is_trivially_copyable<T>::value>
> final
{
private:
    static_assert(
            std::is_const_v<T> == 0,
            "referable_unique requires a non-const content type"
    );
    static_assert(
            std::is_trivially_copyable_v<T>,
            "referable_unique requires a trivially copyable content type"
    );
    
    friend class referable_unique<std::atomic<T>>;
    
    std::shared_ptr<std::atomic<T>> atomic_content_shared_ptr;
    
    /**
     * Disable default constructor
     */
    inline explicit referable_unique() noexcept = delete;
    
    /**
     * Disable copy constructor
     */
    inline explicit referable_unique(const referable_unique<std::atomic<T>> &) noexcept = delete;
    
    /**
     * 禁用拷贝赋值运算符
     */
    inline referable_unique<std::atomic<T>> &operator=(
            const referable_unique<std::atomic<T>> &
    ) noexcept = delete;
    
    /**
     * 禁用拷贝赋值运算符（const版本）
     * This declaration is redundant because const instance can only call const method.
     * Even this declaration removed,const instances are unable to call the non-const version.
     * @return const referable_unique<std::atomic<T>> &
     */
    inline const referable_unique<std::atomic<T>> &operator=(
            const referable_unique<std::atomic<T>> &
    ) const noexcept = delete;

public:
    /**
     * Move constructor
     * @param original_referable_unique
     */
    inline explicit referable_unique(referable_unique<std::atomic<T>> &&original_referable_unique) noexcept :
            atomic_content_shared_ptr(
                    std::move<
                            std::shared_ptr<std::atomic<T>>
                    >(original_referable_unique.atomic_content_shared_ptr)
            )
    {}
    
    /**
     * 禁用移动赋值运算符（const版本）
     * This declaration is redundant because const instance can only call const method.
     * Even this declaration removed,const instances are unable to call the non-const version.
     * @return const referable_unique<std::atomic<T>> &
     */
    inline const referable_unique<std::atomic<T>> &operator=(
            referable_unique<std::atomic<T>> &&
    ) const = delete;
    
    /**
     * Commonly used constructor
     * @param atomic_content_unique_pointer The rvalue reference of std::unique_ptr<std::atomic<T>>
     */
    inline explicit referable_unique(
            std::unique_ptr<std::atomic<T>> &&atomic_content_unique_pointer
    ) noexcept :
            atomic_content_shared_ptr(
                    std::forward<std::unique_ptr<std::atomic<T>>>(atomic_content_unique_pointer)
            )
    {}
    
    /**
     * Commonly used constructor
     * @param atomic_content_shared_pointer The rvalue reference of std::shared_ptr<std::atomic<T>>
     */
    inline explicit referable_unique(
            std::shared_ptr<std::atomic<T>> &&atomic_content_shared_pointer
    ) noexcept :
            atomic_content_shared_ptr(
                    std::forward<std::shared_ptr<std::atomic<T>>>(atomic_content_shared_pointer)
            )
    {}
    
    /**
     * Available constructor
     * @param atomic_content_raw_pointer the rvalue reference of std::atomic<T>*
     */
    inline explicit referable_unique(
            std::atomic<T> *&&atomic_content_raw_pointer
    ) noexcept : atomic_content_shared_ptr(atomic_content_raw_pointer)
    {}
    
    /**
     * Available constructor
     * @param atomic_content_raw_pointer the lvalue reference of std::atomic<T>*
     */
    inline explicit referable_unique(
            std::atomic<T> *&atomic_content_raw_pointer
    ) noexcept : atomic_content_shared_ptr(atomic_content_raw_pointer)
    {
        atomic_content_raw_pointer = nullptr;
    }
    
    /**
     * Commonly used constructor
     * @see https://zh.cppreference.com/w/cpp/atomic/atomic/atomic
     * @param initial_content
     */
    inline explicit referable_unique(const T &initial_content) noexcept :
            atomic_content_shared_ptr(
                    std::make_shared<std::atomic<T>>(initial_content)
            )
    {}
    
    inline operator bool() const noexcept
    {
        return (bool) atomic_content_shared_ptr;
    }
    
    /**
     * 重载解引用运算符
     * 原子地加载并返回原子变量的当前值
     * @return 原子变量的当前值
     */
    inline T operator*() const
    {
        return atomic_content_shared_ptr->load();
    }
    
    /**
     * 重载赋值运算符
     * 原子地以t替换原子变量当前值并返回替换后的值
     * referable_unique具有内容的所有权，可以确保内容有效
     * 所以与weak_ptr::operator=(const T &t)->bool的返回值不同
     * 以便于连续赋值
     * @param t new content
     * @return content after substitution
     */
    inline T operator=(const T &t)
    {
        atomic_content_shared_ptr->store(t);
        return atomic_content_shared_ptr->load();
    }
    
    class weak_ptr final
    {
    private:
        std::weak_ptr<std::atomic<T>> atomic_content_weak_ptr;
        
        /**
         * Disable default constructor
         */
        inline explicit weak_ptr() = delete;
        
        /**
         * Disable unnecessary move constructor
         */
        inline explicit weak_ptr(weak_ptr &&) = delete;
        
        /**
         * 禁用移动赋值运算符
         * @return lvalue reference of current object
         */
        inline weak_ptr &operator=(weak_ptr &&) = delete;
    
    public:
        /**
         * Copy constructor
         * @param another the const lvalue reference of weak_ptr
         */
        inline explicit weak_ptr(const weak_ptr &another) noexcept :
                atomic_content_weak_ptr(another.atomic_content_weak_ptr)
        {}
        
        /**
         * Copy constructor
         * @tparam another_type
         * @param another
         */
        template<typename another_type>
        inline explicit weak_ptr(
                const typename referable_unique<std::atomic<another_type>>::weak_ptr &another
        ) noexcept : atomic_content_weak_ptr(another.atomic_content_weak_ptr)
        {}
        
        /**
         * 拷贝赋值运算符
         * @tparam another_type
         * @param another
         * @return the lvalue reference of this weak_ptr
         */
        template<typename another_type>
        inline weak_ptr &operator=(
                const typename referable_unique<std::atomic<another_type>>::weak_ptr &another
        ) noexcept
        {
            this->atomic_content_weak_ptr = another.atomic_content_weak_ptr;
        }
        
        /**
         * Commonly used constructor
         * @tparam referable_unique_t
         * @param referable_unique__
         */
        template<typename referable_unique_t>
        inline explicit weak_ptr(
                const referable_unique <std::atomic<referable_unique_t>> &referable_unique__
        ) noexcept :atomic_content_weak_ptr(referable_unique__.atomic_content_shared_ptr)
        {}
        
        /**
         * 重载解引用运算符
         * @return std::optional<T>
         */
        inline std::optional<T> operator*() noexcept
        {
            if (auto temp_atomic_shared_ptr = atomic_content_weak_ptr.lock();
                    temp_atomic_shared_ptr)
            {
                return std::optional<T>(
                        std::move(temp_atomic_shared_ptr->load())
                );
            }
        }
        
        /**
         * 重载赋值运算符，调用atomic<T>:::store(t)
         * @param t new value of T
         * @return 若更新成功返回true，若内容已销毁则返回false。
         */
        inline bool operator=(const T &t) noexcept
        {
            if (auto temp_atomic_shared_ptr = atomic_content_weak_ptr.lock();
                    temp_atomic_shared_ptr)
            {
                temp_atomic_shared_ptr->store(t);
                return true;
            }
            else return false;
        }
    };
};

#endif //HEADER_GUARD__c49fd3f8_2085_4b0c_849f_13589592a242__referable_unique_hpp
