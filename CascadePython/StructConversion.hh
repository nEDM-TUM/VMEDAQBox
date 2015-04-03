#ifndef StructConversion_hh_
#define StructConversion_hh_
#include <boost/python.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/str.hpp>
#include <boost/python/list.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/zip.hpp>
#include <boost/fusion/include/at_c.hpp>
#include <boost/fusion/include/mpl.hpp>
#include <boost/type_traits/is_array.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

namespace fusion = boost::fusion;
namespace mpl=boost::mpl;
using boost::python::object;

template<typename Zip, typename Seq>
struct ZipTypes
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

    static std::string field_name()
    {
      return fusion::extension::struct_member_name<Seq,Index::value>::call();
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
  static void decode(const object& from, T& to)
  {
    to = boost::python::extract<T>(from);
  }
};

template<>
struct BaseType<std::string*>
{
  typedef BaseType<std::string*> type;
  static void decode(const object& from, std::string*& to)
  {
    if (to) {
      *to = boost::python::extract<char const*>(from);
    }
  }
};



template<typename T>
struct ArrayType
{
  typedef ArrayType<T> type;
  typedef typename boost::remove_bounds<T>::type slice_t;
  static void decode(const object& from, T& to)
  {
    static const size_t size = sizeof(T)/sizeof(slice_t);
    for(size_t i=0;i<size;i++) {
      DecAll<slice_t>::decode(from[i], to[i]);
    }
  }
};

template<typename Struct>
struct DictType
{
  typedef DictType<Struct> type;
  struct FromDictConvertor
  {
      FromDictConvertor(boost::python::dict& adict) : _d(adict) {}

      template <typename Zip >
      void operator() (const Zip& v) const
      {
          typedef ZipTypes<Zip, Struct> zt;
          typedef typename zt::EditValue T2;

          T2& val_to_edit = const_cast<T2&>(fusion::at_c<1>(v));
          DecAll<T2>::decode(_d[zt::field_name()], val_to_edit);
      }
      boost::python::dict& _d;
  };
  static void decode(const object& from, Struct& to)
  {
      boost::python::dict o = boost::python::extract<boost::python::dict>(from);
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
  static object encode(const T& from)
  {
    return object(from);
  }
};

template<>
struct EncodeBaseType<std::string*>
{
  typedef EncodeBaseType<std::string*> type;
  static object encode(std::string* const& from)
  {
    if (!from) return boost::python::str(); 
    return boost::python::str(from->c_str());
  }
};

// Array encoding 
template<typename T>
struct EncodeArrayType
{
  typedef EncodeArrayType<T> type;
  typedef typename boost::remove_bounds<T>::type slice_t;
  static object encode(const T& from)
  {
    static const size_t size = sizeof(T)/sizeof(slice_t);
    boost::python::list newList;
    for(size_t i=0;i<size;i++) {
      newList.append(EncAll<slice_t>::encode(from[i]));
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
      ToDictConvertor(boost::python::dict& adict) : _d(adict) {}

      template <typename Zip >
      void operator() (const Zip& v) const
      {
          _d[ZipTypes<Zip,Struct>::field_name()] = fusion::at_c<1>(v);
      }
      boost::python::dict& _d;
  };
  static object encode(const Struct& from)
  {
      boost::python::dict newDict;
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
struct ToPython
{
    ToPython()
    {
      boost::python::to_python_converter<T, ToPython<T> >();
    }
    static object convertObj(const T& s)
    {
        return EncAll<T>::encode(s);
    }
    static PyObject* convert(const T& s)
    {
      return boost::python::incref(convertObj(s).ptr());
    }
};

//___________________________________________________________
// Convert a python dictionary to a particular struct
//___________________________________________________________
template<class Struct>
struct FromPython
{
  FromPython ()
  {
    boost::python::converter::registry::push_back(
          &convertible,
          &construct,
          boost::python::type_id<Struct>());
  }
  static void* convertible(PyObject* obj_ptr)
  {
    boost::python::extract<boost::python::dict> x(obj_ptr);
    if (!x.check()) return 0;
    return obj_ptr;
  }

  // Convert dict into a Struct
  static void construct(
    PyObject* obj_ptr,
    boost::python::converter::rvalue_from_python_stage1_data* data)
    {
      boost::python::dict o = boost::python::extract<boost::python::dict>(obj_ptr);
      // Grab pointer to memory into which to construct the new Struct
      void* storage = (
        (boost::python::converter::rvalue_from_python_storage<Struct>*)
        data)->storage.bytes;

      Struct* avar = new (storage) Struct();

      DecAll<Struct>::decode(o, *avar);

      // Stash the memory chunk pointer for later use by boost.python
      data->convertible = storage;
    }
};

#endif
