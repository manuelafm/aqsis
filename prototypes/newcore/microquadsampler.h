// Copyright (C) 2001, Paul C. Gregory and the other authors and contributors
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the software's owners nor the names of its
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// (This is the New BSD license)

#ifndef MICROQUADSAMPLER_H_INCLUDED
#define MICROQUADSAMPLER_H_INCLUDED

#include "attributes.h"
#include "invbilin.h"
#include "sample.h"
#include "util.h"
#include "pointinquad.h"


/// Quadrilateral micropolygon sampler
///
/// This is designed to be constructed just before sampling time; it's not
/// memory efficient, and should not be a long-lived data structure.
class MicroQuadSampler
{
    private:
        /// Information about output variables.
        struct OutVarInfo
        {
            ConstFvecView src;  // Source gridvar data
            int outIdx; // start index of variable in output data

            OutVarInfo(const ConstFvecView& src, int outIdx)
                : src(src), outIdx(outIdx) {}
        };
        std::vector<OutVarInfo> m_outVarInfo;

        // Grid being sampled.
        const QuadGrid& m_grid;
        // Iterator for the current micropoly
        QuadGrid::Iterator m_curr;

        // Grid indices for vertices
        MicroQuadInd m_ind;
        // Point-in-polygon tests
        PointInQuad m_hitTest;
        // Grid storage.
        const GridStorage& m_storage;
        // Storage for the micropoly data
        ConstDataView<Vec3> m_P;

        // Shading interpolation
        InvBilin m_invBilin;
        // uv coordinates of current interpolation point
        Vec2 m_uv;
        // Whether to use smooth shading or not.
        bool m_smoothShading;

    public:
        // Cyclic vertex order:
        // a -- b
        // |    |
        // d -- c
        MicroQuadSampler(const QuadGrid& grid, const Attributes& attrs,
                         const OutvarSet& outVars)
            : m_grid(grid),
            m_curr(grid.begin()),
            m_ind(*m_curr),
            m_hitTest(),
            m_storage(grid.storage()),
            m_P(m_storage.P()),
            m_uv(0.0f),
            m_smoothShading(attrs.smoothShading)
        {
            // Cache the variables which need to be interpolated into
            // fragment outputs.
            const VarSet& gridVars = m_storage.varSet();
            for(int j = 0, jend = outVars.size(); j < jend; ++j)
            {
                // Simplistic linear search through grid variables for now.
                // This only happens once per grid, so maybe it's not too bad?
                for(int i = 0, iend = gridVars.size(); i < iend; ++i)
                {
                    if(gridVars[i] == outVars[j])
                    {
                        m_outVarInfo.push_back(OutVarInfo(
                                m_storage.get(i), outVars[j].offset) );
                        break;
                    }
                }
            }
        }

        /// Advance to next micropolygon.
        void next()
        {
            ++m_curr;
            m_ind = *m_curr;
        }
        /// Test whether we're referencing a valid micropoly
        bool valid()
        {
            return m_curr.valid();
        }

        /// Get bound for current micropoly
        Box bound() const
        {
            Box bnd(m_P[m_ind.a]);
            bnd.extendBy(m_P[m_ind.b]);
            bnd.extendBy(m_P[m_ind.c]);
            bnd.extendBy(m_P[m_ind.d]);
            return bnd;
        }

        /// Initialize the polygon hit tester
        inline void initHitTest()
        {
            m_hitTest.init(vec2_cast(m_P[m_ind.a]), vec2_cast(m_P[m_ind.b]),
                           vec2_cast(m_P[m_ind.c]), vec2_cast(m_P[m_ind.d]),
                           (m_curr.u() + m_curr.v()) % 2);
        }
        /// Return true if the sample is contained in the polygon
        inline bool contains(const Sample& samp)
        {
            return m_hitTest(samp);
        }

        /// Initialize the shading interpolator
        inline void initInterpolator()
        {
            if(m_smoothShading)
            {
                m_invBilin.init(
                    vec2_cast(m_P[m_ind.a]), vec2_cast(m_P[m_ind.b]),
                    vec2_cast(m_P[m_ind.d]), vec2_cast(m_P[m_ind.c]) );
            }
        }
        /// Specify interpolation point
        inline void interpolateAt(const Sample& samp)
        {
            if(m_smoothShading)
                m_uv = m_invBilin(samp.p);
        }

        /// Interpolate depth value at current point.
        inline float interpolateZ() const
        {
            if(m_smoothShading)
                return bilerp(m_P[m_ind.a].z, m_P[m_ind.b].z,
                              m_P[m_ind.d].z, m_P[m_ind.c].z, m_uv);
            else
                return m_P[m_ind.a].z;
        }

        /// Interpolate the colour
        inline void interpolateColor(float* col) const
        {
            int CsIdx = m_storage.varSet().find(StdIndices::Cs);
            assert(CsIdx >= 0);
            ConstFvecView Cs = m_storage.get(CsIdx);
            if(m_smoothShading)
            {
                for(int i = 0; i < 3; ++i)
                    col[i] = bilerp(Cs[m_ind.a][i], Cs[m_ind.b][i],
                                    Cs[m_ind.d][i], Cs[m_ind.c][i], m_uv);
            }
            else
            {
                for(int i = 0; i < 3; ++i)
                    col[i] = Cs[m_ind.a][i];
            }
        }

        /// Compute any output variables at the current hit.
        inline void interpolate(float* samples) const
        {
            // Note: The existence of this loop has a measurable impact on
            // performance; perhaps 10% slower than sampling colour directly
            // (measured on a core2 machine)
            //
            // Such overhead could _possibly_ be eliminated by packing the
            // gridvar storage together, but that potentially introduces
            // overhead elsewhere.
            for(int j = 0, jend = m_outVarInfo.size(); j < jend; ++j)
            {
                // Smooth shading is just bilinear interpolation across the
                // face of the micropolygon.  It's written out in full here to
                // make sure the coefficients are only computed once (maybe
                // the optimizer would do this for us if we used the bilerp()
                // function instead?)
                ConstFvecView in = m_outVarInfo[j].src;
                if(m_smoothShading)
                {
                    const float* in0 = in[m_ind.a];
                    const float* in1 = in[m_ind.b];
                    const float* in2 = in[m_ind.d];
                    const float* in3 = in[m_ind.c];
                    float w0 = (1-m_uv.y)*(1-m_uv.x);
                    float w1 = (1-m_uv.y)*m_uv.x;
                    float w2 = m_uv.y*(1-m_uv.x);
                    float w3 = m_uv.y*m_uv.x;
                    float* out = &samples[m_outVarInfo[j].outIdx];
                    for(int i = 0, size = in.elSize(); i < size; ++i)
                        out[i] = w0*in0[i] + w1*in1[i] + w2*in2[i] + w3*in3[i];
                }
                else
                {
                    const float* in0 = in[m_ind.a];
                    float* out = &samples[m_outVarInfo[j].outIdx];
                    for(int i = 0, size = in.elSize(); i < size; ++i)
                        out[i] = in0[i];
                }
            }
        }
};


#endif // MICROQUADSAMPLER_H_INCLUDED
