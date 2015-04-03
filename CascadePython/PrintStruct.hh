#ifndef PrintStruct_hh
#define PrintStruct_hh

#include "StructConversion.hh"

#include <iostream>
namespace fusion = boost::fusion;
namespace mpl=boost::mpl;

template <typename T> struct PrintAll;

template<typename T>
struct PrintBaseType
{
  typedef PrintBaseType<T> type;
  static void print(const T& x) { std::cout << x << " "; }
};

template<typename T>
struct PrintArrayType
{
  typedef PrintArrayType<T> type;
  typedef typename boost::remove_bounds<T>::type slice_t;
  static void print(const T& x)
  {
    static const size_t size = sizeof(T)/sizeof(slice_t);
    std::cout << " [ ";
    for(size_t i=0;i<size;i++) {
      PrintAll<slice_t>::print(x[i]);
    }
    std::cout << "] ";
  }
};

template <typename Sequence>
struct PrintStruct
{
  struct PrintOut
  {
    template <typename Zip >
    void operator() (const Zip& v) const
    {

        typedef ZipTypes<Zip, Sequence> zt;
        typedef typename zt::EditValue T2;

        std::cout << zt::field_name() << " ";
        PrintAll<T2>::print(fusion::at_c<1>(v));
        std::cout<< std::endl;
    }
  };
  static void print(const Sequence& x)
  {
    ForEach(x, PrintOut());
  }
};


template <typename T> struct PrintAll_s
{
  typedef
    typename boost::mpl::eval_if< 
        boost::mpl::is_sequence<T>,
        boost::mpl::identity< PrintStruct<T> >,
        typename boost::mpl::eval_if< 
           boost::is_array<T>,
           boost::mpl::identity< PrintArrayType<T> >,
           PrintBaseType<T>   
        >
    >
  ::type type;
};

template <typename T> struct PrintAll : public PrintAll_s<T>::type { };


#endif

