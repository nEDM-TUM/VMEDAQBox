#ifndef _BuildLambdaFunctions_hh_
#define _BuildLambdaFunctions_hh_

#include "StructConversion.hh"
#include "autobahn/autobahn.hpp"
#include <boost/any.hpp>
#include <boost/bind.hpp>
#include <boost/fusion/functional/invocation/invoke.hpp>
#include <boost/fusion/adapted/boost_tuple.hpp>
#include <boost/fusion/include/boost_tuple.hpp>

namespace cascade {

namespace detail {

typedef autobahn::anyvec anyvec;
typedef autobahn::anymap anymap;
template<class F>
struct GenFunction;

template<class R, class... Args>
struct GenFunction<R(*)(Args...)> : public GenFunction<R(Args...)>
{};

template<class C, class R, class... Args>
struct GenFunction<R(C::*)(Args...)> : public GenFunction<R(Args...)>
{};

template<class C, class R, class... Args>
struct GenFunction<R(C::*)(Args...) const> : public GenFunction<R(Args...)>
{};


template<class Function, class... Args>
struct GenFunction<Function(Args...)> {
  typedef Function return_type;
  typedef typename boost::tuple<Args...> tuple_type;
  static constexpr std::size_t arity = sizeof...(Args);

};

template<typename T>
struct BuildTuple
{
    BuildTuple(const anyvec& avec) : _v(avec) {}

    template <typename Zip >
    void operator() (const Zip& v) const
    {
        typedef SeqTypes<Zip, T> zt;
        typedef typename zt::EditValue T2;

        T2& val_to_edit = const_cast<T2&>(fusion::at_c<1>(v));
        DecAll<T2>::decode(_v[zt::Index::value], val_to_edit);
    }
    const anyvec& _v;
};

}

template<typename F, typename G = detail::GenFunction<F> >
std::function<any(const detail::anyvec&, const detail::anymap&)>  GenFunction(F func)
{
    //typedef detail::GenFunction<F> gen_func;
    typedef typename G::tuple_type    tuple_type;
    typedef typename G::return_type   return_type;
    return [=] (const detail::anyvec& args, const detail::anymap& kw) {
      if (args.size() != G::arity) throw interpret_error("Number of arguments is incorrect"); 
      if (kw.size() != 0) throw interpret_error("Keyword arguments not accepted");
      // Build the tuple
      tuple_type t;
      ForEach(t, detail::BuildTuple<tuple_type>(args));
      return EncAll<return_type>::encode(boost::fusion::invoke(func, t));
    }; 
}

template<class Class, typename F, typename ...Args>
std::function<any(const detail::anyvec&, const detail::anymap&)> GenFunction(Class& c, F func, Args ... args)
{
  auto fn = boost::bind(func, &c, args...);
  return GenFunction<decltype(fn), detail::GenFunction<F> >(fn);
}

}


#endif
