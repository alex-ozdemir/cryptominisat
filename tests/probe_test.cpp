/*
 * CryptoMiniSat
 *
 * Copyright (c) 2009-2015, Mate Soos. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation
 * version 2.0 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301  USA
*/

#include "gtest/gtest.h"

#include <set>
using std::set;

#include "src/solver.h"
#include "src/prober.h"
#include "src/solverconf.h"
using namespace CMSat;
#include "test_helper.h"

struct probe : public ::testing::Test {
    probe()
    {
        must_inter = false;
        SolverConf conf;
        //conf.verbosity = 20;
        s = new Solver(&conf, &must_inter);
        s->new_vars(30);
        p = s->prober;
    }
    ~probe()
    {
        delete s;
    }

    Solver* s;
    Prober* p;
    std::vector<uint32_t> vars;
    bool must_inter;
};

//Regular, 1UIP fails

TEST_F(probe, uip_fail_1)
{
    //s->conf.verbosity = 20;
    s->add_clause_outer(str_to_cl(" 1,  2"));
    s->add_clause_outer(str_to_cl("-2,  3"));
    s->add_clause_outer(str_to_cl("-2, -3"));

    s->conf.doBothProp = false;
    s->conf.doStamp = false;
    s->conf.otfHyperbin = false;
    vars = tovars("1");
    p->probe(&vars);

    //1UIP is -2
    check_zero_assigned_lits_contains(s, "-2");
}

TEST_F(probe, uip_fail_2)
{
    //s->conf.verbosity = 20;
    s->add_clause_outer(str_to_cl("1, -2"));
    s->add_clause_outer(str_to_cl("1, -3"));
    s->add_clause_outer(str_to_cl("1, -4"));
    s->add_clause_outer(str_to_cl("1, -5"));
    s->add_clause_outer(str_to_cl("2, 3, 4, 5, 6"));
    s->add_clause_outer(str_to_cl("2, 3, 4, 5, -6"));

    s->conf.doBothProp = false;
    s->conf.doStamp = false;
    s->conf.otfHyperbin = false;
    vars = tovars("1");
    p->probe(&vars);

    //First UIP is 1
    check_zero_assigned_lits_eq(s, "1");
}

//BFS, DFS fails

TEST_F(probe, fail_dfs)
{
    //s->conf.verbosity = 20;
    s->add_clause_outer(str_to_cl("1, -2"));
    s->add_clause_outer(str_to_cl("1, -3"));
    s->add_clause_outer(str_to_cl("1, -4"));
    s->add_clause_outer(str_to_cl("1, -5"));
    s->add_clause_outer(str_to_cl("2, 3, 4, 5, 6"));
    s->add_clause_outer(str_to_cl("2, 3, 4, 5, -6"));

    s->conf.doBothProp = false;
    s->conf.doStamp = false;
    s->conf.otfHyperbin = true;
    p->force_stamp = 1;
    vars = tovars("1");
    p->probe(&vars);

    //deepest common ancestor
    check_zero_assigned_lits_eq(s, "1");
}

TEST_F(probe, fail_bfs)
{
    //s->conf.verbosity = 10;
    s->add_clause_outer(str_to_cl("1, -2"));
    s->add_clause_outer(str_to_cl("1, -3"));
    s->add_clause_outer(str_to_cl("1, -4"));
    s->add_clause_outer(str_to_cl("1, -5"));
    s->add_clause_outer(str_to_cl("2, 3, 4, 5, 6"));
    s->add_clause_outer(str_to_cl("2, 3, 4, 5, -6"));

    s->conf.doBothProp = false;
    s->conf.doStamp = false;
    s->conf.otfHyperbin = true;
    p->force_stamp = 0;
    vars = tovars("1");
    p->probe(&vars);

    //deepest common ancestor
    check_zero_assigned_lits_eq(s, "1");
}

//Stamp correctness

TEST_F(probe, stamp)
{
    //s->conf.verbosity = 20;
    s->add_clause_outer(str_to_cl("1, -2"));

    s->conf.doBothProp = false;
    s->conf.doStamp = true;
    s->conf.otfHyperbin = true;
    p->force_stamp = 2;
    vars = tovars("1");
    p->probe(&vars);

    check_stamp_contains(s, "1, -2", STAMP_IRRED);
}

TEST_F(probe, stamp_2)
{
    //s->conf.verbosity = 20;
    s->add_clause_outer(str_to_cl("1, -2"));
    s->add_clause_outer(str_to_cl("2, 3"));

    s->conf.doBothProp = false;
    s->conf.doStamp = true;
    s->conf.otfHyperbin = true;
    p->force_stamp = 2;
    vars = tovars("1");
    p->probe(&vars);

    check_stamp_contains(s, "1, 3", STAMP_IRRED);
}

TEST_F(probe, stamp_3)
{
    //s->conf.verbosity = 20;
    s->add_clause_outer(str_to_cl("1, -2"));
    s->add_clause_outer(str_to_cl("2, 3"));

    s->conf.doBothProp = false;
    s->conf.doStamp = true;
    s->conf.otfHyperbin = true;
    p->force_stamp = 1;
    vars = tovars("1");
    p->probe(&vars);

    check_stamp_contains(s, "1, 3", STAMP_RED);
}


TEST_F(probe, stamp_4)
{
    //s->conf.verbosity = 20;
    s->add_clause_outer(str_to_cl("1, -2"));
    s->add_clause_outer(str_to_cl("2, 3"));
    s->add_clause_outer(str_to_cl("2, 4"));

    s->add_clause_outer(str_to_cl("-3, 5"));
    s->add_clause_outer(str_to_cl("-4, 6"));

    s->add_clause_outer(str_to_cl("-5, 7"));
    s->add_clause_outer(str_to_cl("-6, 7"));

    s->conf.doBothProp = false;
    s->conf.doStamp = true;
    s->conf.otfHyperbin = true;
    p->force_stamp = 2;
    vars = tovars("1");
    p->probe(&vars);

    check_stamp_contains(s, "1, 7", STAMP_IRRED);
    check_stamp_contains(s, "2, 7", STAMP_IRRED);
}

