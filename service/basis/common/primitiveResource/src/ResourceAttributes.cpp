//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include <ResourceAttributes.h>

#include <internal/ResourceAtrributesConverter.h>

bool operator==(const char* lhs, const ResourceAttributes::Value& rhs)
{
    return rhs == lhs;
}

bool operator==(const ResourceAttributes::Value& lhs, const ResourceAttributes::Value& rhs)
{
    return *lhs.m_data == *rhs.m_data;
}

bool operator==(const ResourceAttributes& lhs, const ResourceAttributes& rhs)
{
    return lhs.m_keyValues == rhs.m_keyValues;
}

ResourceAttributes::Value::Value() :
        m_data{ new ValueVariant{} }
{
}

ResourceAttributes::Value::Value(const Value& from) :
        m_data{ new ValueVariant{ *from.m_data } }
{
}

ResourceAttributes::Value::Value(Value&& from) :
        m_data{ new ValueVariant{} }
{
    m_data->swap(*from.m_data);
}


auto ResourceAttributes::Value::operator=(const Value& rhs) -> Value&
{
    *m_data = new ValueVariant{ *rhs.m_data };
    return *this;
}

auto ResourceAttributes::Value::operator=(const char* rhs) -> Value&
{
    *m_data = std::string{ rhs };
    return *this;
}

auto ResourceAttributes::Value::operator=(std::nullptr_t) -> Value&
{
    *m_data = nullptr;
    return *this;
}

bool ResourceAttributes::Value::operator==(const char* rhs) const
{
    return equals< std::string >(rhs);
}

//bool ResourceAttributes::Value::operator==(std::nullptr_t) const
//{
//    return isTypeOf< std::nullptr_t >();
//}

auto ResourceAttributes::KeyValuePair::KeyVisitor::operator() (iterator* iter) const
        -> result_type {
    return iter->m_cur->first;
}

auto ResourceAttributes::KeyValuePair::KeyVisitor::operator() (const_iterator* iter) const
        -> result_type {
    return iter->m_cur->first;
}

auto ResourceAttributes::KeyValuePair::ValueVisitor::operator() (iterator* iter)
        -> result_type {
    return iter->m_cur->second;
}

auto ResourceAttributes::KeyValuePair::ValueVisitor::operator() (const_iterator* iter)
        -> result_type {
    // should not reach here.
    throw BadGetException("");
}

auto ResourceAttributes::KeyValuePair::ConstValueVisitor::operator() (iterator*iter) const
        -> result_type {
    return iter->m_cur->second;
}

auto ResourceAttributes::KeyValuePair::ConstValueVisitor::operator() (const_iterator* iter) const
        -> result_type {
    return iter->m_cur->second;
}

auto ResourceAttributes::KeyValuePair::key() const -> const std::string&
{
    return boost::apply_visitor(m_keyVisitor, m_iterRef);
}

auto ResourceAttributes::KeyValuePair::value() const -> const Value&
{
    return boost::apply_visitor(m_constValueVisitor, m_iterRef);
}

auto ResourceAttributes::KeyValuePair::value() -> Value&
{
    return boost::apply_visitor(m_valueVisitor, m_iterRef);
}


ResourceAttributes::KeyValuePair::KeyValuePair(boost::variant<iterator*, const_iterator*>&& ref) :
        m_iterRef{ ref }
{
}


ResourceAttributes::iterator::iterator() :
        iterator{ base_iterator{} }
{
}

ResourceAttributes::iterator::iterator(ResourceAttributes& attrs) :
        iterator{ attrs.m_keyValues.begin() }
{
}

ResourceAttributes::iterator::iterator(base_iterator&& iter) :
        m_cur{ std::move(iter) },
        m_keyValuePair{ this }
{
}

auto ResourceAttributes::iterator::operator*() -> KeyValuePair&
{
    return m_keyValuePair;
}

auto ResourceAttributes::iterator::iterator::operator->() -> KeyValuePair*
{
    return &m_keyValuePair;
}

auto ResourceAttributes::iterator::operator++() -> iterator&
{
    ++m_cur;
    return *this;
}

auto ResourceAttributes::iterator::operator++(int) -> iterator
{
    iterator iter(*this);
    ++(*this);
    return iter;
}

bool ResourceAttributes::iterator::operator==(const iterator& rhs) const
{
    return m_cur == rhs.m_cur;
}

bool ResourceAttributes::iterator::operator!=(const iterator& rhs) const
{
    return !(*this == rhs);
}


ResourceAttributes::const_iterator::const_iterator() :
        const_iterator{ base_iterator{} }
{
}

ResourceAttributes::const_iterator::const_iterator(const ResourceAttributes& attrs) :
        const_iterator{ attrs.m_keyValues.begin() }
{
}

ResourceAttributes::const_iterator::const_iterator(base_iterator&& iter) :
        m_cur{ iter }, m_keyValuePair{ this }
{
}

ResourceAttributes::const_iterator::const_iterator(const ResourceAttributes::iterator& iter) :
        m_cur{ iter.m_cur }, m_keyValuePair{ this }
{
}

auto ResourceAttributes::const_iterator::operator=(const ResourceAttributes::iterator& iter) -> const_iterator& {
    m_cur = iter.m_cur;
    return *this;
}

auto ResourceAttributes::const_iterator::operator*() const -> reference
{
    return m_keyValuePair;
}
auto ResourceAttributes::const_iterator::operator->() const -> pointer
{
    return &m_keyValuePair;
}

auto ResourceAttributes::const_iterator::operator++() -> const_iterator&
{
    ++m_cur;
    return *this;
}

auto ResourceAttributes::const_iterator::operator++(int) -> const_iterator
{
    const_iterator iter(*this);
    ++(*this);
    return iter;
}

bool ResourceAttributes::const_iterator::operator==(const const_iterator& rhs) const
{
    return m_cur == rhs.m_cur;
}

bool ResourceAttributes::const_iterator::operator!=(const const_iterator& rhs) const
{
    return !(*this == rhs);
}

auto ResourceAttributes::begin() -> iterator
{
    return iterator{ *this };
}

auto ResourceAttributes::end() -> iterator
{
    return iterator{ m_keyValues.end() };
}


auto ResourceAttributes::begin() const -> const_iterator
{
    return const_iterator{ m_keyValues.begin() };
}

auto ResourceAttributes::cbegin() const -> const_iterator
{
    return const_iterator{ m_keyValues.begin() };
}

auto ResourceAttributes::cend() const -> const_iterator
{
    return const_iterator{ m_keyValues.end() };
}

auto ResourceAttributes::operator[](const std::string& key) -> Value&
{
    return m_keyValues[key];
}

auto ResourceAttributes::at(const std::string& key) -> Value&
{
    try
    {
        return m_keyValues.at(key);
    }
    catch (const std::out_of_range&)
    {
        throw InvalidKeyException{ "" };
    }
}

bool ResourceAttributes::erase(const std::string& key)
{
    return m_keyValues.erase(key) == 1U;
}

bool ResourceAttributes::contains(const std::string& key) const
{
    return m_keyValues.find(key) != m_keyValues.end();
}

bool ResourceAttributes::empty() const
{
    return m_keyValues.empty();
}

size_t ResourceAttributes::size() const
{
    return m_keyValues.size();
}

