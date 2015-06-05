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
#include <InternalUtil.h>

#include <gtest/gtest.h>

using namespace testing;

#define KEY "key"

class ResourceAttributesTest: public Test
{
public:
    ResourceAttributes resourceAttributes;
};

TEST_F(ResourceAttributesTest, InitialSizeIsZero)
{
    ASSERT_EQ(0U, resourceAttributes.size());
    ASSERT_TRUE(resourceAttributes.empty());
}

TEST_F(ResourceAttributesTest, InsertWithSquareBracket)
{
    resourceAttributes[KEY] = 1;

    ASSERT_TRUE(resourceAttributes[KEY] == 1);
}

TEST_F(ResourceAttributesTest, ValueThrowsIfTypeDoesNotMatch)
{
    resourceAttributes[KEY] = 1;
    auto& valueRef = resourceAttributes[KEY];

    ASSERT_THROW(valueRef.get< std::string >(), BadGetException);
}

TEST_F(ResourceAttributesTest, GettingWithAtThrowsIfThereIsNoMatchedValue)
{
    ASSERT_THROW(resourceAttributes.at(KEY), InvalidKeyException);
}

TEST_F(ResourceAttributesTest, CopyingValueDoesNotShareState)
{
    const char arbitraryStr[] { "ftryb457" };
    resourceAttributes[KEY] = 1;

    ResourceAttributes::Value copied { resourceAttributes[KEY] };
    copied = arbitraryStr;

    ASSERT_TRUE(resourceAttributes[KEY] == 1);
    ASSERT_TRUE(copied == arbitraryStr);
}

TEST_F(ResourceAttributesTest, IsNullWhenAssignmentNullptr)
{
    resourceAttributes[KEY] = nullptr;

    ASSERT_TRUE(resourceAttributes[KEY] == nullptr);
}

TEST_F(ResourceAttributesTest, ValueChangedIfPutWithSameKey)
{
    resourceAttributes[KEY] = "string";
    resourceAttributes[KEY] = true;

    ASSERT_TRUE(resourceAttributes[KEY] == true);
}

TEST_F(ResourceAttributesTest, ObjectIsEmptyAfterMoved)
{
    resourceAttributes[KEY] = 1;

    ResourceAttributes moved{ std::move(resourceAttributes) };

    ASSERT_TRUE(resourceAttributes.empty());
}

TEST_F(ResourceAttributesTest, GettingWithAtThrowsAfterRemoved)
{
    resourceAttributes[KEY] = 1;

    resourceAttributes.erase(KEY);

    ASSERT_THROW(resourceAttributes.at(KEY), InvalidKeyException);
}

TEST_F(ResourceAttributesTest, NoDataErasedIfKeyDoesNotMatch)
{
    ASSERT_FALSE(resourceAttributes.erase(KEY));
}

TEST_F(ResourceAttributesTest, ChangeValueWithAtGetter)
{
    resourceAttributes[KEY] = 1;

    resourceAttributes.at(KEY) = "after";

    ASSERT_TRUE(resourceAttributes[KEY] == "after");
}

TEST_F(ResourceAttributesTest, CanHaveNestedResourceAttributes)
{
    ResourceAttributes nested;
    nested["nested"] = "nested_value";
    resourceAttributes[KEY] = nested;

    ASSERT_TRUE(resourceAttributes[KEY].get<ResourceAttributes>()["nested"] == "nested_value");
}

class ResourceAttributesIteratorTest: public Test
{
public:
    ResourceAttributes resourceAttributes;
};

TEST_F(ResourceAttributesIteratorTest, BeginEqualsEndWhenEmpty)
{
    ASSERT_TRUE(resourceAttributes.begin() == resourceAttributes.end());
}

TEST_F(ResourceAttributesIteratorTest, CanIteratesWithForeach)
{
    resourceAttributes["first"] = 1;
    resourceAttributes["second"] = 2;

    int count = 0;

    for (auto& i : resourceAttributes) {
        i.key();
        ++count;
    }

    ASSERT_EQ(2, count);
}

TEST_F(ResourceAttributesIteratorTest, IteratesWithRef)
{
    const char arbitraryStr[] { "ftryb457" };
    resourceAttributes[KEY] = 1;

    for (auto& i : resourceAttributes) {
        i.value() = arbitraryStr;
    }

    ASSERT_TRUE(resourceAttributes[KEY] == arbitraryStr);
}

