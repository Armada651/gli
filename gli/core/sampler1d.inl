///////////////////////////////////////////////////////////////////////////////////
/// OpenGL Image (gli.g-truc.net)
///
/// Copyright (c) 2008 - 2015 G-Truc Creation (www.g-truc.net)
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
///
/// @ref core
/// @file gli/core/sampler1d.inl
/// @date 2015-10-21 / 2015-10-21
/// @author Christophe Riccio
///////////////////////////////////////////////////////////////////////////////////

#include "clear.hpp"
#include <glm/vector_relational.hpp>

namespace gli
{
	template <typename T, precision P>
	inline sampler1D<T, P>::sampler1D(texture1D const & Texture, gli::wrap Wrap, filter Mip, filter Min, texel_type const & BorderColor)
		: sampler(Wrap, Texture.levels() > 1 ? Mip : FILTER_NEAREST, Min)
		, Texture(Texture)
		, Convert(detail::convert<texture1D, T, P>::call(this->Texture.format()))
		, BorderColor(BorderColor)
		, Filter(Min == FILTER_LINEAR ? detail::filter1D<texture_type, samplecoord_type, fetch_type, texel_type>::linear : detail::filter1D<texture_type, samplecoord_type, fetch_type, texel_type>::nearest)
	{
		GLI_ASSERT(!Texture.empty());
		GLI_ASSERT(!is_compressed(Texture.format()));
	}

	template <typename T, precision P>
	inline texture1D const & sampler1D<T, P>::operator()() const
	{
		return this->Texture;
	}

	template <typename T, precision P>
	inline typename sampler1D<T, P>::texel_type sampler1D<T, P>::texel_fetch(texelcoord_type const & TexelCoord, size_type const & Level) const
	{
		GLI_ASSERT(this->Convert.Fetch);
		return this->Convert.Fetch(this->Texture, TexelCoord, 0, 0, Level);
	}

	template <typename T, precision P>
	inline void sampler1D<T, P>::texel_write(texelcoord_type const & TexelCoord, size_type const & Level, texel_type const & Texel)
	{
		GLI_ASSERT(this->Convert.Write);
		this->Convert.Write(this->Texture, TexelCoord, 0, 0, Level, Texel);
	}

	template <typename T, precision P>
	inline void sampler1D<T, P>::clear(texel_type const & Color)
	{
		detail::clear<texture_type, T, P>::call(this->Texture, this->Convert.Write, Color);
	}

	template <typename T, precision P>
	inline typename sampler1D<T, P>::texel_type sampler1D<T, P>::texture_lod(samplecoord_type const & SampleCoord, T Level) const
	{
		GLI_ASSERT(std::numeric_limits<T>::is_iec559);
		GLI_ASSERT(this->Convert.Fetch);

		samplecoord_type const SampleCoordWrap(this->Wrap(SampleCoord.x));

		if(this->Mip == FILTER_LINEAR)
		{
			texel_type const MinTexel = this->Filter(this->Texture, this->Convert.Fetch, SampleCoordWrap, size_type(0), size_type(0), size_type(floor(Level)), this->BorderColor);
			texel_type const MaxTexel = this->Filter(this->Texture, this->Convert.Fetch, SampleCoordWrap, size_type(0), size_type(0), size_type(ceil(Level)), this->BorderColor);
			return mix(MinTexel, MaxTexel, fract(Level));
		}
		else
		{
			return this->Filter(this->Texture, this->Convert.Fetch, SampleCoordWrap, size_type(0), size_type(0), size_type(round(Level)), this->BorderColor);
		}
	}

	template <typename T, precision P>
	inline void sampler1D<T, P>::generate_mipmaps()
	{
		GLI_ASSERT(!this->Texture.empty());
		GLI_ASSERT(!is_compressed(this->Texture.format()));

		this->generate_mipmaps(this->Texture.base_level(), this->Texture.max_level());
	}

	template <typename T, precision P>
	inline void sampler1D<T, P>::generate_mipmaps(size_type BaseLevel, size_type MaxLevel)
	{
		GLI_ASSERT(!this->Texture.empty());
		GLI_ASSERT(!is_compressed(this->Texture.format()));
		GLI_ASSERT(this->Texture.max_level() >= MaxLevel);
		GLI_ASSERT(BaseLevel <= MaxLevel);

		for(size_type Level = BaseLevel; Level < MaxLevel; ++Level)
		{
			samplecoord_type const SampleCoordScale(static_cast<T>(1) / samplecoord_type(this->Texture.dimensions(Level + 0)));
			texelcoord_type const DimDst = this->Texture.dimensions(Level + 1);

			for(typename texelcoord_type::value_type i = 0; i < DimDst.x; ++i)
			{
				typename texelcoord_type::value_type const x = (i << 1);

				texel_type const Texel0 = this->texture_lod(samplecoord_type(static_cast<T>(x + 0)) * SampleCoordScale, static_cast<T>(Level));
				texel_type const Texel1 = this->texture_lod(samplecoord_type(static_cast<T>(x + 1)) * SampleCoordScale, static_cast<T>(Level));

				texel_type const Texel = (Texel0 + Texel1) * static_cast<T>(0.5);
				this->texel_write(texelcoord_type(i), Level + 1, Texel);
			}
		}
	}
}//namespace gli

