//
// Generated file, do not edit! Created by opp_msgtool 6.1 from src/msg/CTS.msg.
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
#include "CTS_m.h"

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

Register_Class(CTS)

CTS::CTS(const char *name, short kind) : ::omnetpp::cPacket(name, kind)
{
}

CTS::CTS(const CTS& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

CTS::~CTS()
{
}

CTS& CTS::operator=(const CTS& other)
{
    if (this == &other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void CTS::copy(const CTS& other)
{
    this->cts_name = other.cts_name;
    this->rts_name = other.rts_name;
    this->rts_id = other.rts_id;
    this->cts_srcID = other.cts_srcID;
    this->cts_destID = other.cts_destID;
    this->cts_dataID = other.cts_dataID;
    this->cts_TxTime = other.cts_TxTime;
    this->cts_DataTxDelay1 = other.cts_DataTxDelay1;
    this->cts_DataTxDelay2 = other.cts_DataTxDelay2;
    this->cts_TxChannel = other.cts_TxChannel;
    this->cts_TxPower = other.cts_TxPower;
    this->cts_TxDirection = other.cts_TxDirection;
    this->cts_transportationDelay = other.cts_transportationDelay;
}

void CTS::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->cts_name);
    doParsimPacking(b,this->rts_name);
    doParsimPacking(b,this->rts_id);
    doParsimPacking(b,this->cts_srcID);
    doParsimPacking(b,this->cts_destID);
    doParsimPacking(b,this->cts_dataID);
    doParsimPacking(b,this->cts_TxTime);
    doParsimPacking(b,this->cts_DataTxDelay1);
    doParsimPacking(b,this->cts_DataTxDelay2);
    doParsimPacking(b,this->cts_TxChannel);
    doParsimPacking(b,this->cts_TxPower);
    doParsimPacking(b,this->cts_TxDirection);
    doParsimPacking(b,this->cts_transportationDelay);
}

void CTS::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->cts_name);
    doParsimUnpacking(b,this->rts_name);
    doParsimUnpacking(b,this->rts_id);
    doParsimUnpacking(b,this->cts_srcID);
    doParsimUnpacking(b,this->cts_destID);
    doParsimUnpacking(b,this->cts_dataID);
    doParsimUnpacking(b,this->cts_TxTime);
    doParsimUnpacking(b,this->cts_DataTxDelay1);
    doParsimUnpacking(b,this->cts_DataTxDelay2);
    doParsimUnpacking(b,this->cts_TxChannel);
    doParsimUnpacking(b,this->cts_TxPower);
    doParsimUnpacking(b,this->cts_TxDirection);
    doParsimUnpacking(b,this->cts_transportationDelay);
}

const char * CTS::getCts_name() const
{
    return this->cts_name.c_str();
}

void CTS::setCts_name(const char * cts_name)
{
    this->cts_name = cts_name;
}

const char * CTS::getRts_name() const
{
    return this->rts_name.c_str();
}

void CTS::setRts_name(const char * rts_name)
{
    this->rts_name = rts_name;
}

int CTS::getRts_id() const
{
    return this->rts_id;
}

void CTS::setRts_id(int rts_id)
{
    this->rts_id = rts_id;
}

int CTS::getCts_srcID() const
{
    return this->cts_srcID;
}

void CTS::setCts_srcID(int cts_srcID)
{
    this->cts_srcID = cts_srcID;
}

int CTS::getCts_destID() const
{
    return this->cts_destID;
}

void CTS::setCts_destID(int cts_destID)
{
    this->cts_destID = cts_destID;
}

int CTS::getCts_dataID() const
{
    return this->cts_dataID;
}

void CTS::setCts_dataID(int cts_dataID)
{
    this->cts_dataID = cts_dataID;
}

omnetpp::simtime_t CTS::getCts_TxTime() const
{
    return this->cts_TxTime;
}

void CTS::setCts_TxTime(omnetpp::simtime_t cts_TxTime)
{
    this->cts_TxTime = cts_TxTime;
}

double CTS::getCts_DataTxDelay1() const
{
    return this->cts_DataTxDelay1;
}

void CTS::setCts_DataTxDelay1(double cts_DataTxDelay1)
{
    this->cts_DataTxDelay1 = cts_DataTxDelay1;
}

double CTS::getCts_DataTxDelay2() const
{
    return this->cts_DataTxDelay2;
}

void CTS::setCts_DataTxDelay2(double cts_DataTxDelay2)
{
    this->cts_DataTxDelay2 = cts_DataTxDelay2;
}

double CTS::getCts_TxChannel() const
{
    return this->cts_TxChannel;
}

void CTS::setCts_TxChannel(double cts_TxChannel)
{
    this->cts_TxChannel = cts_TxChannel;
}

double CTS::getCts_TxPower() const
{
    return this->cts_TxPower;
}

void CTS::setCts_TxPower(double cts_TxPower)
{
    this->cts_TxPower = cts_TxPower;
}

double CTS::getCts_TxDirection() const
{
    return this->cts_TxDirection;
}

