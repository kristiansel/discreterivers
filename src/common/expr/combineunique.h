#ifndef COMBINEUNIQUE_H
#define COMBINEUNIQUE_H

template< typename... Ts>
struct typelist {};

template < typename TElement, typename TList >
struct is_in {
    static const bool value = false;
};

// If it matches the first type, it is definitely in the list
template < typename TElement,
           template<typename...> typename Typelist,
           typename... TTail >
struct is_in < TElement, Typelist < TElement, TTail... > >
{
    static const bool value = true;
};

// If it is not the first element, check the remaining list
template < typename TElement,
           template<typename...> typename Typelist,
           typename THead, typename... TTail >
struct is_in < TElement, Typelist < THead, TTail... > >
{
  static const bool value = is_in < TElement, Typelist < TTail... > >::value;
};


// add unique
// Append a type to a type_list unless it already exists
template < typename TNew, typename TList,
           bool Tis_duplicate = is_in < TNew, TList >::value
           >
struct add_unique;

// If TNew is in the list, return the list unmodified
template < typename TNew,
           template<typename...> typename Typelist,
           typename... TList >
struct add_unique < TNew, Typelist < TList... >, true >
{
    using type = Typelist < TList... >;
};

// If TNew is not in the list, append it
template < typename TNew,
           template<typename...> typename Typelist,
           typename... TList >
struct add_unique < TNew, Typelist < TList... >, false >
{
    using type = Typelist < TNew, TList... >;
};

// combine unique
template< typename TList1, typename TList2 >
struct combine_unique;

// if the list is empty, return
template<  typename Typelist1,
           template<typename...> typename Typelist2,
           typename    THead2,
           typename... TRest2 >
struct combine_unique< Typelist1, Typelist2 < THead2, TRest2... >>
{
    using type = typename combine_unique< typename add_unique<THead2, Typelist1>::type, Typelist2< TRest2... > >::type;
};

// if the list is empty, return
template<  typename Typelist1,
           template<typename...> typename Typelist2 >
struct combine_unique< Typelist1, Typelist2 < >>
{
    using type = Typelist1;
};

#endif // COMBINEUNIQUE_H
