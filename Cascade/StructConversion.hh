#ifndef StructConversion_hh_
#define StructConversion_hh_
#include <vector>
#include <map>
#include <boost/any.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/zip.hpp>
#include <boost/fusion/include/at_c.hpp>
#include <boost/fusion/include/mpl.hpp>
#include <boost/type_traits/is_array.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

namespace fusion = boost::fusion;
namespace mpl=boost::mpl;
using boost::any;

typedef std::map<std::string, boost::any> map_type;
typedef std::vector<boost::any> list_type;

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
    to = boost::any_cast<T>(from);
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
    const T& from_cast = boost::any_cast<const T&>(from);
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
    return any(from);
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
    list_type newList;
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
          _d[ZipTypes<Zip,Struct>::field_name()] = fusion::at_c<1>(v);
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
//___________________________________________________________


//___________________________________________________________
// Convert to python 
//___________________________________________________________
template<typename T>
struct ToPack
{
    ToPack()
    {
      //boost::python::to_python_converter<T, ToPack<T> >();
    }
    static any convertObj(const T& s)
    {
        return EncAll<T>::encode(s);
    }
};

//___________________________________________________________
// Convert a python dictionary to a particular struct
//___________________________________________________________
template<class Struct>
struct FromPack
{
  FromPack ()
  {
    //boost::python::converter::registry::push_back(
    //      &convertible,
    //      &construct,
    //      boost::python::type_id<Struct>());
  }

  // Convert dict into a Struct
  static void construct(
    const any& obj,
    Struct& data)
    {
      map_type& o = boost::any_cast<map_type&>(obj);
      DecAll<Struct>::decode(o, data);
    }
};

#endif