void CTS::setCts_TxDirection(double cts_TxDirection)
{
    this->cts_TxDirection = cts_TxDirection;
}

double CTS::getCts_transportationDelay() const
{
    return this->cts_transportationDelay;
}

void CTS::setCts_transportationDelay(double cts_transportationDelay)
{
    this->cts_transportationDelay = cts_transportationDelay;
}

class CTSDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_cts_name,
        FIELD_rts_name,
        FIELD_rts_id,
        FIELD_cts_srcID,
        FIELD_cts_destID,
        FIELD_cts_dataID,
        FIELD_cts_TxTime,
        FIELD_cts_DataTxDelay1,
        FIELD_cts_DataTxDelay2,
        FIELD_cts_TxChannel,
        FIELD_cts_TxPower,
        FIELD_cts_TxDirection,
        FIELD_cts_transportationDelay,
    };
  public:
    CTSDescriptor();
    virtual ~CTSDescriptor();

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

Register_ClassDescriptor(CTSDescriptor)

CTSDescriptor::CTSDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(CTS)), "omnetpp::cPacket")
{
    propertyNames = nullptr;
}

CTSDescriptor::~CTSDescriptor()
{
    delete[] propertyNames;
}

bool CTSDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<CTS *>(obj)!=nullptr;
}

const char **CTSDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *CTSDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int CTSDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 13+base->getFieldCount() : 13;
}

unsigned int CTSDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_cts_name
        FD_ISEDITABLE,    // FIELD_rts_name
        FD_ISEDITABLE,    // FIELD_rts_id
        FD_ISEDITABLE,    // FIELD_cts_srcID
        FD_ISEDITABLE,    // FIELD_cts_destID
        FD_ISEDITABLE,    // FIELD_cts_dataID
        FD_ISEDITABLE,    // FIELD_cts_TxTime
        FD_ISEDITABLE,    // FIELD_cts_DataTxDelay1
        FD_ISEDITABLE,    // FIELD_cts_DataTxDelay2
        FD_ISEDITABLE,    // FIELD_cts_TxChannel
        FD_ISEDITABLE,    // FIELD_cts_TxPower
        FD_ISEDITABLE,    // FIELD_cts_TxDirection
        FD_ISEDITABLE,    // FIELD_cts_transportationDelay
    };
    return (field >= 0 && field < 13) ? fieldTypeFlags[field] : 0;
}

const char *CTSDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "cts_name",
        "rts_name",
        "rts_id",
        "cts_srcID",
        "cts_destID",
        "cts_dataID",
        "cts_TxTime",
        "cts_DataTxDelay1",
        "cts_DataTxDelay2",
        "cts_TxChannel",
        "cts_TxPower",
        "cts_TxDirection",
        "cts_transportationDelay",
    };
    return (field >= 0 && field < 13) ? fieldNames[field] : nullptr;
}

int CTSDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "cts_name") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "rts_name") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "rts_id") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "cts_srcID") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "cts_destID") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "cts_dataID") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "cts_TxTime") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "cts_DataTxDelay1") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "cts_DataTxDelay2") == 0) return baseIndex + 8;
    if (strcmp(fieldName, "cts_TxChannel") == 0) return baseIndex + 9;
    if (strcmp(fieldName, "cts_TxPower") == 0) return baseIndex + 10;
    if (strcmp(fieldName, "cts_TxDirection") == 0) return baseIndex + 11;
    if (strcmp(fieldName, "cts_transportationDelay") == 0) return baseIndex + 12;
    return base ? base->findField(fieldName) : -1;
}

const char *CTSDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "string",    // FIELD_cts_name
        "string",    // FIELD_rts_name
        "int",    // FIELD_rts_id
        "int",    // FIELD_cts_srcID
        "int",    // FIELD_cts_destID
        "int",    // FIELD_cts_dataID
        "omnetpp::simtime_t",    // FIELD_cts_TxTime
        "double",    // FIELD_cts_DataTxDelay1
        "double",    // FIELD_cts_DataTxDelay2
        "double",    // FIELD_cts_TxChannel
        "double",    // FIELD_cts_TxPower
        "double",    // FIELD_cts_TxDirection
        "double",    // FIELD_cts_transportationDelay
    };
    return (field >= 0 && field < 13) ? fieldTypeStrings[field] : nullptr;
}

const char **CTSDescriptor::getFieldPropertyNames(int field) const
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

const char *CTSDescriptor::getFieldProperty(int field, const char *propertyName) const
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

int CTSDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    CTS *pp = omnetpp::fromAnyPtr<CTS>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void CTSDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    CTS *pp = omnetpp::fromAnyPtr<CTS>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'CTS'", field);
    }
}

