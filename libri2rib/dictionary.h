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
 *  \brief This dictionary can handle inline declaration as described in RiSpec V3.2
 *  \author Lionel J. Lacour (intuition01@online.fr)
 */

#ifndef RI2RIB_DICTIONARY_H
#define RI2RIB_DICTIONARY_H 1

#include <vector>
#include <string>
#include "aqsis.h"
#include "ri.h"

namespace libri2rib {

enum EqTokenClass { CONSTANT, UNIFORM, VARYING, VERTEX };
enum EqTokenType { FLOAT, POINT, VECTOR, NORMAL, COLOR, STRING, MATRIX, HPOINT, INTEGER };
typedef unsigned int TqTokenId;

struct SqTokenEntry
{
    SqTokenEntry (string n, EqTokenClass tc, EqTokenType tt, TqUint qt, TqBool inln)
	: name(n), tclass(tc), ttype(tt), in_line(inln), quantity(qt) {}
    ~SqTokenEntry() {}

    string       name;
    EqTokenClass tclass;
    EqTokenType  ttype;
    TqBool       in_line;
    TqUint       quantity;

#ifdef DEBUG
    void  printClassType ();
#endif
};


class CqDictionary
{
private:
    std::vector<SqTokenEntry> te;
    TqUint getTypeSize (EqTokenType);
    TqUint getQuantity (TqTokenId);
    void  isValid      (TqTokenId);

public:
    CqDictionary();
    ~CqDictionary() {}

    TqTokenId   addToken   (string n, EqTokenClass tc, EqTokenType tt, TqUint qnt=1, TqBool inln=false);
    TqTokenId   getTokenId (string n);
    TqUint      allocSize  (TqTokenId id, TqUint vertex, TqUint varying, TqUint uniform);
    EqTokenType getType    (TqTokenId id);

#ifdef DEBUG
    void  stats ();
#endif
};


} /* namespace libri2rib */
#endif