//transitive reduction

TEST_F(probe, trans_red1)
{
    s->add_clause_outer(str_to_cl("1, 2"));
    s->add_clause_outer(str_to_cl("-2, 3"));
    s->add_clause_outer(str_to_cl("-3, 4"));
    s->add_clause_outer(str_to_cl("1, 4"));

    vars = tovars("1");
    s->conf.doBothProp = false;
    s->conf.doStamp = false;
    s->conf.otfHyperbin = true;
    p->probe(&vars);
    check_irred_cls_doesnt_contain(s, "1, 4");
}

TEST_F(probe, trans_red2)
{
    s->add_clause_outer(str_to_cl("1, 2"));
    s->add_clause_outer(str_to_cl("-2, 3"));
    s->add_clause_outer(str_to_cl("-3, 4"));
    s->add_clause_outer(str_to_cl("-4, 5"));
    s->add_clause_outer(str_to_cl("-3, 5"));
    s->add_clause_outer(str_to_cl("1, 5"));

    vars = tovars("1");
    s->conf.doBothProp = false;
    s->conf.doStamp = false;
    s->conf.otfHyperbin = true;
    p->probe(&vars);
    check_irred_cls_doesnt_contain(s, "-3, 5");
    check_irred_cls_doesnt_contain(s, "1, 5");
}

//Hyper-binary resolution

TEST_F(probe, hyper_bin)
{
    s->add_clause_outer(str_to_cl("1, 2"));
    s->add_clause_outer(str_to_cl("1, 3"));
    s->add_clause_outer(str_to_cl("-2, -3, 4"));

    vars = tovars("1");
    s->conf.doBothProp = false;
    s->conf.doStamp = false;
    s->conf.otfHyperbin = true;
    p->probe(&vars);
    check_red_cls_contains(s, "1, 4");
}

TEST_F(probe, hyper_bin2)
{
    s->add_clause_outer(str_to_cl("1, 2"));
    s->add_clause_outer(str_to_cl("1, 3"));
    s->add_clause_outer(str_to_cl("1, -4"));
    s->add_clause_outer(str_to_cl("-2, -3, 4, 5"));

    vars = tovars("1");
    s->conf.doBothProp = false;
    s->conf.doStamp = false;
    s->conf.otfHyperbin = true;
    p->probe(&vars);
    check_red_cls_contains(s, "1, 5");
}

TEST_F(probe, hyper_bin3)
{
    s->add_clause_outer(str_to_cl("1, 2"));
    s->add_clause_outer(str_to_cl("1, 3"));
    s->add_clause_outer(str_to_cl("1, -4"));
    s->add_clause_outer(str_to_cl("-2, -3, 4, 5"));
    s->add_clause_outer(str_to_cl("-5, 6"));
    s->add_clause_outer(str_to_cl("-5, 7"));
    s->add_clause_outer(str_to_cl("-5, 8"));
    s->add_clause_outer(str_to_cl("-6, -7, -8, 9"));

    vars = tovars("1");
    s->conf.doBothProp = false;
    s->conf.doStamp = false;
    s->conf.otfHyperbin = true;
    p->probe(&vars);
    check_red_cls_contains(s, "1, 5");
    check_red_cls_contains(s, "-5, 9");
}


//hyper-binary resolution and transitive reduction

TEST_F(probe, hyper_bin_and_trans_red1)
{
    s->add_clause_outer(str_to_cl("1, 2"));
    s->add_clause_outer(str_to_cl("1, 3"));
    s->add_clause_outer(str_to_cl("1, -4"));
    s->add_clause_outer(str_to_cl("-2, -3, 4, 5"));
    s->add_clause_outer(str_to_cl("-5, 6"));
    s->add_clause_outer(str_to_cl("1, 6"), true);
    s->add_clause_outer(str_to_cl("-5, 7"));
    s->add_clause_outer(str_to_cl("-5, 8"));
    s->add_clause_outer(str_to_cl("-6, -7, -8, 9"));

    vars = tovars("1");
    s->conf.doBothProp = false;
    s->conf.doStamp = false;
    s->conf.otfHyperbin = true;
    p->probe(&vars);
    check_red_cls_contains(s, "1, 5");
    check_red_cls_contains(s, "-5, 9");
    check_red_cls_doesnt_contain(s, "1, 6");
}


//Implication cache

TEST_F(probe, imp_cache)
{
    s->add_clause_outer(str_to_cl("1, 2"));
    s->add_clause_outer(str_to_cl("-2, 3"));

    vars = tovars("1, 2, 3");
    p->probe(&vars);
    check_impl_cache_contains(s, "1, 3");
}

TEST_F(probe, imp_cache_2)
{
    s->add_clause_outer(str_to_cl("1, 2"));
    s->add_clause_outer(str_to_cl("-2, 3"));

    vars = tovars("3, 2, 1");
    p->probe(&vars);
    check_impl_cache_contains(s, "3, 1");
}

TEST_F(probe, imp_cache_longer)
{
    s->add_clause_outer(str_to_cl("1, 2"));
    s->add_clause_outer(str_to_cl("-2, 3"));
    s->add_clause_outer(str_to_cl("-3, 4"));
    s->add_clause_outer(str_to_cl("-4, 5"));

    vars = tovars("5, 4, 3, 2, 1");
    p->probe(&vars);
    check_impl_cache_contains(s, "5, 1");
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
