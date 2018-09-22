#include "stdafx.h"
#include "XShader.h"

const D3DVERTEXELEMENT9 MorphMeshDecl[] =
{
	//1st stream is for source mesh - position, normal, texcoord
	{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
	{ 0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
	{ 0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	D3DDECL_END()
};


XShader::XShader()
{
}


XShader::~XShader()
{
}

void XShader::InitialShader(IDirect3DDevice9* device)
{
	_device = device;
	// ´´½¨¶¥µãÃèÊö
	_device->CreateVertexDeclaration(MorphMeshDecl, &_vDecl);
}

D3DXHANDLE XShader::GetAttribute(std::string attr)
{
	if (NULL == _constTable) return NULL;

	return _constTable->GetConstantByName(0, attr.c_str());
}
void XShader::DrawMeshByShader()
{
	//IDirect3DVertexBuffer9 *pVB = NULL;
	//IDirect3DIndexBuffer9 *pIB = NULL;
	//pMesh->GetVertexBuffer(&pVB);
	//pMesh->GetIndexBuffer(&pIB);

	////Get attribute table
	//DWORD NumAttributes;
	//D3DXATTRIBUTERANGE *pAttributes = NULL;
	//pMesh->GetAttributeTable(NULL, &NumAttributes);
	//pAttributes = new D3DXATTRIBUTERANGE[NumAttributes];
	//pMesh->GetAttributeTable(pAttributes, &NumAttributes);

	////Use the vertex shader interface passed
	//_device->SetVertexShader(pShader);
	//_device->SetVertexDeclaration(pDecl);

	////Set stream sources
	//_device->SetStreamSource(0, pVB, 0, D3DXGetFVFVertexSize(pMesh->GetFVF()));
	//_device->SetIndices(pIB);

	////Go through each attribute group and render
	//for (DWORD i = 0; i < NumAttributes; i++)
	//{
	//	if (pAttributes[i].FaceCount)
	//	{
	//		//Get material number
	//		DWORD MatNum = pAttributes[i].AttribId;

	//		//Set texture
	//		_device->SetTexture(0, pTextures[MatNum]);

	//		//Draw the mesh subset
	//		_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0,
	//			pAttributes[i].VertexStart,
	//			pAttributes[i].VertexCount,
	//			pAttributes[i].FaceStart * 3,
	//			pAttributes[i].FaceCount);
	//	}
	//}

	////Free resources
	//SAFE_RELEASE(pVB);
	//SAFE_RELEASE(pIB);
	//delete[] pAttributes;

}
