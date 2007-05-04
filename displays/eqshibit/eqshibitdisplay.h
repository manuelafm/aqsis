// Aqsis
// Copyright � 1997 - 2001, Paul C. Gregory
//
// Contact: pgregory@aqsis.com
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
		\brief Implements the default display devices for Aqsis.
		\author Paul C. Gregory (pgregory@aqsis.com)
*/

#ifndef	___display_Loaded___
#define	___display_Loaded___

#include <aqsis.h>
#include <iostream>

#include "tinyxml.h"
#include "ndspy.h"

static const int INVALID_SOCKET = -1;

START_NAMESPACE( Aqsis )


enum EqDisplayTypes 
{
	Type_File = 0,
	Type_Framebuffer,
	Type_ZFile,
	Type_ZFramebuffer,
	Type_Shadowmap,
};


struct SqDisplayInstance
{
	SqDisplayInstance() :
		m_filename(0),
		m_hostname(0)
	{}
	char*		m_filename;
	char*		m_hostname;
	TqInt		m_hostport;
	int			m_socket;
	// The number of pixels that have already been rendered (used for progress reporting)
	TqInt		m_pixelsReceived;

	friend std::istream& operator >>(std::istream &is,struct SqDisplayInstance &obj);
	friend std::ostream& operator <<(std::ostream &os,const struct SqDisplayInstance &obj);
};

std::istream& operator >>(std::istream &is,struct SqDisplayInstance &obj)
{
//	      is>>strVal;
	            return is;
}

std::ostream& operator <<(std::ostream &os,const struct SqDisplayInstance &obj)
{
//	      os<<obj.strVal;
	            return os;
}

struct SqNetDisplayArgs
{
	int x,y,w,h,d;
};


//-----------------------------------------------------------------------

END_NAMESPACE( Aqsis )

#endif	//	___display_Loaded___
