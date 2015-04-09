#ifndef _StructConversion_hh_
#define _StructConversion_hh_
#include <vector>
#include <map>
#include <iostream>
#include <boost/any.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/zip.hpp>
#include <boost/fusion/include/at_c.hpp>
#include <boost/fusion/include/mpl.hpp>
#include <boost/type_traits/is_array.hpp>
#include <boost/fusion/include/adapt_struct.hpp>


namespace cascade {

  class interpret_error: public std::runtime_error
  {
    public:
      interpret_error(const std::string& msg) : std::runtime_error(msg) {}
  };


namespace detail{

typedef std::function< boost::any( boost::any ) > convert_type;
typedef std::vector< boost::any > anyvec;
typedef std::map< std::string, convert_type > convert_map;
typedef std::map< std::pair<std::string, std::string>, convert_type > from_convert_map;

extern from_convert_map gFromWebConversionMap;
extern convert_map gToWebConversionMap;

template<typename From, typename To>
struct RegisterToWebConversion {
  static void Execute()
  {
    std::string to_str = typeid(From).name();
    if (gToWebConversionMap.find(to_str) != gToWebConversionMap.end()) {
      std::cerr << "Overwriting conversion for: " << to_str << std::endl;
    }
    gToWebConversionMap[to_str] = [] ( boost::any a ) {
      return boost::any(static_cast<To>(boost::any_cast<From>(a)));
    };
  }
};

template<typename From, typename To>
struct RegisterFromWebConversion {
  static void Execute()
  {
    auto pr = std::make_pair<std::string, std::string>(typeid(From).name(), typeid(To).name());
    std::string from_str = typeid(From).name();
    if (gFromWebConversionMap.find(pr) != gFromWebConversionMap.end()) {
      std::cerr << "Overwriting conversion for: " << from_str << std::endl;
    }
    gFromWebConversionMap[pr] = [] ( boost::any a ) {
      return boost::any(static_cast<To>(boost::any_cast<From>(a)));
    };
  }
};

boost::any ConvertFromWeb(const boost::any& a, const std::type_info& id);
boost::any ConvertToWeb(const boost::any& a);
}

namespace fusion = boost::fusion;
namespace mpl=boost::mpl;
using boost::any;

typedef std::map<std::string, boost::any> map_type;

template<typename Zip, typename Seq>
struct SeqTypes
{
    typedef typename boost::remove_const<
                    typename boost::remove_reference<
                        typename fusion::result_of::at_c<Zip,0>::type
                    >::type
                >::type Index;

    typedef typename boost::remove_const<
                    typename boost::remove_reference<
                        typename fusion::result_of::at_c<Zip, 1>::type >::type
                    >::type EditValue;
};

template<typename Zip, typename Seq>
struct ZipTypes
{
    typedef SeqTypes<Zip, Seq> _st;
    typedef typename _st::EditValue EditValue;
    static std::string field_name()
    {
      return fusion::extension::struct_member_name<Seq,_st::Index::value>::call();
    }

};

template <typename Struct, typename F>
void ForEach(Struct const& s, F const& f)
{
      typedef mpl::range_c<unsigned, 0,
        fusion::result_of::size<Struct>::value > Indices;
      fusion::for_each(fusion::zip(Indices(),s), f);
}

//___________________________________________________________
// Decode *FROM* python
template <typename T> struct DecAll;

template<typename T>
struct BaseType
{
  typedef BaseType<T> type;
  static void decode(const any& from, T& to)
  {
    to = boost::any_cast<T>(detail::ConvertFromWeb(from, typeid(T)));
  }
};

template<typename T>
struct ArrayType
{
  typedef ArrayType<T> type;
  typedef typename boost::remove_bounds<T>::type slice_t;
  static void decode(const any& from, T& to)
  {
    static const size_t size = sizeof(T)/sizeof(slice_t);
    const detail::anyvec& from_cast = boost::any_cast<const detail::anyvec&>(from);
    if (from_cast.size() != size) throw interpret_error("Array lengths don't match");
    for(size_t i=0;i<size;i++) {
      DecAll<slice_t>::decode(from_cast[i], to[i]);
    }
  }
};

template<typename Struct>
struct DictType
{
  typedef DictType<Struct> type;
  struct FromDictConvertor
  {
      FromDictConvertor(map_type& adict) : _d(adict) {}

      template <typename Zip >
      void operator() (const Zip& v) const
      {
          typedef ZipTypes<Zip, Struct> zt;
          typedef typename zt::EditValue T2;

          T2& val_to_edit = const_cast<T2&>(fusion::at_c<1>(v));
          DecAll<T2>::decode(_d[zt::field_name()], val_to_edit);
      }
      map_type& _d;
  };
  static void decode(const any& from, Struct& to)
  {
      map_type o = boost::any_cast<map_type>(from);
      ForEach(to, FromDictConvertor(o));
  }
};


template <typename T> struct DecAll_s {
  typedef
    typename boost::mpl::eval_if< 
        boost::mpl::is_sequence<T>,
        boost::mpl::identity< DictType<T> >,
        typename boost::mpl::eval_if< 
           boost::is_array<T>,
           boost::mpl::identity< ArrayType<T> >,
           BaseType<T>   
        >
    >
  ::type type;
};

template <typename T> struct DecAll : public DecAll_s<T>::type { };
//___________________________________________________________

//___________________________________________________________
// Encode *TO* python
template <typename T> struct EncAll;

// Base encoding 
template<typename T>
struct EncodeBaseType
{
  typedef EncodeBaseType<T> type;
  static any encode(const T& from)
  {
    return detail::ConvertToWeb(any(from));
  }
};

// Array encoding 
template<typename T>
struct EncodeArrayType
{
  typedef EncodeArrayType<T> type;
  typedef typename boost::remove_bounds<T>::type slice_t;
  static any encode(const T& from)
  {
    static const size_t size = sizeof(T)/sizeof(slice_t);
    detail::anyvec newList;
    for(size_t i=0;i<size;i++) {
      newList.push_back(EncAll<slice_t>::encode(from[i]));
    }
    return newList;
  }
};

// Sequence/Struc encoding 
template<typename Struct>
struct EncodeDictType
{
  typedef EncodeDictType<Struct> type;
  struct ToDictConvertor
  {
      ToDictConvertor(map_type& adict) : _d(adict) {}

      template <typename Zip >
      void operator() (const Zip& v) const
      {

          typedef typename ZipTypes<Zip,Struct>::EditValue T;
          _d[ZipTypes<Zip,Struct>::field_name()] = EncAll<T>::encode(fusion::at_c<1>(v));
      }
      map_type& _d;
  };
  static any encode(const Struct& from)
  {
      map_type newDict;
      ForEach(from, ToDictConvertor(newDict));
      return newDict;
  }
};

template <typename T> struct EncAll_s {
  typedef
    typename boost::mpl::eval_if< 
        boost::mpl::is_sequence<T>,
        boost::mpl::identity< EncodeDictType<T> >,
        typename boost::mpl::eval_if< 
           boost::is_array<T>,
           boost::mpl::identity< EncodeArrayType<T> >,
           EncodeBaseType<T>   
        >
    >
  ::type type;
};

template <typename T> struct EncAll : public EncAll_s<T>::type { };
}
#endif
