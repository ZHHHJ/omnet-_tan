//
// Generated file, do not edit! Created by opp_msgtool 6.1 from src/msg/Data.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "Data_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

Register_Class(Data)

Data::Data(const char *name, short kind) : ::omnetpp::cPacket(name, kind)
{
}

Data::Data(const Data& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

Data::~Data()
{
}

Data& Data::operator=(const Data& other)
{
    if (this == &other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void Data::copy(const Data& other)
{
    this->data_name = other.data_name;
    this->data_id = other.data_id;
    this->data_srcId = other.data_srcId;
    this->data_destId = other.data_destId;
    this->data_sendPower = other.data_sendPower;
    this->data_sendFrequency = other.data_sendFrequency;
    this->data_dataLenBit = other.data_dataLenBit;
    this->sendingTime = other.sendingTime;
}

void Data::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->data_name);
    doParsimPacking(b,this->data_id);
    doParsimPacking(b,this->data_srcId);
    doParsimPacking(b,this->data_destId);
    doParsimPacking(b,this->data_sendPower);
    doParsimPacking(b,this->data_sendFrequency);
    doParsimPacking(b,this->data_dataLenBit);
    doParsimPacking(b,this->sendingTime);
}

void Data::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->data_name);
    doParsimUnpacking(b,this->data_id);
    doParsimUnpacking(b,this->data_srcId);
    doParsimUnpacking(b,this->data_destId);
    doParsimUnpacking(b,this->data_sendPower);
    doParsimUnpacking(b,this->data_sendFrequency);
    doParsimUnpacking(b,this->data_dataLenBit);
    doParsimUnpacking(b,this->sendingTime);
}

const char * Data::getData_name() const
{
    return this->data_name.c_str();
}

void Data::setData_name(const char * data_name)
{
    this->data_name = data_name;
}

int Data::getData_id() const
{
    return this->data_id;
}

void Data::setData_id(int data_id)
{
    this->data_id = data_id;
}

int Data::getData_srcId() const
{
    return this->data_srcId;
}

void Data::setData_srcId(int data_srcId)
{
    this->data_srcId = data_srcId;
}

int Data::getData_destId() const
{
    return this->data_destId;
}

void Data::setData_destId(int data_destId)
{
    this->data_destId = data_destId;
}

double Data::getData_sendPower() const
{
    return this->data_sendPower;
}

void Data::setData_sendPower(double data_sendPower)
{
    this->data_sendPower = data_sendPower;
}

double Data::getData_sendFrequency() const
{
    return this->data_sendFrequency;
}

void Data::setData_sendFrequency(double data_sendFrequency)
{
    this->data_sendFrequency = data_sendFrequency;
}

double Data::getData_dataLenBit() const
{
    return this->data_dataLenBit;
}

void Data::setData_dataLenBit(double data_dataLenBit)
{
    this->data_dataLenBit = data_dataLenBit;
}

omnetpp::simtime_t Data::getSendingTime() const
{
    return this->sendingTime;
}

void Data::setSendingTime(omnetpp::simtime_t sendingTime)
{
    this->sendingTime = sendingTime;
}

class DataDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_data_name,
        FIELD_data_id,
        FIELD_data_srcId,
        FIELD_data_destId,
        FIELD_data_sendPower,
        FIELD_data_sendFrequency,
        FIELD_data_dataLenBit,
        FIELD_sendingTime,
    };
  public:
    DataDescriptor();
    virtual ~DataDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(DataDescriptor)

DataDescriptor::DataDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(Data)), "omnetpp::cPacket")
{
    propertyNames = nullptr;
}

DataDescriptor::~DataDescriptor()
{
    delete[] propertyNames;
}

bool DataDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<Data *>(obj)!=nullptr;
}

const char **DataDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *DataDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int DataDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 8+base->getFieldCount() : 8;
}

unsigned int DataDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_data_name
        FD_ISEDITABLE,    // FIELD_data_id
        FD_ISEDITABLE,    // FIELD_data_srcId
        FD_ISEDITABLE,    // FIELD_data_destId
        FD_ISEDITABLE,    // FIELD_data_sendPower
        FD_ISEDITABLE,    // FIELD_data_sendFrequency
        FD_ISEDITABLE,    // FIELD_data_dataLenBit
        FD_ISEDITABLE,    // FIELD_sendingTime
    };
    return (field >= 0 && field < 8) ? fieldTypeFlags[field] : 0;
}