TEST_F(ResourceAttributesIteratorTest, IteratorIsCopyable)
{
    ResourceAttributes::iterator it;

    it = resourceAttributes.begin();

    ASSERT_EQ(it, resourceAttributes.begin());
}

TEST_F(ResourceAttributesIteratorTest, IteratorIndicateNextItemAfterIncreased)
{
    resourceAttributes[KEY] = 1;

    ResourceAttributes::iterator it = resourceAttributes.begin();

    it++;

    ASSERT_TRUE(it == resourceAttributes.end());
}

TEST_F(ResourceAttributesIteratorTest, IteratorCanBeConvertedIntoConstIterator)
{
    resourceAttributes[KEY] = 1;
    ResourceAttributes::const_iterator it { resourceAttributes.begin() };
    it = resourceAttributes.cbegin();

    it++;

    ASSERT_TRUE(it == resourceAttributes.cend());
}

TEST_F(ResourceAttributesIteratorTest, ConstIteratesCam)
{
    resourceAttributes[KEY] = 1;

    for (const auto& i : resourceAttributes) {
        i.key();
    }

    ASSERT_EQ(1, resourceAttributes[KEY]);
}

TEST(ResourceAttributesValueTest, MovedValueHasNull)
{
    ResourceAttributes::Value one { 1 };
    ResourceAttributes::Value another { std::move(one) };

    ASSERT_EQ(nullptr, one);
}

TEST(ResourceAttributesConverterTest, OCRepresentationCanBeConvertedIntoResourceAttributes)
{
    constexpr double value = 9876;
    OC::OCRepresentation ocRep;
    ocRep[KEY] = value;

    ResourceAttributes resourceAttributes = ResourceAttributesConverter::fromOCRepresentation(ocRep);

    ASSERT_TRUE(value == resourceAttributes[KEY]);
}


TEST(ResourceAttributesConverterTest, NestedOCRepresentationCanBeConvertedIntoResourceAttributes)
{
    std::string nested_value { "nested" };
    OC::OCRepresentation ocRep;
    OC::OCRepresentation nested;
    nested[KEY] = nested_value;
    ocRep[KEY] = nested;

    ResourceAttributes resourceAttributes = ResourceAttributesConverter::fromOCRepresentation(ocRep);

    ASSERT_TRUE(nested_value == resourceAttributes[KEY].get<ResourceAttributes>()[KEY]);
}


TEST(ResourceAttributesConverterTest, ResourceAttributesCanBeConvertedIntoOCRepresentation)
{
    double value { 3453453 };
    ResourceAttributes resourceAttributes;
    resourceAttributes[KEY] = value;

    OC::OCRepresentation ocRep = ResourceAttributesConverter::toOCRepresentation(resourceAttributes);

    ASSERT_TRUE(value == ocRep[KEY].getValue<double>());
}

TEST(ResourceAttributesConverterTest, NestedResourceAttributesCanBeConvertedIntoOCRepresentation)
{
    std::string nested_value { "nested" };
    ResourceAttributes resourceAttributes;
    ResourceAttributes nested;
    nested[KEY] = nested_value;
    resourceAttributes[KEY] = nested;

    OC::OCRepresentation ocRep = ResourceAttributesConverter::toOCRepresentation(resourceAttributes);

    ASSERT_TRUE(nested_value == ocRep[KEY].getValue<OC::OCRepresentation>()[KEY].getValue<std::string>());
}

TEST(ResourceAttributesConverterTest, OCRepresentationNullTypeIsNullptrInResourceAttributes)
{
    OC::OCRepresentation ocRep;
    ocRep.setNULL(KEY);

    ResourceAttributes resourceAttributes = ResourceAttributesConverter::fromOCRepresentation(ocRep);

    ASSERT_EQ(nullptr, resourceAttributes[KEY]);
}

TEST(ResourceAttributesConverterTest, OCRepresentationHasNullWhenResourceAttributeIsNullptr)
{
    ResourceAttributes resourceAttributes;
    resourceAttributes[KEY] = nullptr;

    OC::OCRepresentation ocRep = ResourceAttributesConverter::toOCRepresentation(resourceAttributes);

    ASSERT_TRUE(ocRep.isNULL(KEY));
}
