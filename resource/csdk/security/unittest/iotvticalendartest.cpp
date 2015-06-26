// Copyright 2015 Intel Mobile Communications GmbH All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//Not supported on Arduino due lack of absolute time need to implement iCalendar
#ifndef WITH_ARDUINO

#include "gtest/gtest.h"
#include "iotvticalendar.h"
#include "logger.h"

#define TAG  PCF("CALENDAR-UT")

static void printPeriod(IotvtICalPeriod_t *period)
{
    OC_LOG_V(INFO, TAG, PCF("period->startDateTime.tm_year = %d"),period->startDateTime.tm_year);
    OC_LOG_V(INFO, TAG, PCF("period->startDateTime.tm_mon = %d"),period->startDateTime.tm_mon);
    OC_LOG_V(INFO, TAG, PCF("period->startDateTime.tm_mday = %d"),period->startDateTime.tm_mday);
    OC_LOG_V(INFO, TAG, PCF("period->startDateTime.tm_hour = %d"),period->startDateTime.tm_hour);
    OC_LOG_V(INFO, TAG, PCF("period->startDateTime.tm_min = %d"),period->startDateTime.tm_min);
    OC_LOG_V(INFO, TAG, PCF("period->startDateTime.tm_sec = %d"),period->startDateTime.tm_sec);

    OC_LOG_V(INFO, TAG, PCF("period->endDateTime.tm_year = %d"),period->endDateTime.tm_year);
    OC_LOG_V(INFO, TAG, PCF("period->endDateTime.tm_mon = %d"),period->endDateTime.tm_mon);
    OC_LOG_V(INFO, TAG, PCF("period->endDateTime.tm_mday = %d"),period->endDateTime.tm_mday);
    OC_LOG_V(INFO, TAG, PCF("period->endDateTime.tm_hour = %d"),period->endDateTime.tm_hour);
    OC_LOG_V(INFO, TAG, PCF("period->endDateTime.tm_min = %d"),period->endDateTime.tm_min);
    OC_LOG_V(INFO, TAG, PCF("period->startDateTime.tm_sec = %d"),period->endDateTime.tm_sec);
}

static void printRecur(IotvtICalRecur_t *recur)
{
    OC_LOG_V(INFO, TAG, PCF("recur->freq = %d"), recur->freq);
    OC_LOG_V(INFO, TAG, PCF("recur->until.tm_year = %d"), recur->until.tm_year);
    OC_LOG_V(INFO, TAG, PCF("recur->until.tm_mon = %d"), recur->until.tm_mon);
    OC_LOG_V(INFO, TAG, PCF("recur->until.tm_mday = %d"), recur->until.tm_mday);

    if(recur->byDay & SUNDAY)
    {
        OC_LOG_V(INFO, TAG, PCF("recur->byDay = %s"), "Sunday");
    }
    if(recur->byDay & MONDAY)
    {
        OC_LOG_V(INFO, TAG, PCF("recur->byDay = %s"), "Monday");
    }
    if(recur->byDay & TUESDAY)
    {
        OC_LOG_V(INFO, TAG, PCF("recur->byDay = %s"), "Tuesday");
    }
    if(recur->byDay & WEDNESDAY)
    {
        OC_LOG_V(INFO, TAG, PCF("recur->byDay = %s"), "Wednesday");
    }
    if(recur->byDay & THURSDAY)
    {
        OC_LOG_V(INFO, TAG, PCF("recur->byDay = %s"), "Thursday");
    }
    if(recur->byDay & FRIDAY)
    {
        OC_LOG_V(INFO, TAG, PCF("recur->byDay = %s"), "Friday");
    }
    if(recur->byDay & SATURDAY)
    {
        OC_LOG_V(INFO, TAG, PCF("recur->byDay = %s"), "Saturday");
    }
}

//ParsePeriod Tests
TEST(ParsePeriodTest, ParsePeriodValidDateTime)
{
    char periodStr[] = "20150629T153050/20150630T233055";
    IotvtICalPeriod_t period = {};
    EXPECT_EQ(IOTVTICAL_SUCCESS, ParsePeriod(periodStr,&period));
    printPeriod(&period);
}

TEST(ParsePeriodTest, ParsePeriodValidDate)
{
    char periodStr[] = "20150629/20150630";
    IotvtICalPeriod_t period = {};
    EXPECT_EQ(IOTVTICAL_SUCCESS, ParsePeriod(periodStr,&period));
    printPeriod(&period);
}