const char *CTSDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    CTS *pp = omnetpp::fromAnyPtr<CTS>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string CTSDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    CTS *pp = omnetpp::fromAnyPtr<CTS>(object); (void)pp;
    switch (field) {
        case FIELD_cts_name: return oppstring2string(pp->getCts_name());
        case FIELD_rts_name: return oppstring2string(pp->getRts_name());
        case FIELD_rts_id: return long2string(pp->getRts_id());
        case FIELD_cts_srcID: return long2string(pp->getCts_srcID());
        case FIELD_cts_destID: return long2string(pp->getCts_destID());
        case FIELD_cts_dataID: return long2string(pp->getCts_dataID());
        case FIELD_cts_TxTime: return simtime2string(pp->getCts_TxTime());
        case FIELD_cts_DataTxDelay1: return double2string(pp->getCts_DataTxDelay1());
        case FIELD_cts_DataTxDelay2: return double2string(pp->getCts_DataTxDelay2());
        case FIELD_cts_TxChannel: return double2string(pp->getCts_TxChannel());
        case FIELD_cts_TxPower: return double2string(pp->getCts_TxPower());
        case FIELD_cts_TxDirection: return double2string(pp->getCts_TxDirection());
        case FIELD_cts_transportationDelay: return double2string(pp->getCts_transportationDelay());
        default: return "";
    }
}

void CTSDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CTS *pp = omnetpp::fromAnyPtr<CTS>(object); (void)pp;
    switch (field) {
        case FIELD_cts_name: pp->setCts_name((value)); break;
        case FIELD_rts_name: pp->setRts_name((value)); break;
        case FIELD_rts_id: pp->setRts_id(string2long(value)); break;
        case FIELD_cts_srcID: pp->setCts_srcID(string2long(value)); break;
        case FIELD_cts_destID: pp->setCts_destID(string2long(value)); break;
        case FIELD_cts_dataID: pp->setCts_dataID(string2long(value)); break;
        case FIELD_cts_TxTime: pp->setCts_TxTime(string2simtime(value)); break;
        case FIELD_cts_DataTxDelay1: pp->setCts_DataTxDelay1(string2double(value)); break;
        case FIELD_cts_DataTxDelay2: pp->setCts_DataTxDelay2(string2double(value)); break;
        case FIELD_cts_TxChannel: pp->setCts_TxChannel(string2double(value)); break;
        case FIELD_cts_TxPower: pp->setCts_TxPower(string2double(value)); break;
        case FIELD_cts_TxDirection: pp->setCts_TxDirection(string2double(value)); break;
        case FIELD_cts_transportationDelay: pp->setCts_transportationDelay(string2double(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CTS'", field);
    }
}

omnetpp::cValue CTSDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    CTS *pp = omnetpp::fromAnyPtr<CTS>(object); (void)pp;
    switch (field) {
        case FIELD_cts_name: return pp->getCts_name();
        case FIELD_rts_name: return pp->getRts_name();
        case FIELD_rts_id: return pp->getRts_id();
        case FIELD_cts_srcID: return pp->getCts_srcID();
        case FIELD_cts_destID: return pp->getCts_destID();
        case FIELD_cts_dataID: return pp->getCts_dataID();
        case FIELD_cts_TxTime: return pp->getCts_TxTime().dbl();
        case FIELD_cts_DataTxDelay1: return pp->getCts_DataTxDelay1();
        case FIELD_cts_DataTxDelay2: return pp->getCts_DataTxDelay2();
        case FIELD_cts_TxChannel: return pp->getCts_TxChannel();
        case FIELD_cts_TxPower: return pp->getCts_TxPower();
        case FIELD_cts_TxDirection: return pp->getCts_TxDirection();
        case FIELD_cts_transportationDelay: return pp->getCts_transportationDelay();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'CTS' as cValue -- field index out of range?", field);
    }
}

void CTSDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CTS *pp = omnetpp::fromAnyPtr<CTS>(object); (void)pp;
    switch (field) {
        case FIELD_cts_name: pp->setCts_name(value.stringValue()); break;
        case FIELD_rts_name: pp->setRts_name(value.stringValue()); break;
        case FIELD_rts_id: pp->setRts_id(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_cts_srcID: pp->setCts_srcID(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_cts_destID: pp->setCts_destID(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_cts_dataID: pp->setCts_dataID(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_cts_TxTime: pp->setCts_TxTime(value.doubleValue()); break;
        case FIELD_cts_DataTxDelay1: pp->setCts_DataTxDelay1(value.doubleValue()); break;
        case FIELD_cts_DataTxDelay2: pp->setCts_DataTxDelay2(value.doubleValue()); break;
        case FIELD_cts_TxChannel: pp->setCts_TxChannel(value.doubleValue()); break;
        case FIELD_cts_TxPower: pp->setCts_TxPower(value.doubleValue()); break;
        case FIELD_cts_TxDirection: pp->setCts_TxDirection(value.doubleValue()); break;
        case FIELD_cts_transportationDelay: pp->setCts_transportationDelay(value.doubleValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CTS'", field);
    }
}

const char *CTSDescriptor::getFieldStructName(int field) const
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

omnetpp::any_ptr CTSDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    CTS *pp = omnetpp::fromAnyPtr<CTS>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void CTSDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    CTS *pp = omnetpp::fromAnyPtr<CTS>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CTS'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

