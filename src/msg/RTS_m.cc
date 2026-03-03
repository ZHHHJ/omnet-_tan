//
// Generated file, do not edit! Created by opp_msgtool 6.1 from src/msg/RTS.msg.
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
#include "RTS_m.h"

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

Register_Class(RTS)

RTS::RTS(const char *name, short kind) : ::omnetpp::cPacket(name, kind)
{
}

RTS::RTS(const RTS& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

RTS::~RTS()
{
}

RTS& RTS::operator=(const RTS& other)
{
    if (this == &other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void RTS::copy(const RTS& other)
{
    this->rts_name = other.rts_name;
    this->rts_isReceived = other.rts_isReceived;
    this->rts_receivedID = other.rts_receivedID;
    this->rts_isTransmit = other.rts_isTransmit;
    this->rts_srcID = other.rts_srcID;
    this->rts_dataDestID = other.rts_dataDestID;
    this->rts_dataID = other.rts_dataID;
    this->rts_dataLenBit = other.rts_dataLenBit;
    this->rts_state = other.rts_state;
    this->rts_sendPower = other.rts_sendPower;
}

void RTS::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->rts_name);
    doParsimPacking(b,this->rts_isReceived);
    doParsimPacking(b,this->rts_receivedID);
    doParsimPacking(b,this->rts_isTransmit);
    doParsimPacking(b,this->rts_srcID);
    doParsimPacking(b,this->rts_dataDestID);
    doParsimPacking(b,this->rts_dataID);
    doParsimPacking(b,this->rts_dataLenBit);
    doParsimPacking(b,this->rts_state);
    doParsimPacking(b,this->rts_sendPower);
}

void RTS::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->rts_name);
    doParsimUnpacking(b,this->rts_isReceived);
    doParsimUnpacking(b,this->rts_receivedID);
    doParsimUnpacking(b,this->rts_isTransmit);
    doParsimUnpacking(b,this->rts_srcID);
    doParsimUnpacking(b,this->rts_dataDestID);
    doParsimUnpacking(b,this->rts_dataID);
    doParsimUnpacking(b,this->rts_dataLenBit);
    doParsimUnpacking(b,this->rts_state);
    doParsimUnpacking(b,this->rts_sendPower);
}

const char * RTS::getRts_name() const
{
    return this->rts_name.c_str();
}

void RTS::setRts_name(const char * rts_name)
{
    this->rts_name = rts_name;
}

bool RTS::getRts_isReceived() const
{
    return this->rts_isReceived;
}

void RTS::setRts_isReceived(bool rts_isReceived)
{
    this->rts_isReceived = rts_isReceived;
}

int RTS::getRts_receivedID() const
{
    return this->rts_receivedID;
}

void RTS::setRts_receivedID(int rts_receivedID)
{
    this->rts_receivedID = rts_receivedID;
}

bool RTS::getRts_isTransmit() const
{
    return this->rts_isTransmit;
}

void RTS::setRts_isTransmit(bool rts_isTransmit)
{
    this->rts_isTransmit = rts_isTransmit;
}

int RTS::getRts_srcID() const
{
    return this->rts_srcID;
}

void RTS::setRts_srcID(int rts_srcID)
{
    this->rts_srcID = rts_srcID;
}

int RTS::getRts_dataDestID() const
{
    return this->rts_dataDestID;
}

void RTS::setRts_dataDestID(int rts_dataDestID)
{
    this->rts_dataDestID = rts_dataDestID;
}

int RTS::getRts_dataID() const
{
    return this->rts_dataID;
}

void RTS::setRts_dataID(int rts_dataID)
{
    this->rts_dataID = rts_dataID;
}

int RTS::getRts_dataLenBit() const
{
    return this->rts_dataLenBit;
}

void RTS::setRts_dataLenBit(int rts_dataLenBit)
{
    this->rts_dataLenBit = rts_dataLenBit;
}

bool RTS::getRts_state() const
{
    return this->rts_state;
}