TEST(ParsePeriodTest, ParsePeriodMismatchStartDTEndDT1)
{
    char periodStr[] = "20150629T153050/20150630";
    IotvtICalPeriod_t period = {};
    EXPECT_EQ(IOTVTICAL_INVALID_PERIOD, ParsePeriod(periodStr,&period));
}

TEST(ParsePeriodTest, ParsePeriodMismatchStartDTEndDT2)
{
    char periodStr[] = "20150629/20150630T203055";
    IotvtICalPeriod_t period = {};
    EXPECT_EQ(IOTVTICAL_INVALID_PERIOD, ParsePeriod(periodStr,&period));
}

TEST(ParsePeriodTest, ParsePeriodInvalidStartDT1)
{
    char periodStr[] = "20150629T1530/20150630T203055";
    IotvtICalPeriod_t period = {};
    EXPECT_EQ(IOTVTICAL_INVALID_PERIOD, ParsePeriod(periodStr,&period));
}

TEST(ParsePeriodTest, ParsePeriodInvalidEndtDT2)
{
    char periodStr[] = "20150629T153050/20150630203055";
    IotvtICalPeriod_t period = {};
    EXPECT_EQ(IOTVTICAL_INVALID_PERIOD, ParsePeriod(periodStr,&period));
}

TEST(ParsePeriodTest, ParsePeriodInvalidStartD3)
{
    char periodStr[] = "201506/20150630";
    IotvtICalPeriod_t period = {};
    EXPECT_EQ(IOTVTICAL_INVALID_PERIOD, ParsePeriod(periodStr,&period));
}

TEST(ParsePeriodTest, ParsePeriodInvalidEndD4)
{
    char periodStr[] = "20150629/201530";
    IotvtICalPeriod_t period = {};
    EXPECT_EQ(IOTVTICAL_INVALID_PERIOD, ParsePeriod(periodStr,&period));
}

TEST(ParsePeriodTest, ParsePeriodEndDTBeforeStartDT)
{
    char periodStr[] = "20150630T203055/20150629T153050";
    IotvtICalPeriod_t period = {};
    EXPECT_EQ(IOTVTICAL_INVALID_PERIOD, ParsePeriod(periodStr,&period));
}
TEST(ParsePeriodTest, ParsePeriodEndDBeforeStartD)
{
    char periodStr[] = "20150630/20150629";
    IotvtICalPeriod_t period = {};
    EXPECT_EQ(IOTVTICAL_INVALID_PERIOD, ParsePeriod(periodStr,&period));
}

TEST(ParseRecurTest, ParseRecurValid1)
{
    char recurStr[] = "FREQ=DAILY; UNTIL=20150703; BYDAY=MO, WE, FR";
    IotvtICalRecur_t recur = {};
    EXPECT_EQ(IOTVTICAL_SUCCESS, ParseRecur(recurStr, &recur));
    printRecur(&recur);
}

TEST(ParseRecurTest, ParseRecurValid2)
{
    char recurStr[] = "FREQ=DAILY";
    IotvtICalRecur_t recur = {};
    EXPECT_EQ(IOTVTICAL_SUCCESS, ParseRecur(recurStr, &recur));
    printRecur(&recur);
}

TEST(ParseRecurTest, ParseRecurInValidFreq1)
{
    char recurStr[] = "FREQ=WEEKLY; UNTIL=20150703; BYDAY=TU";
    IotvtICalRecur_t recur = {};
    EXPECT_EQ(IOTVTICAL_INVALID_RRULE, ParseRecur(recurStr, &recur));
}

TEST(ParseRecurTest, ParseRecurInValidFreq2)
{
    char recurStr[] = "UNTIL=20150703; BYDAY=TU";
    IotvtICalRecur_t recur = {};
    EXPECT_EQ(IOTVTICAL_INVALID_RRULE, ParseRecur(recurStr, &recur));
}

TEST(ParseRecurTest, ParseRecurInValidUntil)
{
    char recurStr[] = "FREQ=DAILY; UNTIL=20150703T095055; BYDAY=MO, WE, FR";
    IotvtICalRecur_t recur = {};
    EXPECT_EQ(IOTVTICAL_INVALID_RRULE, ParseRecur(recurStr, &recur));
}

TEST(ParseRecurTest, ParseRecurInValidByday)
{
    char recurStr[] = "FREQ=DAILY; UNTIL=20150703; BYDAY=";
    IotvtICalRecur_t recur = {};
    EXPECT_EQ(IOTVTICAL_INVALID_RRULE, ParseRecur(recurStr, &recur));
}

