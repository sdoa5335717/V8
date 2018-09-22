/************************************************************************/
/* D3D HLSL, ¿ÉÀ©Õ¹ÎªNVIDIA CG                                        */
/************************************************************************/
#pragma once

class XShader
{
public:
	XShader();
	virtual ~XShader();

	void InitialShader(IDirect3DDevice9* device);

	D3DXHANDLE GetAttribute(std::string attr);

	void DrawMeshByShader();
private:
	
	IDirect3DDevice9* _device = NULL;

	//vertex declaration
	IDirect3DVertexDeclaration9   *_vDecl = NULL;

	//vertex shader parameters
	IDirect3DVertexShader9        *_VS = NULL;

	ID3DXConstantTable			  *_constTable = NULL;
};

