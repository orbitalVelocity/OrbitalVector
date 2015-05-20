/*****************************************************************************
 *   Copyright (C) 2004-2015 The PaGMO development team,                     *
 *   Advanced Concepts Team (ACT), European Space Agency (ESA)               *
 *                                                                           *
 *   https://github.com/esa/pagmo                                            *
 *                                                                           *
 *   act@esa.int                                                             *
 *                                                                           *
 *   This program is free software; you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Public License as published by    *
 *   the Free Software Foundation; either version 2 of the License, or       *
 *   (at your option) any later version.                                     *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *   GNU General Public License for more details.                            *
 *                                                                           *
 *   You should have received a copy of the GNU General Public License       *
 *   along with this program; if not, write to the                           *
 *   Free Software Foundation, Inc.,                                         *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.               *
 *****************************************************************************/

// 17/05/2008: Initial version by Dario Izzo.

#ifndef LAMBERT_H
#define LAMBERT_H

//#include "../config.h"
// We solve the library madness in windows
#ifdef _WIN32
#ifdef PAGMO_DLL_EXPORT_API
#define __PAGMO_VISIBLE __declspec(dllexport)
#elif defined ( PAGMO_DLL_IMPORT_API )
#define __PAGMO_VISIBLE __declspec(dllimport)
#else
#define __PAGMO_VISIBLE
#endif
#define __PAGMO_VISIBLE_FUNC __PAGMO_VISIBLE
#else
#define __PAGMO_VISIBLE __attribute__ ((visibility("default")))
#define __PAGMO_VISIBLE_FUNC
#endif

/// Root PaGMO namespace.
namespace pagmo {}
void __PAGMO_VISIBLE_FUNC LambertI (const double*, const double*, double, const double &, const int &,  //INPUT
			   double*, double*, double&, double&, double& , int&);//OUTPUT

#endif
