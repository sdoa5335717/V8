#include "stdafx.h"
#include "XMesh.h"
#include "XShader.h"
#include "XRender.h"

XMesh::XMesh()
{
}


XMesh::~XMesh()
{
}

void XMesh::CreateMesh(DWORD NumFaces,
	DWORD Options, DWORD FVF, const std::vector<Vertex>& vArray,
	const std::vector<WORD>& iArray, LPDIRECT3DDEVICE9 pDevice)
{
	SAFE_RELEASE(_pMesh);

	HRESULT hr = D3DXCreateMeshFVF(NumFaces, vArray.size(),
		Options, FVF, pDevice, &_pMesh);
	if (FAILED(hr))
	{
		assert(FALSE);
	}

	Vertex* v = 0;
	_pMesh->LockVertexBuffer(0, (void**)&v);
	memcpy_s(v, sizeof(Vertex)*vArray.size(), vArray.data(), sizeof(Vertex)*vArray.size());
	_pMesh->UnlockVertexBuffer();

	WORD* i = 0;
	_pMesh->LockIndexBuffer(0, (void**)&i);
	memcpy_s(i, sizeof(WORD)*iArray.size(), iArray.data(), sizeof(WORD)*iArray.size());
	_pMesh->UnlockIndexBuffer();

}

void XMesh::SetSubset(int nStart, int nEnd, DWORD id)
{
	if (!_pMesh) return;

	DWORD* attributeBuffer = 0;
	_pMesh->LockAttributeBuffer(0, &attributeBuffer);
	for (int i = nStart; i < nEnd; i++)
	{
		attributeBuffer[i] = id;
	}
	_pMesh->UnlockAttributeBuffer();
}

void XMesh::DrawSubset(int i /* = 0 */)
{
	if (!_pMesh) return;
	if (i == -1)
	{
		for (int i = 0; i < 2; i++)
		{
			_pMesh->DrawSubset(i);
		}
	}
	else
	{
		_pMesh->DrawSubset(i);
	}
	

}