#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-ACH");
    BOOST_CHECK(GetBoolArg("-ACH"));
    BOOST_CHECK(GetBoolArg("-ACH", false));
    BOOST_CHECK(GetBoolArg("-ACH", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-ACHo"));
    BOOST_CHECK(!GetBoolArg("-ACHo", false));
    BOOST_CHECK(GetBoolArg("-ACHo", true));

    ResetArgs("-ACH=0");
    BOOST_CHECK(!GetBoolArg("-ACH"));
    BOOST_CHECK(!GetBoolArg("-ACH", false));
    BOOST_CHECK(!GetBoolArg("-ACH", true));

    ResetArgs("-ACH=1");
    BOOST_CHECK(GetBoolArg("-ACH"));
    BOOST_CHECK(GetBoolArg("-ACH", false));
    BOOST_CHECK(GetBoolArg("-ACH", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noACH");
    BOOST_CHECK(!GetBoolArg("-ACH"));
    BOOST_CHECK(!GetBoolArg("-ACH", false));
    BOOST_CHECK(!GetBoolArg("-ACH", true));

    ResetArgs("-noACH=1");
    BOOST_CHECK(!GetBoolArg("-ACH"));
    BOOST_CHECK(!GetBoolArg("-ACH", false));
    BOOST_CHECK(!GetBoolArg("-ACH", true));

    ResetArgs("-ACH -noACH");  // -ACH should win
    BOOST_CHECK(GetBoolArg("-ACH"));
    BOOST_CHECK(GetBoolArg("-ACH", false));
    BOOST_CHECK(GetBoolArg("-ACH", true));

    ResetArgs("-ACH=1 -noACH=1");  // -ACH should win
    BOOST_CHECK(GetBoolArg("-ACH"));
    BOOST_CHECK(GetBoolArg("-ACH", false));
    BOOST_CHECK(GetBoolArg("-ACH", true));

    ResetArgs("-ACH=0 -noACH=0");  // -ACH should win
    BOOST_CHECK(!GetBoolArg("-ACH"));
    BOOST_CHECK(!GetBoolArg("-ACH", false));
    BOOST_CHECK(!GetBoolArg("-ACH", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--ACH=1");
    BOOST_CHECK(GetBoolArg("-ACH"));
    BOOST_CHECK(GetBoolArg("-ACH", false));
    BOOST_CHECK(GetBoolArg("-ACH", true));

    ResetArgs("--noACH=1");
    BOOST_CHECK(!GetBoolArg("-ACH"));
    BOOST_CHECK(!GetBoolArg("-ACH", false));
    BOOST_CHECK(!GetBoolArg("-ACH", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-ACH", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ACH", "eleven"), "eleven");

    ResetArgs("-ACH -bar");
    BOOST_CHECK_EQUAL(GetArg("-ACH", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ACH", "eleven"), "");

    ResetArgs("-ACH=");
    BOOST_CHECK_EQUAL(GetArg("-ACH", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ACH", "eleven"), "");

    ResetArgs("-ACH=11");
    BOOST_CHECK_EQUAL(GetArg("-ACH", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-ACH", "eleven"), "11");

    ResetArgs("-ACH=eleven");
    BOOST_CHECK_EQUAL(GetArg("-ACH", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-ACH", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-ACH", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-ACH", 0), 0);

    ResetArgs("-ACH -bar");
    BOOST_CHECK_EQUAL(GetArg("-ACH", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-ACH=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-ACH", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-ACH=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-ACH", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--ACH");
    BOOST_CHECK_EQUAL(GetBoolArg("-ACH"), true);

    ResetArgs("--ACH=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-ACH", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noACH");
    BOOST_CHECK(!GetBoolArg("-ACH"));
    BOOST_CHECK(!GetBoolArg("-ACH", true));
    BOOST_CHECK(!GetBoolArg("-ACH", false));

    ResetArgs("-noACH=1");
    BOOST_CHECK(!GetBoolArg("-ACH"));
    BOOST_CHECK(!GetBoolArg("-ACH", true));
    BOOST_CHECK(!GetBoolArg("-ACH", false));

    ResetArgs("-noACH=0");
    BOOST_CHECK(GetBoolArg("-ACH"));
    BOOST_CHECK(GetBoolArg("-ACH", true));
    BOOST_CHECK(GetBoolArg("-ACH", false));

    ResetArgs("-ACH --noACH");
    BOOST_CHECK(GetBoolArg("-ACH"));

    ResetArgs("-noACH -ACH"); // ACH always wins:
    BOOST_CHECK(GetBoolArg("-ACH"));
}

BOOST_AUTO_TEST_SUITE_END()
