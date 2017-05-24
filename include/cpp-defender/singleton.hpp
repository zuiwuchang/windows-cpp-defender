#ifndef SINGLETON_HPP_INCLUDED
#define SINGLETON_HPP_INCLUDED

 template<typename T>
class singleton
{
typedef singleton type_t;
protected:
    static T& get_instance()
    {
        static T t;
        return t;
    }
    singleton(const type_t&copy);
    type_t& operator=(const type_t&copy);
public:
    singleton()
    {
    }
    static T& get_mutable_instance()
    {
        return get_instance();
    }
    static const T& get_const_instance()
    {
        return get_instance();
    }
};

#endif // SINGLETON_HPP_INCLUDED
