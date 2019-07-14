# Bug In the CMS API?

This directory may contain a testcase which may demonstrate a bug in the CMS
API.

The testcase consists of two files:

   * `cms.cpp`: A program which uses CMS's API to produce DRAT proofs for UNSAT
     formulas
   * `cnf`: An UNSAT CNF.

## Seeing the Unusual Behavior

First, build CMS in a directory named `REPO_ROOT/build`. Then change into
`REPO_ROOT/bug`.

Run `make check-bin` to make & check a DRAT proof using the CMS binary.
This proof should be accepted.

Run `make check-api` to make & check a DRAT proof using the CMS API and
the `cms` program. This proof is rejected!

## Three Potential Explanations

There are three putative explanations:

   1. The `cms.cpp` program is misusing the CMS API.
   2. The CMS API has a bug, causing the produced DRAT proof to be
      unacceptable. Flushing?
   3. The DRAT proof *is* acceptable, and `drat-trim` has a bug.