void RTS::setRts_state(bool rts_state)
{
    this->rts_state = rts_state;
}

double RTS::getRts_sendPower() const
{
    return this->rts_sendPower;
}

void RTS::setRts_sendPower(double rts_sendPower)
{
    this->rts_sendPower = rts_sendPower;
}

class RTSDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_rts_name,
        FIELD_rts_isReceived,
        FIELD_rts_receivedID,
        FIELD_rts_isTransmit,
        FIELD_rts_srcID,
        FIELD_rts_dataDestID,
        FIELD_rts_dataID,
        FIELD_rts_dataLenBit,
        FIELD_rts_state,
        FIELD_rts_sendPower,
    };
  public:
    RTSDescriptor();
    virtual ~RTSDescriptor();

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

Register_ClassDescriptor(RTSDescriptor)

RTSDescriptor::RTSDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(RTS)), "omnetpp::cPacket")
{
    propertyNames = nullptr;
}

RTSDescriptor::~RTSDescriptor()
{
    delete[] propertyNames;
}

bool RTSDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<RTS *>(obj)!=nullptr;
}

const char **RTSDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *RTSDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int RTSDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 10+base->getFieldCount() : 10;
}

unsigned int RTSDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_rts_name
        FD_ISEDITABLE,    // FIELD_rts_isReceived
        FD_ISEDITABLE,    // FIELD_rts_receivedID
        FD_ISEDITABLE,    // FIELD_rts_isTransmit
        FD_ISEDITABLE,    // FIELD_rts_srcID
        FD_ISEDITABLE,    // FIELD_rts_dataDestID
        FD_ISEDITABLE,    // FIELD_rts_dataID
        FD_ISEDITABLE,    // FIELD_rts_dataLenBit
        FD_ISEDITABLE,    // FIELD_rts_state
        FD_ISEDITABLE,    // FIELD_rts_sendPower
    };
    return (field >= 0 && field < 10) ? fieldTypeFlags[field] : 0;
}

const char *RTSDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "rts_name",
        "rts_isReceived",
        "rts_receivedID",
        "rts_isTransmit",
        "rts_srcID",
        "rts_dataDestID",
        "rts_dataID",
        "rts_dataLenBit",
        "rts_state",
        "rts_sendPower",
    };
    return (field >= 0 && field < 10) ? fieldNames[field] : nullptr;
}

int RTSDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "rts_name") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "rts_isReceived") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "rts_receivedID") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "rts_isTransmit") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "rts_srcID") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "rts_dataDestID") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "rts_dataID") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "rts_dataLenBit") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "rts_state") == 0) return baseIndex + 8;
    if (strcmp(fieldName, "rts_sendPower") == 0) return baseIndex + 9;
    return base ? base->findField(fieldName) : -1;
}

const char *RTSDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "string",    // FIELD_rts_name
        "bool",    // FIELD_rts_isReceived
        "int",    // FIELD_rts_receivedID
        "bool",    // FIELD_rts_isTransmit
        "int",    // FIELD_rts_srcID
        "int",    // FIELD_rts_dataDestID
        "int",    // FIELD_rts_dataID
        "int",    // FIELD_rts_dataLenBit
        "bool",    // FIELD_rts_state
        "double",    // FIELD_rts_sendPower
    };
    return (field >= 0 && field < 10) ? fieldTypeStrings[field] : nullptr;
}

const char **RTSDescriptor::getFieldPropertyNames(int field) const
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

const char *RTSDescriptor::getFieldProperty(int field, const char *propertyName) const
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

int RTSDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    RTS *pp = omnetpp::fromAnyPtr<RTS>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void RTSDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    RTS *pp = omnetpp::fromAnyPtr<RTS>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'RTS'", field);
    }
}

