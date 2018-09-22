/************************************************************************/
/*网格数据
/*包括该网格的HLSL程序
/************************************************************************/
#pragma once
struct Vertex;
class XShader;
class XMesh
{
public:
	XMesh();
	~XMesh();

	void CreateMesh(DWORD NumFaces,
		DWORD Options, DWORD FVF, const std::vector<Vertex>& vArray,
		const std::vector<WORD>& iArray, LPDIRECT3DDEVICE9 pDevice);

	void SetSubset(int nStart, int nEnd, DWORD id);

	void ReadMeshFromFile();

	void DrawSubset(int i = -1);

private:
	// 着色器
	//XShader _shader;
	// 
	ID3DXMesh* _pMesh = NULL;
};

