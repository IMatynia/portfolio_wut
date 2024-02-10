#include "../src/Tuple.h"
#include "../src/TuplePattern.h"
#include <gtest/gtest.h>

TEST(TupleTests, SimpleTupleParsing) {
    TuplePattern pattern(
        "integer:<=5, float:>1.2, string:\"avocado\", integer:*");
    std::string patternstring = pattern.toString();
    auto fields = pattern.getFields();
    ASSERT_EQ(fields.size(), 4);
}

TEST(TupleTests, FieldValues) {
    TuplePattern pattern("integer:10, float:>1.5, string:<=\"hello_world!\"");
    auto fields = pattern.getFields();
    ASSERT_EQ(fields[0].op, ComparisonOperator::EQUAL);
    ASSERT_EQ(fields[1].op, ComparisonOperator::GREATER_THAN);
    ASSERT_EQ(fields[2].op, ComparisonOperator::LESS_THAN_OR_EQUAL);
    ASSERT_EQ(std::get<std::int64_t>(fields[0].data), 10);
    ASSERT_FLOAT_EQ(std::get<float>(fields[1].data), 1.5f);
    ASSERT_EQ(std::get<std::string>(fields[2].data), "hello_world!");
}

TEST(TupleTests, StarOperator) {
    TuplePattern pattern("integer:*, float:*, string:*");
    auto fields = pattern.getFields();
    ASSERT_EQ(fields[0].op, ComparisonOperator::ANY);
    ASSERT_EQ(fields[1].op, ComparisonOperator::ANY);
    ASSERT_EQ(fields[2].op, ComparisonOperator::ANY);
    ASSERT_EQ(std::get<std::int64_t>(fields[0].data), 0);
    ASSERT_EQ(std::get<float>(fields[1].data), 0.0f);
    ASSERT_EQ(std::get<std::string>(fields[2].data), "");
}

TEST(TupleTests, SameFields) {
    TuplePattern pattern("integer:*, integer:*, float:>1.5, float:>1.5");
    auto fields = pattern.getFields();
    ASSERT_EQ(fields.size(), 4);
}

TEST(TupleTests, StringFormat) {
    TuplePattern pattern(
        "integer:<=5, float:>1.2, string:\"avocado\", integer:*");
    std::string patternString = pattern.toString();
    ASSERT_EQ(patternString,
              "integer:<=5,float:>1.200000,string:==avocado,integer:*");
}

TEST(TupleTests, ErrorChecking) {
    ASSERT_THROW(TuplePattern("abc"), std::invalid_argument);
    ASSERT_THROW(TuplePattern("abc:1.5"), std::invalid_argument);
    ASSERT_THROW(TuplePattern("float:1.5"), std::invalid_argument);
    ASSERT_THROW(TuplePattern("float:==1.5"), std::invalid_argument);
    ASSERT_THROW(TuplePattern("float:>\"bcd\""), std::invalid_argument);
    ASSERT_THROW(TuplePattern("integer:ww5.1wwadw"), std::invalid_argument);
    ASSERT_THROW(TuplePattern("integer:5, string:5"), std::invalid_argument);
    ASSERT_THROW(TuplePattern("string:abc"), std::invalid_argument);
    ASSERT_THROW(TuplePattern("string:\"abc"), std::invalid_argument);
    ASSERT_THROW(TuplePattern("string:abc\""), std::invalid_argument);
    ASSERT_THROW(TuplePattern("integer"), std::invalid_argument);
    ASSERT_THROW(TuplePattern("integer:"), std::invalid_argument);
    ASSERT_THROW(TuplePattern("float:"), std::invalid_argument);
    ASSERT_THROW(TuplePattern("string:"), std::invalid_argument);
}

TEST(TupleTests, TupleInit) {
    Tuple tuple("5, 0.123, \"hello_there!\"");
    auto fields = tuple.getFields();
    ASSERT_EQ(fields.size(), 3);
    ASSERT_EQ(std::get<std::int64_t>(fields[0]), 5);
    ASSERT_FLOAT_EQ(std::get<float>(fields[1]), 0.123f);
    ASSERT_EQ(std::get<std::string>(fields[2]), "hello_there!");
}

