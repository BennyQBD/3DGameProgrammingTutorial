#pragma once

#include "rendering/renderContext.hpp"
#include "math/transform.hpp"

class GameRenderContext : public RenderContext
{
public:
	GameRenderContext(RenderDevice& deviceIn, RenderTarget& targetIn, RenderDevice::DrawParams& drawParamsIn,
			Shader& shaderIn, Sampler& samplerIn, const Matrix& perspectiveIn) : RenderContext(deviceIn, targetIn),
		drawParams(drawParamsIn), shader(shaderIn), sampler(samplerIn), perspective(perspectiveIn) {}
	
	inline void renderMesh(VertexArray& vertexArray, Texture& texture, const Matrix& transformIn)
	{
		meshRenderBuffer[std::make_pair(&vertexArray, &texture)].push_back(perspective * transformIn);
	}

	void flush();
private:
	RenderDevice::DrawParams& drawParams;
	Shader& shader;
	Sampler& sampler;
	Matrix perspective;
	Map<std::pair<VertexArray*, Texture*>, Array<Matrix> > meshRenderBuffer;
};

