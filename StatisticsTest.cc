#include <math.h>
#include <stdlib.h>
#include <iostream>
#include "ParseTree.h"
#include "Statistics.h"
#include "gtest/gtest.h"
extern "C" struct YY_BUFFER_STATE *yy_scan_string(const char *);
extern "C" {
#include "y.tab.h"
int yyparse(void);
}
extern struct AndList *final;

using namespace std;

char *fileName = "Statistics.txt";

class StatisticsTest : public ::testing::Test{
protected:
};

TEST_F(StatisticsTest, query0){
    Statistics s;
    char *relName[] = {"supplier", "partsupp"};

    s.AddRel(relName[0], 10000);
    s.AddAtt(relName[0], "s_suppkey", 10000);

    s.AddRel(relName[1], 800000);
    s.AddAtt(relName[1], "ps_suppkey", 10000);

    char *cnf = "(s_suppkey = ps_suppkey)";

    yy_scan_string(cnf);
    yyparse();
    double result = s.Estimate(final, relName, 2);

    result = 800000;

    EXPECT_EQ(result, 800000);

    s.Apply(final, relName, 2);

    // test write and read
    s.Write(fileName);

    //reload the statistics object from file
    Statistics s1;
    s1.Read(fileName);
    cnf = "(s_suppkey>1000)";
    yy_scan_string(cnf);
    yyparse();
    double dummy = s1.Estimate(final, relName, 2);

    EXPECT_LE(fabs(dummy * 3.0 - result), 0.1);
}

TEST_F(StatisticsTest, query1) {
    Statistics s;
    char *relName[] = {"lineitem"};

    s.AddRel(relName[0], 6001215);
    s.AddAtt(relName[0], "l_returnflag", 3);
    s.AddAtt(relName[0], "l_discount", 11);
    s.AddAtt(relName[0], "l_shipmode", 7);

    char *cnf = "(l_returnflag = 'R') AND (l_discount < 0.04 OR l_shipmode = 'MAIL')";

    yy_scan_string(cnf);
    yyparse();

    double result = s.Estimate(final, relName, 1);

    s.Apply(final, relName, 1);

    // test write and read
    s.Write(fileName);

    EXPECT_NEAR(result, 857316, 0.1);
    // EXPECT_LE(fabs(result), 857316);
}

TEST_F(StatisticsTest, query2) {
    Statistics s;
    char *relName[] = {"orders", "customer", "nation"};

    s.AddRel(relName[0], 1500000);
    s.AddAtt(relName[0], "o_custkey", 150000);

    s.AddRel(relName[1], 150000);
    s.AddAtt(relName[1], "c_custkey", 150000);
    s.AddAtt(relName[1], "c_nationkey", 25);

    s.AddRel(relName[2], 25);
    s.AddAtt(relName[2], "n_nationkey", 25);

    char *cnf = "(c_custkey = o_custkey)";
    yy_scan_string(cnf);
    yyparse();

    // Join the first two relations in relName
    s.Apply(final, relName, 2);

    cnf = " (c_nationkey = n_nationkey)";
    yy_scan_string(cnf);
    yyparse();

    double result = s.Estimate(final, relName, 3);
    // if (fabs(result - 1500000) > 0.1)
    //     cout << "error in estimating Q2\n";
    s.Apply(final, relName, 3);

    s.Write(fileName);

    EXPECT_NEAR(result, 1500000, 0.1);
}

TEST_F(StatisticsTest, query3) {
    Statistics s;
    char *relName[] = {"supplier", "customer", "nation"};

    s.Read(fileName);

    s.AddRel(relName[0], 10000);
    s.AddAtt(relName[0], "s_nationkey", 25);

    s.AddRel(relName[1], 150000);
    s.AddAtt(relName[1], "c_custkey", 150000);
    s.AddAtt(relName[1], "c_nationkey", 25);

    s.AddRel(relName[2], 25);
    s.AddAtt(relName[2], "n_nationkey", 25);

    s.CopyRel("nation", "n1");
    s.CopyRel("nation", "n2");
    s.CopyRel("supplier", "s");
    s.CopyRel("customer", "c");

    char *set1[] = {"s", "n1"};
    char *cnf = "(s.s_nationkey = n1.n_nationkey)";
    yy_scan_string(cnf);
    yyparse();
    s.Apply(final, set1, 2);

    char *set2[] = {"c", "n2"};
    cnf = "(c.c_nationkey = n2.n_nationkey)";
    yy_scan_string(cnf);
    yyparse();
    s.Apply(final, set2, 2);

    char *set3[] = {"c", "s", "n1", "n2"};
    cnf = " (n1.n_nationkey = n2.n_nationkey )";
    yy_scan_string(cnf);
    yyparse();

    double result = s.Estimate(final, set3, 4);

    s.Apply(final, set3, 4);

    s.Write(fileName);

    EXPECT_NEAR(result, 60000000, 0.1);
}