TEST(TupleTests, TupleComparisonSimple) {
    Tuple tuple("5, 0.123, \"hello_there!\"");
    Tuple tuple_wrong1("1, 0.123, \"hello_there!\"");
    Tuple tuple_wrong2("10, 10.123, \"hello_there!\"");
    Tuple tuple_wrong3("11, 0.123, \"hello_there :)!\"");
    TuplePattern pattern("integer:>2, float:<2.124, string:\"hello_there!\"");
    TuplePattern patternStar1(
        "integer:*, float:<2.124, string:\"hello_there!\"");
    TuplePattern patternStar2("integer:>2, float:*, string:\"hello_there!\"");
    TuplePattern patternStar3("integer:>2, float:<2.124, string:*");
    TuplePattern patternUniversal("integer:*, float:*, string:*");

    ASSERT_TRUE(pattern.compareSlow(tuple));
    ASSERT_FALSE(pattern.compareSlow(tuple_wrong1));
    ASSERT_FALSE(pattern.compareSlow(tuple_wrong2));
    ASSERT_FALSE(pattern.compareSlow(tuple_wrong3));
    ASSERT_TRUE(patternStar1.compareSlow(tuple));
    ASSERT_TRUE(patternStar2.compareSlow(tuple));
    ASSERT_TRUE(patternStar3.compareSlow(tuple));

    ASSERT_TRUE(patternStar1.compareSlow(tuple_wrong1));
    ASSERT_TRUE(patternStar2.compareSlow(tuple_wrong2));
    ASSERT_TRUE(patternStar3.compareSlow(tuple_wrong3));

    ASSERT_FALSE(patternStar2.compareSlow(tuple_wrong1));
    ASSERT_FALSE(patternStar3.compareSlow(tuple_wrong2));
    ASSERT_FALSE(patternStar1.compareSlow(tuple_wrong3));

    ASSERT_TRUE(patternUniversal.compareSlow(tuple_wrong1));
    ASSERT_TRUE(patternUniversal.compareSlow(tuple_wrong2));
    ASSERT_TRUE(patternUniversal.compareSlow(tuple_wrong3));
}

TEST(TupleTests, TupleComparisonEdgeCases) {
    Tuple tuple("5, 0.123, \"hello_there!\"");
    TuplePattern pattern1("integer:>2, float:<2.124");
    TuplePattern pattern2("float:>2, float:<2.124, string:\"hello_there!\"");

    ASSERT_FALSE(pattern1.compareSlow(tuple));
    ASSERT_FALSE(pattern2.compareSlow(tuple));
}

TEST(TupleTests, TupleEquals) {
    Tuple tuple("5, 0.123, \"hello_there!\", 100");
    Tuple tuple_yes("5, 0.123, \"hello_there!\", 100");
    Tuple tuple1("51, 0.123, \"hello_there!\", 100");
    Tuple tuple2("5, 0.13, \"hello_there!\", 100");
    Tuple tuple3("5, 0.123, \"hell21there!\", 100");
    Tuple tuple4("5, 0.123, \"hell21there!\"");
    Tuple tuple5("0.123, 5, \"hell21there!\", 100");

    ASSERT_EQ(tuple, tuple_yes);
    ASSERT_NE(tuple, tuple1);
    ASSERT_NE(tuple, tuple2);
    ASSERT_NE(tuple, tuple3);
    ASSERT_NE(tuple, tuple4);
    ASSERT_NE(tuple, tuple5);
}

TEST(TupleTests, DoubleSerializationTest) {
    Tuple tuple("5, 0.123, \"hello_there!\", 100");
    Tuple original = tuple;
    std::string buf;
    tuple.serialize(buf);
    Tuple tuple2 = Tuple::deserialize(buf);
    bool res = (tuple == tuple2);
    ASSERT_TRUE(res);
    tuple2.serialize(buf);
    tuple = Tuple::deserialize(buf);
    res = (tuple == original);
    ASSERT_TRUE(res);
}

TEST(TupleTests, TuplePatternEdgeCases) {
    Tuple tuple("0");
    TuplePattern pattern1("integer:0");
    TuplePattern pattern2("integer:==0");

    ASSERT_TRUE(pattern1.compareSlow(tuple));
    ASSERT_TRUE(pattern2.compareSlow(tuple));
}

TEST(TupleTests, BugNo1) {
    Tuple tuple("\"a string\"");
    TuplePattern pattern("integer:*");
    ASSERT_FALSE(pattern.compareSlow(tuple));
}
