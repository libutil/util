//
// Created in February 2020
//

#ifndef HEADER_GUARD__74c56d18_4654_4070_90bf_5ba396cbc172__type_util_hpp
#define HEADER_GUARD__74c56d18_4654_4070_90bf_5ba396cbc172__type_util_hpp

#include <type_traits>

namespace type_util
{
    /**
     * @see https://cloud.tencent.com/developer/article/1433593
     * @see https://www.zhihu.com/question/34929124
     * @tparam instance_class
     * @tparam template_to_check
     */
    template<
            typename instance_class,
            template<typename> class template_to_check
    >
    class is_class_template_instance
    {
    private:
        template<typename template_parameters>
        static constexpr std::true_type check(
                template_to_check<template_parameters> &&
        )
        {
            return std::true_type();
        }
        
        static constexpr std::false_type check(...)
        {
            return std::false_type();
        }
        
        using type = decltype(check(std::declval<instance_class>()));
    public:
        static constexpr bool value = decltype(check(std::declval<instance_class>()))::value;
    };
}

#endif //HEADER_GUARD__74c56d18_4654_4070_90bf_5ba396cbc172__type_util_hpp