//FIXME: ALL IsRequestWithinValidTime tests will fail after 20151230
TEST(IsRequestWithinValidTimeTest, IsRequestWithinValidTimeValidPeriod1)
{
    char periodStr[] = "20150630/20151230";
    EXPECT_EQ(IOTVTICAL_VALID_ACCESS, IsRequestWithinValidTime(periodStr, NULL));
}

TEST(IsRequestWithinValidTimeTest, IsRequestWithinValidTimeValidPeriodAndRecur1)
{
    //Daily on days MO, WE & FR from 6:00:00am to 8:00:00pm until 20151230
    char recurStr[] = "FREQ=DAILY; UNTIL=20151230; BYDAY=MO, WE, FR";
    char periodStr[] = "20150630T060000/20150630T200000";

    time_t rt = time(0);
    IotvtICalDateTime_t *ct = localtime(&rt);

    int byDay = MONDAY | WEDNESDAY | FRIDAY;
    int isValidWD = (0x1 << ct->tm_wday) & byDay;

    if(isValidWD && 6 <= ct->tm_hour && 20>= ct->tm_hour)
    {
        EXPECT_EQ(IOTVTICAL_VALID_ACCESS, IsRequestWithinValidTime(periodStr, recurStr));
    }
    else
    {
        EXPECT_EQ(IOTVTICAL_INVALID_ACCESS, IsRequestWithinValidTime(periodStr, recurStr));
    }
}

TEST(IsRequestWithinValidTimeTest, IsRequestWithinValidTimeValidPeriodAndRecur2)
{
    //Daily forever from 6:00:00am to 8:00:00pm
    char recurStr[] = "FREQ=DAILY";
    char periodStr[] = "20150630T060000/20150630T200000";

    time_t rt = time(0);
    IotvtICalDateTime_t *ct = localtime(&rt);

    if(6 <= ct->tm_hour && 20>= ct->tm_hour)
    {
        EXPECT_EQ(IOTVTICAL_VALID_ACCESS, IsRequestWithinValidTime(periodStr, recurStr));
    }
    else
    {
        EXPECT_EQ(IOTVTICAL_INVALID_ACCESS, IsRequestWithinValidTime(periodStr, recurStr));
    }
}

TEST(IsRequestWithinValidTimeTest, IsRequestWithinValidTimeValidPeriodAndRecur3)
{
    //Daily until 20151230 from 6:00:00am to 8:00:00pm
    char recurStr[] = "FREQ=DAILY; UNTIL=20151230";
    char periodStr[] = "20150630T060000/20150630T200000";

    time_t rt = time(0);
    IotvtICalDateTime_t *ct = localtime(&rt);

    if(6 <= ct->tm_hour && 20>= ct->tm_hour)
    {
        EXPECT_EQ(IOTVTICAL_VALID_ACCESS, IsRequestWithinValidTime(periodStr, recurStr));
    }
    else
    {
        EXPECT_EQ(IOTVTICAL_INVALID_ACCESS, IsRequestWithinValidTime(periodStr, recurStr));
    }
}

TEST(IsRequestWithinValidTimeTest, IsRequestWithinValidTimeValidPeriodAndRecur4)
{
    //Daily forever on days MO, WE & Fr from 6:00:00am to 8:00:00pm
    char recurStr[] = "FREQ=DAILY; BYDAY=MO, WE, FR";
    char periodStr[] = "20150630T060000/20150630T200000";

    time_t rt = time(0);
    IotvtICalDateTime_t *ct = localtime(&rt);

    int byDay = MONDAY | WEDNESDAY | FRIDAY;
    int isValidWD = (0x1 << ct->tm_wday) & byDay;

    if(isValidWD && 6 <= ct->tm_hour && 20>= ct->tm_hour)
    {
        EXPECT_EQ(IOTVTICAL_VALID_ACCESS, IsRequestWithinValidTime(periodStr, recurStr));
    }
    else
    {
        EXPECT_EQ(IOTVTICAL_INVALID_ACCESS, IsRequestWithinValidTime(periodStr, recurStr));
    }
}

TEST(IsRequestWithinValidTimeTest, IsRequestWithinValidTimeInValidPeriodAndValidRecur)
{
    //Daily forever on days MO, WE & Fr
    char recurStr[] = "FREQ=DAILY; BYDAY=MO, WE, FR";
    char periodStr[] = "20150630/20150730";

    EXPECT_EQ(IOTVTICAL_INVALID_ACCESS, IsRequestWithinValidTime(periodStr, recurStr));
}

#endif