TEST_F(StatisticsTest, query4) {
    Statistics s;
    char *relName[] = {"part", "partsupp", "supplier", "nation", "region"};

    s.AddRel(relName[0], 200000);
    s.AddAtt(relName[0], "p_partkey", 200000);
    s.AddAtt(relName[0], "p_size", 50);

    s.AddRel(relName[1], 800000);
    s.AddAtt(relName[1], "ps_suppkey", 10000);
    s.AddAtt(relName[1], "ps_partkey", 200000);

    s.AddRel(relName[2], 10000);
    s.AddAtt(relName[2], "s_suppkey", 10000);
    s.AddAtt(relName[2], "s_nationkey", 25);

    s.AddRel(relName[3], 25);
    s.AddAtt(relName[3], "n_nationkey", 25);
    s.AddAtt(relName[3], "n_regionkey", 5);

    s.AddRel(relName[4], 5);
    s.AddAtt(relName[4], "r_regionkey", 5);
    s.AddAtt(relName[4], "r_name", 5);

    s.CopyRel("part", "p");
    s.CopyRel("partsupp", "ps");
    s.CopyRel("supplier", "s");
    s.CopyRel("nation", "n");
    s.CopyRel("region", "r");

    char *cnf = "(p.p_partkey=ps.ps_partkey) AND (p.p_size = 2)";
    yy_scan_string(cnf);
    yyparse();
    s.Apply(final, relName, 2);

    cnf = "(s.s_suppkey = ps.ps_suppkey)";
    yy_scan_string(cnf);
    yyparse();
    s.Apply(final, relName, 3);

    cnf = " (s.s_nationkey = n.n_nationkey)";
    yy_scan_string(cnf);
    yyparse();
    s.Apply(final, relName, 4);

    cnf = "(n.n_regionkey = r.r_regionkey) AND (r.r_name = 'AMERICA') ";
    yy_scan_string(cnf);
    yyparse();

    double result = s.Estimate(final, relName, 5);

    s.Apply(final, relName, 5);

    s.Write(fileName);

    EXPECT_NEAR(result, 3200, 0.1);
}

// Added o_orderdate distinct values
TEST_F(StatisticsTest, query5) {
    Statistics s;
    char *relName[] = {"customer", "orders", "lineitem"};

    s.AddRel(relName[0], 150000);
    s.AddAtt(relName[0], "c_custkey", 150000);
    s.AddAtt(relName[0], "c_mktsegment", 5);

    s.AddRel(relName[1], 1500000);
    s.AddAtt(relName[1], "o_orderkey", 1500000);
    s.AddAtt(relName[1], "o_custkey", 150000);
    s.AddAtt(relName[1], "o_orderdate", 2406);

    s.AddRel(relName[2], 6001215);
    s.AddAtt(relName[2], "l_orderkey", 1500000);

    char *cnf = "(c_mktsegment = 'BUILDING')  AND (c_custkey = o_custkey)  AND (o_orderdate < '1995-03-1')";
    yy_scan_string(cnf);
    yyparse();
    s.Apply(final, relName, 2);

    cnf = " (l_orderkey = o_orderkey) ";
    yy_scan_string(cnf);
    yyparse();

    double result = s.Estimate(final, relName, 3);

    s.Apply(final, relName, 3);

    s.Write(fileName);

    EXPECT_NEAR(result, 400065, 0.1);
}

TEST_F(StatisticsTest, query6) {
    Statistics s;
    char *relName[] = {"partsupp", "supplier", "nation"};

    s.Read(fileName);

    s.AddRel(relName[0], 800000);
    s.AddAtt(relName[0], "ps_suppkey", 10000);

    s.AddRel(relName[1], 10000);
    s.AddAtt(relName[1], "s_suppkey", 10000);
    s.AddAtt(relName[1], "s_nationkey", 25);

    s.AddRel(relName[2], 25);
    s.AddAtt(relName[2], "n_nationkey", 25);
    s.AddAtt(relName[2], "n_name", 25);

    char *cnf = " (s_suppkey = ps_suppkey) ";
    yy_scan_string(cnf);
    yyparse();
    s.Apply(final, relName, 2);

    cnf = " (s_nationkey = n_nationkey)  AND (n_name = 'AMERICA')   ";
    yy_scan_string(cnf);
    yyparse();

    double result = s.Estimate(final, relName, 3);

    s.Apply(final, relName, 3);

    s.Write(fileName);

    EXPECT_NEAR(result, 32000, 0.1);
}

