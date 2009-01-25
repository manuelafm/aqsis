// Aqsis
// Copyright (C) 1997 - 2007, Paul C. Gregory
//
// Contact: pgregory@aqsis.org
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
 * \brief Define RIBPARSE_SHARE macro for DLL symbol import/export on windows.
 *
 * \author Chris Foster  [chris42f (at) gmail (dot) com]
 */

#ifndef RIBPARSE_SHARE_H_INCLUDED
#define RIBPARSE_SHARE_H_INCLUDED

#include "aqsis_compiler.h"


#ifdef AQSIS_SYSTEM_WIN32
#	ifdef AQSIS_STATIC_LINK
#		define RIBPARSE_SHARE
#	else
#		ifdef RIBPARSE_EXPORTS
#			define RIBPARSE_SHARE __declspec(dllexport)
#		else
#			define RIBPARSE_SHARE __declspec(dllimport)
#		endif
#	endif
#else
#	define RIBPARSE_SHARE
#endif


#endif // RIBPARSE_SHARE_H_INCLUDED
