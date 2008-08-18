// -*- mode: c++; indent-tabs-mode: t; tab-width: 4; c-basic-offset: 4; -*-
/*
 * Copyright (c) 2008, AIST, the University of Tokyo and General Robotix Inc.
 * All rights reserved. This program is made available under the terms of the
 * Eclipse Public License v1.0 which accompanies this distribution, and is
 * available at http://www.eclipse.org/legal/epl-v10.html
 * Contributors:
 * National Institute of Advanced Industrial Science and Technology (AIST)
 * General Robotix Inc. 
 */
#ifndef OPENHRP_MATRIX_SOLVERS_H_INCLUDED
#define OPENHRP_MATRIX_SOLVERS_H_INCLUDED

#include "ublasCommonTypes.h"
#include "hrpModelExportDef.h"


namespace hrp {

	/**
	   b = a * x, x = b^(-1) * a
	*/
	HRPMODEL_EXPORT int solveLinearEquation(const dmatrix &_a, const dvector &_b, dvector &_x, double _sv_ratio=1.0e-3);
	
	HRPMODEL_EXPORT int solveLinearEquationLU(dmatrix a, const dmatrix &b, dmatrix &out_x);
	HRPMODEL_EXPORT int solveLinearEquationLU(const dmatrix &_a, const dvector &_b, dvector &_x);
	HRPMODEL_EXPORT int solveLinearEquationSVD(const dmatrix &_a, const dvector &_b, dvector &_x, double _sv_ratio=1.0e-3);
	
	HRPMODEL_EXPORT int calcPseudoInverse(const dmatrix &_a, dmatrix &_a_pseu, double _sv_ratio=1.0e-3);

	int calcEigenVectors(const dmatrix &_a, dmatrix  &_evec, dvector &_eval);
	double det(const dmatrix &a);

	/**
	   \todo define dgesv wrapper and use it directly to improve the performance
	*/
	inline dmatrix inverse(const dmatrix& M)
	{
		const int dim = M.size1();
		dmatrix E = ublas::identity_matrix<double>(dim);
		dmatrix Minv(dim, dim);
		solveLinearEquationLU(M, E, Minv);
		return Minv;
	}

};

#endif