//////////////////////////////////////////////////////////////////////////////////
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
/// @file gli/core/texture_lod_sampler2d_array.cpp
/// @date 2015-10-24 / 2015-10-24
/// @author Christophe Riccio
///////////////////////////////////////////////////////////////////////////////////

#include <gli/sampler2d_array.hpp>
#include <gli/comparison.hpp>
#include <glm/gtx/component_wise.hpp>
#include <glm/gtc/epsilon.hpp>
#include <ctime>
#include <limits>
#include <array>

namespace load
{
	int test()
	{
		int Error(0);

		gli::texture2DArray Texture(gli::FORMAT_RGBA8_UNORM, gli::texture2DArray::texelcoord_type(4, 2), 1, 1);
		*(Texture.data<glm::u8vec4>() + 0) = glm::u8vec4(255,   0,   0, 255);
		*(Texture.data<glm::u8vec4>() + 1) = glm::u8vec4(255, 128,   0, 255);
		*(Texture.data<glm::u8vec4>() + 2) = glm::u8vec4(255, 255,   0, 255);
		*(Texture.data<glm::u8vec4>() + 3) = glm::u8vec4(128, 255,   0, 255);
		*(Texture.data<glm::u8vec4>() + 4) = glm::u8vec4(  0, 255,   0, 255);
		*(Texture.data<glm::u8vec4>() + 5) = glm::u8vec4(  0, 255, 255, 255);
		*(Texture.data<glm::u8vec4>() + 6) = glm::u8vec4(  0,   0, 255, 255);
		*(Texture.data<glm::u8vec4>() + 7) = glm::u8vec4(255,   0, 255, 255);

		glm::u8vec4 Data0 = Texture.load<glm::u8vec4>(gli::texture2DArray::texelcoord_type(0, 0), 0, 0);
		glm::u8vec4 Data1 = Texture.load<glm::u8vec4>(gli::texture2DArray::texelcoord_type(1, 0), 0, 0);
		glm::u8vec4 Data2 = Texture.load<glm::u8vec4>(gli::texture2DArray::texelcoord_type(2, 0), 0, 0);
		glm::u8vec4 Data3 = Texture.load<glm::u8vec4>(gli::texture2DArray::texelcoord_type(3, 0), 0, 0);
		glm::u8vec4 Data4 = Texture.load<glm::u8vec4>(gli::texture2DArray::texelcoord_type(0, 1), 0, 0);
		glm::u8vec4 Data5 = Texture.load<glm::u8vec4>(gli::texture2DArray::texelcoord_type(1, 1), 0, 0);
		glm::u8vec4 Data6 = Texture.load<glm::u8vec4>(gli::texture2DArray::texelcoord_type(2, 1), 0, 0);
		glm::u8vec4 Data7 = Texture.load<glm::u8vec4>(gli::texture2DArray::texelcoord_type(3, 1), 0, 0);

		Error += Data0 == glm::u8vec4(255,   0,   0, 255) ? 0 : 1;
		Error += Data1 == glm::u8vec4(255, 128,   0, 255) ? 0 : 1;
		Error += Data2 == glm::u8vec4(255, 255,   0, 255) ? 0 : 1;
		Error += Data3 == glm::u8vec4(128, 255,   0, 255) ? 0 : 1;
		Error += Data4 == glm::u8vec4(  0, 255,   0, 255) ? 0 : 1;
		Error += Data5 == glm::u8vec4(  0, 255, 255, 255) ? 0 : 1;
		Error += Data6 == glm::u8vec4(  0,   0, 255, 255) ? 0 : 1;
		Error += Data7 == glm::u8vec4(255,   0, 255, 255) ? 0 : 1;

		return Error;
	}
}//namespace load

namespace texture_lod
{
	int test()
	{
		int Error = 0;

		{
			gli::texture2DArray Texture(gli::FORMAT_RGBA8_UNORM, gli::texture2DArray::texelcoord_type(4), 1, 1);
			Texture.clear(gli::u8vec4(0, 0, 0, 255));
			Texture.store(gli::dim2_t(0, 0), 0, 0, gli::u8vec4(255, 127,   0, 255));
			Texture.store(gli::dim2_t(1, 0), 0, 0, gli::u8vec4(255, 127,   0, 255));
			Texture.store(gli::dim2_t(0, 1), 0, 0, gli::u8vec4(255, 127,   0, 255));
			Texture.store(gli::dim2_t(1, 1), 0, 0, gli::u8vec4(255, 127,   0, 255));
			Texture.store(gli::dim2_t(2, 2), 0, 0, gli::u8vec4(  0, 127, 255, 255));
			Texture.store(gli::dim2_t(3, 2), 0, 0, gli::u8vec4(  0, 127, 255, 255));
			Texture.store(gli::dim2_t(2, 3), 0, 0, gli::u8vec4(  0, 127, 255, 255));
			Texture.store(gli::dim2_t(3, 3), 0, 0, gli::u8vec4(  0, 127, 255, 255));

			gli::fsampler2DArray Sampler(Texture, gli::WRAP_CLAMP_TO_EDGE, gli::FILTER_LINEAR, gli::FILTER_LINEAR, gli::vec4(1.0f, 0.5f, 0.0f, 1.0f));

			gli::vec4 SampleA = Sampler.texture_lod(gli::fsampler2DArray::samplecoord_type(0.25f), 0, 0.0f);
			Error += gli::all(gli::epsilonEqual(SampleA, gli::vec4(1.0f, 0.5f, 0.0f, 1.0f), 0.01f)) ? 0 : 1;

			gli::vec4 SampleB = Sampler.texture_lod(gli::fsampler2DArray::samplecoord_type(0.8f), 0, 0.0f);
			Error += gli::all(gli::epsilonEqual(SampleB, gli::vec4(0.0f, 0.5f, 1.0f, 1.0f), 0.01f)) ? 0 : 1;

			gli::vec4 SampleC = Sampler.texture_lod(gli::fsampler2DArray::samplecoord_type(0.20f, 0.8f), 0, 0.0f);
			Error += gli::all(gli::epsilonEqual(SampleC, gli::vec4(0.0f, 0.0f, 0.0f, 1.0f), 0.01f)) ? 0 : 1;
		}

		return Error;
	}
}//namespace texture_lod

namespace sampler_type
{
	int test()
	{
		int Error = 0;

		{
			gli::texture2DArray Texture(gli::FORMAT_RGBA8_UNORM, gli::texture2D::texelcoord_type(4), 1, 1);
			gli::fsampler2DArray Sampler(Texture, gli::WRAP_CLAMP_TO_EDGE, gli::FILTER_LINEAR, gli::FILTER_LINEAR);
		}

		{
			gli::texture2DArray Texture(gli::FORMAT_RGBA8_UNORM, gli::texture2D::texelcoord_type(4), 1, 1);
			gli::dsampler2DArray Sampler(Texture, gli::WRAP_CLAMP_TO_EDGE, gli::FILTER_LINEAR, gli::FILTER_LINEAR);
		}

#		if ENABLE_INTEGER_TESTS
		{
			gli::texture2DArray Texture(gli::FORMAT_RGBA8_UNORM, gli::texture2D::texelcoord_type(4), 1, 1);
			gli::isampler2DArray Sampler(Texture, gli::WRAP_CLAMP_TO_EDGE, gli::FILTER_LINEAR, gli::FILTER_LINEAR);
		}
#		endif//ENABLE_INTEGER_TESTS
		
		return Error;
	}
}//namespace sampler_type

int main()
{
	int Error(0);

	Error += texture_lod::test();
	Error += load::test();
	Error += sampler_type::test();

	return Error;
}

