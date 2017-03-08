#ifndef MAPTUPLE_H
#define MAPTUPLE_H

template < template<typename> class F,
           std::size_t i, typename T, typename ...Ts>
struct map_tuple_helper
{
    template<typename R>
    static void add(const std::tuple<Ts...> &t, const std::vector<R> &out)
    {

    }
};

template < template<typename> class F >
struct map_tuple_func
{
    using R = decltype(F<int>()(1));

    template<typename ...Ts>
    struct tuple
    {
        template<std::size_t i, typename ... Typelist>
        struct helper;

        template<std::size_t i, typename Head, typename... Tail>
        struct helper<i, Head, Tail...>
        {
            static void add(const std::tuple<Ts...> &t, std::vector<R> &v)
            {
                Head th = std::get<i>(t);
                v.push_back(F<Head>()(th));
                helper<i+1, Tail...>::add(t, v);
            }
        };

        template<std::size_t i, typename Head>
        struct helper<i, Head>
        {
            static void add(const std::tuple<Ts...> &t, std::vector<R> &v)
            {
                Head th = std::get<i>(t);
                v.push_back(F<Head>()(th));
            }
        };

        static std::vector<R> onto(const std::tuple<Ts...> &t)
        {
            std::vector<R> out;
            helper<std::size_t(0), Ts...>::add(t, out);
            return out;
        }
    };
};



#endif // MAPTUPLE_H