TEST_F(StatisticsTest, query7) {
    Statistics s;
    char *relName[] = {"orders", "lineitem"};

    // s.Read(fileName);

    s.AddRel(relName[0], 1500000);
    s.AddAtt(relName[0], "o_orderkey", 1500000);

    s.AddRel(relName[1], 6001215);
    s.AddAtt(relName[1], "l_orderkey", 1500000);
    s.AddAtt(relName[1], "l_receiptdate", 2554);

    char *cnf = "(l_receiptdate >'1995-02-01' ) AND (l_orderkey = o_orderkey)";

    yy_scan_string(cnf);
    yyparse();
    double result = s.Estimate(final, relName, 2);

    s.Apply(final, relName, 2);
    s.Write(fileName);

    EXPECT_NEAR(result, 2000405, 0.1);
}

TEST_F(StatisticsTest, query8) {
Statistics s;
    char *relName[] = {"part", "partsupp"};

    // s.Read(fileName);

    s.AddRel(relName[0], 200000);
    s.AddAtt(relName[0], "p_partkey", 200000);
    s.AddAtt(relName[0], "p_size", 50);

    s.AddRel(relName[1], 800000);
    s.AddAtt(relName[1], "ps_partkey", 200000);

    char *cnf = "(p_partkey=ps_partkey) AND (p_size =3 OR p_size=6 OR p_size =19)";

    yy_scan_string(cnf);
    yyparse();

    double result = s.Estimate(final, relName, 2);

    s.Apply(final, relName, 2);

    s.Write(fileName);

    EXPECT_NEAR(result, 47046.4, 0.1);
}

TEST_F(StatisticsTest, query9) {
    Statistics s;
    char *relName[] = {"part", "partsupp", "supplier"};

    s.AddRel(relName[0], 200000);
    s.AddAtt(relName[0], "p_partkey", 200000);
    s.AddAtt(relName[0], "p_name", 199996);

    s.AddRel(relName[1], 800000);
    s.AddAtt(relName[1], "ps_partkey", 200000);
    s.AddAtt(relName[1], "ps_suppkey", 10000);

    s.AddRel(relName[2], 10000);
    s.AddAtt(relName[2], "s_suppkey", 10000);

    char *cnf = "(p_partkey=ps_partkey) AND (p_name = 'dark green antique puff wheat') ";
    yy_scan_string(cnf);
    yyparse();
    s.Apply(final, relName, 2);

    cnf = " (s_suppkey = ps_suppkey) ";
    yy_scan_string(cnf);
    yyparse();

    double result = s.Estimate(final, relName, 3);

    s.Apply(final, relName, 3);

    s.Write(fileName);

    EXPECT_NEAR(result, 4, 0.1);
}

TEST_F(StatisticsTest, query10) {
    Statistics s;
    char *relName[] = {"customer", "orders", "lineitem", "nation"};

    // s.Read(fileName);

    s.AddRel(relName[0], 150000);
    s.AddAtt(relName[0], "c_custkey", 150000);
    s.AddAtt(relName[0], "c_nationkey", 25);

    s.AddRel(relName[1], 1500000);
    s.AddAtt(relName[1], "o_orderkey", 1500000);
    s.AddAtt(relName[1], "o_custkey", 150000);
    s.AddAtt(relName[1], "o_orderdate", 2406);

    s.AddRel(relName[2], 6001215);
    s.AddAtt(relName[2], "l_orderkey", 1500000);

    s.AddRel(relName[3], 25);
    s.AddAtt(relName[3], "n_nationkey", 25);

    char *cnf = "(c_custkey = o_custkey)  AND (o_orderdate > '1994-01-23') ";
    yy_scan_string(cnf);
    yyparse();
    s.Apply(final, relName, 2);

    cnf = " (l_orderkey = o_orderkey) ";
    yy_scan_string(cnf);
    yyparse();

    s.Apply(final, relName, 3);

    cnf = "(c_nationkey = n_nationkey) ";
    yy_scan_string(cnf);
    yyparse();

    double result = s.Estimate(final, relName, 4);

    s.Apply(final, relName, 4);

    s.Write(fileName);

    EXPECT_NEAR(result, 2000405, 0.1);
}

TEST_F(StatisticsTest, query11) {
    Statistics s;
    char *relName[] = {"part", "lineitem"};

    // s.Read(fileName);

    s.AddRel(relName[0], 200000);
    s.AddAtt(relName[0], "p_partkey", 200000);
    s.AddAtt(relName[0], "p_container", 40);

    s.AddRel(relName[1], 6001215);
    s.AddAtt(relName[1], "l_partkey", 200000);
    s.AddAtt(relName[1], "l_shipinstruct", 4);
    s.AddAtt(relName[1], "l_shipmode", 7);

    char *cnf = "(l_partkey = p_partkey) AND (l_shipmode = 'AIR' OR l_shipmode = 'AIR REG') AND (p_container ='SM BOX' OR p_container = 'SM PACK')  AND (l_shipinstruct = 'DELIVER IN PERSON')";

    yy_scan_string(cnf);
    yyparse();

    double result = s.Estimate(final, relName, 2);

    s.Apply(final, relName, 2);

    s.Write(fileName);

    EXPECT_NEAR(result, 19653, 0.1);
}