const char *DataDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "data_name",
        "data_id",
        "data_srcId",
        "data_destId",
        "data_sendPower",
        "data_sendFrequency",
        "data_dataLenBit",
        "sendingTime",
    };
    return (field >= 0 && field < 8) ? fieldNames[field] : nullptr;
}

int DataDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "data_name") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "data_id") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "data_srcId") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "data_destId") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "data_sendPower") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "data_sendFrequency") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "data_dataLenBit") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "sendingTime") == 0) return baseIndex + 7;
    return base ? base->findField(fieldName) : -1;
}

const char *DataDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "string",    // FIELD_data_name
        "int",    // FIELD_data_id
        "int",    // FIELD_data_srcId
        "int",    // FIELD_data_destId
        "double",    // FIELD_data_sendPower
        "double",    // FIELD_data_sendFrequency
        "double",    // FIELD_data_dataLenBit
        "omnetpp::simtime_t",    // FIELD_sendingTime
    };
    return (field >= 0 && field < 8) ? fieldTypeStrings[field] : nullptr;
}

const char **DataDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *DataDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int DataDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    Data *pp = omnetpp::fromAnyPtr<Data>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void DataDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    Data *pp = omnetpp::fromAnyPtr<Data>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'Data'", field);
    }
}

const char *DataDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    Data *pp = omnetpp::fromAnyPtr<Data>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string DataDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    Data *pp = omnetpp::fromAnyPtr<Data>(object); (void)pp;
    switch (field) {
        case FIELD_data_name: return oppstring2string(pp->getData_name());
        case FIELD_data_id: return long2string(pp->getData_id());
        case FIELD_data_srcId: return long2string(pp->getData_srcId());
        case FIELD_data_destId: return long2string(pp->getData_destId());
        case FIELD_data_sendPower: return double2string(pp->getData_sendPower());
        case FIELD_data_sendFrequency: return double2string(pp->getData_sendFrequency());
        case FIELD_data_dataLenBit: return double2string(pp->getData_dataLenBit());
        case FIELD_sendingTime: return simtime2string(pp->getSendingTime());
        default: return "";
    }
}

void DataDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    Data *pp = omnetpp::fromAnyPtr<Data>(object); (void)pp;
    switch (field) {
        case FIELD_data_name: pp->setData_name((value)); break;
        case FIELD_data_id: pp->setData_id(string2long(value)); break;
        case FIELD_data_srcId: pp->setData_srcId(string2long(value)); break;
        case FIELD_data_destId: pp->setData_destId(string2long(value)); break;
        case FIELD_data_sendPower: pp->setData_sendPower(string2double(value)); break;
        case FIELD_data_sendFrequency: pp->setData_sendFrequency(string2double(value)); break;
        case FIELD_data_dataLenBit: pp->setData_dataLenBit(string2double(value)); break;
        case FIELD_sendingTime: pp->setSendingTime(string2simtime(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Data'", field);
    }
}

omnetpp::cValue DataDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    Data *pp = omnetpp::fromAnyPtr<Data>(object); (void)pp;
    switch (field) {
        case FIELD_data_name: return pp->getData_name();
        case FIELD_data_id: return pp->getData_id();
        case FIELD_data_srcId: return pp->getData_srcId();
        case FIELD_data_destId: return pp->getData_destId();
        case FIELD_data_sendPower: return pp->getData_sendPower();
        case FIELD_data_sendFrequency: return pp->getData_sendFrequency();
        case FIELD_data_dataLenBit: return pp->getData_dataLenBit();
        case FIELD_sendingTime: return pp->getSendingTime().dbl();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'Data' as cValue -- field index out of range?", field);
    }
}

void DataDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    Data *pp = omnetpp::fromAnyPtr<Data>(object); (void)pp;
    switch (field) {
        case FIELD_data_name: pp->setData_name(value.stringValue()); break;
        case FIELD_data_id: pp->setData_id(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_data_srcId: pp->setData_srcId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_data_destId: pp->setData_destId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_data_sendPower: pp->setData_sendPower(value.doubleValue()); break;
        case FIELD_data_sendFrequency: pp->setData_sendFrequency(value.doubleValue()); break;
        case FIELD_data_dataLenBit: pp->setData_dataLenBit(value.doubleValue()); break;
        case FIELD_sendingTime: pp->setSendingTime(value.doubleValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Data'", field);
    }
}

const char *DataDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr DataDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    Data *pp = omnetpp::fromAnyPtr<Data>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void DataDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    Data *pp = omnetpp::fromAnyPtr<Data>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Data'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