const char *RTSDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    RTS *pp = omnetpp::fromAnyPtr<RTS>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string RTSDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    RTS *pp = omnetpp::fromAnyPtr<RTS>(object); (void)pp;
    switch (field) {
        case FIELD_rts_name: return oppstring2string(pp->getRts_name());
        case FIELD_rts_isReceived: return bool2string(pp->getRts_isReceived());
        case FIELD_rts_receivedID: return long2string(pp->getRts_receivedID());
        case FIELD_rts_isTransmit: return bool2string(pp->getRts_isTransmit());
        case FIELD_rts_srcID: return long2string(pp->getRts_srcID());
        case FIELD_rts_dataDestID: return long2string(pp->getRts_dataDestID());
        case FIELD_rts_dataID: return long2string(pp->getRts_dataID());
        case FIELD_rts_dataLenBit: return long2string(pp->getRts_dataLenBit());
        case FIELD_rts_state: return bool2string(pp->getRts_state());
        case FIELD_rts_sendPower: return double2string(pp->getRts_sendPower());
        default: return "";
    }
}

void RTSDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    RTS *pp = omnetpp::fromAnyPtr<RTS>(object); (void)pp;
    switch (field) {
        case FIELD_rts_name: pp->setRts_name((value)); break;
        case FIELD_rts_isReceived: pp->setRts_isReceived(string2bool(value)); break;
        case FIELD_rts_receivedID: pp->setRts_receivedID(string2long(value)); break;
        case FIELD_rts_isTransmit: pp->setRts_isTransmit(string2bool(value)); break;
        case FIELD_rts_srcID: pp->setRts_srcID(string2long(value)); break;
        case FIELD_rts_dataDestID: pp->setRts_dataDestID(string2long(value)); break;
        case FIELD_rts_dataID: pp->setRts_dataID(string2long(value)); break;
        case FIELD_rts_dataLenBit: pp->setRts_dataLenBit(string2long(value)); break;
        case FIELD_rts_state: pp->setRts_state(string2bool(value)); break;
        case FIELD_rts_sendPower: pp->setRts_sendPower(string2double(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'RTS'", field);
    }
}

omnetpp::cValue RTSDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    RTS *pp = omnetpp::fromAnyPtr<RTS>(object); (void)pp;
    switch (field) {
        case FIELD_rts_name: return pp->getRts_name();
        case FIELD_rts_isReceived: return pp->getRts_isReceived();
        case FIELD_rts_receivedID: return pp->getRts_receivedID();
        case FIELD_rts_isTransmit: return pp->getRts_isTransmit();
        case FIELD_rts_srcID: return pp->getRts_srcID();
        case FIELD_rts_dataDestID: return pp->getRts_dataDestID();
        case FIELD_rts_dataID: return pp->getRts_dataID();
        case FIELD_rts_dataLenBit: return pp->getRts_dataLenBit();
        case FIELD_rts_state: return pp->getRts_state();
        case FIELD_rts_sendPower: return pp->getRts_sendPower();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'RTS' as cValue -- field index out of range?", field);
    }
}

void RTSDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    RTS *pp = omnetpp::fromAnyPtr<RTS>(object); (void)pp;
    switch (field) {
        case FIELD_rts_name: pp->setRts_name(value.stringValue()); break;
        case FIELD_rts_isReceived: pp->setRts_isReceived(value.boolValue()); break;
        case FIELD_rts_receivedID: pp->setRts_receivedID(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_rts_isTransmit: pp->setRts_isTransmit(value.boolValue()); break;
        case FIELD_rts_srcID: pp->setRts_srcID(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_rts_dataDestID: pp->setRts_dataDestID(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_rts_dataID: pp->setRts_dataID(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_rts_dataLenBit: pp->setRts_dataLenBit(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_rts_state: pp->setRts_state(value.boolValue()); break;
        case FIELD_rts_sendPower: pp->setRts_sendPower(value.doubleValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'RTS'", field);
    }
}

const char *RTSDescriptor::getFieldStructName(int field) const
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

omnetpp::any_ptr RTSDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    RTS *pp = omnetpp::fromAnyPtr<RTS>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void RTSDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    RTS *pp = omnetpp::fromAnyPtr<RTS>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'RTS'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

