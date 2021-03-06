/**
 * Autogenerated by Thrift Compiler (0.11.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#include "completion_hints_types.h"

#include <algorithm>
#include <ostream>

#include <thrift/TToString.h>



int _kTCompletionHintTypeValues[] = {
  TCompletionHintType::COLUMN,
  TCompletionHintType::TABLE,
  TCompletionHintType::VIEW,
  TCompletionHintType::SCHEMA,
  TCompletionHintType::CATALOG,
  TCompletionHintType::REPOSITORY,
  TCompletionHintType::FUNCTION,
  TCompletionHintType::KEYWORD
};
const char* _kTCompletionHintTypeNames[] = {
  "COLUMN",
  "TABLE",
  "VIEW",
  "SCHEMA",
  "CATALOG",
  "REPOSITORY",
  "FUNCTION",
  "KEYWORD"
};
const std::map<int, const char*> _TCompletionHintType_VALUES_TO_NAMES(::apache::thrift::TEnumIterator(8, _kTCompletionHintTypeValues, _kTCompletionHintTypeNames), ::apache::thrift::TEnumIterator(-1, NULL, NULL));

std::ostream& operator<<(std::ostream& out, const TCompletionHintType::type& val) {
  std::map<int, const char*>::const_iterator it = _TCompletionHintType_VALUES_TO_NAMES.find(val);
  if (it != _TCompletionHintType_VALUES_TO_NAMES.end()) {
    out << it->second;
  } else {
    out << static_cast<int>(val);
  }
  return out;
}


TCompletionHint::~TCompletionHint() throw() {
}


void TCompletionHint::__set_type(const TCompletionHintType::type val) {
  this->type = val;
}

void TCompletionHint::__set_hints(const std::vector<std::string> & val) {
  this->hints = val;
}

void TCompletionHint::__set_replaced(const std::string& val) {
  this->replaced = val;
}
std::ostream& operator<<(std::ostream& out, const TCompletionHint& obj)
{
  obj.printTo(out);
  return out;
}


uint32_t TCompletionHint::read(::apache::thrift::protocol::TProtocol* iprot) {

  ::apache::thrift::protocol::TInputRecursionTracker tracker(*iprot);
  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          int32_t ecast0;
          xfer += iprot->readI32(ecast0);
          this->type = (TCompletionHintType::type)ecast0;
          this->__isset.type = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_LIST) {
          {
            this->hints.clear();
            uint32_t _size1;
            ::apache::thrift::protocol::TType _etype4;
            xfer += iprot->readListBegin(_etype4, _size1);
            this->hints.resize(_size1);
            uint32_t _i5;
            for (_i5 = 0; _i5 < _size1; ++_i5)
            {
              xfer += iprot->readString(this->hints[_i5]);
            }
            xfer += iprot->readListEnd();
          }
          this->__isset.hints = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 3:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->replaced);
          this->__isset.replaced = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t TCompletionHint::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  ::apache::thrift::protocol::TOutputRecursionTracker tracker(*oprot);
  xfer += oprot->writeStructBegin("TCompletionHint");

  xfer += oprot->writeFieldBegin("type", ::apache::thrift::protocol::T_I32, 1);
  xfer += oprot->writeI32((int32_t)this->type);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("hints", ::apache::thrift::protocol::T_LIST, 2);
  {
    xfer += oprot->writeListBegin(::apache::thrift::protocol::T_STRING, static_cast<uint32_t>(this->hints.size()));
    std::vector<std::string> ::const_iterator _iter6;
    for (_iter6 = this->hints.begin(); _iter6 != this->hints.end(); ++_iter6)
    {
      xfer += oprot->writeString((*_iter6));
    }
    xfer += oprot->writeListEnd();
  }
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("replaced", ::apache::thrift::protocol::T_STRING, 3);
  xfer += oprot->writeString(this->replaced);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

void swap(TCompletionHint &a, TCompletionHint &b) {
  using ::std::swap;
  swap(a.type, b.type);
  swap(a.hints, b.hints);
  swap(a.replaced, b.replaced);
  swap(a.__isset, b.__isset);
}

TCompletionHint::TCompletionHint(const TCompletionHint& other7) {
  type = other7.type;
  hints = other7.hints;
  replaced = other7.replaced;
  __isset = other7.__isset;
}
TCompletionHint& TCompletionHint::operator=(const TCompletionHint& other8) {
  type = other8.type;
  hints = other8.hints;
  replaced = other8.replaced;
  __isset = other8.__isset;
  return *this;
}
void TCompletionHint::printTo(std::ostream& out) const {
  using ::apache::thrift::to_string;
  out << "TCompletionHint(";
  out << "type=" << to_string(type);
  out << ", " << "hints=" << to_string(hints);
  out << ", " << "replaced=" << to_string(replaced);
  out << ")";
}


