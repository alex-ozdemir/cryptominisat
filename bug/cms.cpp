#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "cryptominisat5/cryptominisat.h"

using namespace std;

int main(int argc, char** argv)
{
  std::vector<std::string> args(argv, argv + argc);
  assert(args.size() == 3);
  const auto& dimacs = args[1];
  const auto& drat = args[2];

  ifstream dimacs_stream{dimacs};
  ofstream proof_stream{drat};

  CMSat::SATSolver solver{};
  solver.set_drat(&proof_stream, false);

  std::string p, cnf;
  uint64_t n_vars, n_clauses;
  dimacs_stream >> p >> cnf >> n_vars >> n_clauses;
  assert(p == "p");
  assert(cnf == "cnf");
  solver.new_vars(n_vars);

  for (size_t c_i = 0; c_i < n_clauses; c_i++)
  {
    std::vector<CMSat::Lit> clause;
    int64_t i;
    while (true)
    {
      dimacs_stream >> i;
      assert(dimacs_stream.good());
      if (i == 0)
      {
        break;
      }
      else
      {
        uint32_t var = static_cast<uint32_t>(abs(i));
        assert(var <= n_vars);
        CMSat::Lit l{var - 1, i < 0};
        clause.push_back(l);
      }
    }
    if (not solver.add_clause(clause)) {
        std::cerr << "Solver not OK" << std::endl;
    }
  }
  std::cerr << "Clauses added" << std::endl;

  CMSat::lbool res = solver.solve();

  assert(proof_stream.good());
  proof_stream.flush();
  proof_stream.close();

  solver.print_stats();
  std::cerr << "Res: " << res << std::endl;
}
