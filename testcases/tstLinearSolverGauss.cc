// SPDX-License-Identifier: LGPL-2.1-or-later
#include "gtest/gtest.h"
#include "tstLinearSolver.h"
#include "csim/internal/LinearSolver.h"

namespace csim
{
    TEST(tstLinearSolverGauss, equ_real_1)
    {
        LinearSolver *gauss = LinearSolver::createInstance("gauss");
        ASSERT_NE(nullptr, gauss);
        tst_equ_real_1(gauss);
        delete gauss;
    }

    TEST(tstLinearSolverGauss, equ_real_2)
    {
        LinearSolver *gauss = LinearSolver::createInstance("gauss");
        ASSERT_NE(nullptr, gauss);
        tst_equ_real_2(gauss);
        delete gauss;
    }

    TEST(tstLinearSolverGauss, equ_complex_1)
    {
        LinearSolver *gauss = LinearSolver::createInstance("gauss");
        ASSERT_NE(nullptr, gauss);
        tst_equ_complex_1(gauss);
        delete gauss;
    }

} // namespace
